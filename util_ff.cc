
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// Utility functions for the FreeForm data server.
//
// jhrg 3/29/96

// $Log: util_ff.cc,v $
// Revision 1.5  1998/08/14 19:44:35  jimg
// Added return for non-void functions that could return implicitly (without a
// value).
//
// Revision 1.4  1998/08/12 21:21:18  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:14:10  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:26  jimg
// Sequence support added by Reza
//
// Revision 1.3  1996/05/20 21:11:29  jimg
// make_output_format now returns a string instead of creating a temporary
// file.
// find_format_file was changed to `find_ancillary_file' to better reflect what
// it actually does.
//
// Revision 1.2  1996/04/17 20:40:10  jimg
// First release version of the DODS-FreeForm data server. This works with
// version 1 of the DODS core and client software.
//
// Revision 1.1.2.1  1996/04/10 15:51:45  jimg
// The initial version of the FreeForm data server.

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: util_ff.cc,v 1.5 1998/08/14 19:44:35 jimg Exp $"};

#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include <String.h>
#include "FreeForm.h"

#define DODS_DATA_PRX "dods-"	// prefix for temp format file names

// Given the name of a DODS data type, return the name of the corresponding
// FreeForm data type.
//
// Returns: a const char * if the DODS type maps into a FreeForm type,
// otherwise NULL.

const String
ff_types(const String &dods_type)
{
    if (dods_type == "Byte")
	return "unsigned char";
    else if (dods_type == "Int32")
	return "int32";
    else if (dods_type == "UInt32")
	return "uint32";
    else if (dods_type == "Float64")
	return "float64";
    else if (dods_type == "Str")
	return "char";
    else if (dods_type == "Url")
	return "char";
    else {
	cerr << "ff_types: DODS type " << dods_type
	     << " does not map to a FreeForm type." << endl;
	return "";
    }
}
	    
// Given the name of a DODS data type, return the precision of the
// corresponding FreeForm data type.
//
// Returns: a positive integer if the DODS type maps into a FreeForm type,
// otherwise -1.

int
ff_prec(const String &dods_type)
{
    if (dods_type == "Byte")
	return 0;
    else if (dods_type == "Int32")
	return 0;
    else if (dods_type == "UInt32")
	return 0;
    else if (dods_type == "Float64")
	return 15;
    else if (dods_type == "Str")
	return 0;
    else if (dods_type == "Url")
	return 0;
    else {
	cerr << "ff_prec: DODS type " << dods_type
	    << " does not map to a FreeForm type." << endl;
	return -1;
    }
}    

/** Make a FreeForm output format specification.
    For the current instance of FFArray, build a FreeForm output format
    specification.
    @return The format string. */

const String
make_output_format(const String &name, const String &type, const int width)
{
    ostrstream str;

    str << "binary_output_data \"DODS binary output data\"" << endl;
    str << name << " 1 " << width << " " << ff_types(type) 
	<< " " << ff_prec(type) << endl;
    
    return str.str();
}

// format for multi-dimension array 
const String
makeND_output_format(const String &name, const String &type, const int width,
		     int ndim, const long *start, const long *edge, const
		     long * stride, String *dname)
{
    ostrstream str;
    str << "binary_output_data \"DODS binary output data\"" << endl;
    str << name << " 1 " << width << " ARRAY";

    for (int i=0; i < ndim; i++)
	str << "[" << "\"" << dname[i] << "\" " << start[i]+1 << " to "
	    << start[i]+edge[i] <<" by " << stride[i] << " ]";

    str << " of " << ff_types(type) << " " << ff_prec(type) << endl << "\0";

#ifdef TEST    
	 cout <<str.str();
#endif   

    return str.str();
}

/** Find the format file using a delimiter character.
    Given a special sequence of one or more characters, use that to determine
    the format file name. Assume that the format file ends with EXTENSION.

    NB: DELIMITER defaults to "." and EXTENSION defaults to ".fmt" using the
    utility functions format_delimiter() and format_extension().

    @return A const String object which contains the format file name. */

const String
find_ancillary_file(const String &dataset, const String &delimiter,
		    const String &extension)
{
    // cast away DATASET's const...
    String basename = ((String)dataset).before(delimiter);

    return String(basename + extension);
}

/** Set or get the format file delimiter.
    If given no argument, return the format file basename delimiter. If given
    a String argument, set the format file basename delimiter to that string.

    @return A reference to the delimiter string. */

const String &
format_delimiter(const String &new_delimiter)
{
    static String delimiter = ".";

    if (new_delimiter != "")
	delimiter = new_delimiter;

    return delimiter;
}

/** Set or get the format file extension.
    If given no argument, retrun the format file extension. If given a String
    argument, set the format file extension to that string.

    @return A reference to the format file extension. */

const String &
format_extension(const String &new_extension)
{
    static String extension = ".fmt";

    if (new_extension != "")
	extension = new_extension;

    return extension;
}

/** FreeForm data and format initializattion calls (input format only) */

static bool 
cmp_array_conduit(FF_ARRAY_CONDUIT_PTR src_conduit,
		  FF_ARRAY_CONDUIT_PTR trg_conduit)
{
    if (src_conduit->input && trg_conduit->input)
	return (bool)ff_format_comp(src_conduit->input->fd->format, 
				    trg_conduit->input->fd->format);
    else if (src_conduit->output && trg_conduit->output)
	return (bool) ff_format_comp(src_conduit->output->fd->format,
				     trg_conduit->output->fd->format);
    else
	return false;
}

static int 
merge_redundant_conduits(FF_ARRAY_CONDUIT_LIST conduit_list)
{
    int error = 0;  
    error = list_replace_items((pgenobj_cmp_t)cmp_array_conduit, 
			       conduit_list); 
    return(error);
}

int 
SetDodsDB(FF_STD_ARGS_PTR std_args, DATA_BIN_HANDLE dbin_h, char *Msgt)
{
    FORMAT_DATA_LIST format_data_list = NULL;
    int error = 0;
	
    assert(dbin_h);
	
    if (!dbin_h) {
	sprintf(Msgt, "Error: NULL DATA_BIN_HANDLE in %s", ROUTINE_NAME);
	return(ERR_API);
    }

    if (!*dbin_h) {
	*dbin_h = db_make(std_args->input_file);

	if (!*dbin_h){
	    sprintf (Msgt, "Error in Standard Data Bin");
	    return(ERR_MEM_LACK);
	}
    }
	
    /* Now set the formats and the auxillary files */
	
    if (db_set(*dbin_h, DBSET_READ_EQV, std_args->input_file)) {
	sprintf(Msgt, "Error making name table for %s", std_args->input_file);
	return(DBSET_READ_EQV);
    }
	
    if (db_set(*dbin_h,
	       DBSET_INPUT_FORMATS,
	       std_args->input_file,
	       std_args->output_file,
	       std_args->input_format_file,
	       std_args->input_format_buffer,
	       std_args->input_format_title,
	       &format_data_list)) {
	if (format_data_list)
	    dll_free_holdings(format_data_list);
	    
	sprintf(Msgt, "Error setting an input format for %s",
		std_args->input_file); 
	return(DBSET_INPUT_FORMATS);
    }
	
    error = db_set(*dbin_h, DBSET_CREATE_CONDUITS, std_args, format_data_list);
    dll_free_holdings(format_data_list); // dll == dyn link lib? 8/13/98 jhrg
    if (error) {
	sprintf(Msgt, "Error creating array information for %s",
		std_args->input_file); 
	return(DBSET_CREATE_CONDUITS);
    }
		
    // I think these next two return calls signal errors. 8/13/98 jhrg
    if (db_set(*dbin_h, DBSET_HEADER_FILE_NAMES, FFF_INPUT,
	       std_args->input_file)) {
	sprintf(Msgt, "Error determining input header file names for %s",
		std_args->input_file); 
	return(DBSET_HEADER_FILE_NAMES);
    }
	
    if (db_set(*dbin_h, DBSET_INIT_CONDUITS, FFF_DATA,
	       std_args->records_to_read)) {
	sprintf(Msgt, "Error creating array information for %s",
		std_args->input_file); 
	return(DBSET_INIT_CONDUITS);
    }
	
    error = merge_redundant_conduits((*dbin_h)->array_conduit_list);
    if(error)
	sprintf(Msgt, "Error merging redundent conduits");
	
    return(error);
}

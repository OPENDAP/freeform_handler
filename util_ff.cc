
// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// Utility functions for the FreeForm data server.
//
// jhrg 3/29/96

// $Log: util_ff.cc,v $
// Revision 1.12  1999/05/04 02:55:38  jimg
// Merge with no-gnu
//
// Revision 1.11  1999/04/30 14:48:40  dan
// Modified find_ancillary_file to use the setdbin:find_format_files
// routine to locate the local FreeForm description file.
//
// Revision 1.9  1999/03/18 01:12:38  jimg
// Fixed up the file_exist90 function so that it no longer does the open/close
// thing.
//
// Revision 1.8  1999/03/18 00:25:38  jimg
// Fixed find_ancillary_file(). It now works correctly for files that have .s
// in their names (not just spearating the basename from the extension.
//
// Revision 1.7.12.1  1999/05/01 04:40:30  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.7  1998/08/31 04:06:15  reza
// Added String support.
// Fixed data alignment problem (64-bit Architectures).
// Removed Warnings and added a check for file existence.
// Updated FFND to fix a bug in stride.
//
// Revision 1.6  1998/08/18 16:58:24  reza
// Files with headers are now handled correctly
//
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

static char rcsid[] not_used ={"$Id: util_ff.cc,v 1.12 1999/05/04 02:55:38 jimg Exp $"};

#include <unistd.h>

#include <iostream>
#include <strstream>
#include <fstream>
#include <string>

#include "BaseType.h"
#include "InternalErr.h"
#include "dods-limits.h"
#include "FreeForm.h"

extern "C" int find_format_files(DATA_BIN_PTR, char*, ...);

#define DODS_DATA_PRX "dods-"	// prefix for temp format file names

// Given the name of a DODS data type, return the name of the corresponding
// FreeForm data type.
//
// Returns: a const char * if the DODS type maps into a FreeForm type,
// otherwise NULL.

const string
ff_types(Type dods_type)
{
    switch (dods_type) {
      case dods_byte_c:
	return "int8";
      case dods_int16_c:
	return "int16";
      case dods_uint16_c:
	return "uint16";
      case dods_int32_c:
	return "int32";
      case dods_uint32_c:
	return "uint32";
      case dods_float32_c:
	return "float32";
      case dods_float64_c:
	return "float64";
      case dods_str_c:
	return "text";
      case dods_url_c:
	return "text";
      default:
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
ff_prec(Type dods_type)
{
    switch (dods_type) {
      case dods_byte_c:
      case dods_int16_c:
      case dods_uint16_c:
      case dods_int32_c:
      case dods_uint32_c:
	return 0;
      case dods_float32_c:
	return DODS_FLT_DIG;
      case dods_float64_c:
	return DODS_DBL_DIG;
      case dods_str_c:
      case dods_url_c:
	return 0;
      default:
	cerr << "ff_types: DODS type " << dods_type
	     << " does not map to a FreeForm type." << endl;
	return -1;
    }
}

/** Make a FreeForm output format specification.
    For the current instance of FFArray, build a FreeForm output format
    specification.

    @return The format string. */
const string
make_output_format(const string &name, Type type, const int width)
{
    ostrstream str;

    str << "binary_output_data \"DODS binary output data\"" << endl;
    str << name << " 1 " << width << " " << ff_types(type) 
	<< " " << ff_prec(type) << endl;
    
    string ret = str.str();
    str.freeze(0);
	
    return ret;
}

// format for multi-dimension array 
const string
makeND_output_format(const string &name, Type type, const int width,
		     int ndim, const long *start, const long *edge, const
		     long * stride, string *dname)
{
    ostrstream str;
    str << "binary_output_data \"DODS binary output data\"" << endl;
    str << name << " 1 " << width << " ARRAY";

    for (int i=0; i < ndim; i++)
	str << "[" << "\"" << dname[i] << "\" " << start[i]+1 << " to "
	    << start[i]+(edge[i]-1)*stride[i]+1 <<" by " << stride[i] << " ]";

    str << " of " << ff_types(type) << " " << ff_prec(type) << endl << "\0";

    string ret = str.str();
    str.freeze(0);
	
    return ret;
}

/** Set or get the format file delimiter.
    If given no argument, return the format file basename delimiter. If given
    a string argument, set the format file basename delimiter to that string.

    @return A reference to the delimiter string. */
const string &
format_delimiter(const string &new_delimiter)
{
    static string delimiter = ".";

    if (new_delimiter != "")
	delimiter = new_delimiter;

    return delimiter;
}

/** Set or get the format file extension.
    If given no argument, retrun the format file extension. If given a string
    argument, set the format file extension to that string.

    @return A reference to the format file extension. */

const string &
format_extension(const string &new_extension)
{
    static string extension = ".fmt";

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
    dll_free_holdings(format_data_list); 
    if (error) {
	sprintf(Msgt, "Error creating array information for %s",
		std_args->input_file); 
	return(DBSET_CREATE_CONDUITS);
    }
		
    if (db_set(*dbin_h, DBSET_HEADER_FILE_NAMES, FFF_INPUT,
	       std_args->input_file)) {
	sprintf(Msgt, "Error determining input header file names for %s",
		std_args->input_file); 
	return(DBSET_HEADER_FILE_NAMES);
    }
    
    if (db_set(*dbin_h, DBSET_HEADERS)) 
      { 
	sprintf(Msgt, "getting header file for %s", std_args->input_file); 
	return(DBSET_HEADERS); 
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

bool
file_exist(const char * filename)
{
    return access(filename, F_OK) == 0;
}

/** Find the format file using a delimiter character.
    Given a special sequence of one or more characters, use that to determine
    the format file name. Assume that the format file ends with EXTENSION.

    NB: DELIMITER defaults to "." and EXTENSION defaults to ".fmt" using the
    utility functions format_delimiter() and format_extension().

    @return A const string object which contains the format file name. */
const string
find_ancillary_file(const string &dataset, const string &delimiter,
		    const string &extension)
{
    size_t delim = dataset.find(delimiter);
    string basename = dataset.substr(0, delim);

    if (extension != ".fmt")
	return string(basename + extension);

    else {
	// 
	// Use the FreeForm setdbin:find_format_files() to locate
	// the input format description file.
	//
	// find_format_files() requires a valid DATA_BIN_PTR and DATA_BIN.
	//  - to create one, populate SetUps and call SetDodsDB().
	//
	// find_format_files() will return the input format_file name
	// in the char** formats parameter, if num_formats > 0, then
	// it returns the first valid format_file name in formats[0].
	//

	DATA_BIN_PTR dbin = NULL;
	FF_STD_ARGS_PTR SetUps = NULL;  
	static char Msgt[255];
	char **formats;
	int error = 0;
   
	char *FileName = new char [dataset.length()+1];
	(void) strcpy(FileName, dataset.c_str());
      
	SetUps = ff_create_std_args();
	if (!SetUps)
	    throw InternalErr("Could not create interface record for FreeForm");
    
	/** set the structure values to create the FreeForm DB**/
	SetUps->input_file = FileName; 
	SetUps->output_file = NULL;
      
	error = SetDodsDB(SetUps, &dbin, Msgt);
	if (error && error < ERR_WARNING_ONLY) {
	    db_destroy(dbin);
	    string msg = "Could not set up FreeForm DB structure.";
	    msg += (string)"\n" + (string)Msgt;
	    throw InternalErr(msg);
	}
      
	if (find_format_files(dbin, FileName, &formats)) {
	    string FormatFile = formats[0];
	    free(formats[0]);
	    return string(FormatFile);
	}
	else {
	    string msg = "Could not find an input format for ";
	    msg += FileName;
	    db_destroy(dbin);
	    throw InternalErr(msg);
	}

	db_destroy(dbin);
    }
}

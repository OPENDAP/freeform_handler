
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// Utility functions for the FreeForm data server.
//
// jhrg 3/29/96

// $Log: util_ff.cc,v $
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

static char rcsid[] __unused__ ={"$Id: util_ff.cc,v 1.3 1998/04/21 17:14:10 jimg Exp $"};

#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include <String.h>

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

// format for multi-dimension array (may not be able to handle Grids!!)
const String
makeND_output_format(const String &name, const String &type, const int width,
		     int ndim, const long *start, const long *edge, const
		     long * stride, const char **dname)
{
    ostrstream str;

    str << "binary_output_data \"DODS binary output data\"" << endl;
    str << name << " 1 " << width << " ARRAY";

    for (int i=0; i < ndim; i++)
	str << "[" << "\"" << dname[i] << "\" " << start[i] << " to "
	    << start[i]+edge[i] <<" by " << stride[i] << " ]";

    str << " of " << ff_types(type) << " " << ff_prec(type) << endl;
    
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


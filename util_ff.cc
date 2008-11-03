
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of ff_handler a FreeForm API handler for the OPeNDAP
// DAP2 data server.

// Copyright (c) 2005 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// Utility functions for the FreeForm data server.
//
// jhrg 3/29/96

#include "config_ff.h"

static char rcsid[] not_used =
    { "$Id$" };

#ifndef WIN32
#include <unistd.h> // for access
#else
#define F_OK 0
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>

#include <BaseType.h>
#include <InternalErr.h>
#include <dods-limits.h>
#include <debug.h>

#include "util_ff.h"
// #include "FreeForm.h" Included by the util_ff header.

using namespace std;

extern "C" int dods_find_format_files(DATA_BIN_PTR, char *, const char *,
                                      ...);
extern "C" int dods_find_format_compressed_files(DATA_BIN_PTR, char *,
                                                 char ***, ...);

#define DODS_DATA_PRX "dods-"   // prefix for temp format file names

// Given the name of a DODS data type, return the name of the corresponding
// FreeForm data type.
//
// Returns: a const char * if the DODS type maps into a FreeForm type,
// otherwise NULL.

const string ff_types(Type dods_type)
{
    switch (dods_type) {
    case dods_byte_c:
        return "uint8";
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

int ff_prec(Type dods_type)
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
make_output_format(const string & name, Type type, const int width)
{
    ostringstream str;

    str << "binary_output_data \"DODS binary output data\"" << endl;
    str << name << " 1 " << width << " " << ff_types(type)
        << " " << ff_prec(type) << endl;

    return str.str();
}

// format for multi-dimension array
const string
makeND_output_format(const string & name, Type type, const int width,
                     int ndim, const long *start, const long *edge, const
                     long *stride, string * dname)
{
    ostringstream str;
    str << "binary_output_data \"DODS binary output data\"" << endl;
    str << name << " 1 " << width << " ARRAY";

    for (int i = 0; i < ndim; i++)
        str << "[" << "\"" << dname[i] << "\" " << start[i] + 1 << " to "
            << start[i] + (edge[i] - 1) * stride[i] +
            1 << " by " << stride[i] << " ]";

    str << " of " << ff_types(type) << " " << ff_prec(type) << endl;

    DBG(cerr << "ND output format: " << str.str() << endl);

    return str.str();
}

/** Set or get the format file delimiter.
    If given no argument, return the format file basename delimiter. If given
    a string argument, set the format file basename delimiter to that string.

    @return A reference to the delimiter string. */
const string & format_delimiter(const string & new_delimiter)
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

const string & format_extension(const string & new_extension)
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
        return (bool) ff_format_comp(src_conduit->input->fd->format,
                                     trg_conduit->input->fd->format);
    else if (src_conduit->output && trg_conduit->output)
        return (bool) ff_format_comp(src_conduit->output->fd->format,
                                     trg_conduit->output->fd->format);
    else
        return false;
}

static int merge_redundant_conduits(FF_ARRAY_CONDUIT_LIST conduit_list)
{
    int error = 0;
    error = list_replace_items((pgenobj_cmp_t) cmp_array_conduit,
                               conduit_list);
    return (error);
}

int SetDodsDB(FF_STD_ARGS_PTR std_args, DATA_BIN_HANDLE dbin_h, char *Msgt)
{
    FORMAT_DATA_LIST format_data_list = NULL;
    int error = 0;

    assert(dbin_h);

    if (!dbin_h) {
        snprintf(Msgt, Msgt_size, "Error: NULL DATA_BIN_HANDLE in %s", ROUTINE_NAME);
        return (ERR_API);
    }

    if (!*dbin_h) {
        *dbin_h = db_make(std_args->input_file);

        if (!*dbin_h) {
            snprintf(Msgt, Msgt_size, "Error in Standard Data Bin");
            return (ERR_MEM_LACK);
        }
    }

    /* Now set the formats and the auxillary files */

    if (db_set(*dbin_h, DBSET_READ_EQV, std_args->input_file)) {
        snprintf(Msgt, Msgt_size, "Error making name table for %s",
                std_args->input_file);
        return (DBSET_READ_EQV);
    }

    if (db_set(*dbin_h,
               DBSET_INPUT_FORMATS,
               std_args->input_file,
               std_args->output_file,
               std_args->input_format_file,
               std_args->input_format_buffer,
               std_args->input_format_title, &format_data_list)) {
        if (format_data_list)
            dll_free_holdings(format_data_list);

        snprintf(Msgt, Msgt_size, "Error setting an input format for %s",
                std_args->input_file);
        return (DBSET_INPUT_FORMATS);
    }

    error =
        db_set(*dbin_h, DBSET_CREATE_CONDUITS, std_args, format_data_list);
    dll_free_holdings(format_data_list);
    if (error) {
        snprintf(Msgt, Msgt_size, "Error creating array information for %s",
                std_args->input_file);
        return (DBSET_CREATE_CONDUITS);
    }

    if (db_set(*dbin_h, DBSET_HEADER_FILE_NAMES, FFF_INPUT,
               std_args->input_file)) {
        snprintf(Msgt, Msgt_size, "Error determining input header file names for %s",
                std_args->input_file);
        return (DBSET_HEADER_FILE_NAMES);
    }

    if (db_set(*dbin_h, DBSET_HEADERS)) {
        snprintf(Msgt, Msgt_size, "getting header file for %s", std_args->input_file);
        return (DBSET_HEADERS);
    }


    if (db_set(*dbin_h, DBSET_INIT_CONDUITS, FFF_DATA,
               std_args->records_to_read)) {
        snprintf(Msgt, Msgt_size, "Error creating array information for %s",
                std_args->input_file);
        return (DBSET_INIT_CONDUITS);
    }

    error = merge_redundant_conduits((*dbin_h)->array_conduit_list);
    if (error)
        snprintf(Msgt, Msgt_size, "Error merging redundent conduits");

    return (error);
}

bool file_exist(const char *filename)
{
    return access(filename, F_OK) == 0;
}

/** Find the RSS (Remote Sensing Systems) format file using their naming
    convention.

    File naming convention: <data source> + '_' + <date_string> + <version> +
    (optional)< _d3d > When <date_string> includes YYYYMMDDVV ('DD') the file
    contains 'daily' data. When <date_string> only includes YYYYMMVV ( no
    'DD'), or includes ('DD') and optional '_d3d' then the file contains
    averaged data.

    Different format files are required for 'daily' and 'averaged' data.

    @return A const string object which contains the format file name. */
const string
find_ancillary_rss_formats(const string & dataset, const string & delimiter,
			   const string & extension)
{
    string FormatFile;
    //string FormatPath = getenv("FREEFORM_HANDLER_FORMATS");
    string FormatPath = FREEFORM_HANDLER_FORMATS;
    string BaseName;
    string FileName;

    size_t delim = dataset.rfind("#");
    if ( delim != string::npos ) 
      FileName = dataset.substr(delim+1,dataset.length()-delim+1);
    else {
      	delim = dataset.rfind("/");
	if ( delim != string::npos ) 
	  FileName = dataset.substr(delim+1,dataset.length()-delim+1);
	else
	  FileName = dataset;
    }

    delim = FileName.find("_");
    if ( delim != string::npos ) {
      BaseName = FileName.substr(0,delim+1);
    }
    else {
      string msg = "Could not find input format for: ";
      msg += dataset;
      throw InternalErr(msg);
    }

    string DatePart = FileName.substr(delim+1, FileName.length()-delim+1);
    
    if ( (DatePart.find("_") != string::npos) || (DatePart.length() < 10) )
        FormatFile = FormatPath + BaseName + "averaged.fmt";
    else
        FormatFile = FormatPath + BaseName + "daily.fmt";

    return string(FormatFile);
}

/** Find the RSS (Remote Sensing Systems) format file using their naming
    convention.

    File naming convention: <data source> + '_' + <date_string> + <version> +
    (optional)< _d3d > When <date_string> includes YYYYMMDDVV ('DD') the file
    contains 'daily' data. When <date_string> only includes YYYYMMVV ( no
    'DD'), or includes ('DD') and optional '_d3d' then the file contains
    averaged data.

    Different format files are required for 'daily' and 'averaged' data.

    @return A const string object which contains the format file name. */
const string
find_ancillary_rss_das(const string & dataset, const string & delimiter,
		       const string & extension)
{
    string FormatFile;
    //string FormatPath = getenv("FREEFORM_HANDLER_FORMATS");
    string FormatPath = FREEFORM_HANDLER_FORMATS;
    string BaseName;
    string FileName;

    size_t delim = dataset.rfind("#");
    if ( delim != string::npos ) 
      FileName = dataset.substr(delim+1,dataset.length()-delim+1);
    else {
      	delim = dataset.rfind("/");
	if ( delim != string::npos ) 
	  FileName = dataset.substr(delim+1,dataset.length()-delim+1);
	else
	  FileName = dataset;
    }

    delim = FileName.find("_");
    if ( delim != string::npos ) {
        BaseName = FileName.substr(0,delim+1);
    }
    else {
        string msg = "Could not find input format for: ";
        msg += dataset;
        throw InternalErr(msg);
    }

    string DatePart = FileName.substr(delim+1, FileName.length()-delim+1);
    
    if ( (DatePart.find("_") != string::npos) || (DatePart.length() < 10) )
        FormatFile = FormatPath + BaseName + "averaged.das";
    else
        FormatFile = FormatPath + BaseName + "daily.das";

    return string(FormatFile);
}

// *** Cruft from a --reintegrate merge from 3.7.9? 
#if 0
/** Find the format file using a delimiter character.
    Given a special sequence of one or more characters, use that to determine
    the format file name. Assume that the format file ends with EXTENSION.

    NB: DELIMITER defaults to "." and EXTENSION defaults to ".fmt" using the
    utility functions format_delimiter() and format_extension().

    @return A const string object which contains the format file name. */
const string
find_ancillary_formats(const string & dataset, const string & delimiter,
		       const string & extension)
{
    size_t delim = dataset.find(delimiter);
    string basename = dataset.substr(0, delim);

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

        char *FileName = new char[dataset.length() + 1];
        dataset.copy(FileName, dataset.length() + 1);
        FileName[dataset.length()]='\0';

        SetUps = ff_create_std_args();
        if (!SetUps) {
            delete [] FileName;
            throw InternalErr(__FILE__, __LINE__,
            		"Could not create interface record for FreeForm");
        }
        
        /** set the structure values to create the FreeForm DB**/
        SetUps->input_file = FileName;
        SetUps->output_file = NULL;

        error = SetDodsDB(SetUps, &dbin, Msgt);
        if (error && error < ERR_WARNING_ONLY) {
            delete [] FileName;
            db_destroy(dbin);
            throw InternalErr(__FILE__, __LINE__,
            		string("Could not set up FreeForm DB structure.\n")
            		+ string(Msgt));
        }

        if (dods_find_format_files(dbin, FileName, extension.c_str(),
                                   &formats)) {
            string FormatFile = formats[0];
            free(formats[0]);
            return string(FormatFile);
        } else if (dods_find_format_compressed_files(dbin, FileName,
                                                     &formats)) {
            string FormatFile = formats[0];
            free(formats[0]);
            return string(FormatFile);
        } else {
        	delete [] FileName;
            db_destroy(dbin);
            throw InternalErr(__FILE__, __LINE__,
            				  string("Could not find an input format for ")
            				  + string(FileName));
        }
}
#endif

// These functions are used by the Date/Time Factory classes but they might
// be generally useful in writing server-side functions. 1/21/2002 jhrg

bool is_integer_type(BaseType * btp)
{
    switch (btp->type()) {
    case dods_null_c:
        return false;

    case dods_byte_c:
    case dods_int16_c:
    case dods_uint16_c:
    case dods_int32_c:
    case dods_uint32_c:
        return true;

    case dods_float32_c:
    case dods_float64_c:
    case dods_str_c:
    case dods_url_c:
    case dods_array_c:
    case dods_structure_c:
    case dods_sequence_c:
    case dods_grid_c:
    default:
        return false;
    }
}

bool is_float_type(BaseType * btp)
{
    switch (btp->type()) {
    case dods_null_c:
    case dods_byte_c:
    case dods_int16_c:
    case dods_uint16_c:
    case dods_int32_c:
    case dods_uint32_c:
        return false;

    case dods_float32_c:
    case dods_float64_c:
        return true;

    case dods_str_c:
    case dods_url_c:
    case dods_array_c:
    case dods_structure_c:
    case dods_sequence_c:
    case dods_grid_c:
    default:
        return false;
    }
}

/** Get the value of the BaseType Variable. If it's not something that we can
    convert to an interger, throw InternalErr. */

dods_uint32 get_integer_value(BaseType * var) throw(InternalErr)
{
    if (!var)
        return 0;

    switch (var->type()) {
    case dods_byte_c:{
            dods_byte value = 0;
            dods_byte *value_p = &value;
            if (var)
                var->buf2val((void **) &value_p);

            return dods_uint32(value);
        }

    case dods_int16_c:{
            dods_int16 value = 0;
            dods_int16 *value_p = &value;
            if (var)
                var->buf2val((void **) &value_p);

            return dods_uint32(value);
        }

    case dods_int32_c:{
            dods_int32 value = 0;
            dods_int32 *value_p = &value;
            if (var)
                var->buf2val((void **) &value_p);

            return dods_uint32(value);
        }

    case dods_uint16_c:{
            dods_uint16 value = 0;
            dods_uint16 *value_p = &value;
            if (var)
                var->buf2val((void **) &value_p);

            return dods_uint32(value);
        }
    case dods_uint32_c:{
            dods_uint32 value = 0;
            dods_uint32 *value_p = &value;
            if (var)
                var->buf2val((void **) &value_p);

            return value;
        }

    default:
        throw InternalErr(__FILE__, __LINE__,
                          "Tried to get an integer value for a non-integer datatype!");
    }
}

dods_float64 get_float_value(BaseType * var) throw(InternalErr)
{
    if (!var)
        return 0.0;

    switch (var->type()) {
    case dods_int16_c:
    case dods_uint16_c:
    case dods_int32_c:
    case dods_uint32_c:
        return get_integer_value(var);

    case dods_float32_c:{
            dods_float32 value = 0;
            dods_float32 *value_p = &value;
            if (var)
                var->buf2val((void **) &value_p);

            return dods_float64(value);
        }

    case dods_float64_c:{
            dods_float64 value = 0;
            dods_float64 *value_p = &value;
            if (var)
                var->buf2val((void **) &value_p);

            return value;
        }

    default:
        throw InternalErr(__FILE__, __LINE__,
                          "Tried to get an integer value for a non-integer datatype!");
    }
}


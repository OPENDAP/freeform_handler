

// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors: reza (Reza Nekovei)

// This file contains functions which read the variables and their attributes
// from a netcdf file and build the in-memeory DAS. These functions form the
// core of the server-side software necessary to extract the DAS from a
// netcdf data file.
//
// ReZa 6/23/97

// $Log: ffdas.cc,v $
// Revision 1.10  1999/05/27 17:02:23  jimg
// Merge with alpha-3-0-0
//
// Revision 1.9.2.1  1999/05/20 21:43:51  edavis
// Fix spelling of COPYRIGHT and remove some #if 0 stuff.
//
// Revision 1.9  1999/05/04 02:55:38  jimg
// Merge with no-gnu
//
// Revision 1.8  1999/03/26 20:03:32  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.7.12.1  1999/05/01 04:40:30  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.7  1998/08/31 04:06:13  reza
// Added String support.
// Fixed data alignment problem (64-bit Architectures).
// Removed Warnings and added a check for file existence.
// Updated FFND to fix a bug in stride.
//
// Revision 1.6  1998/08/14 18:26:33  reza
// Removed embedded double quotes in attributes.
//
// Revision 1.5  1998/08/13 20:23:45  jimg
// Removed the attribute about file info.
//
// Revision 1.4  1998/08/12 21:21:14  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:14:09  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:24  jimg
// Sequence support added by Reza
//
//

#include "config_ff.h"

static char rcsid[] not_used = {"$Id: ffdas.cc,v 1.10 1999/05/27 17:02:23 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>
#include <string>

#include "cgi_util.h"
#include "DAS.h"
// Hack. Header files from the WWW library (which are included from Connect.h
// within cgi_util.h) also define BOOLEAN. Suppressing the definition avoids
// a warning (g++ 2.8.1) or error (egcs). 3/26/99 jhrg
#define _BOOLEAN_DEFINED	
#include "FreeForm.h"
#include "util_ff.h"

int StrLens[MaxStr]; // List of string lengths

// Used by ErrMsgT
static char Msgt[255];

// reads the attributes and store their names and values in the attribute
// table. 
//
// Returns: false if an error was detected reading from the freeform format
// file, true otherwise. 

bool
read_attributes(string filename, AttrTable *at, string *err_msg)
{
    int error = 0;
    size_t pos;
    FF_BUFSIZE_PTR bufsize = NULL;
    DATA_BIN_PTR dbin = NULL;
    FF_STD_ARGS_PTR SetUps = NULL;  

    if (!file_exist(filename.c_str())) {
	sprintf(Msgt, "ff_das: Could not open %s", filename.c_str());
	ErrMsgT(Msgt);  
	*err_msg = (string)"\"" + Msgt + " \"";
	return false;
    }
    
    SetUps = ff_create_std_args();
    if (!SetUps) {
	sprintf(Msgt, "ff_das: Insufficient memory -- free more memory and try again");
	ErrMsgT(Msgt);  
	*err_msg = (string)"\"" + Msgt + " \"";
	return false;
    }
    
    /** set the structure values to create the FreeForm DB**/
    SetUps->user.is_stdin_redirected = 0;

    SetUps->input_file = new char[filename.length() + 1];
    strcpy(SetUps->input_file, filename.c_str());

    string iff = find_ancillary_file(filename);
    SetUps->input_format_file = new char[iff.length() + 1];
    strcpy(SetUps->input_format_file, iff.c_str()); // strcpy needs the /0
    SetUps->output_file = NULL;

    error = SetDodsDB(SetUps, &dbin, Msgt);
    if (error && error < ERR_WARNING_ONLY) {
	db_destroy(dbin);
	ErrMsgT(Msgt);  
	*err_msg = "\"" + (string)Msgt + " \"";
	return false;
    }

    error = db_ask(dbin,DBASK_FORMAT_SUMMARY,FFF_INPUT, &bufsize);
    if (error) {
	sprintf(Msgt, "ff_das: db_ask can not get Format Summary");
	ErrMsgT(Msgt);  
	*err_msg = "\"" + (string)Msgt + " \"";
	return false;
    }

    at->append_attr("Server", "STRING", "\"DODS FreeFrom based on FFND release "FFND_LIB_VER"\"");

    //fix the format of the info. string
    string fmt_info = bufsize->buffer;
    while ((pos = fmt_info.find('"')) < fmt_info.size()) 
	fmt_info.replace(pos, 1, '`');
    while ((pos = fmt_info.find('\n')) < fmt_info.size()) 
	fmt_info.replace(pos, 1, ' ');
    fmt_info = "\"" + fmt_info + "\"";
    at->append_attr("Native_file", "STRING", fmt_info);

    return true;
}

// Given a reference to an instance of class DAS and a filename that refers
// to a freefrom file, read the format file and extract the existing
// attributes and add them to the instance of DAS.
//
// Returns: false if an error accessing the file was detected, true
// otherwise. 

bool
get_attributes(DAS &das, string filename, string *error)
{

    AttrTable *attr_table_ptr;
  
    // global attributes (no variable attributes in freefrom)
    attr_table_ptr = das.add_table("FF_GLOBAL", new AttrTable);
    if (!read_attributes(filename, attr_table_ptr, error))
	return false;

  
    return true;
}

#ifdef TEST

int
main(int argc, char *argv[])
{
    DAS das;

    if(!get_attributes(&das, (string)argv[1], ""))
	abort();

    das.print();
}

#endif




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

#include "config_ff.h"

static char rcsid[] not_used = {"$Id: ffdas.cc,v 1.16 2003/05/14 19:30:03 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <iostream>
#include <string>

#include "cgi_util.h"
#include "util.h"
#include "DAS.h"
#include "Error.h"
#include "InternalErr.h"

// Hack. Header files from the WWW library (which are included from Connect.h
// within cgi_util.h) also define BOOLEAN. Suppressing the definition avoids
// a warning (g++ 2.8.1) or error (egcs). 3/26/99 jhrg
#if 0
#define _BOOLEAN_DEFINED	
#endif
#include "FreeForm.h"
#include "util_ff.h"
#include "freeform.h"

extern int StrLens[MaxStr]; // List of string lengths, used by FFSequence. 

// Read header information and populate an AttrTable with the information.

static void
header_to_attributes(AttrTable *at, DATA_BIN_PTR dbin) throw(Error)
{
    PROCESS_INFO_LIST pinfo_list = NULL;
    PROCESS_INFO_PTR hd_pinfo = NULL;

    char text[256];

    int error = db_ask(dbin, DBASK_PROCESS_INFO, 
		       FFF_INPUT | FFF_FILE | FFF_HEADER, &pinfo_list);
    // A general error indicates that the dataset does not have a header.
    // Anything else is bad news. 1/29/2001 jhrg
    if (error) {
	if (error == ERR_GENERAL)
	    return;
	else {
	    string msg = "Cannot get attribute values. FreeForm error code: ";
	    append_long_to_string((long)error, 10, msg);
	    throw Error(msg);
	}
    }    

    pinfo_list = dll_first(pinfo_list);
    hd_pinfo = ((PROCESS_INFO_PTR)(pinfo_list)->data.u.pi);
    if (hd_pinfo) {
      VARIABLE_LIST vlist = NULL;
      VARIABLE_PTR var = NULL;
      
      vlist = FFV_FIRST_VARIABLE(PINFO_FORMAT(hd_pinfo));
      var = ((VARIABLE_PTR)(vlist)->data.u.var);

      while (var) {
	if (IS_EOL(var)) {
	  vlist = (vlist)->next;
	  var = ((VARIABLE_PTR)(vlist)->data.u.var);
		
	  continue;
	}

	switch (FFV_DATA_TYPE(var)) {
          case FFV_TEXT:
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_TEXT, text);
	    // Multiword strings must be quoted.
	    if (strpbrk(text, " \r\t")) {// Any whitespace?
		string quoted_text;
		quoted_text = (string)"\"" + text + "\"";
		at->append_attr(var->name, "STRING", quoted_text.c_str());
	    }
	    else
		at->append_attr(var->name, "STRING", text);
	    break;

	  case FFV_INT8:
	    unsigned char int8;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_INT8, &int8);
	    sprintf(text,"%d",int8);
	    at->append_attr(var->name, "BYTE", text);
	    break;

	  case FFV_INT16:
	    short int16;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_INT16, &int16);
	    sprintf(text,"%d",int16);
	    at->append_attr(var->name, "INT16", text);
	    break;

	  case FFV_INT32:
	    int int32;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_INT32, &int32);
	    sprintf(text,"%d",int32);
	    at->append_attr(var->name, "INT32", text);
	    break;

	  case FFV_INT64:
	    long int64;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_INT64, &int64);
	    sprintf(text,"%ld",int64);
	    at->append_attr(var->name, "INT32", text);
	    break;

	  case FFV_UINT8:
	    unsigned char uint8;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_UINT8, &uint8);
	    sprintf(text,"%d",uint8);
	    at->append_attr(var->name, "BYTE", text);
	    break;

	  case FFV_UINT16:
	    unsigned short uint16;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_UINT16, &uint16);
	    sprintf(text,"%d",uint16);
	    at->append_attr(var->name, "UINT16", text);
	    break;

	  case FFV_UINT32:
	    unsigned int uint32;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_UINT32, &uint32);
	    sprintf(text,"%u",uint32);
	    at->append_attr(var->name, "UINT32", text);
	    break;

	  case FFV_UINT64:
	    unsigned long uint64;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_UINT64, &uint64);
	    sprintf(text,"%lu",uint64);
	    at->append_attr(var->name, "UINT32", text);
	    break;

	  case FFV_FLOAT32:
	    float float32;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_FLOAT32, &float32);
	    sprintf(text,"%f",float32);
	    at->append_attr(var->name, "FLOAT32", text);
	    break;

	  case FFV_FLOAT64:
	    double float64;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_FLOAT64, &float64);
	    sprintf(text,"%f",float64);
	    at->append_attr(var->name, "FLOAT64", text);
	    break;

	  case FFV_ENOTE:
	    double enote;
	    nt_ask(dbin, FFF_FILE | FFF_HEADER, var->name, FFV_ENOTE, &enote);
	    sprintf(text,"%e",enote);
	    at->append_attr(var->name, "FLOAT64", text);
	    break;

          default:
	    throw InternalErr(__FILE__, __LINE__, "Unknown FreeForm type!");
        }           
	vlist = (vlist)->next;
	var = ((VARIABLE_PTR)(vlist)->data.u.var);
      }
    }
}

// reads the attributes and store their names and values in the attribute
// table. 
//
// Returns: false if an error was detected reading from the freeform format
// file, true otherwise. 

void
read_attributes(string filename, AttrTable *at) throw(Error)
{
    int error = 0;
    FF_BUFSIZE_PTR bufsize = NULL;
    DATA_BIN_PTR dbin = NULL;
    FF_STD_ARGS_PTR SetUps = NULL;  

    if (!file_exist(filename.c_str())) {
      string msg = (string)"ff_das: Could not open file " + 
	path_to_filename(filename) + ".";
      throw Error(msg);
    }
  
    SetUps = ff_create_std_args();
    if (!SetUps) {
      string msg = "ff_das: Insufficient memory";
      throw Error(msg);
    }
    
    /** set the structure values to create the FreeForm DB**/
    SetUps->user.is_stdin_redirected = 0;

    SetUps->input_file = new char[filename.length() + 1];
    strcpy(SetUps->input_file, filename.c_str());

#ifdef TEST
    string iff = find_ancillary_file(filename);
    SetUps->input_format_file = new char[iff.length() + 1];
    strcpy(SetUps->input_format_file, iff.c_str()); // strcpy needs the /0
#endif

    SetUps->output_file = NULL;

    char Msgt[255];
    error = SetDodsDB(SetUps, &dbin, Msgt);
    if (error && error < ERR_WARNING_ONLY) {
	db_destroy(dbin);
	throw Error(Msgt);
    }

    error = db_ask(dbin,DBASK_FORMAT_SUMMARY,FFF_INPUT, &bufsize);
    if (error) {
      string msg = (string)"Cannot get Format Summary. FreeForm error code: ";
      append_long_to_string((long)error, 10, msg);
      throw Error(msg);
    }

    at->append_attr("Server", "STRING", 
		    "\"DODS FreeFrom based on FFND release "FFND_LIB_VER"\"");

    header_to_attributes(at, dbin); // throws Error
}

static void
add_variable_containers(DAS &das, const string &filename) throw(Error)
{
    if (!file_exist(filename.c_str())) {
	throw Error(string("ff_dds: Could not open file ")
		    + path_to_filename(filename) + string("."));
    }

    // Setup the DB access.
    FF_STD_ARGS_PTR SetUps = ff_create_std_args();
    if (!SetUps) {
	throw Error("Insufficient memory");
    }

    SetUps->user.is_stdin_redirected = 0;
    SetUps->input_file = new char[filename.length() + 1];
    strcpy(SetUps->input_file, filename.c_str());
    SetUps->output_file = NULL;
  
    // Set the structure values to create the FreeForm DB
    char Msgt[255];
    DATA_BIN_PTR dbin = NULL;
    int error = SetDodsDB(SetUps, &dbin, Msgt);
    if (error && error < ERR_WARNING_ONLY) {
	db_destroy(dbin);
	string msg;
	msg = string(Msgt) + string(" FreeForm error code: ");
	append_long_to_string((long)error, 10, msg);
	throw Error(msg);
    }
  
    // Get the names of all the variables.
    int num_names = 0;
    char **var_names_vector = NULL;
    error = db_ask(dbin, DBASK_VAR_NAMES, FFF_INPUT | FFF_DATA, 
		       &num_names, &var_names_vector);
    if (error) {
	string msg;
	msg = string("Could not get varible list from the input file.\n")
	    + string(" FreeForm error code: ");
	append_long_to_string((long)error, 10, msg);
	throw Error(msg);
    }

    // I don't understand why this has to happen here, but maybe it's moved
    // outside the loop because FreeForm is designed so that the pinfo list
    // only needs to be accessed once and can be used when working with
    // any/all of the variables. 4/4/2002 jhrg
    PROCESS_INFO_LIST pinfo_list = NULL;
    error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA, 
		   &pinfo_list);
    if(error){
	string msg = (string)"Could not get process info for the input file."
	    + " FreeForm error code: ";
	append_long_to_string((long)error, 10, msg);
	throw Error(msg);
    }
 
     // For each variable, figure out what its name really is (arrays have
    // funny names).
    for (int i=0; i<num_names; i++) { 
	int num_dim_names = 0;
	char **dim_names_vector = NULL;
	error = db_ask(dbin, DBASK_ARRAY_DIM_NAMES, var_names_vector[i], 
		       &num_dim_names, &dim_names_vector);
	if (error) {
	    string msg;
	    msg = string("Could not get array dimension names for variable: ")
		+ string(var_names_vector[i]) 
		+ string(", FreeForm error code: ");
	    append_long_to_string((long)error, 10, msg);
	    throw Error(msg);
	}

	// Note: FreeForm array names are returned appened to their format
	// name with '::'.
	char *cp = NULL;
	if (num_dim_names == 0)	// sequence names
	    cp = var_names_vector[i];
	else 	  
	    cp = strstr(var_names_vector[i], "::")+2;

	// We need this to figure out if this variable is the/a EOL
	// valriable. We read the pinfo_list just before starting this
	// for-loop. 
	pinfo_list = dll_first(pinfo_list);
	PROCESS_INFO_PTR pinfo = ((PROCESS_INFO_PTR)(pinfo_list)->data.u.pi);
	FORMAT_PTR iformat = PINFO_FORMAT(pinfo);
	VARIABLE_PTR var = ff_find_variable(cp, iformat);	

	// For some formats: Freefrom sends an extra EOL variable at the end of
	// the list. Add an attribute container for all the other variables.
	if(!IS_EOL(var))
	    das.add_table(cp, new AttrTable);
    }
}

// Given a reference to an instance of class DAS and a filename that refers
// to a freefrom file, read the format file and extract the existing
// attributes and add them to the instance of DAS.
//
// Returns: false if an error accessing the file was detected, true
// otherwise. 

void
get_attributes(DAS &das, string filename) throw(Error)
{
    AttrTable *attr_table_p = new AttrTable;

    das.add_table("FF_GLOBAL", attr_table_p);
    read_attributes(filename, attr_table_p);
    // Add a table/container for each variable. See bug 284. The DAP spec
    // calls for each variable to have an attribute container, even if it is
    // empty. Previously the server did not have containers for all the
    // variables. 4/4/2002 jhrg
    add_variable_containers(das, filename);
}

#ifdef TEST

int
main(int argc, char *argv[])
{
    DAS das;

    try {
      get_attributes(&das, (string)argv[1]);
      das.print();
    }
    catch (Error &e) {
      e.display_message();
      return 1;
    }
    
    return 0;
}

#endif

// $Log: ffdas.cc,v $
// Revision 1.16  2003/05/14 19:30:03  jimg
// Changed StrLens so taht it's declared extern; the definition is in
// FFSequence.cc
//
// Revision 1.15  2003/02/10 23:01:53  jimg
// Merged with 3.2.5
//
// Revision 1.14  2001/09/28 23:19:43  jimg
// Merged with 3.2.3.
//
// Revision 1.13.4.6  2002/06/21 00:31:40  jimg
// I changed many files throughout the source so that the 'make World' build
// works with the new versions of Connect and libdap++ that use libcurl.
// Most of these changes are either to Makefiles, configure scripts or to
// the headers included by various C++ files. In a few places the oddities
// of libwww forced us to hack up code and I've undone those and some of the
// clients had code that supported libwww's generous tracing capabilities
// (that's one part of libwww I'll miss); I had to remove support for that.
// Once this code compiles and more work is done on Connect, I'll return to
// each of these changes and polish them.
//
// Revision 1.13.4.5  2002/04/04 21:41:43  jimg
// Added a new function add_variable_containers(...) which scans the FreeForm
// file and adds an Attribute container for each variable (as per the DAP spec,
// see bug 284). There's a fair amount of code duplication between the various
// functions that query the FF API; that may be inevitable unless we build our
// own layer on top of it.
//
// Revision 1.13.4.4  2001/09/17 06:41:29  reza
// Fixed error reporting bugs.
//
// Revision 1.13.4.3  2001/05/23 20:24:35  dan
// Removed calls to find_ancillary_files to locate format
// descriptor files.  Use the functions in the FreeForm
// code to perform this function (which happens anyways).
//
// Revision 1.13.4.2  2001/05/23 18:14:53  jimg
// Merged with changes on the release-3-1 branch. This apparently was not
// done corrrectly the first time around.
//
// Revision 1.13.4.1  2001/01/30 01:07:43  jimg
// Factored out the code in read_attributes that extracts attributes from data
// file headers. I also added error trapping so that files without headers won't
// crash the server(!). Also, I modified the code which adds the attributes to
// quote any multiword String attributes (using space, \r and \t as separators).
//
// Revision 1.13  2000/10/12 18:11:45  jimg
// Added more exception stuff. Reworked the get/read_description() functions
// so they throw Error and InternalErr instead of returning codes.
//
// Revision 1.12  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.11.2.1  1999/08/28 01:25:53  jimg
// Modified the code which reads header variables and loads them into the DAS
// in read_attributes(). When the second FF db_ask(...) call returns an error
// this code should assume that there are no header variables, not that a
// fatal error was detected. This assumes that the previous db_ask(...) call
// was successful. Since there are no header variables when the second
// db_ask(...) call returns an error, make sure not to access the pinfo_list
// data structure.
//
// Revision 1.11  1999/07/22 21:28:10  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.10.2.1  1999/07/21 20:27:08  dan
// Added Attribute handling for FreeForm File_Headers.  FreeForm
// header variables defined in the format-descriptor file will be
// added the the FF_GLOBAL DAS container.  File_Headers are commonly
// used by FreeForm to provide metadata concerning the data contained
// in the dataset.
//
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

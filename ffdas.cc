/*
  Copyright 1997 The University of Rhode Island and The Massachusetts
  Institute of Technology.

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

// This file contains functions which read the variables and their attributes
// from a netcdf file and build the in-memeory DAS. These functions form the
// core of the server-side software necessary to extract the DAS from a
// netcdf data file.
//
// ReZa 6/23/97

// $Log: ffdas.cc,v $
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

static char rcsid[] __unused__ ={"$Id: ffdas.cc,v 1.5 1998/08/13 20:23:45 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>

#include "cgi_util.h"
#include "DAS.h"
#include "FreeForm.h"
#include "util_ff.h"

// Used by ErrMsgT
static char Msgt[255];

// reads the attributes and store their names and values in the attribute table.
//
// Returns: false if an error was detected reading from the freeform format
// file, true otherwise. 

bool
read_attributes(const char *filename, AttrTable *at, String *err_msg)
{
  int error = 0;
  FF_BUFSIZE_PTR bufsize = NULL;
  DATA_BIN_PTR dbin = NULL;
  FF_STD_ARGS_PTR SetUps = NULL;  
  
  SetUps = ff_create_std_args();
  if (!SetUps)
    {
      sprintf(Msgt, "ff_das: Insufficient memory -- free more memory and try again");
      ErrMsgT(Msgt);  
      cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
      return false;
    }
    
  /** set the structure values to create the FreeForm DB**/
  SetUps->user.is_stdin_redirected = 0;
  SetUps->input_file = (char *)filename;
  String ds = (char *)filename;
  String iff = find_ancillary_file(ds);
  char *if_f = new char[iff.length() + 1];
  strcpy(if_f, iff);
  SetUps->input_format_file = if_f;
  SetUps->output_file = NULL;

  error = SetDodsDB(SetUps, &dbin, Msgt);
  if (error && error < ERR_WARNING_ONLY)
    {
      db_destroy(dbin);
      ErrMsgT(Msgt);  
      cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
      return false;
    }
  error = db_ask(dbin,DBASK_FORMAT_SUMMARY,FFF_INPUT, &bufsize);
  if(error) {
    sprintf(Msgt, "ff_das: db_ask can not get Format Summary");
    ErrMsgT(Msgt);  
    cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
    return false;
  }
  
  at->append_attr("Server", "STRING", "\"DODS FreeFrom based on FFND release "FFND_LIB_VER"\"");
  // at->append_attr("Title", "STRING", dbin->title);

  // Add the file information back once we have code to process the string.
  // It contains embedded double quotes that confuse the DAS parser. 8/13/98
  // jhrg.
#if 0
  at->append_attr("file_information", "STRING", bufsize->buffer);
#endif

  return true;
}

// Given a reference to an instance of class DAS and a filename that refers
// to a freefrom file, read the format file and extract the existing
// attributes and add them to the instance of DAS.
//
// Returns: false if an error accessing the file was detected, true
// otherwise. 

bool
get_attributes(DAS &das, const char *filename, String *error)
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

    if(!get_attributes(das, argv[1], ""))
	abort();

    das.print();
}

#endif




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
// Revision 1.2  1998/04/16 18:11:24  jimg
// Sequence support added by Reza
//
//

static char rcsid[]={"$Id: ffdas.cc,v 1.2 1998/04/16 18:11:24 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>
// #include <assert.h>

#include "cgi_util.h"
#include "DAS.h"
#include "FreeForm.h"


// These are used as the return values for print_type().

static const char STRING[]="String";
static const char BYTE[]="Byte";
static const char INT32[]="Int32";
static const char FLOAT64[]="Float64";

// Used by ErrMsgT

static char Msgt[255];

// reads the attributes and store their names and values in the attribute table.
//
// Returns: false if an error was detected reading from the freeform format
// file, true otherwise. 

bool
read_attributes(char *filename, AttrTable *at, String *error1)
{

  FF_BUFSIZE_PTR bufsize = NULL;
  
  DATA_BIN_PTR dbin = NULL;
  int error = 0;
  FF_STD_ARGS_PTR std_args = NULL;

  std_args = ff_create_std_args();
  if (!std_args)
    {
      error = ERR_MEM_LACK;
      exit(1);
    }

  std_args->input_file = filename;
  String format_file = ((String) filename).before(".")+".fmt";
  std_args->input_format_file = (const char *) format_file;

  error = db_init(std_args, &dbin, NULL);
  if (error && error < ERR_WARNING_ONLY)
    exit(1);
  else if (error)
    error = 0;

  //cout << dbin->title;

  //	db_ask(dbin, DBASK_FORMAT_SUMMARY, &bufsize);

  //	at->append_attr("Title", STRING, bufsize->buffer);

	at->append_attr("Server", STRING, "\"DODS FreeFrom based on FFND release "FFND_LIB_VER"\"");

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




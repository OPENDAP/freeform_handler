// -*- C++ -*-

// Prototypes for the FreeForm server utility functions.
//
// jhrg 4/2/96

// $Id: util_ff.h,v 1.7 1999/05/04 02:55:38 jimg Exp $

#ifndef _util_ff_h_
#define _util_ff_h_

#include "FreeForm.h"

#include "BaseType.h"

#define MaxStr 100

const string ff_types(Type dods_type);
int ff_prec(Type dods_type);

const string make_output_format(const string &name, Type type, 
				const int width);

const string makeND_output_format(const string &name, Type type, 
				  const int width, int ndim, 
				  const long *start, const long *edge, const
				  long * stride, string *dname);

const string &format_extension(const string &new_extension = "");
const string &format_delimiter(const string &new_delimiter = "");

const string find_ancillary_file(const string &dataset, 
				 const string &delimiter = format_delimiter(),
				 const string &extension = format_extension());

int SetDodsDB(FF_STD_ARGS_PTR std_args, DATA_BIN_HANDLE dbin_h, char * Msgt);

bool file_exist(const char * filename);

extern "C" long read_ff(char *dataset, char *if_file, char *o_format, char *o_buffer, unsigned long size);


#endif // _util_ff_h_

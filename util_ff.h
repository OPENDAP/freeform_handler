// -*- C++ -*-

// Prototypes for the FreeForm server utility functions.
//
// jhrg 4/2/96

// $Id: util_ff.h,v 1.4 1998/08/12 21:21:19 jimg Exp $

#ifndef _util_ff_h_
#define _util_ff_h_

#include "FreeForm.h"

const String ff_types(const String &dods_type);
int ff_prec(const String &dods_type);

const String make_output_format(const String &name, const String &type, 
				const int width);

const String makeND_output_format(const String &name, const String &type, 
				  const int width, int ndim, 
				  const long *start, const long *edge, const
				  long * stride, String *dname);

const String &format_extension(const String &new_extension = "");
const String &format_delimiter(const String &new_delimiter = "");

const String find_ancillary_file(const String &dataset, 
			      const String &delimiter = format_delimiter(),
			      const String &extension = format_extension());

int SetDodsDB(FF_STD_ARGS_PTR std_args, DATA_BIN_HANDLE dbin_h, char * Msgt);

extern "C" long read_ff(char *dataset, char *if_file, char *o_format, char *o_buffer, unsigned long size);

#endif // _util_ff_h_

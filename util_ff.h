// -*- C++ -*-

// Prototypes for the FreeForm server utility functions.
//
// jhrg 4/2/96

// $Id: util_ff.h,v 1.6 1999/03/26 20:03:32 jimg Exp $

#ifndef _util_ff_h_
#define _util_ff_h_

#include "FreeForm.h"

#include "BaseType.h"

#define MaxStr 100

const String ff_types(Type dods_type);
int ff_prec(Type dods_type);

const String make_output_format(const String &name, Type type, 
				const int width);

const String makeND_output_format(const String &name, Type type, 
				  const int width, int ndim, 
				  const long *start, const long *edge, const
				  long * stride, String *dname);

// These four versions of the ff_types, ..., makeND_output_format functions
// are deprecated. Use the versions that take the type argument as a Type
// rather than as a String. 3/25/99 jhrg
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

bool file_exist(const char * filename);

extern "C" long read_ff(char *dataset, char *if_file, char *o_format, char *o_buffer, unsigned long size);


#endif // _util_ff_h_

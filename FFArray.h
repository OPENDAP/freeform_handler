// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFArray.h,v $
// Revision 1.6  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.5  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.4.12.1  1999/05/01 04:40:30  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.4  1998/08/12 21:20:50  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:13:42  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:10:59  jimg
// Sequence support added by Reza

#ifndef _FFArray_h
#define _FFArray_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <string>

#include "Array.h"

extern Array * NewArray(const string &n = "", BaseType *v = 0);

class FFArray: public Array {
private:
    long Seq_constraint(long *cor, long *step, long *edg, bool *has_stride);
    long Arr_constraint(long *cor, long *step, long *edg, string *dim_nms,
			bool *has_stride);

    /** Read an array of simple types into this objects #_buf# memeber. */
    template <class T> bool extract_array<T>(char *ds, char *if_fmt, 
					     char *o_fmt);

public:
    FFArray(const string &n = "", BaseType *v = 0);
    virtual ~FFArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset, int &error);
};

#endif

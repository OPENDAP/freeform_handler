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

#include "Array.h"
extern Array * NewArray(const String &n = (char *)0, BaseType *v = 0);

class FFArray: public Array {
public:
    FFArray(const String &n = (char *)0, BaseType *v = 0);
    virtual ~FFArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);

    long Seq_constraint(long *cor, long *step, long *edg, bool *has_stride);
    long Arr_constraint(long *cor, long *step, long *edg, char **dim_nms,
			bool *has_stride);
};

#endif



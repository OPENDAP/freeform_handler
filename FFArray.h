// -*- C++ -*-

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

/* $Log: FFArray.h,v $
/* Revision 1.1  1997/10/03 17:01:47  jimg
/* Initial version from Reza
/*
 *
 */

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
    long format_constraint(long *cor, long *step, long *edg, bool *has_stride);
};

#endif



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

// $Log: FFStr.h,v $
// Revision 1.3  1998/04/21 17:14:01  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:17  jimg
// Sequence support added by Reza

#ifndef _FFStr_h
#define _FFStr_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <limits.h>

#include "Str.h"
extern Str * NewStr(const String &n = (char *)0);

class FFStr: public Str {
public:
    FFStr(const String &n = (char *)0);
    virtual ~FFStr() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
};

typedef FFStr * FFStrPtr;

#endif


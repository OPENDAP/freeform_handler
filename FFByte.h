// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// (c) COPYRIGHT URI/MIT 1994-1997
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      reza            Reza Nekovei (reza@intcomm.net)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/16/97

// $Log: FFByte.h,v $
// Revision 1.3  1998/04/21 17:13:44  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:01  jimg
// Sequence support added by Reza

#ifndef _FFByte_h
#define _FFByte_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Byte.h"
extern Byte * NewByte(const String &n = (char *)0);

class FFByte: public Byte {
public:
    FFByte(const String &n = (char *)0);
    virtual ~FFByte() {}

    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);
};

typedef FFByte * FFBytePtr;

#endif


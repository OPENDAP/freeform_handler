// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFInt32.h,v $
// Revision 1.4  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.3.12.1  1999/05/01 04:40:30  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.3  1998/04/21 17:13:54  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:11  jimg
// Sequence support added by Reza

#ifndef _FFInt32_h
#define _FFInt32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int32.h"
extern Int32 * NewInt32(const string &n = "");

class FFInt32: public Int32 {
public:
    FFInt32(const string &n = "");
    virtual ~FFInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

typedef FFInt32 * FFInt32Ptr;

#endif


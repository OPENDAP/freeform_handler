// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFFloat64.h,v $
// Revision 1.5  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.4  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.3.12.1  1999/05/01 04:40:30  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.3  1998/04/21 17:13:47  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:04  jimg
// Sequence support added by Reza

#ifndef _FFFloat64_h
#define _FFFloat64_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Float64.h"
extern Float64 * NewFloat64(const string &n = "");

class FFFloat64: public Float64 {
public:
    FFFloat64(const string &n = "");
    virtual ~FFFloat64() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

#endif


// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1997
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      reza            Reza Nekovei (reza@intcomm.net)

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

/* $Log: FFInt32.h,v $
/* Revision 1.1  1997/10/03 17:02:08  jimg
/* Initial version from Reza
/*
 *
*/

#ifndef _FFInt32_h
#define _FFInt32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int32.h"
extern Int32 * NewInt32(const String &n = (char *)0);

class FFInt32: public Int32 {
public:
    FFInt32(const String &n = (char *)0);
    virtual ~FFInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
};

typedef FFInt32 * FFInt32Ptr;

#endif


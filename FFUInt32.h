// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1997
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors:
//      reza            Reza Nekovei (reza@intcomm.net)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

/* 
 * $Log: FFUInt32.h,v $
 * Revision 1.2  1998/04/16 18:11:21  jimg
 * Sequence support added by Reza
 *
 *
 */

#ifndef _FFUInt32_h
#define _FFUInt32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt32.h"
extern UInt32 * NewUInt32(const String &n = (char *)0);

class FFUInt32: public UInt32 {
public:
    FFUInt32(const String &n = (char *)0);
    virtual ~FFUInt32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
};

#endif


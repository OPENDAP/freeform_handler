// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFStructure.h,v $
// Revision 1.5  1999/05/27 17:02:22  jimg
// Merge with alpha-3-0-0
//
// Revision 1.4.2.1  1999/05/20 21:42:15  edavis
// Fix spelling of COPYRIGHT and remove some #if 0 stuff.
//
// Revision 1.4  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.3.12.1  1999/05/01 04:40:30  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.3  1998/04/21 17:14:03  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:19  jimg
// Sequence support added by Reza

#ifndef _FFStructure_h
#define _FFStructure_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Structure.h"
extern Structure * NewStructure(const string &n = "");

class FFStructure: public Structure {
public:
    FFStructure(const string &n = (char *)0);
    virtual ~FFStructure();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset, int &error);
};

#endif

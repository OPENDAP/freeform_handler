// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// Freeform sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFUrl.h,v $
// Revision 1.4  1999/05/04 02:55:37  jimg
// Merge with no-gnu
//
// Revision 1.3.12.1  1999/05/01 04:40:31  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.3  1998/04/21 17:14:05  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:22  jimg
// Sequence support added by Reza

#ifndef _FFUrl_h
#define _FFUrl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Url.h"
extern Url * NewUrl(const string &n = "");

class FFUrl: public Url {
public:
    FFUrl(const string &n = "");
    virtual ~FFUrl() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

typedef FFUrl * FFUrlPtr;

#endif


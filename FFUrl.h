// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      reza            Reza Nekovei (reza@intcomm.net)

// Freeform sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

/* $Log: FFUrl.h,v $
/* Revision 1.1  1997/10/03 17:02:29  jimg
/* Initial version from Reza
/*
 *
*/

#ifndef _FFUrl_h
#define _FFUrl_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Url.h"
extern Url * NewUrl(const String &n = (char *)0);

class FFUrl: public Url {
public:
    FFUrl(const String &n = (char *)0);
    virtual ~FFUrl() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const String &dataset, int &error);
};

typedef FFUrl * FFUrlPtr;

#endif


// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1994-1996
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      reza            Reza Nekovei (reza@intcomm.net)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

/* $Log: FFSequence.h,v $
/* Revision 1.1  1997/10/03 17:02:16  jimg
/* Initial version from Reza
/*
 *
*/

#ifndef _FFSequence_h
#define _FFSequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Sequence.h"
extern Sequence * NewSequence(const String &n = (char *)0);

class FFSequence: public Sequence {
public:
    FFSequence(const String &n = (char *)0);
    virtual ~FFSequence();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);
};

#endif

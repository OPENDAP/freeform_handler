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

// $Log: FFSequence.h,v $
// Revision 1.4  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.3.12.1  1999/05/01 04:40:30  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.3  1998/04/21 17:13:58  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:15  jimg
// Sequence support added by Reza

#ifndef _FFSequence_h
#define _FFSequence_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Sequence.h"
extern Sequence * NewSequence(const string &n = "");

class FFSequence: public Sequence {
public:
    FFSequence(const string &n = "");
    virtual ~FFSequence();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset, int &error);
};

#endif

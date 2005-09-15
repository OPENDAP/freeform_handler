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

#ifndef _ffsequence_h
#define _ffsequence_h 1


#include "Sequence.h"
extern Sequence * NewSequence(const string &n = "");

class FFSequence: public Sequence {
public:
    FFSequence(const string &n = "");
    virtual ~FFSequence();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

// $Log: FFSequence.h,v $
// Revision 1.6  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.5  1999/05/27 17:02:22  jimg
// Merge with alpha-3-0-0
//
// Revision 1.4.2.1  1999/05/20 21:41:59  edavis
// Fix spelling of COPYRIGHT and remove some #if 0 stuff.
//
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

#endif

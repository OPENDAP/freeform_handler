// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: James Gallagher

#ifndef _ffuint16_h
#define _ffuint16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt16.h"

class FFUInt16: public UInt16 {
public:
    FFUInt16(const string &n = "");
    virtual ~FFUInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset);
};

// $Log: FFUInt16.h,v $
// Revision 1.2  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.1  1999/05/19 17:52:42  jimg
// Added
//

#endif


// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: James Gallagher

// $Log: FFUInt16.h,v $
// Revision 1.1  1999/05/19 17:52:42  jimg
// Added
//

#ifndef _FFUInt16_h
#define _FFUInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "UInt16.h"

class FFUInt16: public UInt16 {
public:
    FFUInt16(const string &n = "");
    virtual ~FFUInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

#endif


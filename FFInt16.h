// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: James Gallagher

// $Log: FFInt16.h,v $
// Revision 1.1  1999/05/19 17:52:41  jimg
// Added
//

#ifndef _FFInt16_h
#define _FFInt16_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Int16.h"

class FFInt16: public Int16 {
public:
    FFInt16(const string &n = "");
    virtual ~FFInt16() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

#endif


// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors: James Gallagher

// $Log: FFFloat32.h,v $
// Revision 1.1  1999/05/19 17:52:41  jimg
// Added
//

#ifndef _FFFloat32_h
#define _FFFloat32_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Float32.h"
extern Float32 * NewFloat32(const string &n = "");

class FFFloat32: public Float32 {
public:
    FFFloat32(const string &n = "");
    virtual ~FFFloat32() {}

    virtual BaseType *ptr_duplicate();
    
    virtual bool read(const string &dataset, int &error);
};

#endif


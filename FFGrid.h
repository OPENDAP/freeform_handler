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

/* $Log: FFGrid.h,v $
/* Revision 1.2  1998/04/16 18:11:09  jimg
/* Sequence support added by Reza
/*
 *
*/


#ifndef _FFGrid_h
#define _FFGrid_h 1

#ifdef _GNUG_
#pragma interface
#endif

#include "Grid.h"
extern Grid * NewGrid(const String &n = (char *)0);

class FFGrid: public Grid {
public:
    FFGrid(const String &n = (char *)0);
    virtual ~FFGrid();
    
    virtual BaseType *ptr_duplicate();

    virtual bool read(const String &dataset, int &error);
};

#endif






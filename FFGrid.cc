
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFGrid.cc,v $
// Revision 1.3  1998/04/21 17:13:50  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:08  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFGrid.cc,v 1.3 1998/04/21 17:13:50 jimg Exp $"};

#include "FFGrid.h"

Grid *
NewGrid(const String &n)
{
    return new FFGrid(n);
}

// protected

BaseType *
FFGrid::ptr_duplicate()
{
    return new FFGrid(*this);
}

// public

FFGrid::FFGrid(const String &n) : Grid(n)
{
}

FFGrid::~FFGrid()
{
}

bool
FFGrid::read(const String &dataset, int &error)
{
    bool status;

    if (read_p()) // nothing to do
        return true;

    // read array elements
    if (!(status = array_var()->read(dataset, error))) 
        return status;

    // read maps elements
    for (Pix p = first_map_var(); p; next_map_var(p)){
      if(!(status = map_var(p)->read(dataset, error)))
            break;
      }
    if( status ) set_read_p(true);
    return status;

}



// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFGrid.cc,v $
// Revision 1.7  1999/05/27 17:02:22  jimg
// Merge with alpha-3-0-0
//
// Revision 1.6.2.1  1999/05/20 21:41:07  edavis
// Fix spelling of COPYRIGHT and remove some #if 0 stuff.
//
// Revision 1.6  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.5  1998/08/14 19:41:23  jimg
// Removed unused bool variable.
//
// Revision 1.4  1998/08/13 20:24:26  jimg
// Fixed read mfunc semantics
//
// Revision 1.3  1998/04/21 17:13:50  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:08  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] not_used ={"$Id: FFGrid.cc,v 1.7 1999/05/27 17:02:22 jimg Exp $"};

#include "FFGrid.h"

Grid *
NewGrid(const string &n)
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

FFGrid::FFGrid(const string &n) : Grid(n)
{
}

FFGrid::~FFGrid()
{
}

bool
FFGrid::read(const string &dataset, int &error)
{
    if (read_p()) // nothing to do
        return false;

    // read array elements
    array_var()->read(dataset, error);
    if (error) 
        return false;

    // read maps elements
    for (Pix p = first_map_var(); p; next_map_var(p)) {
	map_var(p)->read(dataset, error);
	if (error)
	    return false;
    }

    set_read_p(true);

    return false;
}


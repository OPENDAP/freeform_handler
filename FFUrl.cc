
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFUrl.cc,v $
// Revision 1.3  1998/04/21 17:14:05  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:21  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFUrl.cc,v 1.3 1998/04/21 17:14:05 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include "FFUrl.h"

Url *
NewUrl(const String &n)
{
    return new FFUrl(n);
}

FFUrl::FFUrl(const String &n) : Url(n)
{
}

BaseType *
FFUrl::ptr_duplicate()
{
    return new FFUrl(*this);
}

bool
FFUrl::read(const String &, int &)
{
    return true;
}



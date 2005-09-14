
// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

#include "config_ff.h"

static char rcsid[] not_used = {"$Id$"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <string>

#include "InternalErr.h"
#include "FFStructure.h"

Structure *
NewStructure(const string &n)
{
    return new FFStructure(n);
}

BaseType *
FFStructure::ptr_duplicate()
{
    return new FFStructure(*this);
}

FFStructure::FFStructure(const string &n) : Structure(n)
{
}

FFStructure::~FFStructure()
{
}

bool
FFStructure::read(const string &)
{
  throw InternalErr(__FILE__, __LINE__, "Unimplemented read methods called.");
}

// $Log: FFStructure.cc,v $
// Revision 1.6  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.5  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.4  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.3.12.1  1999/05/01 04:40:29  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.3  1998/04/21 17:14:02  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:18  jimg
// Sequence support added by Reza


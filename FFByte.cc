
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

#include "FFByte.h"
#include "util_ff.h"
#include "util.h"

extern long BufPtr;
extern char * BufVal;

// This `helper functions' creates a pointer to the a FFByte and returns
// that pointer. It takes the same arguments as the class's ctor. If any of
// the variable classes are subclassed (e.g., to make a new Byte like
// HDFByte) then the corresponding function here, and in the other class
// definition files, needs to be changed so that it creates an instnace of
// the new (sub)class. Continuing the earlier example, that would mean that
// NewByte() would return a HDFByte, not a Byte.
//
// It is important that these function's names and return types do not change
// - they are called by the parser code (for the dds, at least) so if their
// names changes, that will break.
//
// The declarations for these fuctions (in util.h) should *not* need
// changing. 

Byte *
NewByte(const string &n)
{
    return new FFByte(n);
}

FFByte::FFByte(const string &n) : Byte(n)
{
}

BaseType *
FFByte::ptr_duplicate()
{
    return new FFByte(*this);
}

bool
FFByte::read(const string &dataset)
{
    if (read_p()) // nothing to do
	return false;

    if (BufVal){ // Data in cache
	char * ptr = BufVal+BufPtr;
	val2buf((void *) ptr);
	set_read_p(true);

	BufPtr += width();
	return false;
    }
    else {

	return false;
    }
}

// $Log: FFByte.cc,v $
// Revision 1.10  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.9.2.1  2002/12/18 23:30:42  pwest
// gcc3.2 compile corrections, mainly regarding the using statement
//
// Revision 1.9  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.8  1999/05/27 17:02:22  jimg
// Merge with alpha-3-0-0
//
// Revision 1.7.2.1  1999/05/20 21:39:27  edavis
// Fix spelling of COPYRIGHT and remove some #if 0 stuff.
//
// Revision 1.7  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.6  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.5.12.1  1999/05/01 04:40:29  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.5  1998/08/13 20:24:21  jimg
// Fixed read mfunc semantics
//
// Revision 1.4  1998/08/12 21:20:51  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:13:43  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:00  jimg
// Sequence support added by Reza

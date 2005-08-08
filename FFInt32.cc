
// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

#include "config_ff.h"

static char rcsid[] not_used = {"$Id$"};

#ifdef __GNUG__
#pragma implementation
#endif

#include "FFInt32.h"
#include "util_ff.h"
#include "util.h"

extern long BufPtr;
extern char *BufVal;

Int32 *
NewInt32(const string &n)
{
    return new FFInt32(n);
}

FFInt32::FFInt32(const string &n) : Int32(n)
{
}

BaseType *
FFInt32::ptr_duplicate(){

    return new FFInt32(*this);
}

bool
FFInt32::read(const string &dataset)
{
    if (read_p()) // nothing to do
	return false;

    if (BufVal) { // data in cache
	char * ptr = BufVal+BufPtr;

	dods_int32 align;
	memcpy((void*)&align, (void *)ptr, width());

	val2buf((void *) &align);
	set_read_p(true);

#ifdef TEST
	dods_int32 *tmo;
	tmo = (dods_int32 *)ptr;
	printf("\n%ld offset=%ld\n",*tmo,BufPtr);
#endif 
	BufPtr += width();
	return false;
    }
    else {
	return false;
    }
}

// $Log: FFInt32.cc,v $
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
// Revision 1.8  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.7  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.6.12.1  1999/05/01 04:40:29  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.6  1998/08/31 04:06:01  reza
// Added String support.
// Fixed data alignment problem (64-bit Architectures).
// Removed Warnings and added a check for file existence.
// Updated FFND to fix a bug in stride.
//
// Revision 1.5  1998/08/13 20:24:28  jimg
// Fixed read mfunc semantics
//
// Revision 1.4  1998/08/12 21:20:55  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:13:53  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:10  jimg
// Sequence support added by Reza

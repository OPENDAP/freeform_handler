
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFInt32.cc,v $
// Revision 1.7  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
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

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFInt32.cc,v 1.7 1999/03/26 20:03:31 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include "FFInt32.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;

Int32 *
NewInt32(const String &n)
{
    return new FFInt32(n);
}

FFInt32::FFInt32(const String &n) : Int32(n)
{
}

BaseType *
FFInt32::ptr_duplicate(){

    return new FFInt32(*this);
}

bool
FFInt32::read(const String &dataset, int &error)
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

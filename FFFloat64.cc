
// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFFloat64.cc,v $
// Revision 1.8  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.7  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.6.12.1  1999/05/01 04:40:29  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.6  1998/08/31 04:06:00  reza
// Added String support.
// Fixed data alignment problem (64-bit Architectures).
// Removed Warnings and added a check for file existence.
// Updated FFND to fix a bug in stride.
//
// Revision 1.5  1998/08/13 20:24:23  jimg
// Fixed read mfunc semantics
//
// Revision 1.4  1998/08/12 21:20:53  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:13:46  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:03  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] not_used = {"$Id: FFFloat64.cc,v 1.8 1999/05/04 02:55:36 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <string>

#include "FFFloat64.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;

Float64 *
NewFloat64(const string &n)
{
    return new FFFloat64(n);
}

FFFloat64::FFFloat64(const string &n) : Float64(n)
{
}

BaseType *
FFFloat64::ptr_duplicate()
{
    return new FFFloat64(*this); // Copy ctor calls duplicate to do the work
}
 
bool
FFFloat64::read(const string &dataset, int &error)
{
    if (read_p()) // nothing to do
	return false;
  
    if(BufVal){ // data in cache
	char * ptr = BufVal+BufPtr;

	dods_float64 align;
	memcpy((void*)&align, (void *)ptr, width());

	val2buf((void *) &align);
	set_read_p(true);

	BufPtr += width();
	return false;
    }
    else {
	return false;
    }
}










// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors:
//      ReZa       Reza Nekovei (reza@intcomm.net)

#include "config_ff.h"

static char rcsid[] not_used = {"$Id: FFUInt32.cc,v 1.10 2001/09/28 23:19:43 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include "FFUInt32.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;

UInt32 *
NewUInt32(const string &n)
{
    return new FFUInt32(n);
}

FFUInt32::FFUInt32(const string &n) : UInt32(n)
{
}

BaseType *
FFUInt32::ptr_duplicate(){

    return new FFUInt32(*this);
}

bool
FFUInt32::read(const string &dataset)
{
    if (read_p()) // nothing to do
	return false;

    // *** I don't know why no error is signaled when there's no data in
    // BufVal. 10/11/2000 jhrg

    if(BufVal){ // data in cache
	char * ptr = BufVal+BufPtr;

	dods_uint32 align;
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

// $Log: FFUInt32.cc,v $
// Revision 1.10  2001/09/28 23:19:43  jimg
// Merged with 3.2.3.
//
// Revision 1.9.2.1  2001/05/23 18:14:53  jimg
// Merged with changes on the release-3-1 branch. This apparently was not
// done corrrectly the first time around.
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
// Revision 1.6  1998/08/31 04:06:06  reza
// Added String support.
// Fixed data alignment problem (64-bit Architectures).
// Removed Warnings and added a check for file existence.
// Updated FFND to fix a bug in stride.
//
// Revision 1.5  1998/08/13 20:24:34  jimg
// Fixed read mfunc semantics
//
// Revision 1.4  1998/08/12 21:21:00  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:14:04  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:20  jimg
// Sequence support added by Reza

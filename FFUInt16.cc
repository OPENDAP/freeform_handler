
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: James Gallagher

#include "config_ff.h"

static char rcsid[] not_used ={"$Id$"};

#ifdef __GNUG__
#pragma implementation
#endif

#include "FFUInt16.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;

UInt16 *
NewUInt16(const string &n)
{
    return new FFUInt16(n);
}

FFUInt16::FFUInt16(const string &n) : UInt16(n)
{
}

BaseType *
FFUInt16::ptr_duplicate(){

    return new FFUInt16(*this);
}

bool
FFUInt16::read(const string &dataset)
{
    if (read_p()) // nothing to do
	return false;

    if(BufVal){ // data in cache
	char * ptr = BufVal+BufPtr;

	dods_uint16 align;
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

// $Log: FFUInt16.cc,v $
// Revision 1.2  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.1  1999/05/19 17:52:41  jimg
// Added
//

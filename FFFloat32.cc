
// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: James Gallagher

#include "config_ff.h"

static char rcsid[] not_used ={"$Id: FFFloat32.cc,v 1.3 2003/02/10 23:01:52 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include "FFFloat32.h"
#include "util_ff.h"
#include "util.h"

extern long BufPtr;
extern char *BufVal;

Float32 *
NewFloat32(const string &n)
{
    return new FFFloat32(n);
}

FFFloat32::FFFloat32(const string &n) : Float32(n)
{
}

BaseType *
FFFloat32::ptr_duplicate()
{
    return new FFFloat32(*this); // Copy ctor calls duplicate to do the work
}
 
bool
FFFloat32::read(const string &dataset)
{
    if (read_p()) // nothing to do
	return false;
  
    if(BufVal){ // data in cache
	char * ptr = BufVal+BufPtr;

	dods_float32 align;
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

// $Log: FFFloat32.cc,v $
// Revision 1.3  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.2.2.1  2002/12/18 23:30:42  pwest
// gcc3.2 compile corrections, mainly regarding the using statement
//
// Revision 1.2  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.1  1999/05/19 17:52:41  jimg
// Added
//


// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: James Gallagher

// $Log: FFFloat32.cc,v $
// Revision 1.1  1999/05/19 17:52:41  jimg
// Added
//

#include "config_ff.h"

static char rcsid[] not_used ={"$Id: FFFloat32.cc,v 1.1 1999/05/19 17:52:41 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "FFFloat32.h"
#include "util_ff.h"

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
FFFloat32::read(const string &dataset, int &error)
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










// (c) COPYRIGHT URI/MIT 1999
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: James Gallagher

// $Log: FFInt16.cc,v $
// Revision 1.1  1999/05/19 17:52:41  jimg
// Added
//

#include "config_ff.h"

static char rcsid[] not_used ={"$Id: FFInt16.cc,v 1.1 1999/05/19 17:52:41 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include "FFInt16.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;

Int16 *
NewInt16(const string &n)
{
    return new FFInt16(n);
}

FFInt16::FFInt16(const string &n) : Int16(n)
{
}

BaseType *
FFInt16::ptr_duplicate(){

    return new FFInt16(*this);
}

bool
FFInt16::read(const string &dataset, int &error)
{
    if (read_p()) // nothing to do
	return false;

    if (BufVal) { // data in cache
	char * ptr = BufVal+BufPtr;

	dods_int16 align;
	memcpy((void*)&align, (void *)ptr, width());

	val2buf((void *) &align);
	set_read_p(true);

#ifdef TEST
	dods_int16 *tmo;
	tmo = (dods_int16 *)ptr;
	printf("\n%ld offset=%ld\n",*tmo,BufPtr);
#endif 
	BufPtr += width();
	return false;
    }
    else {
	return false;
    }
}
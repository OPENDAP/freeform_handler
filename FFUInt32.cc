
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors:
//      ReZa       Reza Nekovei (reza@intcomm.net)

// $Log: FFUInt32.cc,v $
// Revision 1.7  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
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

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFUInt32.cc,v 1.7 1999/03/26 20:03:31 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include "FFUInt32.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;

UInt32 *
NewUInt32(const String &n)
{
    return new FFUInt32(n);
}

FFUInt32::FFUInt32(const String &n) : UInt32(n)
{
}

BaseType *
FFUInt32::ptr_duplicate(){

    return new FFUInt32(*this);
}

bool
FFUInt32::read(const String &dataset, int &error)
{
    if (read_p()) // nothing to do
	return false;

    if(BufVal){ // data in cache
	char * ptr = BufVal+BufPtr;

	dods_uint32 align;
	memcpy((void*)&align, (void *)ptr, width());

	val2buf((void *) &align);
	set_read_p(true);

	BufPtr += width();    
	return false;
    }
    else{
	char *ds = new char[dataset.length() + 1];
	strcpy(ds, dataset);

	String o_format = make_output_format(name(), type(), width());
	char *o_f = new char[o_format.length() + 1];
	strcpy(o_f, o_format);

	String i_format_file = find_ancillary_file(dataset);
	char *if_f = new char[i_format_file.length() + 1];
	strcpy(if_f, i_format_file);

	dods_uint32 *i = new dods_uint32[width() + 1];
	long bytes = read_ff(ds, if_f, o_f, (char *)i, width()+1);

	if (bytes == -1){
	    error = 1;
	}
	else {
	    set_read_p(true);
	    val2buf(i);     
	}

	if (i)
	    delete(i);

	delete [] ds;
	delete [] o_f;
	delete [] if_f;
  
	return false;
    }
}



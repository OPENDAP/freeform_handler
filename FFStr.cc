
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFStr.cc,v $
// Revision 1.4  1998/08/12 21:20:58  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:14:00  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:16  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFStr.cc,v 1.4 1998/08/12 21:20:58 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include "FFStr.h"

extern long BufPtr;
extern char * BufVal;

// extern long Get_constraint(int num_dim,String constraint,long *cor,long *edge);


Str *
NewStr(const String &n)
{
    return new FFStr(n);
}

FFStr::FFStr(const String &n) : Str(n)
{
}

BaseType *
FFStr::ptr_duplicate()
{
    return new FFStr(*this);
}

bool
FFStr::read(const String &dataset, int &)
{
  if (read_p()) // nothing to do
    return true;

  if (BufVal){ // Data in cache
    char * ptr = BufVal+BufPtr;
    String *Nstr = new String((const char *)ptr);
    val2buf(Nstr);
    set_read_p(true);

    BufPtr += Nstr->length()+1;
    return true;
  }
  else 
    return false;

}

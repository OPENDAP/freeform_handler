
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
// Revision 1.5  1998/08/31 04:06:04  reza
// Added String support.
// Fixed data alignment problem (64-bit Architectures).
// Removed Warnings and added a check for file existence.
// Updated FFND to fix a bug in stride.
//
// Revision 1.4  1998/08/12 21:20:58  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:14:00  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:16  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFStr.cc,v 1.5 1998/08/31 04:06:04 reza Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>
#include <ctype.h>
#include <strings.h>
#include "FFStr.h"

extern long BufPtr;   // set by read functions
extern char * BufVal; // set by first call to sequence
extern int StrLength; // Set by sequence before String read 

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

    char *TmpBuf = new char[StrLength+1];
    int i, j;

    //remove trailing white space
    for(i=StrLength-1; i>=0; i--)
      if(!isspace(*(ptr+i)))
	break;

    //remove leading white space
    for(j=0; j<i; j++)
      if(!isspace(*(ptr+j)))
	break;

    strncpy(TmpBuf, ptr+j,i-j+1);
    TmpBuf[i-j+1]='\0';

    String *Nstr = new String((const char *)TmpBuf);

    val2buf(Nstr);
    set_read_p(true);

    BufPtr += StrLength;
    return true;
  }
  else 
    return false;

}

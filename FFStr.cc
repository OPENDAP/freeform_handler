
// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

#include "config_ff.h"

static char rcsid[] not_used = {"$Id: FFStr.cc,v 1.9 2000/10/11 19:37:56 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <strings.h>

#include <ctype.h>
#include <string>

#include "FFStr.h"

extern long BufPtr;   // set by read functions
extern char * BufVal; // set by first call to sequence
extern int StrLength; // Set by sequence before String read 

Str *
NewStr(const string &n)
{
    return new FFStr(n);
}

FFStr::FFStr(const string &n) : Str(n)
{
}

BaseType *
FFStr::ptr_duplicate()
{
    return new FFStr(*this);
}

bool
FFStr::read(const string &dataset)
{
    if (read_p())			// nothing to do
	return false;

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

	// Fix me; use _buf directly. 10/19/98 jhrg
	string *Nstr = new string((const char *)TmpBuf);

	val2buf(Nstr);
	set_read_p(true);

	BufPtr += StrLength;
	return false;
    }
    else 
	return false;
}

// $Log: FFStr.cc,v $
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
// Revision 1.6.8.1  1999/05/01 04:40:29  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.6  1998/11/13 05:43:08  jimg
// Formatting changes
//
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

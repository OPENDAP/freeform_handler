
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of ff_handler a FreeForm API handler for the OPeNDAP
// DAP2 data server.

// Copyright (c) 2005 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
// 
// This software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

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

static char rcsid[] not_used = {"$Id$"};


#ifndef WIN32
#include <strings.h>
#endif

#include <ctype.h>
#include <string>

#include "FFStr.h"
#include "util.h"

extern long BufPtr;   // set by read functions
extern char * BufVal; // set by first call to sequence
extern int StrLength; // Set by sequence before String read 

FFStr::FFStr(const string &n) : Str(n)
{
}

BaseType *
FFStr::ptr_duplicate()
{
    return new FFStr(*this);
}

bool
FFStr::read(const string &)
{
    if (read_p())			// nothing to do
	return false;

    if (BufVal){ // Data in cache
	char * ptr = BufVal+BufPtr;

	char *TmpBuf = new char[StrLength+1];
#if 1
	// This code prunes both trailing and leading spaces from strings.
	// Spaces are often added to URLs in file server data sets since the
	// URL length can vary but in FF a field is a fixed size. However, if
	// you want the FF handler to return _exactly_ the string data, this
	// should be turned off. Once the subject of much debate... jhrg

	int i,j;

	//remove trailing white space
	for(i=StrLength-1; i>=0; i--)
	    if(!isspace(*(ptr+i)))
		break;

	//remove leading white space
	for(j=0; j<i; j++)
	    if(!isspace(*(ptr+j)))
		break;
#else
	int i=StrLength-1;;
	int j=0;
#endif

	strncpy(TmpBuf, ptr+j,i-j+1);
	TmpBuf[i-j+1]='\0';

	string *Nstr = new string((const char *)TmpBuf);
	delete [] TmpBuf;

	val2buf(Nstr);
	delete Nstr;
	
	set_read_p(true);

	BufPtr += StrLength;
	return false;
    }
    else 
	return false;
}

// $Log: FFStr.cc,v $
// Revision 1.11  2004/07/09 17:54:24  jimg
// Merged with release-3-4-3FCS.
//
// Revision 1.10.4.3  2004/03/07 22:05:51  rmorris
// Final code changes to port the freeform server to win32.
//
// Revision 1.10.4.2  2003/09/06 23:33:14  jimg
// I modified the read() method implementations so that they test the new
// in_selection property. If it is true, the methods will read values
// even if the send_p property is not true. This is so that variables used
// in the selection part of the CE, or as function arguments, will be read.
// See bug 657.
//
// Revision 1.10.4.1  2003/07/24 03:23:15  dan
// Rollback to usage where FFserver strips leading whitespace from strings.
//
// Revision 1.10  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.9.2.2  2002/12/18 23:30:42  pwest
// gcc3.2 compile corrections, mainly regarding the using statement
//
// Revision 1.9.2.1  2002/08/05 16:59:06  dan
// Updated FFStr::read() such that it no longer strips
// leading and trailing whitespace from string variables.
// CAUTION, this new behavior radically changes the behavior
// of string variables returned from the ff-dods server.
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


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

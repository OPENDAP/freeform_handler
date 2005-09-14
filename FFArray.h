// -*- C++ -*-

// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGHT.  
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

#ifndef _ffarray_h
#define _ffarray_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include <string>

#include "Array.h"

class FFArray: public Array {
private:
    long Seq_constraint(long *cor, long *step, long *edg, bool *has_stride);
    long Arr_constraint(long *cor, long *step, long *edg, string *dim_nms,
			bool *has_stride);

    /** Read an array of simple types into this objects #_buf# memeber. */
    template <class T> bool extract_array(char *ds, char *if_fmt, 
					  char *o_fmt);

public:
    FFArray(const string &n = "", BaseType *v = 0);
    virtual ~FFArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

// $Log: FFArray.h,v $
// Revision 1.10  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.9  2001/09/28 23:19:43  jimg
// Merged with 3.2.3.
//
// Revision 1.8.2.2  2002/12/18 23:30:42  pwest
// gcc3.2 compile corrections, mainly regarding the using statement
//
// Revision 1.8.2.1  2001/05/23 18:14:53  jimg
// Merged with changes on the release-3-1 branch. This apparently was not
// done corrrectly the first time around.
//
// Revision 1.8  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.7.4.1  1999/08/28 01:19:05  jimg
// Changed the extract_array declaration from `template <class t> bool
// extract_array<T>(...)' to template <class t> bool extract_array(...).
// I.E.: I removed the second <T> which was allowed by gcc 2.8.1 but was, in
// fact, not legal C++.
//
// Revision 1.7  1999/05/27 17:02:22  jimg
// Merge with alpha-3-0-0
//
// Revision 1.6.2.1  1999/05/20 21:38:29  edavis
// Fix spelling of COPYRIGHT and remove some #if 0 stuff.
//
// Revision 1.6  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.5  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.4.12.1  1999/05/01 04:40:30  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.4  1998/08/12 21:20:50  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:13:42  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:10:59  jimg
// Sequence support added by Reza

#endif

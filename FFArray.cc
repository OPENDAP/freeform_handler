
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

static char rcsid[] not_used ={"$Id$"};


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <iostream>
#include <string>

#include "dods-datatypes.h"
#include "Error.h"
#include "InternalErr.h"

#include "FFArray.h"
#include "util_ff.h"
#include "util.h"

BaseType *
FFArray::ptr_duplicate()
{
    return new FFArray(*this);
}

FFArray::FFArray(const string &n, const string &d, BaseType *v) : Array(n, d, v)
{
}

FFArray::~FFArray()
{
}

// parse constraint expr. and make coordinate point location for an array.
// return number of elements to read. 

long
FFArray::Arr_constraint(long *cor, long *step, long *edg, string *dim_nms, 
			bool *has_stride)
{
    long start, stride, stop;

    int id = 0;
    long nels = 1;
    *has_stride = false;

    Array::Dim_iter i = dim_begin();
    while (i != dim_end()) {
        start = (long) dimension_start(i, true); 
        stride = (long) dimension_stride(i, true);
        stop = (long) dimension_stop(i, true);
        string dimname = dimension_name(i);

	// Check for empty constraint
	if(start+stop+stride == 0)
	    return -1;
	
	dim_nms[id]= dimname;
	//	(void) strcpy(dim_nms[id], dimname.c_str());
	
	cor[id]= start;
	step[id]= stride;
	edg[id]= ((stop - start)/stride) + 1; // count of elements
	
	nels *= edg[id];      // total number of values for variable
	
	if (stride != 1)
	    *has_stride = true;

        ++id;
        ++i;
    }
    return nels;
}

// parse constraint expr. and make coordinate point location.
// return number of elements to read. 

long
FFArray::Seq_constraint(long *cor, long *step, long *edg, bool *has_stride)
{
    int start, stride, stop;
    int id = 0;
    long nels = 1;
    *has_stride = false;

    Array::Dim_iter i = dim_begin();
    while (i != dim_end()) {
        start = (long) dimension_start(i, true); 
        stride = (long) dimension_stride(i, true);
        stop = (long) dimension_stop(i, true);

      // Check for empty constraint
      if(start+stop+stride == 0)
	return -1;

      cor[id]= start;
      step[id]= stride;
      edg[id]= ((stop - start)/stride) + 1; // count of elements
      nels *= edg[id];      // total number of values for variable
      if (stride != 1)
	*has_stride = true;

      ++id;
      ++i;
    }
    return nels;
}


static int
hyper_get(void *dest, void *src, unsigned szof, const int dim_num, int index, 
          const int dimsz[], const long start[], const long edge[])
{
    long jump = 1;
    
    // The THEN part of this IF handles the cases where we are hyperslabbing
    // any dimension *other* than the rightmost dimension. E.G. Suppose
    // a[10][10][10] is in SRC, for INDEX == 0 and 1 we do the code in the
    // THEN clause and for INDEX == 2 the ELSE clause is executed.  

    // NOTE: I have added casts for src and dest from void * to char * since
    // ANSI C++ won't allows pointer arithmetic on void * variables. 4/17/98
    // jhrg 

    if(dim_num != (index+1)) {
        // number of lines, pages, etc to skip 
        for (int i = dim_num-1; i > index; i--)
            jump *= dimsz[i];

        for (int edge_tmp = 0; edge_tmp < edge[index] ; edge_tmp++){
            void *srctmp = ((char *)src + (start[index] + edge_tmp) 
			    * jump * szof);
            dest = (char *)dest + hyper_get(dest, srctmp, szof, dim_num,
					    index+1, dimsz, start, edge);
        }
        return (0);
    }
    else {
        // number of bytes to jump
        void *srctmp = (char *)src + start[index] * szof ;
        memcpy(dest, srctmp, (size_t)edge[index]*szof);
        return (edge[index] * szof);
    }
}


// Store the contents of the buffer returned from read_ff() in the FFArray
// object ARRAY. 
//
// Returns: void

template < class T > 
static void 
seq2vects(T * t, FFArray & array)
{
    bool has_stride;
    int ndim = array.dimensions();
    long *start = new long[ndim];
    long *stride = new long[ndim];
    long *edge = new long[ndim];

    long count = array.Seq_constraint(start, stride, edge, &has_stride);

    if (count != -1) {          // non-null hyperslab
        T *t_hs = new T[count];
        int *dimsz = new int[array.dimensions()];

        int i = 0;
        Array::Dim_iter p = array.dim_begin();
        while (p != array.dim_end()) {
            dimsz[i] = array.dimension_size(p);
            ++i;
            ++p;
        }

        hyper_get(t_hs, t, array.var()->width(), ndim, 0, dimsz, start, edge);

        array.set_read_p(true);         // reading is done
        array.val2buf((void *) t_hs);   // put values in the buffer

        delete[]t_hs;
        delete[]dimsz;
    } else {
        array.set_read_p(true);
        array.val2buf((void *) t);
    }

    delete[]start;
    delete[]stride;
    delete[]edge;
}

// Read cardinal types and ctor types separately. Cardinal types are
// stored in arrays of the C++ data type while ctor types are stored
// in arrays of the DODS classes used to store those types.
//
// NB: Currently this code only reads arrays of Byte, Int32 and
// Float64. Str and Url as well as all the ctor  types are not
// supported. 
//
// Throws an Error object if an error was detected. 
// Returns true if more data still needs to be read, otherwise returns false.

bool
FFArray::read()
{    
    if (read_p())  // Nothing to do
        return false;

    bool has_stride;
    int ndims = dimensions();
    string *dname = new string[ndims];	       
    long *start = new long[ndims];
    long *stride = new long[ndims];
    long *edge = new long[ndims];    
    long count = Arr_constraint(start, stride, edge, dname, &has_stride);    

    if (!count) {
        delete[] dname;

        delete[] start;
        delete[] stride;
        delete[] edge;

        throw Error(unknown_error, "Constraint returned an empty dataset.");
    }

    string output_format =
      makeND_output_format(name(), var()->type(), var()->width(), 
			   ndims, start, edge, stride, dname);

    string input_format_file = find_ancillary_file(dataset());

    // For each cardinal-type variable, do the following:
    //     Use ff to read the data
    //     Store the (possibly constrained) data
    //     NB: extract_array throws an Error object to signal problems.
    
    switch(var()->type()) {
      case dods_byte_c:
	extract_array<dods_byte>(dataset(), input_format_file, output_format);
	break;

      case dods_int16_c:
	extract_array<dods_int16>(dataset(), input_format_file, output_format);
	break;

      case dods_uint16_c:
	extract_array<dods_uint16>(dataset(), input_format_file, output_format);
	break;

      case dods_int32_c:
	extract_array<dods_int32>(dataset(), input_format_file, output_format);
	break;

      case dods_uint32_c:
	extract_array<dods_uint32>(dataset(), input_format_file, output_format);
	break;

      case dods_float32_c:
	extract_array<dods_float32>(dataset(), input_format_file, output_format);
	break;

      case dods_float64_c:
	extract_array<dods_float64>(dataset(), input_format_file, output_format);
	break;

      default:
        delete[] dname;
        delete[] start;
        delete[] stride;
        delete[] edge;
    
	throw InternalErr(__FILE__, __LINE__,
			  (string)"FFArray::read: Unsupported array type "
			  + var()->type_name() + ".");
    }

    // clean up
    delete[] dname;
    delete[] start;
    delete[] stride;
    delete[] edge;

    return false;
}

// This template reads arrays of simple types into the Array object's _buf
// memeber. It returns true if successful, false otherwise.

template <class T>
bool
FFArray::extract_array(const string &ds, const string &if_fmt, 
                       const string &o_fmt)
{
    T *d = (T *)new char[width()];
    long bytes = read_ff(ds.c_str(), if_fmt.c_str(), o_fmt.c_str(), 
                         (char *)d, width());

    if (bytes == -1) {
      throw Error(unknown_error, "Could not read values from the dataset.");
    }
    else {
	set_read_p(true);
	val2buf((void *) d);
    }

    if (d)
	delete[] (d);

    return true;
}

// $Log: FFArray.cc,v $
// Revision 1.13  2003/02/10 23:01:52  jimg
// Merged with 3.2.5
//
// Revision 1.12.2.1  2002/12/18 23:30:42  pwest
// gcc3.2 compile corrections, mainly regarding the using statement
//
// Revision 1.12  2000/10/11 19:37:56  jimg
// Moved the CVS log entries to the end of files.
// Changed the definition of the read method to match the dap library.
// Added exception handling.
// Added exceptions to the read methods.
//
// Revision 1.11  2000/08/31 22:16:55  jimg
// Merged with 3.1.7
//
// Revision 1.10.2.2  2000/08/03 20:18:57  jimg
// Removed config_dap.h and replaced it with config_ff.h (in *.cc files;
// neither should be included in a header file).
// Changed code that calculated leap year information so that it uses the
// functions in date_proc.c/h.
//
// Revision 1.10.2.1  1999/08/28 01:18:53  jimg
// Changed the extract_array declaration from `template <class t> bool
// extract_array<T>(...)' to template <class t> bool extract_array(...).
// I.E.: I removed the second <T> which was allowed by gcc 2.8.1 but was, in
// fact, not legal C++.
//
// Revision 1.10  1999/07/22 21:28:09  jimg
// Merged changes from the release-3-0-2 branch
//
// Revision 1.9.6.1  1999/06/07 17:32:18  edavis
// Changed 'data()' to 'c_str()'.
//
// Revision 1.9  1999/05/04 02:55:36  jimg
// Merge with no-gnu
//
// Revision 1.8  1999/03/26 20:03:31  jimg
// Added support for the Int16, UInt16 and Float32 datatypes
//
// Revision 1.7.8.1  1999/05/01 04:40:29  brent
// converted old String.h to the new std C++ <string> code
//
// Revision 1.7  1998/11/10 19:23:01  jimg
// Minor formatting changes...
//
// Revision 1.6  1998/08/31 04:05:58  reza
// Added String support.
// Fixed data alignment problem (64-bit Architectures).
// Removed Warnings and added a check for file existence.
// Updated FFND to fix a bug in stride.
//
// Revision 1.5  1998/08/13 20:24:20  jimg
// Fixed read mfunc semantics
//
// Revision 1.4  1998/08/12 21:20:49  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:13:41  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:10:58  jimg
// Sequence support added by Reza

/*
  Copyright 1997 The University of Rhode Island and The Massachusetts
  Institute of Technology.

  Portions of this software were developed by the Graduate School of
  Oceanography (GSO) at the University of Rhode Island (URI) in collaboration
  with The Massachusetts Institute of Technology (MIT).

  Access and use of this software shall impose the following obligations and
  understandings on the user. The user is granted the right, without any fee
  or cost, to use, copy, modify, alter, enhance and distribute this software,
  and any derivative works thereof, and its supporting documentation for any
  purpose whatsoever, provided that this entire notice appears in all copies
  of the software, derivative works and supporting documentation.  Further,
  the user agrees to credit URI/MIT in any publications that result from the
  use of this software or in any product that includes this software. The
  names URI, MIT and/or GSO, however, may not be used in any advertising or
  publicity to endorse or promote any products or commercial entity unless
  specific written permission is obtained from URI/MIT. The user also
  understands that URI/MIT is not obligated to provide the user with any
  support, consulting, training or assistance of any kind with regard to the
  use, operation and performance of this software nor to provide the user
  with any updates, revisions, new versions or "bug fixes".

  THIS SOFTWARE IS PROVIDED BY URI/MIT "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL URI/MIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
  DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
  PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS
  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE ACCESS, USE OR PERFORMANCE
  OF THIS SOFTWARE.
*/

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFArray.cc,v $
// Revision 1.1  1997/10/03 17:01:44  jimg
// Initial version from Reza
//
//

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFArray.cc,v 1.1 1997/10/03 17:01:44 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>

#include <assert.h>

#include "config_dap.h"

#include "FFArray.h"
#include "util_ff.h"

Array *
NewArray(const String &n, BaseType *v)
{
    return new FFArray(n, v);
}

BaseType *
FFArray::ptr_duplicate()
{
    return new FFArray(*this);
}

FFArray::FFArray(const String &n, BaseType *v) : Array(n, v)
{
}

FFArray::~FFArray()
{
}

// parse constraint expr. and make coordinate point location.
// return number of elements to read. 
long
FFArray::format_constraint(long *cor, long *step, long *edg, bool *has_stride)
{
    int start, stride, stop;
    int id = 0;
    long nels = 1;

    *has_stride = false;

    for (Pix p = first_dim(); p ; next_dim(p), id++) {
      start = dimension_start(p, true); 
      stride = dimension_stride(p, true);
      stop = dimension_stop(p, true);
      // Check for empty constraint
      if(start+stop+stride == 0)
	return -1;

      cor[id] = start;
      step[id] = stride;
      edg[id] = ((stop - start)/stride) + 1; // count of elements
      nels *= edg[id];      // total number of values for variable
      if (stride != 1)
	*has_stride = true;
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

    if(dim_num != (index+1)) {
        // number of lines, pages, etc to skip 
        for (int i = dim_num-1; i > index; i--)
            jump *= dimsz[i];

        for (int edge_tmp = 0; edge_tmp < edge[index] ; edge_tmp++){
            void *srctmp = ( src + ( start[index] + edge_tmp ) * jump * szof);
            dest += hyper_get(dest, srctmp, szof, dim_num, index+1, dimsz,
                              start, edge);
        }
        return (0);
    }
    else {
        // number of bytes to jump
        void *srctmp = src + start[index] * szof ;
        memcpy((void *) dest, (void *) srctmp, (size_t)edge[index]*szof);
        return (edge[index] * szof);
    }

}


// Store the contents of the buffer returned from read_ff() in the FFArray
// object ARRAY. 
//
// Returns: void

template <class T>
static void
save_raw_data(T *t, FFArray &array)
{  
  bool has_stride;
  int ndim = array.dimensions();
  long *start = new long[ndim];
  long *stride = new long[ndim];
  long *edge = new long[ndim];

  long count = array.format_constraint(start, stride, edge, &has_stride);  

  if (count != -1) {		// non-null hyperslab
    T *t_hs = new T[count];
    int *dimsz = new int[array.dimensions()];
    int i; Pix p;
    
    for (p = array.first_dim(), i = 0; p; array.next_dim(p), ++i)
      dimsz[i] = array.dimension_size(p);
    
    hyper_get(t_hs, t, array.var()->width(), ndim, 0, dimsz, start, edge);
    
    // reading is done (dont need to read each individual array value)
    array.set_read_p(true);  
    // put values in the buffers
    array.val2buf((void *)t_hs);
    
    delete[] t_hs;
    delete[] dimsz;
  }
  else {
    // reading is done (dont need to read each individual array value)
    array.set_read_p(true);  
    // put values in the buffers
    array.val2buf((void *)t);    
  }
    // clean up
    delete[] start;
    delete[] stride;
    delete[] edge;
}


// Read cardinal types and ctor types separately. Cardinal types are
// stored in arrays of the C++ data type while ctor types are stored
// in arrays of the DODS classes used to store those types.
//
// NB: Currently this code only reads arrays of Byte, Int32 and
// Float64. Str and Url as well as all the ctor  types are not
// supported. 
//
// Returns: False if an error was detected, True otherwise.

bool
FFArray::read(const String &dataset, int &)
{    
    if (read_p())  // Nothing to do
        return true;

    bool status = true;

    // make char * variables to hold String data for read_ff

    char *ds = new char[dataset.length() + 1];
    strcpy(ds, dataset);

    String output_format = make_output_format(name(), var()->type_name(), 
					      var()->width());
    char *o_f = new char[output_format.length() + 1];
    strcpy(o_f, output_format);

    String input_format_file = find_ancillary_file(dataset);
    char *if_f = new char[input_format_file.length() + 1];
    strcpy(if_f, input_format_file);

    // For each cadinal-type variable, do the following:
    //     Use ff to read the data
    //     Store the (possibly constrained) data

    if (var()->type_name() == "Byte") {
	dods_byte *b = (dods_byte *)new char[width()]; // `+1' hack for FreeForm
	long bytes = read_ff(ds, if_f, o_f, (char *)b, width());

	if (bytes == -1)
	    status = false;
	else 
	    save_raw_data(b, *this);

	if (b)
	    delete(b);
    }
    else if (var()->type_name() == "Int32") {
	dods_int32 *i = (dods_int32 *)new char[width()];
	long bytes = read_ff(ds, if_f, o_f, (char *)i, width());

	if (bytes == -1)
	    status = false;
	else
	    save_raw_data(i, *this);

	if (i)
	    delete(i);
    }
    else if (var()->type_name() == "UInt32") {
	dods_uint32 *ui = (dods_uint32 *)new char[width()];
	long bytes = read_ff(ds, if_f, o_f, (char *)ui, width());

	if (bytes == -1)
	    status = false;
	else
	    save_raw_data(ui, *this);

	if (ui)
	    delete(ui);
    }
    else if (var()->type_name() == "Float64") {
	dods_float64 *d = (dods_float64 *)new char[width()];
	long bytes = read_ff(ds, if_f, o_f, (char *)d, width());

	if (bytes == -1)
	    status = false;
	else
	    save_raw_data(d, *this);

	if (d)
	    delete(d);
    }
    else if (var()->type_name() == "Str" || var()->type_name() == "Url") {
	cerr << "FFArray::read: Unsupported array type " << var()->type_name()
	    << endl;
	status = false;
    }
    else {
	cerr << "FFArray::read: Unsupported array type " << var()->type_name()
	    << endl;
	status = false;
    }

    // clean up
    delete[] ds;		// delete temporary char * arrays
    delete[] o_f;
    delete[] if_f;

    return status;
}


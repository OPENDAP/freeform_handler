
// (c) COPYRIGHT URI/MIT 1997-99
// Please read the full copyright statement in the file COPYRIGHT.
//
// Authors: reza (Reza Nekovei)

// FreeFrom sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFArray.cc,v $
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

#include "config_ff.h"

static char rcsid[] not_used ={"$Id: FFArray.cc,v 1.9 1999/05/04 02:55:36 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iostream.h>

#include <assert.h>
#include <string>

#include "config_dap.h"

#include "FFArray.h"
#include "util_ff.h"

Array *
NewArray(const string &n, BaseType *v)
{
    return new FFArray(n, v);
}

BaseType *
FFArray::ptr_duplicate()
{
    return new FFArray(*this);
}

FFArray::FFArray(const string &n, BaseType *v) : Array(n, v)
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
    //    string dimname;

    *has_stride = false;

    for (Pix p = first_dim(); p ; next_dim(p), id++) {
	start = (long) dimension_start(p, true); 
	stride = (long) dimension_stride(p, true);
	stop = (long) dimension_stop(p, true);
	string dimname = dimension_name(p);

	// Check for empty constraint
	if(start+stop+stride == 0)
	    return -1;
	
	dim_nms[id] = dimname;
	//	(void) strcpy(dim_nms[id], dimname.data());
	
	cor[id] = start;
	step[id] = stride;
	edg[id] = ((stop - start)/stride) + 1; // count of elements
	
	nels *= edg[id];      // total number of values for variable
	
	if (stride != 1)
	    *has_stride = true;
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

template <class T>
static void
seq2vects(T *t, FFArray &array)
{  
  bool has_stride;
  int ndim = array.dimensions();
  long *start = new long[ndim];
  long *stride = new long[ndim];
  long *edge = new long[ndim];

  long count = array.Seq_constraint(start, stride, edge, &has_stride);  

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
FFArray::read(const string &dataset, int &error)
{    
    if (read_p())  // Nothing to do
        return false;

    // make char * variables to hold string data for read_ff
    char *ds = new char[dataset.size() + 1];
    strcpy(ds, dataset.data());

    // This was used for original Sequence to Array translation 
    // string output_format = make_output_format(name(), var()->type_name(), 
    //					      var()->width());
   
    bool has_stride;
    int ndims = dimensions();
    string *dname = new string[ndims];	       
    long *start = new long[ndims];
    long *stride = new long[ndims];
    long *edge = new long[ndims];    
    long count = Arr_constraint(start, stride, edge, dname, 
     				&has_stride);    

    if (!count) {
      cerr << "constraint returned empty dataset" << endl;
      error = 1;
      return false;
    }

    string output_format =
      makeND_output_format(name(), var()->type(), var()->width(), 
			   ndims, start, edge, stride, dname);

    char *o_fmt = new char[output_format.length() + 1];
    strcpy(o_fmt, output_format.c_str());

    string input_format_file = find_ancillary_file(dataset);
    char *if_fmt = new char[input_format_file.length() + 1];
    strcpy(if_fmt, input_format_file.c_str());

    // For each cardinal-type variable, do the following:
    //     Use ff to read the data
    //     Store the (possibly constrained) data

    switch(var()->type()) {
      case dods_byte_c:
	if (!extract_array<dods_byte>(ds, if_fmt, o_fmt)) {
	    error = 1;
	    return false;
	}
	break;

      case dods_int16_c:
	if (!extract_array<dods_int16>(ds, if_fmt, o_fmt)) {
	    error = 1;
	    return false;
	}
	break;

      case dods_uint16_c:
	if (!extract_array<dods_uint16>(ds, if_fmt, o_fmt)) {
	    error = 1;
	    return false;
	}
	break;

      case dods_int32_c:
	if (!extract_array<dods_int32>(ds, if_fmt, o_fmt)) {
	    error = 1;
	    return false;
	}
	break;

      case dods_uint32_c:
	if (!extract_array<dods_uint32>(ds, if_fmt, o_fmt)) {
	    error = 1;
	    return false;
	}
	break;

      case dods_float32_c:
	if (!extract_array<dods_float32>(ds, if_fmt, o_fmt)) {
	    error = 1;
	    return false;
	}
	break;

      case dods_float64_c:
	if (!extract_array<dods_float64>(ds, if_fmt, o_fmt)) {
	    error = 1;
	    return false;
	}
	break;

      default:
	cerr << "FFArray::read: Unsupported array type " << var()->type_name()
	     << endl;
	error = 1;
	return false;
	break;
    }

    // clean up
    delete[] ds;		// delete temporary char * arrays
    delete[] o_fmt;
    delete[] if_fmt;

    delete[] start;
    delete[] stride;
    delete[] edge;

    return false;
}

// This template reads arrays of simple types into the Array object's _buf
// memeber. It returns true if successful, false otherwise.

template <class T>
bool
FFArray::extract_array<T>(char *ds, char *if_fmt, char *o_fmt)
{
    T *d = (T *)new char[width()];
    long bytes = read_ff(ds, if_fmt, o_fmt, (char *)d, width());

    if (bytes == -1) {
	return false;
    }
    else {
	//    seq2vects(d, *this);
	set_read_p(true);
	val2buf((void *) d);
    }

    if (d)
	delete(d);

    return true;
}

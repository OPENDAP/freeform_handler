
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFFloat64.cc,v $
// Revision 1.4  1998/08/12 21:20:53  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:13:46  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:03  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFFloat64.cc,v 1.4 1998/08/12 21:20:53 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "FFFloat64.h"
#include "util_ff.h"

extern long BufPtr;
extern char *BufVal;

Float64 *
NewFloat64(const String &n)
{
    return new FFFloat64(n);
}

FFFloat64::FFFloat64(const String &n) : Float64(n)
{
}

BaseType *
FFFloat64::ptr_duplicate()
{
    return new FFFloat64(*this); // Copy ctor calls duplicate to do the work
}
 
bool
FFFloat64::read(const String &dataset, int &)
{
  if (read_p()) // nothing to do
    return true;
  
  if(BufVal){ // data in cache
    char * ptr = BufVal+BufPtr;
    val2buf((void *) ptr);
    set_read_p(true);

    BufPtr += width();
    return true;
  }
  else{

    bool status = true;

    char *ds = new char[dataset.length() + 1];
    strcpy(ds, dataset);

    String o_format = make_output_format(name(), type_name(), width());
    char *o_f = new char[o_format.length() + 1];
    strcpy(o_f, o_format);

    String i_format_file = find_ancillary_file(dataset);
    char *if_f = new char[i_format_file.length() + 1];
    strcpy(if_f, i_format_file);

    dods_float64 *i = new dods_float64[width() + 1];
    long bytes = read_ff(ds, if_f, o_f, (char *)i, width()+1);

    if (bytes == -1){
        status = false;
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

    return status;
  }
}









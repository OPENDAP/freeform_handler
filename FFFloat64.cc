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

// FreeForm sub-class implementation for FFByte,...FFGrid.
// The files are patterned after the subcalssing examples 
// Test<type>.c,h files.
//
// ReZa 6/18/97

// $Log: FFFloat64.cc,v $
// Revision 1.1  1997/10/03 17:01:55  jimg
// Initial version from Reza
//
//

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFFloat64.cc,v 1.1 1997/10/03 17:01:55 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "FFFloat64.h"
#include "util_ff.h"


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









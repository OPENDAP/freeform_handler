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

// $Log: FFByte.cc,v $
// Revision 1.2  1998/04/16 18:11:00  jimg
// Sequence support added by Reza
//
//

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFByte.cc,v 1.2 1998/04/16 18:11:00 jimg Exp $"};

#ifdef __GNUG__
#pragma implementation
#endif

#include <assert.h>

#include "FFByte.h"
#include "util_ff.h"

// This `helper functions' creates a pointer to the a FFByte and returns
// that pointer. It takes the same arguments as the class's ctor. If any of
// the variable classes are subclassed (e.g., to make a new Byte like
// HDFByte) then the corresponding function here, and in the other class
// definition files, needs to be changed so that it creates an instnace of
// the new (sub)class. Continuing the earlier example, that would mean that
// NewByte() would return a HDFByte, not a Byte.
//
// It is important that these function's names and return types do not change
// - they are called by the parser code (for the dds, at least) so if their
// names changes, that will break.
//
// The declarations for these fuctions (in util.h) should *not* need
// changing. 

Byte *
NewByte(const String &n)
{
    return new FFByte(n);
}

FFByte::FFByte(const String &n) : Byte(n)
{
}

BaseType *
FFByte::ptr_duplicate()
{
    return new FFByte(*this);
}

bool
FFByte::read(const String &dataset, int &)
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

    dods_byte *i = new dods_byte[width() + 1];
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


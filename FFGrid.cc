/*
  Copyright 1995 The University of Rhode Island and The Massachusetts
  Institute of Technology

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

// $Log: FFGrid.cc,v $
// Revision 1.2  1998/04/16 18:11:08  jimg
// Sequence support added by Reza
//
//
//

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: FFGrid.cc,v 1.2 1998/04/16 18:11:08 jimg Exp $"};

#include "FFGrid.h"

Grid *
NewGrid(const String &n)
{
    return new FFGrid(n);
}

// protected

BaseType *
FFGrid::ptr_duplicate()
{
    return new FFGrid(*this);
}

// public

FFGrid::FFGrid(const String &n) : Grid(n)
{
}

FFGrid::~FFGrid()
{
}

bool
FFGrid::read(const String &dataset, int &error)
{
    bool status;

    if (read_p()) // nothing to do
        return true;

    // read array elements
    if (!(status = array_var()->read(dataset, error))) 
        return status;

    // read maps elements
    for (Pix p = first_map_var(); p; next_map_var(p)){
      if(!(status = map_var(p)->read(dataset, error)))
            break;
      }
    if( status ) set_read_p(true);
    return status;

}


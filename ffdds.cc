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

// This file contains functions which read the variables and their description
// from a freform API and build the in-memeory DDS. These functions form the
// core of the server-side software necessary to extract the DDS from a
// FreeForm data file.
//
// It also contains test code which will print the in-memeory DDS to
// stdout. 
//
// ReZa 6/20/97

// $Log: ffdds.cc,v $
// Revision 1.2  1998/04/16 18:11:25  jimg
// Sequence support added by Reza
//
//

static char rcsid[]={"$Id: ffdds.cc,v 1.2 1998/04/16 18:11:25 jimg Exp $"};

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include <iostream.h>

#include "DDS.h"
#include "FFInt32.h"
#include "FFFloat64.h"
#include "FFByte.h"
#include "FFArray.h"
#include "FFGrid.h"
#include "FFStr.h"
#include "cgi_util.h"
#include "FreeForm.h"

// Used by ErrMsgT

static char Msgt[255];
#ifdef NEVER
extern void ErrMsgT(char *Msgt);
extern char *name_path(char *path);
#endif
 
// Array support only, sequnce must be in the external dds file for now.

bool
read_descriptors(DDS &dds_table, const char *filename, String *error)
{
  int error = 0;
  int i = 0;
  int num_names = 0;
  char **var_names_vector = NULL;

  error = db_ask(dbin, DBASK_VAR_NAMES, FFF_INPUT, &num_names, 
		 &var_names_vector);
  if (error)
    goto main_exit;

  for (i = 0; i < num_names; i++)
  {
    int num_dim_names = 0;
    char **dim_names_vector = NULL;

    error = db_ask(dbin, DBASK_ARRAY_DIM_NAMES, var_names_vector[i], 
		   &num_dim_names, &dim_names_vector);
    if (!error)
    {
      FF_ARRAY_DIM_INFO_PTR array_dim_info = NULL;
      int j = 0;

      for (j = 0; j < num_dim_names; j++)
      {
        error = db_ask(dbin, DBASK_ARRAY_DIM_INFO, var_names_vector[i], 
		       dim_names_vector[j], &array_dim_info);
        if (!error)
        {

#ifdef TEST
          printf("Array %s, dimension %s:\n", var_names_vector[i], 
		 dim_names_vector[j]);
          printf("Start index is %ld\n", array_dim_info->start_index);
          printf("End index is %ld\n", array_dim_info->end_index);
          printf("Granularity is %ld\n", array_dim_info->granularity);
          printf("Separation is %ld\n", array_dim_info->separation);
          printf("Grouping is %ld\n\n", array_dim_info->grouping);
#endif
          memFree(array_dim_info, "");
          array_dim_info = NULL;
        }
      }

      memFree(dim_names_vector, "");
      dim_names_vector = NULL;
    }
  }

  memFree(var_names_vector, "");
  var_names_vector = NULL;
}

  return true;
}

#ifdef TEST

int
main(int argc, char *argv[])
{
  DDS dds;
  
  if(!read_descriptors(dds, argv[1]))
    abort();
  
  dds.print();
}

#endif


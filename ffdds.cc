
// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

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
// Revision 1.3  1998/04/21 17:14:10  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:25  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: ffdds.cc,v 1.3 1998/04/21 17:14:10 jimg Exp $"};

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

#ifdef NEVER
// Used by ErrMsgT

static char Msgt[255];
extern void ErrMsgT(char *Msgt);
extern char *name_path(char *path);
#endif
 
// Array support only, sequnce must be in the external dds file for now.

bool
read_descriptors(DDS &dds_table, String filename, String *err_msg)
{
    int error = 0;
    int i = 0;
    int num_names = 0;
    char **var_names_vector = NULL;

    error = db_ask(dbin, DBASK_VAR_NAMES, FFF_INPUT, &num_names, 
		   &var_names_vector);
    if (error)
	goto main_exit;
    
    for (i = 0; i < num_names; i++) {
	int num_dim_names = 0;
	char **dim_names_vector = NULL;

	error = db_ask(dbin, DBASK_ARRAY_DIM_NAMES, var_names_vector[i], 
		       &num_dim_names, &dim_names_vector);
	if (!error) {
	    FF_ARRAY_DIM_INFO_PTR array_dim_info = NULL;
	    int j = 0;

	    for (j = 0; j < num_dim_names; j++) {
		error = db_ask(dbin, DBASK_ARRAY_DIM_INFO, 
			       var_names_vector[i], dim_names_vector[j], 
			       &array_dim_info);
		if (!error) {

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

    return true;
}

#ifdef TEST

int
main(int argc, char *argv[])
{
    DDS dds;
    String err;

    if(!read_descriptors(dds, argv[1], &err))
	abort();
  
    dds.print();
}

#endif


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
// Revision 1.1  1997/10/03 17:02:43  jimg
// Initial version from Reza
//
//

static char rcsid[]={"$Id: ffdds.cc,v 1.1 1997/10/03 17:02:43 jimg Exp $"};

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


// This function returns the appropriate DODS BaseType for the given 
// netCDF data type.
//

BaseType *
Get_bt(char *varname, nc_type datatype) 
{
    switch (datatype) {
      case NC_CHAR:
	return (NewStr(varname));

      case NC_BYTE:
	return (NewByte(varname));
	
      case NC_SHORT:
      case NC_LONG:
	return (NewInt32(varname));

      case NC_FLOAT:
      case NC_DOUBLE:
	return (NewFloat64(varname));
	
      default:
	return (NewStr(varname));
    }
}

// Read given number of variables (nvars) from the opened netCDF file 
// (ncid) and add them with their appropriate type and dimensions to 
// the given instance of the DDS class.
//
// Returns: false if an error accessing the netcdf file was detected, true
// otherwise.

bool
read_class(DDS &dds_table, int ncid, int nvars, String *error)
{
  char varname1[MAX_NC_NAME];
  char varname2[MAX_VAR_DIMS][MAX_NC_NAME];
  char dimname[MAX_NC_NAME];
  char dimname2[MAX_NC_NAME];
  char *var_match[MAX_VAR_DIMS];
  char *map_name[MAX_VAR_DIMS];
  int dim_ids[MAX_VAR_DIMS];
  int dim_szs[MAX_VAR_DIMS];
  char dim_nms[MAX_VAR_DIMS][MAX_NC_NAME];
  int tmp_dim_ids[MAX_VAR_DIMS];
  nc_type typ_match[MAX_VAR_DIMS];
  int ndims, ndims2, dim_match; 
  int vars_in_grid = 0;
  long dim_sz, tmp_sz;  
  nc_type nctype;
  Array *ar;
  Grid *gr;
  Part pr;
  
  //add all the variables in this file to DDS table 

  for (int v1 = 0; v1 < nvars; ++v1) {
    if (lncvarinq(ncid,v1,varname1,&nctype,&ndims,dim_ids,(int *)0) == -1){
      sprintf (Msgt,"ncdds server: 
           could not get variable name or dimension number for variable %d ",v1);
      ErrMsgT(Msgt); //local error messag
      cat((String)"\"",(String)Msgt,(String)" \"",*(error));//remote error message
      return false;
    }

    BaseType *bt = Get_bt(varname1,nctype);
    
    // is an Atomic-class ?

    if (ndims == 0){
      dds_table.add_var(bt);
    }
	
    // Grid vs. Array type matching
    
    else {
      
      dim_match = 0;
	  
      // match all the dimensions of this variable to other variables
      int d;
      for (d = 0; d < ndims; ++d){
	if (lncdiminq(ncid, dim_ids[d], dimname, &dim_sz) == -1){
	  sprintf (Msgt,"ncdds server: could not get dimension size for dimension
                  %d in variable %d ",d,v1);
	  ErrMsgT(Msgt); //server error messag
	  cat((String)"\"",(String)Msgt,(String)" \"",*(error));//Client
	  return false;
	}
	dim_szs[d] = (int) dim_sz;
	(void) strcpy(dim_nms[d],dimname);
	
	for (int v2 = 0; v2 < nvars; ++v2) { 
	  if (lncvarinq(ncid,v2,varname2[v2],&nctype,&ndims2,
			tmp_dim_ids,(int *)0) == -1){
	    sprintf (Msgt,"ncdds server: could not get variable name or dimension 
                    number for variable %d ",v2);
            ErrMsgT(Msgt); 
            cat((String)"\"",(String)Msgt,(String)" \"",*(error));
	    return false;
	  }
	  
	  // Is it a Grid ?     1) variable name = the dimension name
	  //                    2) The variable has only one dimension
	  //                    3) It is not itself
	  //                    4) They are the same size
	  if ((v1 != v2) && (strcmp(dimname,varname2[v2]) == 0) && 
	      (ndims2 == 1)){
	    if (lncdiminq(ncid,tmp_dim_ids[0],(char *)0, &tmp_sz)== -1){
	      sprintf (Msgt,"ncdds server: could not get dimension size for 
                    dimension %d in variable %d ",d,v2);
	      ErrMsgT(Msgt);
	      cat((String)"\"",(String)Msgt,(String)" \"",*(error));
	      return false;
	    }
	    if (tmp_sz == dim_sz){  // a map part of a grid was identified    
	      typ_match[dim_match] = nctype; 
	      var_match[dim_match] = varname2[v2]; 
	      dim_match++;
	      
	      // The variable is now in DDS as map part of a grid and
	      // its duplicate independent variable will be removed from DDS 
	      bool added = false;
	      for (int ii=0; ii<vars_in_grid; ii++)
		if(strcmp(map_name[ii],varname2[v2])==0)
		  added = true;
	      if(!added){
		map_name[vars_in_grid]=varname2[v2];
		vars_in_grid++;
	      }

	      break; // Stop var search, matching variable 
	            // for the given dimension was found
	    }
	  }
	}		
	
	// Stop dimensions search, the variable does not 
	// fit into a grid, due to a dimension mis-match.
	// Also, get the size for the remainder of the   
	// dimensions in the variable.
	if (dim_match != d+1) {
	  for (int d2 = d+1; d2 < ndims; ++d2){
	    if (lncdiminq(ncid, dim_ids[d2], dimname2, &dim_sz) == -1){
	      sprintf (Msgt,"ncdds server: could not get dimension size 
                      for dimension %d in variable %d (ncdds)",d2,v1);
		     ErrMsgT(Msgt);
		     cat((String)"\"",(String)Msgt,(String)" \"",*(error));
		     return false;
	    }
	    dim_szs[d2] = (int) dim_sz;
	    (void) strcpy(dim_nms[d2],dimname2);
	  }
	  break;
	}
      }
      
      // Create common array for both Array and Grid types
      
      ar = NewArray(varname1);
      ar->add_var(bt);
      for (d = 0; d < ndims; ++d) 
	ar->append_dim(dim_szs[d],dim_nms[d]);
      
#ifndef NOGRID
      
      if (ndims == dim_match){   // Found Grid type, add it
	gr = NewGrid(varname1);
	pr = array;
	gr->add_var(ar,pr);
	pr = maps;
	for ( d = 0; d < ndims; ++d){
	  ar = new NCArray; 
	  bt = Get_bt(var_match[d],typ_match[d]);
	  ar->add_var(bt);     
	  ar->append_dim(dim_szs[d],dim_nms[d]);
	  gr->add_var(ar,pr);
	}
	dds_table.add_var(gr);
      }
      else {                    // must be an Array, add it
	dds_table.add_var(ar);
      }
#else
      
      dds_table.add_var(ar);
      
#endif
    }
  }

  // Remove duplicate variables already in DDS as the map part of a Grid  
  for(int ii=0; ii<vars_in_grid; ii++)
    dds_table.del_var(map_name[ii]);


  return true;
}

// Given a reference to an instance of class DDS and a filename that refers
// to a netcdf file, read the netcdf file and extract all the dimensions of
// each of its variables. Add the variables and their dimensions to the
// instance of DDS.
//
// Returns: false if an error accessing the netcdf file was detected, true
// otherwise. 

bool
read_descriptors(DDS &dds_table, const char *filename, String *error)
{
  int ncid = lncopen(filename, NC_NOWRITE);
  int nvars; 
  
  if (ncid == -1) {
    sprintf (Msgt,"ncdds server: could not open file %s ", filename);
    ErrMsgT(Msgt); //local error messag
    cat((String)"\"",(String)Msgt,(String)" \"",*(error));//remote error message
    return false;
  }
  
  // how many variables? 
  if (lncinquire(ncid, (int *)0, &nvars, (int *)0, (int *)0) == -1) {
    ErrMsgT("Could not inquire about netcdf file (ncdds)");
    *(error) = (String)"\"ncdds server: Could not inquire about netcdf file \"";
    return false;
  }
  // dataset name
  dds_table.set_dataset_name(name_path(filename));
  
  // read variables class
  if (!read_class(dds_table,ncid,nvars,error))
    return false;
  
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



// (c) COPYRIGHT URI/MIT 1997-98
// Please read the full copyright statement in the file COPYRIGH.  
//
// Authors: reza (Reza Nekovei)

// This file contains functions which read the variables and their description
// from a freeform API and build the in-memeory DDS. These functions form the
// core of the server-side software necessary to extract the DDS from a
// FreeForm data file.
//
// It also contains test code which will print the in-memeory DDS to
// stdout. 
//
// ReZa 6/20/97

// $Log: ffdds.cc,v $
// Revision 1.4  1998/08/12 21:21:15  jimg
// Massive changes from Reza. Compatible with the new FFND library
//
// Revision 1.3  1998/04/21 17:14:10  jimg
// Fixes for warnings, etc
//
// Revision 1.2  1998/04/16 18:11:25  jimg
// Sequence support added by Reza

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: ffdds.cc,v 1.4 1998/08/12 21:21:15 jimg Exp $"};

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
#include "util_ff.h"

// Used by ErrMsgT
static char Msgt[255];

bool
read_descriptors(DDS &dds_table,const char *filename, String *err_msg)
{
  int error = 0;
  int i = 0;
  int num_names = 0;
  char **var_names_vector = NULL;
  DATA_BIN_PTR dbin = NULL;
  FF_STD_ARGS_PTR SetUps = NULL;  
  VARIABLE_PTR var = NULL;
  FORMAT_PTR iformat=NULL;
  PROCESS_INFO_LIST pinfo_list = NULL;
  PROCESS_INFO_PTR pinfo = NULL;
  bool newseq = true;
  bool is_array = true;
  Array *ar = NULL;
  Sequence *seq = NULL;

  // Set dataset name
  dds_table.set_dataset_name(name_path(filename));
   
  SetUps = ff_create_std_args();
  if (!SetUps)
    {
      sprintf(Msgt, "ff_dds: Insufficient memory -- free more memory and try again");
      ErrMsgT(Msgt);  
      cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
      return false;
    }
    
  /** set the structure values to create the FreeForm DB**/
  SetUps->user.is_stdin_redirected = 0;
  SetUps->input_file = (char *)filename;
  String &ds = (char *)filename;
  String iff = find_ancillary_file(ds);
  char *if_f = new char[iff.length() + 1];
  strcpy(if_f, iff);
  SetUps->input_format_file = if_f;
  SetUps->output_file = NULL;
  
  error = SetDodsDB(SetUps, &dbin, Msgt);
  if (error && error < ERR_WARNING_ONLY)
    {
      db_destroy(dbin);
      ErrMsgT(Msgt);  
      cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
      return false;
    }
  
  error = db_ask(dbin, DBASK_VAR_NAMES, FFF_INPUT, &num_names, 
		 &var_names_vector);
  if (error){
    sprintf(Msgt, "ff_dds: db_ask could not get varible list from the input file");
    ErrMsgT(Msgt);  
    cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
    return false;
  }
  
  
  error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &pinfo_list);
  if (!error)
    {
      pinfo_list = dll_first(pinfo_list); 					  
      //	  pinfo      = FF_PI(pinfo_list);
      pinfo = ((PROCESS_INFO_PTR)(pinfo_list)->data.u.pi);
      iformat = PINFO_FORMAT(pinfo);
    }
  else {
    sprintf(Msgt, "ff_dds: db_ask could not get process info. for the input file");
    ErrMsgT(Msgt);  
    cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
    return false;
  }
  
 
  /*For some formats: Freefrom sends extra NULL variables at the end of the list */
  for (i=0; ((*var_names_vector)[i])&&(i<num_names);i++) { 
    int num_dim_names = 0;
    char **dim_names_vector = NULL;
    char *cp = NULL;
    FF_ARRAY_DIM_INFO_PTR array_dim_info = NULL;
    int j = 0;	
    /***/
    
    error = db_ask(dbin, DBASK_ARRAY_DIM_NAMES, var_names_vector[i], 
		   &num_dim_names, &dim_names_vector);
    if (error) {
      sprintf(Msgt, "ff_dds: db_ask could not get Array_Dim_names for var. %s",var_names_vector[i]);
      ErrMsgT(Msgt);  
      cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
      return false;
    }

    var = NULL;
    /* sequence names  */
    if(num_dim_names == 0)
      cp = var_names_vector[i];
    else 	  
      /* Note: FreeForm array names are returned appened to their format
	 name with '::' ? */ 
      cp = strstr(var_names_vector[i], "::")+2;
    
    var = ff_find_variable(cp,iformat);	

    while(!var){ /* search formats in the format list for the variable */
	
      //pinfo_list = dll_next(pinfo_list);
      pinfo_list = (pinfo_list)->next;
      
      //pinfo      = FF_PI(pinfo_list);
      pinfo = ((PROCESS_INFO_PTR)(pinfo_list)->data.u.pi);
	
      if(!pinfo) {
	sprintf(Msgt, "ff_dds: Variable %s was not in found in any of format list",cp);
	ErrMsgT(Msgt);  
	cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
	return false;
      }
      iformat = PINFO_FORMAT(pinfo);
      var = ff_find_variable(cp,iformat);	
    }
       
    if(num_dim_names == 0){
      if(newseq){
	newseq = false;
	// The format name cannot contain spaces! 8/12/98 jhrg
	seq = NewSequence(iformat->name);
      }
      is_array=false;
    }
    else {
      ar = NewArray(cp);
      newseq = true; // An array terminates the old sequence
      is_array = true;
    }
    
    BaseType *bt = NULL;
    switch (FFV_DATA_TYPE(var))
      {
	case FFV_TEXT:
	  bt = NewByte(cp);
	  break;
	  
	case FFV_INT8:
	  bt = NewInt32(cp);
	  break;
	  
	case FFV_UINT8:
	  bt = NewUInt32(cp);
	  break;
	  
	case FFV_INT16:
	  bt = NewInt32(cp);
	  break;
	  
	case FFV_UINT16:
	  bt = NewUInt32(cp);
	  break;

	case FFV_INT32:
	  bt = NewInt32(cp);
	  break;
	  
	case FFV_UINT32:
	  bt = NewUInt32(cp);
	  break;
	  
	case FFV_INT64:
	  bt = NewInt32(cp);
	  break;
	  
	case FFV_UINT64:
	  bt = NewUInt32(cp);
	  break;
	  
	case FFV_FLOAT32:
	  bt = NewFloat64(cp);
	  break;
	  
	case FFV_FLOAT64:
	  bt = NewFloat64(cp);
	  break;
	  
	case FFV_ENOTE:
	  bt = NewStr(cp);
	  break;
      }	
    
    if(is_array)
      ar->add_var(bt);
    else
      seq->add_var(bt);
      
    for (j = 0; j < num_dim_names; j++) {
      error = db_ask(dbin, DBASK_ARRAY_DIM_INFO, 
		     var_names_vector[i], dim_names_vector[j], 
		     &array_dim_info);
      if (error) {
	sprintf(Msgt, "ff_dds: db_ask could not get Array_Dim_Info for var. %s",var_names_vector[i]);
	ErrMsgT(Msgt);  
	cat((String)"\"",(String)Msgt,(String)" \"",*(err_msg));
	return false;
      }
      
      int DimSiz = (array_dim_info->end_index-array_dim_info->start_index+1)/array_dim_info->granularity;
      ar->append_dim(DimSiz, (String)dim_names_vector[j]);

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
    
    memFree(dim_names_vector, "");
    dim_names_vector = NULL;
    if(is_array)
      dds_table.add_var(ar);
    else
      if(newseq)
	dds_table.add_var(seq);
  }
  if(!is_array)
    dds_table.add_var(seq);
  
  memFree(var_names_vector, "");
  var_names_vector = NULL;
  
  return 1;
 
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


/* FILENAME: showdbin.c
 *
 * CONTAINS: 
 * Public functions:
 *
 * db_ask
 *
 * Private functions:
 *
 * add_format_info
 * create_format_info
 * create_format_info_list
 * set_bufsize
 * dbask_format_summary
 * show_format_info_list
 */

/*
 * NAME:        db_ask
 *              
 * PURPOSE:     db_ask shows various attributes of a data bin.
 *
 * USAGE:       db_ask(DATA_BIN_PTR, attribute, [args], attribute, args,... NULL)
 *
 * RETURNS:     0 if all goes well.
 *
 * DESCRIPTION: Provides information about a data bin according to message code.
 * The message codes and their arguments are:
 *
 * DBASK_PROCESS_INFO: FF_TYPES_t format_type, PROCESS_INFO_LIST_HANDLE hpinfo_list
 * DBASK_FORMAT_SUMMARY: FF_BUFSIZE_HANDLE hbufsize
 * DBASK_VAR_NAMES: FF_TYPES_t format_type, int *num_names, char **names_vector
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * AUTHOR:      T. Habermann, NGDC, (303) 497 - 6472, haber@mail.ngdc.noaa.gov
 *
 * COMMENTS:
 *                                 
 * KEYWORDS: databins
 *
*/

#include <freeform.h>

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "print_format_type"

static int print_format_type
	(
	 PROCESS_INFO_PTR pinfo,
	 FF_BUFSIZE_PTR bufsize
	)
{	
	FF_VALIDATE(pinfo);
	FF_VALIDATE(bufsize);

	if (IS_ASCII_TYPE(PINFO_TYPE(pinfo)))
		sprintf(bufsize->buffer + bufsize->bytes_used, "ASCII");
	else if (IS_BINARY_TYPE(PINFO_TYPE(pinfo)))
		sprintf(bufsize->buffer + bufsize->bytes_used, "Binary");
	else if (IS_DBASE_TYPE(PINFO_TYPE(pinfo)))
		sprintf(bufsize->buffer + bufsize->bytes_used, "dBASE");
	else
	{
		assert(!ERR_SWITCH_DEFAULT);
		return(err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d", ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__));
	}
	
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}

	if (IS_INPUT_TYPE(PINFO_TYPE(pinfo)))
		sprintf(bufsize->buffer + bufsize->bytes_used, " Input");
	else if (IS_OUTPUT_TYPE(PINFO_TYPE(pinfo)))
		sprintf(bufsize->buffer + bufsize->bytes_used, " Output");
	else
	{
		assert(!ERR_SWITCH_DEFAULT);
		return(err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d", ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__));
	}
	
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}


	if (IS_DATA_TYPE(PINFO_TYPE(pinfo)))
	{
		sprintf(bufsize->buffer + bufsize->bytes_used, " Data");
		bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	}
	else if (IS_HEADER_TYPE(PINFO_TYPE(pinfo)))
	{
		if (IS_SEPARATE_TYPE(PINFO_TYPE(pinfo)))
			sprintf(bufsize->buffer + bufsize->bytes_used, " Separate");
		else if (IS_EMBEDDED_TYPE(PINFO_TYPE(pinfo)))
			sprintf(bufsize->buffer + bufsize->bytes_used, " Embedded");
		else
		{
			assert(!ERR_SWITCH_DEFAULT);
			return(err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d", ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__));
		}
		
		bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);

		if (IS_VARIED_TYPE(PINFO_TYPE(pinfo)))
			sprintf(bufsize->buffer + bufsize->bytes_used, " Varied");
		
		bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);

		if (IS_FILE_TYPE(PINFO_TYPE(pinfo)))
			sprintf(bufsize->buffer + bufsize->bytes_used, " File");
		else if (IS_REC_TYPE(PINFO_TYPE(pinfo)))
			sprintf(bufsize->buffer + bufsize->bytes_used, " Record");
		else
		{
			assert(!ERR_SWITCH_DEFAULT);
			return(err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d", ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__));
		}
		
		bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);

		sprintf(bufsize->buffer + bufsize->bytes_used, " Header");
		bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	}
	else
	{
		assert(!ERR_SWITCH_DEFAULT);
		return(err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d", ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__));
	}

	if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}

	return(0);
}

/*
 * NAME:	ff_format_info
 *			   
 * PURPOSE:	To get the header information from a format.
 *				     
 * USAGE:	ff_format_info(FORMAT *format, char *scratch_buffer)
 *		  
 * RETURNS:	Zero on success, an error code on failure
 *
 * DESCRIPTION:	This function prints the information from a format header
 *				into a character buffer. This information includes:
 *					the number of variables
 *					the maximum length of the format
 *					the format type
 *					
 * Any new text is appended to bufsize.
 *
 * SYSTEM DEPENDENT FUNCTIONS:	
 *
 * ERRORS:
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * COMMENTS:	The buffer must be large enough to hold the information.
 *
 * KEYWORDS:	
 *
 */
#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_process_info"

static int ff_process_info(PROCESS_INFO_PTR pinfo, FF_BUFSIZE_PTR bufsize)
{
	int error = 0;

	FF_VALIDATE(pinfo);
	FF_VALIDATE(bufsize);

	if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}

	sprintf(bufsize->buffer + bufsize->bytes_used,"\n(%s) ", PINFO_ORIGIN(pinfo));

	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}

	error = print_format_type(pinfo, bufsize);
	if (error)
		return(error);

	sprintf(bufsize->buffer + bufsize->bytes_used,":  \"%s\"\n", PINFO_NAME(pinfo));

	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}

	sprintf(bufsize->buffer + bufsize->bytes_used,
	        "%s %ld %s%s%s (%ld byte%s) %s: ",
	        IS_INPUT_TYPE(PINFO_TYPE(pinfo)) ? "Reading" : "Writing",
	        (long)PINFO_SUB_ARRAY_ELS(pinfo),
			  !FD_IS_NATIVE_BYTE_ORDER(PINFO_FD(pinfo)) && IS_BINARY(PINFO_FORMAT(pinfo)) ? "byteswapped " : "",
	        IS_ARRAY_TYPE(PINFO_TYPE(pinfo)) ? "element" : "record",
	        PINFO_SUB_ARRAY_ELS(pinfo) == 1 ? "" : "s",
	        (long)PINFO_ARRAY_BYTES(pinfo),
	        PINFO_ARRAY_BYTES(pinfo) == 1 ? "" : "s",
	        IS_INPUT_TYPE(PINFO_TYPE(pinfo)) ? "from" : "to");

	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}

	if (PINFO_IS_FILE(pinfo))
		sprintf(bufsize->buffer + bufsize->bytes_used, "%s\n", PINFO_FNAME(pinfo));
	else
		sprintf(bufsize->buffer + bufsize->bytes_used, "Application Program\n");

	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}


	sprintf(bufsize->buffer + bufsize->bytes_used,
	        "Each %s contains %u field%s and is %u %s%s long.\n",
			  IS_ARRAY_TYPE(PINFO_TYPE(pinfo)) ? "element" : "record",
		      (unsigned)PINFO_NUMVARS(pinfo), PINFO_NUMVARS(pinfo) == 1 ? "" : "s",
		      (unsigned)PINFO_RECL(pinfo),
		      IS_BINARY_TYPE(PINFO_TYPE(pinfo)) ? "byte" : "character",
		      PINFO_RECL(pinfo) > 1 ? "s" : ""
		     );
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);

	return(0);
}

static PROCESS_INFO_PTR create_process_info(FF_ARRAY_DIPOLE_PTR pole)
{
	PROCESS_INFO_PTR pinfo = NULL;
	
	FF_VALIDATE(pole);
	
	pinfo = (PROCESS_INFO_PTR)memMalloc(sizeof(PROCESS_INFO), "pinfo");
	if (pinfo)
	{
#ifdef FF_CHK_ADDR
		pinfo->check_address = (void *)pinfo;
#endif
		pinfo->pole = pole;

		if (pole->mate)
		{
			pinfo->mate = (PROCESS_INFO_PTR)memMalloc(sizeof(PROCESS_INFO), "pinfo->mate");
			if (pinfo->mate)
			{
#ifdef FF_CHK_ADDR
				pinfo->mate->check_address = (void *)pinfo->mate;
#endif
				pinfo->mate->pole = pole->mate;
				pinfo->mate->mate = pinfo; /* This may not be necessary */
			}
			else
			{
				memFree(pinfo, "pinfo");
				pinfo = NULL;
				err_push(ERR_MEM_LACK, NULL);
			}
		}
		else
			pinfo->mate = NULL;
	}
	else
		err_push(ERR_MEM_LACK, NULL);

	return(pinfo);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "dbask_format_summary"

/*
 * NAME: dbask_format_summary
 *
 * PURPOSE: This function lists the formats in a format list into a buffer.
 *
 * AUTHOR: T. Habermann, NGDC, (303) 497 - 6472, haber@ngdc.noaa.gov
 *
 * USAGE: db_ask_format_list(FORMAT_LIST format_list, char *buffer)
 *
 * DESCRIPTION: 
 *
 * COMMENTS:  
 *
 * RETURNS:     
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 */

static int dbask_format_summary
	(
	 DATA_BIN_PTR dbin,
	 FF_BUFSIZE_HANDLE hbufsize
	)
{
	int error;

	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR  pinfo = NULL;
	
	assert(hbufsize);
	assert(*hbufsize == NULL);

	*hbufsize = ff_create_bufsize(SCRATCH_QUANTA);
	if (!*hbufsize)
		return(ERR_MEM_LACK);
			
	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_NULL, &pinfo_list);
	if (error)
		return(error);

	pinfo_list = dll_first(pinfo_list);
	pinfo = FF_PI(pinfo_list);
	while (pinfo)
	{
		FF_VALIDATE(pinfo);
		
		error = ff_process_info(pinfo, *hbufsize);
		if (error)
			return(error);

		pinfo_list = dll_next(pinfo_list);
		pinfo = FF_PI(pinfo_list);
	}

	ff_destroy_process_info_list(pinfo_list);

	return(0);
}

static int add_process_info
	(
	 FF_ARRAY_DIPOLE_PTR pole,
	 PROCESS_INFO_LIST pinfo_list
	)
{
	PROCESS_INFO_LIST step_list = NULL;
	PROCESS_INFO_PTR pinfo = NULL;
	
	pinfo = create_process_info(pole);
	if (!pinfo)
	{
		ff_destroy_process_info_list(pinfo_list);
		return(ERR_MEM_LACK);
	}
			
	step_list = dll_add(pinfo_list);
	if (!step_list)
	{
		ff_destroy_process_info(pinfo);
		ff_destroy_process_info_list(pinfo_list);
		return(ERR_MEM_LACK);
	}
			
	dll_assign(pinfo, DLL_PI, step_list);

	return(0);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "create_process_info_list"

static int create_process_info_list
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t format_type,
	 PROCESS_INFO_LIST_HANDLE hpinfo_list
	)
{
	FF_ARRAY_CONDUIT_LIST conduit_list = NULL;
	FF_ARRAY_CONDUIT_PTR conduit = NULL;
	BOOLEAN added = FALSE;
	int error = 0;

	if (dbin->array_conduit_list)
		FF_VALIDATE(dbin->array_conduit_list);
	else
		return(ERR_GENERAL);

	*hpinfo_list = dll_init();
	if (!*hpinfo_list)
		return(err_push(ERR_MEM_LACK, NULL));

	conduit_list = dll_first(dbin->array_conduit_list);
	conduit = FF_AC(conduit_list);
	while (conduit)
	{
		FF_VALIDATE(conduit);
		
		if ((conduit->input->fd->format->type & format_type) == format_type)
		{
			error = add_process_info(conduit->input, *hpinfo_list);
			if (error)
				return(error);
			
			added = TRUE;
		}
			
		if (conduit->output && (conduit->output->fd->format->type & format_type) == format_type)
		{
			error = add_process_info(conduit->output, *hpinfo_list);
			if (error)
				return(error);
			
			added = TRUE;
		}
			
		conduit_list = dll_next(conduit_list);
		conduit = FF_AC(conduit_list);
	}
	
	if (!added)
	{
		dll_free_list(*hpinfo_list);
		*hpinfo_list = NULL;
		return(ERR_GENERAL);
	}
	
	return(0);
}

static int dbask_process_info
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t format_type,
	 PROCESS_INFO_LIST_HANDLE hpinfo_list
	)
{
	int error = 0;
	
	assert(hpinfo_list);
	
	*hpinfo_list = NULL;
	
	error = create_process_info_list(dbin, format_type, hpinfo_list);

	return(error);
}

/*****************************************************************************
 * NAME:  dbask_var_names
 *
 * PURPOSE:  Return a string vector with formats' variables' names.
 *
 * USAGE:  error = dbask_var_names(dbin, format_type, num_names, names_vector);
 *
 * RETURNS:  Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Application programmers do not call this function directly,
 * instead they call db_ask with DBASK_VAR_NAMES.
 *
 * This function finds all formats matching format_type and builds a string
 * vector with the names of the matching formats' variables' names.  This string
 * vector and its companion int num_names is like int argc and char **argv.
 *
 * Calling code should free names_vector but not its elements.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:  Array variables will have format title and colon-colon "::" prepended
 * to the variable name.
 *
 * Are there arrays of records?  For example:
 *
 * dbase_record "one-two"
 * one 1 1 text 0
 * two 2 2 text 0
 * 
 * ASCII_input_data "fu"
 * x 1 2 ARRAY["x" 1 to 1] OF "one-two" 
 * y 3 4 ARRAY["y" 1 to 1] OF "one-two" 
 *
 * So what names do we want?  "one", "two", or "x", "y"?
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int dbask_var_names
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t format_type,
	 int *num_names,
	 char **names_vector[]
	)
{
	int error = 0;
	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR pinfo = NULL;

	int num_vars = 0;

	*num_names = 0;

	FF_VALIDATE(dbin);
	assert(format_type);
	assert(num_names);
	assert(names_vector);

	if (!dbin || !format_type || !num_names || !names_vector)
		return(err_push(ERR_API, "function argument is undefined (NULL value)"));

	error = create_process_info_list(dbin, format_type, &pinfo_list);
	if (!error)
	{
		pinfo_list = dll_first(pinfo_list);
		pinfo = FF_PI(pinfo_list);
		while (pinfo)
		{
			if (PINFO_IS_ARRAY(pinfo))
				num_vars++;
			else
				num_vars += PINFO_NUMVARS(pinfo);

			pinfo_list = dll_next(pinfo_list);
			pinfo = FF_PI(pinfo_list);
		}

		*names_vector = (char **)memMalloc(sizeof(char *) * (num_vars + 1), "*names_vector");
		if (!*names_vector)
			error = err_push(ERR_MEM_LACK, "Allocating vector of %d strings", *num_names);
	}

	if (!error)
	{
		int i = 0;

		pinfo_list = dll_first(pinfo_list);
		pinfo = FF_PI(pinfo_list);
		while (pinfo)
		{
			if (PINFO_IS_ARRAY(pinfo))
			{
				(*names_vector)[(*num_names)++] = PINFO_NAME(pinfo);
			}
			else
			{
				FORMAT_PTR format = NULL;
				VARIABLE_LIST vlist = NULL;
				VARIABLE_PTR var = NULL;

				format = PINFO_FORMAT(pinfo);
				FF_VALIDATE(format);

				vlist = FFV_FIRST_VARIABLE(format);
				var = FF_VARIABLE(vlist);
				while (var)
				{
					if (FFV_DATA_TYPE(var))
						(*names_vector)[(*num_names)++] = var->name;

					vlist = FFV_NEXT_VARIABLE(vlist);
					var = FF_VARIABLE(vlist);
				}
			}

			pinfo_list = dll_next(pinfo_list);
			pinfo = FF_PI(pinfo_list);
		}

		for (i = *num_names; i <= num_vars; i++)
			(*names_vector)[i] = NULL;

		assert(*num_names <= num_vars);
	}

	if (pinfo_list)
		ff_destroy_process_info_list(pinfo_list);

	return(error);
}

/*****************************************************************************
 * NAME:  dbask_array_dim_names
 *
 * PURPOSE:  Return a string vector with the array variable's dimension names.
 *
 * USAGE:  error = dbask_array_dim_names(dbin, array_var_name, num_dim_names, dim_names_vector);
 *
 * RETURNS:  Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Application programmers do not call this function directly,
 * instead they call db_ask with DBASK_ARRAY_DIM_NAMES.
 *
 * This function finds all array variables with the given name and builds a string
 * vector with the names of its dimensions' names.  This string
 * vector and its companion int num_names is like int argc and char **argv.
 *
 * Calling code should free dim_names_vector but not its elements.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:  Input and output formats with the same title will cause ambiguity problems.
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int dbask_array_dim_names
	(
	 DATA_BIN_PTR dbin,
	 char *array_var_name,
	 int *num_dim_names,
	 char **dim_names_vector[]
	)
{
	int error = 0;
	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR pinfo = NULL;

	int num_dims = 0;

	*num_dim_names = 0;

	FF_VALIDATE(dbin);
	assert(array_var_name);
	assert(num_dim_names);
	assert(dim_names_vector);

	/* Has *dim_names_vector already been allocated?  If so, this would be a memory leak.
	   Do we automatically free *dim_names_vector, or do we force the AP to clean up?
	*/
	assert(*dim_names_vector == NULL);

	if (!dbin || !array_var_name || !num_dim_names || !dim_names_vector)
		return(err_push(ERR_API, "function argument is undefined (NULL value)"));

	error = create_process_info_list(dbin, 0, &pinfo_list);
	if (!error)
	{
		pinfo_list = dll_first(pinfo_list);
		pinfo = FF_PI(pinfo_list);
		while (pinfo)
		{
			if (PINFO_IS_ARRAY(pinfo) && !strcmp(array_var_name, PINFO_NAME(pinfo)))
			{
				num_dims += PINFO_NUM_DIMS(pinfo);
			}

			pinfo_list = dll_next(pinfo_list);
			pinfo = FF_PI(pinfo_list);
		}

		*dim_names_vector = (char **)memMalloc(sizeof(char *) * (num_dims + 1), "*dim_names_vector");
		if (!*dim_names_vector)
			error = err_push(ERR_MEM_LACK, "Allocating vector of %d strings", *num_dim_names);
	}

	if (!error)
	{
		int i = 0;

		pinfo_list = dll_first(pinfo_list);
		pinfo = FF_PI(pinfo_list);
		while (pinfo)
		{
			if (PINFO_IS_ARRAY(pinfo) && !strcmp(array_var_name, PINFO_NAME(pinfo)))
			{
				for (i = 0; i < PINFO_NUM_DIMS(pinfo); i++)
					(*dim_names_vector)[(*num_dim_names)++] = PINFO_DIM_NAME(pinfo, i);
			}

			pinfo_list = dll_next(pinfo_list);
			pinfo = FF_PI(pinfo_list);
		}

		for (i = *num_dim_names; i <= num_dims; i++)
			(*dim_names_vector)[i] = NULL;

		assert(*num_dim_names <= num_dims);
	}

	if (pinfo_list)
		ff_destroy_process_info_list(pinfo_list);

	return(error);
}

/*****************************************************************************
 * NAME:  dbask_array_dimension_info
 *
 * PURPOSE:  Return a structure with start and end indices, granularity, separation,
 * and grouping for the given array variable and dimension name.
 *
 * USAGE:  error = dbask_array_dimension_info(dbin, array_var_name, dim_names, array_dim_info);
 *
 * RETURNS:  Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Application programmers do not call this function directly,
 * instead they call db_ask with DBASK_ARRAY_DIM_INFO.
 *
 * Calling code should free array_dim_info.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:  Input and output formats with the same title will cause ambiguity problems.
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int dbask_array_dim_info
	(
	 DATA_BIN_PTR dbin,
	 char *array_var_name,
	 char *dim_name,
	 FF_ARRAY_DIM_INFO_HANDLE array_dim_info
	)
{
	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR pinfo = NULL;

	int i = 0;
	int error = 0;

	FF_VALIDATE(dbin);
	assert(array_var_name);
	assert(dim_name);
	assert(array_dim_info);

	/* Has *array_dim_info already been allocated?  If so, this would be a memory leak.
	   Do we automatically free *array_dim_info, or do we force the AP to clean up?
	*/
	assert(*array_dim_info == NULL);

	*array_dim_info = NULL;

	if (!dbin || !array_var_name || !dim_name || !array_dim_info)
		return(err_push(ERR_API, "function argument is undefined (NULL value)"));

	error = create_process_info_list(dbin, 0, &pinfo_list);
	if (!error)
	{
		pinfo_list = dll_first(pinfo_list);
		pinfo = FF_PI(pinfo_list);
		while (pinfo)
		{
			if (PINFO_IS_ARRAY(pinfo) && !strcmp(array_var_name, PINFO_NAME(pinfo)))
			{
				i = 0;
				while (i < PINFO_NUM_DIMS(pinfo))
				{
					if (!strcmp(dim_name, PINFO_DIM_NAME(pinfo,i)))
					{
						*array_dim_info = (FF_ARRAY_DIM_INFO_PTR)memMalloc(sizeof(FF_ARRAY_DIM_INFO), "array_dim_info");
						if (!*array_dim_info)
							error = err_push(ERR_MEM_LACK, "");
						else
						{
#ifdef FF_CHK_ADDR
							(*array_dim_info)->check_address = *array_dim_info;
#endif
							(*array_dim_info)->start_index = PINFO_DIM_START_INDEX(pinfo, i);
							(*array_dim_info)->end_index = PINFO_DIM_END_INDEX(pinfo, i);
							(*array_dim_info)->granularity = PINFO_DIM_GRANULARITY(pinfo, i);
							(*array_dim_info)->separation = PINFO_DIM_SEPARATION(pinfo, i);
							(*array_dim_info)->grouping = PINFO_DIM_GROUPING(pinfo, i);
							(*array_dim_info)->num_array_elements = PINFO_SUPER_ARRAY_ELS(pinfo);
						}

						break;
					}

					i++;
				}

				break;
			}

			pinfo_list = dll_next(pinfo_list);
			pinfo = FF_PI(pinfo_list);
		}
	}

	if (pinfo_list)
		ff_destroy_process_info_list(pinfo_list);

	if (!*array_dim_info)
		error = err_push(ERR_GENERAL, "Couldn't get array dimension information for %s=>%s", array_var_name, dim_name);

	return(error);
}

static int dbask_var_minmaxs
	(
	 char *mm_spec,
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t mm_type,
	 int num_names,
	 char *names_vector[],
	 void **mm_vector[]
	)
{
	int error = 0;
	int error_state = 0;
	int i = 0;
	char name_buffer[MAX_NAME_LENGTH];

	size_t mm_vector_size = 0;
	size_t mm_data_size = 0;
	size_t mm_type_size = 0;

	FF_VALIDATE(dbin);
	assert(mm_type);
	assert(num_names);
	assert(names_vector);
	assert(mm_vector);

	/* Has *mm_vector already been allocated?  If so, this would be a memory leak.
	   Do we automatically free *mm_vector, or do we force the AP to clean up?
	*/
	assert(*mm_vector == NULL);

	if (!dbin || !mm_type || !num_names || !names_vector || !mm_vector)
		return(err_push(ERR_API, "function argument is undefined (NULL value)"));

	mm_type_size = ffv_type_size(mm_type);
	mm_vector_size = (num_names + 1) * sizeof(double *);
	mm_data_size = num_names * mm_type_size;

	*mm_vector = (void **)memMalloc(mm_vector_size + mm_data_size, "*mm_vector");
	if (!*mm_vector)
		return(err_push(ERR_MEM_LACK, "Cannot allocate vector of %d %simums", num_names, mm_spec));

	((double **)*mm_vector)[num_names] = NULL;

	for (i = 0; i < num_names; i++)
	{
		void *data_dest = NULL;

		data_dest = (char *)*mm_vector + mm_vector_size + (i * mm_type_size);

		((double **)*mm_vector)[i] = NULL;

		sprintf(name_buffer, "%s_%s", names_vector[i], mm_spec);
		error = nt_ask(dbin, NT_ANYWHERE, name_buffer, mm_type, data_dest);
		if (error && error != ERR_NT_KEYNOTDEF)
			error_state = err_push(error, "Problem retrieving value for %s", name_buffer);

		if (error)
		{
			sprintf(name_buffer, "band_%d_%s", i + 1, mm_spec);
			error = nt_ask(dbin, NT_ANYWHERE, name_buffer, mm_type, data_dest);
			if (error && error != ERR_NT_KEYNOTDEF)
				error_state = err_push(error, "Problem retrieving value for %s", name_buffer);
		}

		if (error)
		{
			sprintf(name_buffer, "%simum_value", mm_spec);
			error = nt_ask(dbin, NT_ANYWHERE, name_buffer, mm_type, data_dest);
			if (error && error != ERR_NT_KEYNOTDEF)
				error_state = err_push(error, "Problem retrieving value for %s", name_buffer);
		}

		if (!error)
			((double **)*mm_vector)[i] = data_dest;
	}

	if (error && error != ERR_NT_KEYNOTDEF)
		error_state = error;

	return(error_state);
}

/*****************************************************************************
 * NAME:  dbask_var_mins
 *
 * PURPOSE:  Return a vector of minimum values corresponding to the given
 * string vector of variable names.
 *
 * USAGE:  error = dbask_var_mins(dbin, mins_type, num_names, names_vector, mins_vector);
 *
 * RETURNS:  Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Application programmers do not call this function directly,
 * instead they call db_ask with DBASK_VAR_MINS.
 *
 * This function calls nt_ask to see if a minimum value is defined in a header,
 * constant part of an equivalence section, or environment for each name
 * given in names_vector.  num_names and names_vector should be built with a 
 * call to db_ask:DBASK_VAR_NAMES.
 * 
 * If a minimum value is defined for a name then the corresponding pointer in the
 * vector is the address of memory containing that minimum value, with the data type
 * given by mins_type.  If no minimum value is defined then the corresponding pointer
 * will be NULL.  For example:
 *
 * double **mins_vector = NULL;
 * error = db_ask(dbin, DBASK_VAR_MINS, FFV_DOUBLE, num_names, names_vector, &mins_vector);
 * if (error)
 *    err_disp(std_args);
 *
 * if (mins_vector)
 * {
 *    int i = 0;
 *    for (i = 0; i < num_names; i++)
 *    {
 *      if (mins_vector[i])
 *        printf("Minimum of %s is %lf\n", names_vector[i], *(mins_vector[i]));
 *    }
 *
 *   free(mins_vector);
 * }
 *
 * Calling code should free mins_vector but not its elements.
 *
 * db_ask will return an error if there was a problem getting a value for any variable (but
 * not if the value is simply not defined) but other variables may have had no error.  Thus
 * an error return indicates a problem ocurred, but the presence of a NULL pointer in the
 * vector indicates a failure to get a value (either because it was not defined, or there
 * was an actual error.
 *
 * Minimums are not calculated as in Checkvar.  Instead a sequence of nt_ask calls are made
 * to see if minimum value is defined in a header, constant table, or environment.  First
 * a call for the name appended with "_min" is made, then a call for "band_"#"_min" where
 * # is the sequence number of the name in names_vector, and last a call for "minimum_value".
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:  This code assumes that pointers are the same
 * size regardless of the data type they point to.
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int dbask_var_mins
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t mm_type,
	 int num_names,
	 char *names_vector[],
	 void **mm_vector[]
	)
{
	return dbask_var_minmaxs("min", dbin, mm_type, num_names, names_vector, mm_vector);
}

/*****************************************************************************
 * NAME:  dbask_var_maxs
 *
 * PURPOSE:  Return a vector of maximum values corresponding to the given
 * string vector of variable names.
 *
 * USAGE:  error = dbask_var_maxs(dbin, maxs_type, num_names, names_vector, maxs_vector);
 *
 * RETURNS:  Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Application programmers do not call this function directly,
 * instead they call db_ask with DBASK_VAR_MAXS.
 *
 * This function calls nt_ask to see if a maximum value is defined in a header,
 * constant part of an equivalence section, or environment for each name
 * given in names_vector.  num_names and names_vector should be built with a 
 * call to db_ask:DBASK_VAR_NAMES.
 * 
 * If a maximum value is defined for a name then the corresponding pointer in the
 * vector is the address of memory containing that maximum value, with the data type
 * given by maxs_type.  If no maximum value is defined then the corresponding pointer
 * will be NULL.  For example:
 *
 * double **maxs_vector = NULL;
 * error = db_ask(dbin, DBASK_VAR_MAXS, FFV_DOUBLE, num_names, names_vector, &maxs_vector);
 * if (error)
 *   err_disp(std_args);
 *
 * if (maxs_vector)
 * {
 *    int i = 0;
 *    for (i = 0; i < num_names; i++)
 *    {
 *      if (maxs_vector[i])
 *        printf("Minimum of %s is %lf\n", names_vector[i], *(maxs_vector[i]));
 *    }
 *
 *   free(maxs_vector);
 * }
 *
 * Calling code should free maxs_vector but not its elements.
 *
 * db_ask will return an error if there was a problem getting a value for any variable (but
 * not if the value is simply not defined) but other variables may have had no error.  Thus
 * an error return indicates a problem ocurred, but the presence of a NULL pointer in the
 * vector indicates a failure to get a value (either because it was not defined, or there
 * was an actual error.
 *
 * Maximums are not calculated as in Checkvar.  Instead a sequence of nt_ask calls are made
 * to see if maximum value is defined in a header, constant table, or environment.  First
 * a call for the name appended with "_max" is made, then a call for "band_"#"_max" where
 * # is the sequence number of the name in names_vector, and last a call for "maximum_value".
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:  This code assumes that pointers are the same
 * size regardless of the data type they point to.
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int dbask_var_maxs
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t mm_type,
	 int num_names,
	 char *names_vector[],
	 void **mm_vector[]
	)
{
	return dbask_var_minmaxs("max", dbin, mm_type, num_names, names_vector, mm_vector);
}

/*****************************************************************************
 * NAME:  dbask_var_units
 *
 * PURPOSE:  Return a vector of unit definitions corresponding to the given
 * string vector of variable names.
 *
 * USAGE:  error = dbask_var_units(dbin, num_names, names_vector, units_vector);
 *
 * RETURNS:  Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Application programmers do not call this function directly,
 * instead they call db_ask with DBASK_VAR_UNITS.
 *
 * This function calls nt_ask to see if a unit definition is defined in a header,
 * constant part of an equivalence section, or environment for each name
 * given in names_vector.  num_names and names_vector should be built with a 
 * call to db_ask:DBASK_VAR_NAMES.
 * 
 * If a unit definition is defined for a name then the corresponding pointer in the
 * vector is the address of memory containing that unit definition.  If no unit
 * definition is defined then the corresponding pointer will be NULL.  For example:
 *
 * char **units_vector = NULL;
 * error = db_ask(dbin, DBASK_VAR_UNITS, num_names, names_vector, &units_vector);
 * if (error)
 *    err_disp(std_args);
 *
 * if (units_vector)
 * {
 *    int i = 0;
 *    for (i = 0; i < num_names; i++)
 *    {
 *      if (units_vector[i])
 *        printf("Units of %s are %s\n", names_vector[i], units_vector[i]);
 *    }
 *
 *   free(units_vector);
 * }
 *
 * db_ask will return an error if there was a problem getting a value for any variable (but
 * not if the value is simply not defined) but other variables may have had no error.  Thus
 * an error return indicates a problem ocurred, but the presence of a NULL pointer in the
 * vector indicates a failure to get a value (either because it was not defined, or there
 * was an actual error.
 *
 * Calling code should free units_vector but not its elements.
 *
 * A sequence of nt_ask calls are made	to see if a unit definition is defined in a header,
 * constant table, or environment.  First a call for the name appended with "_unit" is made,
 * then a call for "band_"#"_unit" where # is the sequence number of the name in names_vector,
 * and last a call for "value_unit".
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:  This code assumes that pointers are the same
 * size regardless of the data type they point to.
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int dbask_var_units
	(
	 DATA_BIN_PTR dbin,
	 int num_names,
	 char *names_vector[],
	 char **units_vector[]
	)
{
	int i = 0;
	int error = 0;
	int error_state = 0;
	size_t units_vector_size = 0;
	size_t units_data_size = 0;

	char *data_dest = NULL;
	char name_buffer[MAX_NAME_LENGTH];

	FF_VALIDATE(dbin);
	assert(num_names);
	assert(names_vector);
	assert(units_vector);

	/* Has *units_vector already been allocated?  If so, this would be a memory leak.
	   Do we automatically free *units_vector, or do we force the AP to clean up?
	*/
	assert(*units_vector == NULL);

	if (!dbin || !num_names || !names_vector || !units_vector)
		return(err_push(ERR_API, "function argument is undefined (NULL value)"));

	units_vector_size = (num_names + 1) * sizeof(char *);
	units_data_size = num_names * MAX_NAME_LENGTH;

	*units_vector = (char **)memMalloc(units_vector_size + units_data_size, "*units_vector");
	if (!*units_vector)
		return(err_push(ERR_MEM_LACK, "Cannot allocate vector of %d strings", num_names));

	(*units_vector)[num_names] = NULL;

	data_dest = (char *)*units_vector + units_vector_size;
	for (i = 0; i < num_names; i++)
	{
		(*units_vector)[i] = NULL;

		sprintf(name_buffer, "%s_unit", names_vector[i]);
		error = nt_ask(dbin, NT_ANYWHERE, name_buffer, FFV_TEXT, data_dest);
		if (error && error != ERR_NT_KEYNOTDEF)
			error_state = err_push(error, "Problem retrieving value for %s", name_buffer);

		if (error)
		{
			sprintf(name_buffer, "band_%d_unit", i + 1);
			error = nt_ask(dbin, NT_ANYWHERE, name_buffer, FFV_TEXT, data_dest);
			if (error && error != ERR_NT_KEYNOTDEF)
				error_state = err_push(error, "Problem retrieving value for %s", name_buffer);
		}

		if (error)
		{
			sprintf(name_buffer, "value_unit");
			error = nt_ask(dbin, NT_ANYWHERE, name_buffer, FFV_TEXT, data_dest);
			if (error && error != ERR_NT_KEYNOTDEF)
				error_state = err_push(error, "Problem retrieving value for %s", name_buffer);
		}

		if (!error)
		{
			(*units_vector)[i] = data_dest;
			data_dest += strlen(data_dest) + 1;
		}
	}

	return(error_state);
}

/*****************************************************************************
 * NAME:  dbask_var_flags
 *
 * PURPOSE:  Return a vector of missing flag values corresponding to the given
 * string vector of variable names.
 *
 * USAGE:  error = dbask_var_flags(dbin, flags_type, num_names, names_vector, flags_vector);
 *
 * RETURNS:  Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Application programmers do not call this function directly,
 * instead they call db_ask with DBASK_VAR_FLAGS.
 *
 * This function calls nt_ask to see if a missing flag value is defined in a header,
 * constant part of an equivalence section, or environment for each name
 * given in names_vector.  num_names and names_vector should be built with a 
 * call to db_ask:DBASK_VAR_NAMES.
 * 
 * If a flag value is defined for a name then the corresponding pointer in the
 * vector is the address of memory containing that flag value, with the data type
 * given by flags_type.  If no flag value is defined then the corresponding pointer
 * will be NULL.  For example:
 *
 * double **flags_vector = NULL;
 * error = db_ask(dbin, DBASK_VAR_FLAGS, FFV_DOUBLE, num_names, names_vector, &flags_vector);
 * if (error)
 *   err_disp(std_args);
 *
 * if (flags_vector)
 * {
 *    int i = 0;
 *    for (i = 0; i < num_names; i++)
 *    {
 *      if (flags_vector[i])
 *        printf("Flag value of %s is %f\n", names_vector[i], *(flags_vector[i]));
 *    }
 *
 *   free(flags_vector);
 * }
 *
 * db_ask will return an error if there was a problem getting a value for any variable (but
 * not if the value is simply not defined) but other variables may have had no error.  Thus
 * an error return indicates a problem ocurred, but the presence of a NULL pointer in the
 * vector indicates a failure to get a value (either because it was not defined, or there
 * was an actual error.
 *
 * Calling code should free flags_vector but not its elements.
 *
 * A sequence of nt_ask calls are made	to see if a flag value is defined in a header,
 * constant table, or environment.  First a call for the name appended with "_missing_flag" is made,
 * then a call for "band_"#"_missing_flag" where # is the sequence number of the name in names_vector,
 * and last a call for "missing_flag".
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:  This code assumes that pointers are the same
 * size regardless of the data type they point to.
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int dbask_var_flags
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t flags_type,
	 int num_names,
	 char *names_vector[],
	 void **flags_vector[]
	)
{
	int error = 0;
	int error_state = 0;
	int i = 0;
	char name_buffer[MAX_NAME_LENGTH];

	size_t flags_vector_size = 0;
	size_t flags_data_size = 0;
	size_t flags_type_size = 0;

	FF_VALIDATE(dbin);
	assert(flags_type);
	assert(num_names);
	assert(names_vector);
	assert(flags_vector);

	/* Has *flags_vector already been allocated?  If so, this would be a memory leak.
	   Do we automatically free *flags_vector, or do we force the AP to clean up?
	*/
	assert(*flags_vector == NULL);

	if (!dbin || !flags_type || !num_names || !names_vector || !flags_vector)
		return(err_push(ERR_API, "function argument is undefined (NULL value)"));

	flags_type_size = ffv_type_size(flags_type);
	flags_vector_size = (num_names + 1) * sizeof(double *);
	flags_data_size = num_names * flags_type_size;

	*flags_vector = (void **)memMalloc(flags_vector_size + flags_data_size, "*flags_vector");
	if (!*flags_vector)
		return(err_push(ERR_MEM_LACK, "Cannot allocate vector of %d flag values", num_names));

	((double **)*flags_vector)[num_names] = NULL;

	for (i = 0; i < num_names; i++)
	{
		void *data_dest = NULL;

		data_dest = (char *)*flags_vector + flags_vector_size + (i * flags_type_size);

		((double **)*flags_vector)[i] = NULL;

		sprintf(name_buffer, "%s_missing_flag", names_vector[i]);
		error = nt_ask(dbin, NT_ANYWHERE, name_buffer, flags_type, data_dest);
		if (error && error != ERR_NT_KEYNOTDEF)
			error_state = err_push(error, "Problem retrieving value for %s", name_buffer);

		if (error)
		{
			sprintf(name_buffer, "band_%d_missing_flag", i + 1);
			error = nt_ask(dbin, NT_ANYWHERE, name_buffer, flags_type, data_dest);
			if (error && error != ERR_NT_KEYNOTDEF)
				error_state = err_push(error, "Problem retrieving value for %s", name_buffer);
		}

		if (error)
		{
			sprintf(name_buffer, "missing_flag");
			error = nt_ask(dbin, NT_ANYWHERE, name_buffer, flags_type, data_dest);
			if (error && error != ERR_NT_KEYNOTDEF)
				error_state = err_push(error, "Problem retrieving value for %s", name_buffer);
		}

		if (!error)
			((double **)*flags_vector)[i] = data_dest;
	}

	if (error && error != ERR_NT_KEYNOTDEF)
		error_state = error;

	return(error_state);
}

static long dbask_bytes_to_process(PROCESS_INFO_LIST pinfo_list)
{
	long bytes_to_process = 0;
	PROCESS_INFO_PTR pinfo = NULL;

	pinfo_list = dll_first(pinfo_list);
	pinfo      = FF_PI(pinfo_list);
	while (pinfo)
	{
		if (PINFO_MATE(pinfo))
			bytes_to_process += PINFO_MATE_BYTES_LEFT(pinfo);

		pinfo_list = dll_next(pinfo_list);
		pinfo      = FF_PI(pinfo_list);
	}

	return(bytes_to_process);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "Show_dbin"
#define LINE_LENGTH             256

int db_ask(DATA_BIN_PTR dbin, int message, ...)
{
	va_list args;

	int error = 0;

	FF_VALIDATE(dbin);
	
	va_start(args, message);

	switch (message)
	{
		case DBASK_PROCESS_INFO:
		{
			FF_TYPES_t format_type = va_arg(args, FF_TYPES_t);
			PROCESS_INFO_LIST_HANDLE hpinfo_list = va_arg(args, PROCESS_INFO_LIST_HANDLE);

			error = dbask_process_info(dbin, format_type, hpinfo_list);
		}
		break;

	case DBASK_FORMAT_SUMMARY:
	{
		FF_BUFSIZE_HANDLE hbufsize = va_arg(args, FF_BUFSIZE_HANDLE);

		error = dbask_format_summary(dbin, hbufsize);
	}
	break;

	case DBASK_VAR_NAMES:
	{
		FF_TYPES_t format_type = va_arg(args, FF_TYPES_t);
		int *num_names = va_arg(args, int *);
		char ***names_vector = va_arg(args, char ***);

		error = dbask_var_names(dbin, format_type, num_names, names_vector);
	}
	break;

	case DBASK_ARRAY_DIM_NAMES:
	{
		char *array_var_name = va_arg(args, char *);
		int *num_dims = va_arg(args, int *);
		char ***dim_names_vector = va_arg(args, char ***);

		error = dbask_array_dim_names(dbin, array_var_name, num_dims, dim_names_vector);
	}
	break;

	case DBASK_ARRAY_DIM_INFO:
	{
		char *array_var_name = va_arg(args, char *);
		char *dim_name = va_arg(args, char *);
		FF_ARRAY_DIM_INFO_HANDLE array_dim_info = va_arg(args, FF_ARRAY_DIM_INFO_HANDLE);

		error = dbask_array_dim_info(dbin, array_var_name, dim_name, array_dim_info);
	}
	break;

	case DBASK_VAR_MINS:
	{
		FF_TYPES_t mins_type = va_arg(args, FF_TYPES_t);
		int num_names = va_arg(args, int);
		char **names_vector = va_arg(args, char **);
		void ***mins_vector = va_arg(args, void ***);

		error = dbask_var_mins(dbin, mins_type, num_names, names_vector, mins_vector);
	}
	break;

	case DBASK_VAR_MAXS:
	{
		FF_TYPES_t maxs_type = va_arg(args, FF_TYPES_t);
		int num_names = va_arg(args, int);
		char **names_vector = va_arg(args, char **);
		void ***maxs_vector = va_arg(args, void ***);

		error = dbask_var_maxs(dbin, maxs_type, num_names, names_vector, maxs_vector);
	}
	break;

	case DBASK_VAR_UNITS:
	{
		int num_names = va_arg(args, int);
		char **names_vector = va_arg(args, char **);
		char ***units_vector = va_arg(args, char ***);

		error = dbask_var_units(dbin, num_names, names_vector, units_vector);
	}
	break;

	case DBASK_VAR_FLAGS:
	{
		FF_TYPES_t flags_type = va_arg(args, FF_TYPES_t);
		int num_names = va_arg(args, int);
		char **names_vector = va_arg(args, char **);
		void ***flags_vector = va_arg(args, void ***);

		error = dbask_var_flags(dbin, flags_type, num_names, names_vector, flags_vector);
	}
	break;

	case DBASK_BYTES_TO_PROCESS:
	{
		PROCESS_INFO_LIST pinfo_list = va_arg(args, PROCESS_INFO_LIST);

		error = dbask_bytes_to_process(pinfo_list);
	}
	break;

	default:
		assert(!ERR_SWITCH_DEFAULT);
		error = err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d", ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__);
	}               

	va_end(args);
	return(error);
}



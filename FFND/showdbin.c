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
 * DESCRIPTION: This function provides access to the attributes and methods
 *                              of the DATA_BINs. The function processes a list of argument
 *                              groups which have the form: attribute [arguments]. The list
 *                              of groups is terminated by a NULL, which ends processing.
 *                              The presently supported attributes and their arguments are:
 *
 * SYSTEM DEPENDENT FUNCTIONS:  Processing of RETURN formats assumes EOL = 2 bytes
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
	        "%s %ld %s%s (%ld byte%s) %s: ",
	        IS_INPUT_TYPE(PINFO_TYPE(pinfo)) ? "Reading" : "Writing",
	        (long)PINFO_SUB_ARRAY_ELS(pinfo),
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
	PROCESS_INFO_PTR ft_ptr = NULL;
	int error = 0;
	
	assert(hpinfo_list);
	
	*hpinfo_list = NULL;
	
	error = create_process_info_list(dbin, format_type, hpinfo_list);

	return(error);
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

	default:
		assert(!ERR_SWITCH_DEFAULT);
		error = err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d", ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__);
	}               

	va_end(args);
	return(error);
}



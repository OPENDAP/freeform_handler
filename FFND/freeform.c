/* FILENAME: freeform.c
 *
 * CONTAINS: 
 * Public functions:
 *
 * create_array_conduit_list_core
 * endian
 * fd_create_format_data
 * fd_destroy_format_data
 * fd_destroy_format_data_list
 * fd_put_format_data
 * ff_create_bufsize
 * ff_create_format
 * ff_create_std_args
 * ff_create_variable
 * ff_destroy_array_conduit
 * ff_destroy_array_conduit_list
 * ff_destroy_bufsize
 * ff_destroy_format
 * ff_destroy_format_info
 * ff_destroy_process_info_list
 * ff_destroy_std_args
 * ff_destroy_variable
 * display_format
 * display_variable
 * display_variable_list
 * ff_find_variable
 * ff_format_to_debug_log
 * ff_lock
 * ff_lock__
 * ff_lookup_number
 * ff_lookup_string
 * ff_resize_bufsize
 * ff_unlock
 * ff_unlock__
 * ffv_type_size
 * learn_about
 * new_name_string__
 *
 * Private functions:
 *
 * ask_EOL_string
 * determine_EOL_type
 * determine_io_type_EOL
 * create_array_conduit
 * create_array_pole
 * destroy_array_pole
 * destroy_mapping
 * graft_format_data
 * find_EOL_var
 * format_has_newlines
 * get_relative_file_offset
 * init_std_args
 * make_tabular_array_conduit
 * make_tabular_format_array_mapping
 * length_in_file
 * make_tabular_format_array_mappings
 * read_EOL_from_file
 * read_EOL_string
 * reset_array_mappings
 * resize_for_EOL
 * set_array_mappings
 * update_sizes
*/

#define WANT_NCSA_TYPES
#include <freeform.h>

BOOLEAN endian(void)
/*****************************************************************************
 * NAME: endian()
 *
 * PURPOSE:  Determine native byte order
 *
 * USAGE: is_little_endian = (endian() == 0)
 *
 * RETURNS:  0 if native byte order is little endian, 1 big endian
 *
 * DESCRIPTION:  Tests the left byte of a short initialized to the value of 1.
 * Little endian machines (Least Significant Byte first) will set the left
 * byte to 1 and the right byte to zero, and big endian machines (MSB) will
 * set the left byte to zero and the right byte to 1.
 *
 * AUTHOR:  unknown
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "endian"
{
	short s = 1;

	if (*(unsigned char *)&s == 1)
		return(0);
	else
		return(1);
}

static void init_std_args(FF_STD_ARGS_PTR std_args)
{
	std_args->input_file = NULL;
	std_args->input_bufsize = NULL;
	std_args->input_format_file = NULL;
	std_args->input_format_buffer = NULL;
	std_args->input_format_title = NULL;
	std_args->output_file = NULL;
	std_args->log_file = NULL;
	std_args->output_bufsize = NULL;
	std_args->output_format_file = NULL;
	std_args->output_format_title = NULL;
	std_args->output_format_buffer = NULL;
	std_args->var_file = NULL;
	std_args->query_file = NULL;
	std_args->cache_size = 0;
	std_args->records_to_read = 0L;

	std_args->error_log = NULL;
	std_args->error_prompt = TRUE;

	std_args->cv_precision = 0;
	std_args->cv_missing_data = 0;
	std_args->cv_maxbins = 0;
	std_args->cv_maxmin_only = FALSE;
	
	std_args->user.set_cv_precision = 0;
	std_args->user.set_cv_missing_data = 0;
	std_args->user.is_stdin_redirected = 0;
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "ff_create_std_args"
FF_STD_ARGS_PTR ff_create_std_args(void)
{	
	FF_STD_ARGS_PTR std_args = NULL;
	
	std_args = (FF_STD_ARGS_PTR)memMalloc(sizeof(FF_STD_ARGS), "std_args");
	if (std_args)
	{
		init_std_args(std_args);
#ifdef FF_CHK_ADDR
		std_args->check_address = (void *)std_args;
#endif
	}
	else
		err_push(ERR_MEM_LACK, NULL);
	
	return(std_args);
}

void ff_destroy_std_args(FF_STD_ARGS_PTR std_args)
{
	assert(std_args);
	if (std_args)
	{
		init_std_args(std_args);
#ifdef FF_CHK_ADDR
		std_args->check_address = NULL;
#endif
	}
	
	memFree(std_args, "std_args");
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "ff_destroy_format_info"

void ff_destroy_process_info(PROCESS_INFO_PTR pinfo)
{
	FF_VALIDATE(pinfo);
	if (pinfo->mate)
		FF_VALIDATE(pinfo->mate);
	
#ifdef FF_CHK_ADDR
	pinfo->check_address = NULL;
#endif

	pinfo->pole = NULL;

	if (pinfo->mate)
	{
#ifdef FF_CHK_ADDR
		pinfo->mate->check_address = NULL;
#endif

		pinfo->mate->pole = NULL;
		pinfo->mate->mate = NULL;

		memFree(pinfo->mate, "pinfo->mate");
	}
	
	memFree(pinfo, "pinfo");
}

void ff_destroy_process_info_list(PROCESS_INFO_LIST pinfo)
{
	assert(pinfo);
	
	dll_free_holdings(pinfo);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "ff_destroy_format"

/*****************************************************************************
 * NAME:  ff_destroy_format
 *
 * PURPOSE:  To free a format and its variable list
 *
 * USAGE:  ff_destroy_format(format_ptr);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  Frees the variable list, the array list,
 * zero's the format's contents and frees it.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

void ff_destroy_format(FORMAT_PTR format)
{
	memTrace("Entering");
	/* Error checking on NULL parameter */
	FF_VALIDATE(format);

#ifdef FF_CHK_ADDR
	format->check_address = NULL;
#endif

	if (format->variables)
	{
		dll_free_holdings(format->variables);
		format->variables = NULL;
	}
	
	format->type = FFF_NULL;
	format->num_vars = 0;
	format->length = 0;

	if (format->name)
	{
		memFree(format->name, "format->name");
		format->name = NULL;
	}

	assert(format->locus);
	if (format->locus)
	{
		memFree(format->locus, "format->locus");
		format->locus = NULL;
	}
	
	memFree(format, "format");
	memTrace("Leaving");
}

/*****************************************************************************
 * NAME: ff_create_format
 *
 * PURPOSE:  Allocate memory, initialize fields of a format structure
 *
 * USAGE:  format = ff_create_format();
 *
 * RETURNS:  NULL if insufficient memory, an empty format structure otherwise.
 *
 * DESCRIPTION:
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_create_format"

FORMAT_PTR ff_create_format
	(
	 char *name,
	 char *origin
	)
{
	FORMAT_PTR format;
	
	format = (FORMAT_PTR)memMalloc(sizeof(FORMAT), "format");
	if (format)
	{
#ifdef FF_CHK_ADDR
		format->check_address = (void *)format;
#endif
		format->variables = NULL;

		if (name)
		{
			format->name = memStrdup(name, "name");
			if (!format->name)
			{
				memFree(format, "format");
				err_push(ERR_MEM_LACK, "new format");
				return(NULL);
			}
		}

		format->locus = memStrdup(origin ? origin : "run-time", "format->locus");
		if (!format->locus)
		{
			memFree(format, "format");
			err_push(ERR_MEM_LACK, "new format");
			return(NULL);
		}

		format->type = FFF_NULL;
		format->num_vars = 0;
		format->length = 0;
	}
	else
		err_push(ERR_MEM_LACK, "new format");
	
	return(format);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_destroy_variable"

/*****************************************************************************
 * NAME: ff_destroy_variable
 *
 * PURPOSE: To free a variable
 *
 * USAGE:  ff_destroy_variable(variable_ptr);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  De-initializes the variable's contents, then frees it.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

void ff_destroy_variable(VARIABLE_PTR variable)
{
	/* Error checking on NULL parameter */
	FF_VALIDATE(variable);

	if (variable->eqn_info)
	{
		ee_free_einfo(variable->eqn_info);
		variable->eqn_info = NULL;
	}

	if (IS_TRANSLATOR_VAR(variable) && variable->misc.nt_trans)
		nt_free_trans(variable->misc.nt_trans);
	
#ifdef FF_CHK_ADDR
	variable->check_address = NULL;
#endif
	variable->misc.nt_trans = FFV_MISC_INIT;

	if (variable->array_desc_str)
	{
		strncpy(variable->array_desc_str, "This variable has been freed", strlen(variable->array_desc_str));
		memFree(variable->array_desc_str, "variable->array_desc_str");
		variable->array_desc_str = NULL;
	}

	variable->type = FFV_NULL;
	variable->start_pos = 0;
	variable->end_pos = 0;
	variable->precision = 0;
	
	memFree(variable->name, "variable->name");
	variable->name = NULL;

	variable->misc.nt_trans = FFV_MISC_INIT;

	if (variable->record_title)
		memFree(variable->record_title, "variable->record_title");

	memFree(variable, "variable");
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_create_variable"

/*****************************************************************************
 * NAME: ff_create_variable
 *
 * PURPOSE: To create a variable
 *
 * USAGE:  ff_create_variable(variable_ptr);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  Allocates and initializes a variable.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

VARIABLE_PTR ff_create_variable(char *name)
{
	VARIABLE_PTR variable;

	variable = (VARIABLE_PTR)memMalloc(sizeof(VARIABLE), "variable");
	if (variable)
	{
#ifdef FF_CHK_ADDR
		variable->check_address = (void *)variable;
#endif
		variable->eqn_info = NULL;

		variable->misc.nt_trans = FFV_MISC_INIT;
	
		variable->name = (char *)memStrdup(name, "name");
		if (!variable->name)
		{
			memFree(variable, "variable");
			err_push(ERR_MEM_LACK, "new variable");
			return(NULL);
		}

		os_str_replace_unescaped_char1_with_char2('%', ' ', variable->name);

		variable->array_desc_str = NULL;
		variable->record_title = NULL;

		variable->type = FFV_NULL;
		variable->start_pos = 0;
		variable->end_pos = 0;
		variable->precision = 0;
		variable->misc.nt_trans = FFV_MISC_INIT;
	}
	else
		err_push(ERR_MEM_LACK, "new variable");
	
	return(variable);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif 
#define ROUTINE_NAME "ff_copy_variable"

/*****************************************************************************
 * NAME: ff_copy_variable
 *
 * PURPOSE:  Duplicate a variable
 *
 * USAGE:  error = ff_copy_variable(source_var, target_var);
 *
 * RETURNS:  zero on success, an error code on failure
 *
 * DESCRIPTION:
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS: Not copying the EQUATION_INFO -- I need to look into this.
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

int ff_copy_variable
	(
	 VARIABLE_PTR source_var,
	 VARIABLE_PTR target_var
	)
{
	int error = 0;
	
	FF_VALIDATE(source_var);
	FF_VALIDATE(target_var);
	
	error = 0;
	if (IS_TRANSLATOR_VAR(source_var) && source_var->misc.nt_trans)
	{
		error = nt_copy_translator_sll(source_var, target_var);
		if (error)
			return(error);
	}

	if (IS_ARRAY(source_var))
	{
		if (target_var->array_desc_str)
			memFree(target_var->array_desc_str, "target_var->array_desc_str");

		target_var->array_desc_str = memStrdup(source_var->array_desc_str, "source_var->array_desc_str");
		if (!target_var->array_desc_str)
		{
			return(error = err_push(ERR_MEM_LACK, ""));
		}
	}

	/* Does source_var have a keyworded variable type?  If it does, record_title contains
	   the keyword name
	*/
	if (source_var->record_title)
	{
		if (target_var->record_title)
			memFree(target_var->record_title, "target_var->record_title");

		target_var->record_title = memStrdup(source_var->record_title, "source_var->record_title");
		if (!target_var->record_title)
		{
			return(error = err_push(ERR_MEM_LACK, ""));
		}
	}

	error = new_name_string__(source_var->name, &target_var->name);
	if (error)
		return(error);

	target_var->type = source_var->type;
	target_var->start_pos = source_var->start_pos;
	target_var->end_pos = source_var->end_pos;
	target_var->precision = source_var->precision;

	return(error);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_create_bufsize"

/*****************************************************************************
 * NAME:  ff_create_bufsize()
 *
 * PURPOSE:  Create an FF_BUFSIZE object
 *
 * USAGE:  bufsize = ff_create_bufsize(size);
 *
 * RETURNS:  NULL if memory allocations failed, otherwise a pointer to an
 * FF_BUFSIZE structure
 *
 * DESCRIPTION:  Allocate memory for FF_BUFSIZE structure.  Allocated buffer
 * field according to size argument.  Initialize bytes_used field to zero and
 * total_bytes field to size argument.
 *
 * If size argument is zero, usage is set to FFBS_GRAFT (zero), indicating unknown
 * usage (externally managed memory is being grafted).  If size argument is not
 * zero, usage is set to one.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

FF_BUFSIZE_PTR ff_create_bufsize(long total_bytes)
{
	FF_BUFSIZE_PTR bufsize = NULL;
	
	assert(total_bytes >= 0);
	assert((unsigned)total_bytes < UINT_MAX);
	assert(total_bytes < LONG_MAX);
	
	if ((unsigned)total_bytes >= UINT_MAX || total_bytes >= LONG_MAX)
	{
		err_push(ERR_PARAM_VALUE, "Requested internal buffer size is set too large");
		return(NULL);
	}
	
	if (total_bytes < 0)
	{
		err_push(ERR_PARAM_VALUE, "Requested internal buffer size is negative");
		return(NULL);
	}

	bufsize = (FF_BUFSIZE_PTR)memMalloc(sizeof(FF_BUFSIZE), "bufsize");
	if (bufsize)
	{
#ifdef FF_CHK_ADDR
		bufsize->check_address = (void *)bufsize;
#endif
		
		bufsize->bytes_used = 0;
		if (total_bytes)
		{
			bufsize->buffer = (char *)memMalloc((size_t)total_bytes, "bufsize->buffer");
			if (bufsize->buffer)
			{
				bufsize->total_bytes = (FF_BSS_t)total_bytes;
				bufsize->usage = 1;
			}
			else
			{
				err_push(ERR_MEM_LACK, "Requesting %ld bytes of memory", total_bytes);
				bufsize->total_bytes = 0;
				memFree(bufsize, "bufsize");
				bufsize = NULL;
			}
		}
		else
		{
			bufsize->total_bytes = 0;
			bufsize->buffer = NULL;
			bufsize->usage = FFBS_GRAFT;
		}
	}
	else
		err_push(ERR_MEM_LACK, "Internal buffer");
	
	return(bufsize);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_resize_bufsize"

/*****************************************************************************
 * NAME:  ff_resize_bufsize()
 *
 * PURPOSE:  Resize an existing FF_BUFSIZE object.
 *
 * USAGE:  error = ff_resize_bufsize(new_size, bufsize_handle);
 *
 * RETURNS:  Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Calls realloc() on the buffer field.  Adjusts bytes_used
 * and total_bytes fields.  If new_size is smaller than bytes_used, bytes_used
 * will be set to new_size -- it seems preferable that the calling routine
 * do this to avoid surprises.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

int ff_resize_bufsize(long new_size, FF_BUFSIZE_HANDLE hbufsize)
{
	char *cp = NULL;

	assert(hbufsize);
	assert(new_size);
	FF_VALIDATE(*hbufsize);

	assert((FF_BSS_t)new_size != (*hbufsize)->total_bytes);
	assert((*hbufsize)->bytes_used <= (*hbufsize)->total_bytes);
	
	assert(new_size >= 0);
	assert((unsigned)new_size < UINT_MAX);
	
	if ((unsigned)new_size >= UINT_MAX)
		return(err_push(ERR_PARAM_VALUE, "Requested internal buffer size is set too big"));
	
	if (new_size < 0)
		return(err_push(ERR_PARAM_VALUE, "Requested internal buffer size is negative"));

	if (!hbufsize ||
	    !new_size ||
	    !*hbufsize
	   )
		return(ERR_PARAM_VALUE);

	if ((FF_BSS_t)new_size == (*hbufsize)->total_bytes)
		return(0);

	assert((*hbufsize)->usage != FFBS_GRAFT);
	
	cp = (char *)memRealloc((*hbufsize)->buffer, (size_t)new_size, "hbufsize-->buffer");
	if (cp)
	{
		(*hbufsize)->buffer = cp;

		if ((*hbufsize)->bytes_used > (FF_BSS_t)new_size)
			(*hbufsize)->bytes_used = (FF_BSS_t)new_size;

		(*hbufsize)->total_bytes = (FF_BSS_t)new_size;

		return(0);
	}
	else
		return(err_push(ERR_MEM_LACK, "resizing smart buffer"));
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_destroy_bufsize"

/*****************************************************************************
 * NAME:  ff_destroy_bufsize()
 *
 * PURPOSE:  Destroy an existing FF_BUFSIZE object.
 *
 * USAGE:  ff_destroy_bufsize(bufsize);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  If bufsize is not NULL and usage equals one, Frees the object's memory blocks, sets bytes_used and
 * total_bytes to zero, and buffer to NULL.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

void ff_destroy_bufsize(FF_BUFSIZE_PTR bufsize)
{
	memTrace("Entering");
	FF_VALIDATE(bufsize);
	
	if (bufsize)
	{
		assert(bufsize->bytes_used <= bufsize->total_bytes);

		if (bufsize->usage == 1)
		{
			if (bufsize->buffer)
			{
				strncpy(bufsize->buffer, "This FreeForm Buffer has been freed", bufsize->total_bytes);
				memFree(bufsize->buffer, "bufsize->buffer");
				bufsize->buffer = NULL;
			}
			bufsize->bytes_used = bufsize->total_bytes = 0;
			
			bufsize->usage = 0;
#ifdef FF_CHK_ADDR
			bufsize->check_address = NULL;
#endif
				
			memFree(bufsize, "bufsize");
		}
		else if (bufsize->usage != FFBS_GRAFT)
			--bufsize->usage;
	}
	memTrace("Leaving");
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "ffv_type_size"

size_t ffv_type_size(FF_TYPES_t var_type)
{
	size_t byte_size = 0;
	
	switch (FFV_DATA_TYPE_TYPE(var_type))
	{
		case FFV_TEXT:
			byte_size = 1; /* unknown, determine elsehow */
		break;
		
		case FFV_INT8:
			byte_size = SIZE_INT8;
		break;
					
		case FFV_UINT8:
			byte_size = SIZE_UINT8;
		break;
					
		case FFV_INT16:
			byte_size = SIZE_INT16;
		break;
					
		case FFV_UINT16:
			byte_size = SIZE_UINT16;
		break;
					
		case FFV_INT32:
			byte_size = SIZE_INT32;
		break;
					
		case FFV_UINT32:
			byte_size = SIZE_UINT32;
		break;
					
		case FFV_INT64:
			byte_size = SIZE_INT64;
		break;
					
		case FFV_UINT64:
			byte_size = SIZE_UINT64;
		break;
					
		case FFV_FLOAT32:
			byte_size = SIZE_FLOAT32;
		break;
					
		case FFV_FLOAT64:
			byte_size = SIZE_FLOAT64;
		break;
		
		case FFV_ENOTE:
			byte_size = SIZE_ENOTE;
		break;
					
		default:
			assert(!ERR_SWITCH_DEFAULT);
			err_push(ERR_SWITCH_DEFAULT, "%d, %s:%d", (int)var_type, os_path_return_name(__FILE__), __LINE__);
			byte_size = 0;
		break;
	}
	
	return(byte_size);
}

#ifdef FF_DBG

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "display_variable"

/*****************************************************************************
 * NAME: display_variable
 *
 * PURPOSE: Display a variable to a text buffer
 *
 * USAGE:  error = display_variable(format->variables, bufsize_ptr);
 *
 * RETURNS:  Zero on success, an error code on failure
 *
 * DESCRIPTION:  Appends to the bufsize a text representation
 * of a variable.  The format looks like:
 *
 * Variable <number>: <name>
 * <type name>
 * <start_pos>
 * <end_pos>
 * <precision>
 * <check_address>
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int display_variable
	(
	 VARIABLE_PTR var,
	 FF_BUFSIZE_PTR bufsize
	)
{
	FF_VALIDATE(var);

	/* print variable name */
	if (bufsize->total_bytes - bufsize->bytes_used + strlen(var->name) < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + strlen(var->name) + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}

	sprintf(bufsize->buffer + bufsize->bytes_used, "\tVariable: %s\n", var->name);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print variable type */
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t\t%s\n", ff_lookup_string(variable_types, var->type));
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print variable start_pos */
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t\t%d\n", (int)var->start_pos);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print variable end_pos */
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t\t%d\n", (int)var->end_pos);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print variable precision */
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t\t%d\n", (int)var->precision);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print variable check_address */
#ifdef FF_CHK_ADDR
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t\t%lX\n", (long)var->check_address);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
#endif

	return(0);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "display_variable_list"

/*****************************************************************************
 * NAME: display_variable_list
 *
 * PURPOSE: Display variables in a list to a text buffer
 *
 * USAGE:  error = display_variable_list(format->variables, bufsize_ptr);
 *
 * RETURNS:  Zero on success, an error code on failure
 *
 * DESCRIPTION:  Appends to the bufsize a text representation of a variable list.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int display_variable_list
	(
	 VARIABLE_LIST vlist,
	 FF_BUFSIZE_PTR bufsize
	)
{
	VARIABLE_PTR var = NULL;
	int error = 0;
	
	vlist = dll_first(vlist);
	var   = FF_VARIABLE(vlist);
	while (var && !error)
	{
		error = display_variable(var, bufsize);

		vlist = FFV_NEXT_VARIABLE(vlist);
		var   = FF_VARIABLE(vlist);
	}
	
	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "display_format"

/*****************************************************************************
 * NAME: display_format
 *
 * PURPOSE: Display a format to a text buffer for debugging
 *
 * USAGE:  error = display_format(format, bufsize_ptr);
 *
 * RETURNS:  Zero on success, an error code on failure
 *
 * DESCRIPTION:  Appends to the bufsize a text representation of a format.
 * The format looks like:
 *
 * Format: <name>
 * <type name>
 * <num_in_list>
 * <max_length>
 * <check_address>
 * <variables...>
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int display_format
	(
	 FORMAT_PTR format,
	 FF_BUFSIZE_PTR bufsize
	)
{
	int error = 0;
	
	FF_VALIDATE(format);

	/* print format name */
	if (bufsize->total_bytes - bufsize->bytes_used + strlen(format->name) < SCRATCH_QUANTA)
	{
		if (ff_resize_bufsize(bufsize->total_bytes + strlen(format->name) + SCRATCH_QUANTA, &bufsize))
			return(ERR_MEM_LACK);
	}

	sprintf(bufsize->buffer + bufsize->bytes_used, "Format: %s\n", format->name);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print format type */
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t%s\n", ff_lookup_string(format_types, format->type));
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print format number in list */
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t%d\n", (int)format->num_vars);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print variable maximum length */
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t%d\n", (int)format->length);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
	
	/* print format check_address */
#ifdef FF_CHK_ADDR
	sprintf(bufsize->buffer + bufsize->bytes_used, "\t%lX\n", (long)format->check_address);
	bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
#endif

	error = display_variable_list(format->variables, bufsize);
	return(error);
}

int ff_format_to_debug_log(FORMAT_PTR format)
{
	int error;
	FF_BUFSIZE_PTR bufsize = NULL;
	
	bufsize = ff_create_bufsize(FF_DBG_LOG_SIZE);
	if (!bufsize)
		return(ERR_MEM_LACK);
	
	error = display_format(format, bufsize);
	if (!error)
		error = ff_bufsize_to_textfile_append(FF_DBG_LOG, bufsize);
	
	ff_destroy_bufsize(bufsize);

	return(error);
}

#endif /* FF_DBG */

/*
 * NAME:	fd_destroy_format_data
 *              
 * PURPOSE:	To free the FORMAT_DATA_PTR structure and associated memory   
 *
 * USAGE:	FORMAT_DATA_PTR fd_destroy_format_data(FORMAT_DATA_PTR)
 *
 * RETURNS:	NULL
 *
 * DESCRIPTION: 
 *
 * SYSTEM DEPENDENT FUNCTIONS:  none
 *
 * GLOBALS:     none
 *
 * AUTHOR:	Ted Habermann, NGDC, (303) 497 - 6472, haber@ngdc.noaa.gov
 *
 * COMMENTS:    
 *
 * KEYWORDS:    dictionary, name table.
 *
 */

#undef ROUTINE_NAME
#define ROUTINE_NAME "fd_destroy_format_data"

void fd_destroy_format_data(FORMAT_DATA_PTR fd)
{
	memTrace("Entering");
	if (!fd)
		return;
	
	FF_VALIDATE(fd);
	assert(!fd->state.locked);
	
	if (fd->data)
		ff_destroy_bufsize(fd->data);
	
	if (fd->format)
		ff_destroy_format(fd->format);

#ifdef FF_CHK_ADDR
	fd->check_address = NULL;
#endif
	fd->format = NULL;
	fd->data = NULL;
	
	fd->state.byte_order = 0;
	fd->state.new_record = 0;
	fd->state.locked     = 0;
	fd->state.unused = ULONG_MAX;
	
	memFree(fd, "fd");
	memTrace("Leaving");
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "fd_create_format_data"

/*****************************************************************************
 * NAME:  fd_create_format_data()
 *
 * PURPOSE:  Create an FORMAT_DATA object
 *
 * USAGE:  format_data = fd_create_format_data(format, data_size);
 *
 * RETURNS:  NULL if memory allocations failed, otherwise a pointer to an
 * FORMAT_DATA structure.
 *
 * DESCRIPTION:  Allocate the FORMAT_DATA structure.  Allocate the bufsize
 * according to size argument.  Initialize bytes_used field to zero and
 * total_bytes field to size argument.  If format is NULL, create a new
 * format, otherwise attach the given format to the FORMAT_DATA.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

FORMAT_DATA_PTR fd_create_format_data
	(
	 FORMAT_PTR format,
	 long data_size,
	 char *name
	)
{
	FORMAT_DATA_PTR format_data = NULL;
	int error = 0;
	
	format_data = (FORMAT_DATA_PTR)memMalloc(sizeof(FORMAT_DATA), "format_data");
	if (!format_data)
		error = err_push(ERR_MEM_LACK, "new format-data");

	if (!error)
	{
#ifdef FF_CHK_ADDR
		format_data->check_address = (void *)format_data;
#endif
		
		/* default data byte_order=native order = endian()
			1=big endian, 0=little endian */
		format_data->state.byte_order = (unsigned char)endian();
		format_data->state.new_record = 0;
		format_data->state.locked     = 0;
		format_data->state.unused     = 0;
	}
	
	format_data->data = ff_create_bufsize(data_size);
	if (!format_data->data)
	{
		error = err_push(ERR_MEM_LACK, "new format-data");
		memFree(format_data, "format_data");
		format_data = NULL;
	}

	if (!error)
	{
		if (format)
		{
			FF_VALIDATE(format);
			format_data->format = format;
		}
		else
		{
			format_data->format = ff_create_format(name, NULL);
			if (!format_data->format)
			{
				error = err_push(ERR_MEM_LACK, "new format-data");
				ff_destroy_bufsize(format_data->data);
				memFree(format_data, "format_data");
				format_data = NULL;
			}
		}
	}
	
	return(format_data);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_create_format_data_mapping"

/*****************************************************************************
 * NAME:  ff_create_format_data_mapping()
 *
 * PURPOSE:  Create an FORMAT_DATA_MAPPING object
 *
 * USAGE:  error = ff_create_format_data_mapping();
 *
 * RETURNS:  Zero on success, error on failure
 *
 * DESCRIPTION:  Allocate the FORMAT_DATA_MAPPING structure.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

int ff_create_format_data_mapping
	(
	 FORMAT_DATA_PTR input,
	 FORMAT_DATA_PTR output,
	 FORMAT_DATA_MAPPING_HANDLE format_data_map_h
	)
{
	int error = 0;
	int error_return = 0;
	
	assert(format_data_map_h);
	assert(*format_data_map_h == NULL);

	FF_VALIDATE(input);
	FF_VALIDATE(output);
	
	*format_data_map_h = (FORMAT_DATA_MAPPING_PTR)memMalloc(sizeof(FORMAT_DATA_MAPPING), "*format_data_map_h");
	if (*format_data_map_h)
	{
		FORMAT_DATA_PTR middle = NULL;

#ifdef FF_CHK_ADDR
		(*format_data_map_h)->check_address = (void *)*format_data_map_h;
#endif
		
		(*format_data_map_h)->input  =  input;
		(*format_data_map_h)->output = output;
		
		middle = fd_create_format_data(NULL, FORMAT_LENGTH(output->format), "middle format data");
		if (!middle)
		{
			err_push(ERR_MEM_LACK, "interim format");
			memFree(*format_data_map_h, "*format_data_map_h");
			*format_data_map_h = NULL;
			return(ERR_MEM_LACK);
		}
	
		error = initialize_middle_data(input, output, middle);
		if (error && error < ERR_WARNING_ONLY)
		{
			fd_destroy_format_data(middle);
			memFree(*format_data_map_h, "*format_data_map_h");
			*format_data_map_h = NULL;
			return(error);
		}
		else if (error)
			error_return = error;
	
		(*format_data_map_h)->middle = middle;
	}
	
	return(error_return);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "ff_destroy_format_data_mapping"

/*****************************************************************************
 * NAME:  ff_destroy_format_data_mapping
 *
 * PURPOSE:  Deallocate memory the middle format and format_data_mapping structure. 
 *
 * USAGE:  ff_destory_format_data_mapping(format_data_mapping);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  Free format_data_mapping->middle and format_data_mapping.  Leave
 * format_data_mapping->input and format_data_mapping->output alone!  These are
 * almost always grafted.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

void ff_destroy_format_data_mapping(FORMAT_DATA_MAPPING_PTR format_data_map)
{
	memTrace("Entering");

	FF_VALIDATE(format_data_map);
	
	if (!format_data_map)
		return;

	if (format_data_map->input)
	{
		FF_VALIDATE(format_data_map->input);
		FF_VALIDATE(format_data_map->input->format);
	}

	FF_VALIDATE(format_data_map->output);
	FF_VALIDATE(format_data_map->output->format);
	
	if (format_data_map->middle)
	{
		FF_VALIDATE(format_data_map->middle);
		fd_destroy_format_data(format_data_map->middle);
	}
	
#ifdef FF_CHK_ADDR
	format_data_map->check_address = NULL;
#endif
	format_data_map->input = NULL;
	format_data_map->middle = NULL;
	format_data_map->output = NULL;
		
	memFree(format_data_map, "format_data_map");
	memTrace("Leaving");
}

static void destroy_mapping(ARRAY_MAPPING_PTR mapping)
{
	if (mapping->sub_array)
	{
		ndarr_free_descriptor(mapping->sub_array);
		mapping->sub_array = NULL;
	}

	if (mapping->super_array)
	{
		ndarr_free_descriptor(mapping->super_array);
		mapping->super_array = NULL;
	}
	
	ndarr_free_mapping(mapping);
	mapping = NULL;
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "destroy_array_pole"

/*****************************************************************************
 * NAME: destroy_array_pole
 *
 * PURPOSE: To free an array pole
 *
 * USAGE:  destroy_array_pole(array_ptr);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

void ff_destroy_array_pole(FF_ARRAY_DIPOLE_PTR pole)
{
	/* Error checking on NULL parameter */
	FF_VALIDATE(pole);

	if (pole)
	{
#ifdef FF_CHK_ADDR
		pole->check_address = NULL;
#endif
		pole->mate = NULL;

		if (pole->format_data_mapping)
			ff_destroy_format_data_mapping(pole->format_data_mapping);
		
		if (pole->array_mapping)
		{
			destroy_mapping(pole->array_mapping);
			pole->array_mapping = NULL;
		}
		
		if (pole->fd)
		{
			fd_destroy_format_data(pole->fd);
			pole->fd = NULL;
		}
		
		if (pole->connect.id & NDARRS_FILE && pole->connect.locus.filename)
		{
			memFree(pole->connect.locus.filename, "pole->connect.locus.filename");
			pole->connect.locus.filename = NULL;
		}

		pole->connect.locus.bufsize = NULL;

		assert(pole->name);
		if (pole->name)
		{
			memFree(pole->name, "pole->name");
			pole->name = NULL;
		}
	
		pole->connect.file_info.array_offset = 0;

		pole->connect.array_done = 0;
		pole->connect.bytes_left = 0;

		memFree(pole, "pole");
	}
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_destroy_array_conduit"

/*****************************************************************************
 * NAME: ff_destroy_array_conduit
 *
 * PURPOSE: To free an array conduit
 *
 * USAGE:  ff_destroy_array_conduit(conduit_ptr);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

void ff_destroy_array_conduit(FF_ARRAY_CONDUIT_PTR conduit)
{
	FF_VALIDATE(conduit);
	
	if (conduit)
	{
#ifdef FF_CHK_ADDR
		conduit->check_address = NULL;
#endif
		
		if (conduit->input)
		{
			ff_destroy_array_pole(conduit->input);
			conduit->input = NULL;

			if (conduit->output)
			{
				if (conduit->output->format_data_mapping)
					conduit->output->format_data_mapping->input = NULL;
			}
		}
		
		if (conduit->output)
		{
			ff_destroy_array_pole(conduit->output);
			conduit->output = NULL;
		}
		
		strcpy(conduit->name, "This array conduit has been freed");
		
		memFree(conduit, "conduit");
	}
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_destroy_array_conduit_list"

/*****************************************************************************
 * NAME: ff_destroy_array_conduit_list
 *
 * PURPOSE: To free an array conduit list
 *
 * USAGE:  ff_destroy_array_conduit_list(conduit_LIST);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  STUBBED
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

void ff_destroy_array_conduit_list(FF_ARRAY_CONDUIT_LIST conduit_list)
{
	dll_free_holdings(conduit_list);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "make_tabular_format_array_mapping"

static int make_tabular_format_array_mapping
	(
	 FF_ARRAY_DIPOLE_PTR pole,
	 long num_records,
	 long records_to_do
	)
{
	char input_desc_str[35];
	char output_desc_str[35];
	ARRAY_DESCRIPTOR_PTR input_desc = NULL;
	ARRAY_DESCRIPTOR_PTR output_desc = NULL;
	
	if (pole)
	{
		long start_record = 0;
		long end_record = 0;

		sprintf(input_desc_str,
		        "[\"t\" 1 to %ld] %u",
				  num_records ? num_records : records_to_do,
		        (unsigned)FORMAT_LENGTH(pole->fd->format)
		       );
		input_desc = ndarr_create_from_str(input_desc_str);
		if (!input_desc)
			return(ERR_GEN_ARRAY);
	
		if (records_to_do >= 0)
		{
			start_record = 1;
			end_record = records_to_do ? records_to_do : num_records;
		}
		else
		{
			start_record = num_records - -records_to_do + 1;
			end_record = num_records;
		}

		sprintf(output_desc_str,
		        "[\"t\" %ld to %ld] %u",
				  start_record,
		        end_record,
		        (unsigned)FORMAT_LENGTH(pole->fd->format)
		       );
		output_desc = ndarr_create_from_str(output_desc_str);
		if (!output_desc)
			return(ERR_GEN_ARRAY);
				
		pole->array_mapping = ndarr_create_mapping(output_desc, input_desc);
		if (!pole->array_mapping)
			return(ERR_GEN_ARRAY);
	}
	
	pole->connect.array_done = 0;

	return(0);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "make_tabular_format_array_mappings"

int make_tabular_format_array_mappings
	(
	 PROCESS_INFO_PTR pinfo,
	 long num_records,
	 long records_to_do
	)
{
	int error = 0;
	
	FF_VALIDATE(pinfo);

	if (PINFO_ARRAY_MAP(pinfo))
		destroy_mapping(PINFO_ARRAY_MAP(pinfo));

	error = make_tabular_format_array_mapping(PINFO_POLE(pinfo), num_records, records_to_do ? records_to_do : num_records);
	if (error)
		return(error);

	PINFO_BYTES_LEFT(pinfo) = PINFO_SUB_ARRAY_BYTES(pinfo);

	if (PINFO_MATE(pinfo))
	{
		if (PINFO_MATE_ARRAY_MAP(pinfo))
			destroy_mapping(PINFO_MATE_ARRAY_MAP(pinfo));

		error = make_tabular_format_array_mapping(PINFO_MATE_POLE(pinfo), records_to_do ? labs(records_to_do) : num_records, records_to_do ? labs(records_to_do) : num_records);
		if (error)
			return(error);

		PINFO_MATE_BYTES_LEFT(pinfo) = PINFO_MATE_SUB_ARRAY_BYTES(pinfo);
	}

	return(0);
}

#if 0
#undef ROUTINE_NAME
#define ROUTINE_NAME "db_clear_array_mappings"

/*****************************************************************************
 * NAME: db_clear_array_mappings
 *
 * PURPOSE: Reset the size and mapping fields of each ARRAY_DIPOLE.
 *
 * USAGE:
 *
 * RETURNS:
 *
 * DESCRIPTION:  Sizes are set to zero, and mappings are free'd.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int reset_array_mappings
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t io_type
	)
{
	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR pinfo = NULL;

	int error = 0;

	error = db_show(dbin, DBASK_PROCESS_INFO, io_type, &pinfo_list);
	if (error)
		return(error);
	
	pinfo_list = dll_first(pinfo_list);
	pinfo = FF_PI(pinfo_list);
	while (pinfo)
	{
		FF_VALIDATE(pinfo)

		PINFO_ARRAY_OFFSET(pinfo) = 0;
		
		if (PINFO_ARRAY_MAP(pinfo))
		{
			destroy_mapping(PINFO_ARRAY_MAP(pinfo));
			PINFO_ARRAY_MAP(pinfo) = NULL;
		}

		pinfo_list = dll_next(pinfo_list);
		pinfo = FF_PI(pinfo_list);
	}

	ff_destroy_process_info_list(pinfo_list);

	return(0);
}

int db_clear_array_mappings
	(
	 DATA_BIN_PTR dbin
	)
{
	int error = 0;

	error = reset_array_mappings(dbin, FFF_INPUT);
	if (error)
		return(error);

	error = reset_array_mappings(dbin, FFF_OUTPUT);
	if (error)
		return(error);

	return(0);
}
#endif /* 0 */


int new_name_string__
	(
	 const char *new_name,
	 FF_STRING_HANDLE name_h
	)
{
	assert(name_h);
	assert(*name_h);
	assert(new_name);

	if (strlen(*name_h) < strlen(new_name))
	{
		char *cp = memRealloc(*name_h, strlen(new_name) + 1, "*name_h");
		if (!cp)
			return(err_push(ERR_MEM_LACK, "changing name of object"));

		*name_h = cp;
	}


	memStrcpy(*name_h, new_name, "*name_h, new_name");
	
	return(0);
}


/* FILENAME: dbevents.c
 *
 * CONTAINS: db_do()
 *           write_output_fmt_file (a module-level function to db_do)
 *           read_formats
 *           convert_formats
 *
 */

/*
 * NAME:        db_do
 *              
 * PURPOSE:     Handle events for data bins.
 *
 * USAGE:       db_do(DATA_BIN_PTR, event, [args])
 *
 * RETURNS:     0 if all goes well.
 *
 * DESCRIPTION: This function handles events which are sent to data bins.
 * The function processes a list of argument groups which have
 * the form: event [arguments].
 * The presently supported events and their arguments are:
 *
 *
 * ERRORS:
 *
 * SYSTEM DEPENDENT FUNCTIONS: 
 *
 * AUTHOR:      T. Habermann, NGDC, (303) 497 - 6472, haber@ngdc.noaa.gov
 *
 * COMMENTS: 
 *
 * KEYWORDS: databins
 *
*/

#include <freeform.h>

#ifdef DBDO_WRITE_OUTPUT_FMT_FILE

static long digit_count(long l)
{
	return( l ? (long)(log10(labs(l)) + 1) : 1);
}

/*
 * NAME:	ff_list_vars
 *
 * PURPOSE:	List the variables in a format into text_buffer.
 *										     
 * USAGE:	ff_list_vars(FORMAT_PTR format, char *text_buffer)
 *
 * RETURNS:	Zero on success, an error code on failure
 *
 * DESCRIPTION:	This function writes the format specification information
 * into a text buffer. The text buffer consists of lines (separated with the
 * newline character, i.e., '\n') with the following fields for each variable:
 *
 * 1) name,
 * 2) start position,
 * 3) end position,
 * 4) type, and
 * 5) precision
 * 
 * Each field is separated by space characters (' ', i.e., '\x20').  Each
 * field is justified within a fixed field whose width is determined by the
 * maximum length for that field in the format specification (nice fixed 
 * columns).
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
#define ROUTINE_NAME "ff_list_vars"

static int ff_list_vars(FORMAT_PTR format, FF_BUFSIZE_PTR bufsize)
{
	int var_fw        = 0; /* name field width */
	int start_pos_fw  = 0; /* start pos field width */
	int end_pos_fw    = 0; /* end pos field width */
	int type_fw       = 0; /* type field width */
	int prec_fw       = 0; /* precision field width */
	VARIABLE_LIST  v_list        = NULL;
	VARIABLE_PTR var = NULL;
	BOOLEAN            any_neg_prec  = FALSE;

	/* Error checking on NULL parameters and corrupt memory */
	FF_VALIDATE(format);
	FF_VALIDATE(bufsize);

	assert(bufsize->bytes_used <= bufsize->total_bytes);

	any_neg_prec = FALSE;
	v_list = FFV_FIRST_VARIABLE(format);
	var = FF_VARIABLE(v_list);
	while (var)
	{
		var_fw = max(var_fw, (int)strlen(IS_EOL_VAR(var) ? "newline" : var->name));
		start_pos_fw = max(start_pos_fw, (int)digit_count(var->start_pos));
		end_pos_fw = max(end_pos_fw, (int)digit_count(var->end_pos));
		type_fw = max(type_fw, (int)strlen(ff_lookup_string(variable_types, var->type)));
		prec_fw = max(prec_fw, (int)digit_count(FF_VARIABLE(v_list)->precision));
		if (FF_VARIABLE(v_list)->precision < 0)
			any_neg_prec = TRUE;

		v_list = dll_next(v_list);
		var = FF_VARIABLE(v_list);
	}
	
	if (any_neg_prec)
		prec_fw++;

	v_list = FFV_FIRST_VARIABLE(format);
	var = FF_VARIABLE(v_list);
	while (var)
	{
		FF_NDX_t start_pos;
		FF_NDX_t end_pos;
		
		if (IS_VARIED(format))
		{
			start_pos = 0;
			end_pos   = 0;
		}
		else
		{
			start_pos = var->start_pos;
			end_pos   = var->end_pos;
		}
		
		os_str_replace_unescaped_char1_with_char2(' ', '%', var->name);

		if (bufsize->total_bytes - bufsize->bytes_used < SCRATCH_QUANTA)
		{
			if (ff_resize_bufsize(bufsize->total_bytes + SCRATCH_QUANTA, &bufsize))
				return(0);
		}

		sprintf(bufsize->buffer + bufsize->bytes_used,
		        "%-*s %*hu %*hu %*s %*hd\n",
		        var_fw, IS_EOL_VAR(var) ? "newline" : var->name,
		        start_pos_fw, start_pos,
		        end_pos_fw, end_pos,
		        type_fw, ff_lookup_string(variable_types, var->type),
		        prec_fw, var->precision
		       );

		os_str_replace_unescaped_char1_with_char2('%', ' ', var->name);

		bufsize->bytes_used += strlen(bufsize->buffer + bufsize->bytes_used);
		assert(bufsize->bytes_used <= bufsize->total_bytes);

		v_list = dll_next(v_list);
		var = FF_VARIABLE(v_list);
	}

	strcat(bufsize->buffer + bufsize->bytes_used, "\n");
	bufsize->bytes_used++;

	return(0);
}

/*****************************************************************************
 * NAME:  ff_show_format_description()
 *
 * PURPOSE:  Show the format description, with exact syntax
 *
 * USAGE:  ff_show_format_description(format, bufsize);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  Much like ff_show_format(), except this function returns
 * a text buffer which is syntactically correct, such that it can be parsed
 * by db_make_format_list().
 *
 * Any new text is appended to bufsize.
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

static int ff_show_format_description(FORMAT_PTR format, FF_BUFSIZE_HANDLE hbufsize)
{
	int error = 0;
	
	FF_VALIDATE(format);
	assert(hbufsize);
	FF_VALIDATE(*hbufsize);
	assert((*hbufsize)->bytes_used <= (*hbufsize)->total_bytes);
	
	*hbufsize = ff_create_bufsize(SCRATCH_QUANTA);
	if (!*hbufsize)
		return(ERR_MEM_LACK);
	
	sprintf((*hbufsize)->buffer + (*hbufsize)->bytes_used, "%s\t\"%s\"\n",
	        ff_lookup_string(format_types, FFF_TYPE(format)),
	        format->name
	       );
	
	(*hbufsize)->bytes_used += strlen((*hbufsize)->buffer + (*hbufsize)->bytes_used);
	assert((*hbufsize)->bytes_used <= (*hbufsize)->total_bytes);

	error = ff_list_vars(format, *hbufsize);
	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "write_output_fmt_file"

/*****************************************************************************
 * NAME: write_output_fmt_file
 *
 * PURPOSE:  Write output format file for output data file
 *
 * USAGE:  error = write_output_fmt_file(dbin, fmt_filename);
 *
 * RETURNS:  Zero on success, error code on failure.
 *
 * DESCRIPTION:  Takes the dbin's format list and writes all formats to
 * output format file, with input formats becoming output formats, and
 * vice versa.
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
 * ERRORS:  Writes only the default formats to the output format file.  If
 * the user selects a non-defaulting format file, or format titles, the wrong
 * formats will be written.
 ****************************************************************************/

static int write_output_fmt_file(DATA_BIN_PTR dbin, char *fmtfname)
{
	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR pinfo = NULL;
	int error = 0;
	
	FF_BUFSIZE_PTR bufsize = NULL;

	FF_VALIDATE(dbin);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_NULL, &pinfo_list);
	if (error)
		return(error);
	
	pinfo_list = dll_first(pinfo_list);
	pinfo      = FF_PI(pinfo_list);
	while (pinfo)
	{
		FORMAT_PTR format = NULL;
		FF_TYPES_t format_type = FFF_NULL;

		format = PINFO_FORMAT(pinfo);

		format_type = FFF_TYPE(format);
			
		if (IS_INPUT(format))
		{
			format->type &= ~FFF_INPUT;
			format->type |= FFF_OUTPUT;
		}
		else if (IS_OUTPUT(format))
		{
			format->type &= ~FFF_OUTPUT;
			format->type |=   FFF_INPUT;
		}

		error = ff_show_format_description(format, &bufsize);
		if (error)
			return(error);

		format->type = format_type;
				
		pinfo_list = dll_next(pinfo_list);
		pinfo      = FF_PI(pinfo_list);
	}
	
	error = ff_bufsize_to_textfile_overwrite(fmtfname, bufsize);
	
	ff_destroy_process_info_list(pinfo_list);
	ff_destroy_bufsize(bufsize);
	return(error);
}
#endif /* WRITE_OUTPUT_FMT_FILE */

static int ff_lock__
	(
	 PROCESS_INFO_PTR pinfo,
	 void **hbuffer,
	 unsigned long *pused,
	 unsigned long *pcapacity
	)
{
	assert(!PINFO_LOCKED(pinfo));
	
	if (hbuffer)
		*hbuffer = (char HUGE *)PINFO_BUFFER(pinfo);
	
	if (pused && PINFO_NEW_RECORD(pinfo))
		*pused = (unsigned long)PINFO_BYTES_USED(pinfo);
	else if (pused)
		*pused = 0;
	
	if (pcapacity)
		*pcapacity = (unsigned long)PINFO_TOTAL_BYTES(pinfo) - 1;

	PINFO_LOCKED(pinfo) = 1;
		
	return(0);
}

static int ff_unlock__
	(
	 PROCESS_INFO_PTR pinfo,
	 void **hbuffer,
	 long size
	)
{
	assert(PINFO_LOCKED(pinfo));

	if (hbuffer)
	{
		assert((char HUGE *)*hbuffer == (char HUGE *)PINFO_BUFFER(pinfo));
		*hbuffer = NULL;
	}
   
	PINFO_LOCKED(pinfo) = 0;
	
	if (size)
	{
		PINFO_BYTES_USED(pinfo) = (FF_BSS_t)size;
		PINFO_NEW_RECORD(pinfo) = 1; /* buffer has been filled */
	}
	else
	{
		PINFO_BYTES_USED(pinfo) = 0;
		PINFO_NEW_RECORD(pinfo) = 0; /* buffer has been used and discarded */
	}
		
	return(0);
}

static int ff_unlock
	(
	 PROCESS_INFO_PTR pinfo,
	 void **hbuffer
	)
{
	int error = 0;

	assert(hbuffer);
	assert(*hbuffer);
	
	error = ff_unlock__(pinfo, hbuffer, 0L);
	if (error)
		return(error);
		
	return(0);
}

/*****************************************************************************
 * NAME: ff_lock
 *
 * PURPOSE: To lock a buffer of data for use
 *
 * USAGE:  error = ff_lock(pinfo, &buffer, &size);
 *
 * RETURNS:  Zero on success, an error code if buffer cannot be locked.
 *
 * DESCRIPTION:  buffer and size will be set according to information in
 * pinfo.  pinfo is a format-info item that is an element in a list of
 * format-info's created by calling db_show, e.g.,
 * error = db_show(dbin, SHOW_FORMAT_INFO, &pinfo_list);
 *
 * Call ff_unlock to release the buffer.  Processing cannot continue until
 * the buffer is unlocked, as its memory must be reused.  This means that
 * while you have the buffer locked this is your one chance to access its
 * data.  Attempting to lock the buffer again after unlocking it will either
 * fail, or will lock the buffer with the buffer containing different data.
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

static int ff_lock
	(
	 PROCESS_INFO_PTR pinfo,
	 void **hbuffer,
	 unsigned long *psize
	)
{
	int error = 0;
	
	assert(hbuffer);
	assert(psize);

	error = ff_lock__(pinfo, hbuffer, psize, NULL);
	if (error)
		return(error);
	
	if (!*psize)
		PINFO_LOCKED(pinfo) = 0;
		
	return(0);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "write_formats"

/*****************************************************************************
 * NAME:
 *
 * PURPOSE:
 *
 * USAGE:
 *
 * RETURNS:
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

static int write_formats
	(
	 PROCESS_INFO_LIST pinfo_list
	)
{
	unsigned long bytes_to_write = 0;
	unsigned long bytes_written = 0;
	unsigned output_length = 0;

	int error = 0;

	PROCESS_INFO_PTR input_pinfo = NULL;
	
	pinfo_list = dll_first(pinfo_list);
	input_pinfo = FF_PI(pinfo_list);
	while (input_pinfo && !error)
	{
		PROCESS_INFO_PTR pinfo = NULL;

		FF_VALIDATE(input_pinfo);
		
		pinfo = PINFO_MATE(input_pinfo);
		if (pinfo)
		{
			void *buffer = NULL;
			
			if (!PINFO_NEW_RECORD(pinfo))
			{
				pinfo_list = dll_next(pinfo_list);
				input_pinfo = FF_PI(pinfo_list);
							
				continue;
			}

			error = ff_lock(pinfo, &buffer, &bytes_to_write);
			if (error)
				return(error);
			
			if (PINFO_LOCUS(pinfo))
			{
				bytes_written = ndarr_reorient(PINFO_ARRAY_MAP(pinfo),
														 NDARRS_BUFFER,
														 buffer,
														 bytes_to_write,
														 PINFO_ID(pinfo),
														 PINFO_LOCUS(pinfo),
														 PINFO_IS_FILE(pinfo) ? PINFO_ARRAY_OFFSET(pinfo) : PINFO_BUFFER_SIZE(pinfo),
														 &PINFO_ARRAY_DONE(pinfo)
														);
			}
			else
			{
				bytes_written = fwrite(buffer, 1, (size_t)bytes_to_write, stdout);
			}

			/* bytes_written might be more than bytes_to_write if separation on output */
			if ((long)bytes_written == -1 || bytes_written < bytes_to_write)
				error = err_push(ERR_WRITE_FILE, "Writing to \"%s\" (processing \"%s\")", PINFO_FNAME(pinfo), PINFO_NAME(pinfo));

			ff_unlock(pinfo, &buffer);

			PINFO_BYTES_LEFT(pinfo) -= bytes_written;

			if (IS_DATA(PINFO_FORMAT(pinfo)) && PINFO_ARRAY_DONE(pinfo) && !IS_ARRAY(PINFO_FORMAT(pinfo)))
			{
				PROCESS_INFO_PTR rech_pinfo = FF_PI(dll_previous(pinfo_list));
				if (rech_pinfo)
					rech_pinfo = PINFO_MATE(rech_pinfo);

				if (rech_pinfo && pinfo)
				{
					if (IS_SEPARATE_TYPE(PINFO_TYPE(rech_pinfo)))
					{
						if (PINFO_IS_FILE(rech_pinfo))
							PINFO_ARRAY_OFFSET(rech_pinfo) += PINFO_RECL(rech_pinfo);
						else
							assert(PINFO_IS_FILE(rech_pinfo));

						if (PINFO_IS_FILE(pinfo))
							PINFO_ARRAY_OFFSET(pinfo) += PINFO_SUB_ARRAY_BYTES(pinfo);
						else
							assert(PINFO_IS_FILE(pinfo));
					}
					else
					{
						if (PINFO_IS_FILE(rech_pinfo))
							PINFO_ARRAY_OFFSET(rech_pinfo) += PINFO_RECL(rech_pinfo) + PINFO_SUB_ARRAY_BYTES(pinfo);
						else
							assert(PINFO_IS_FILE(rech_pinfo));

						if (PINFO_IS_FILE(pinfo))
							PINFO_ARRAY_OFFSET(pinfo) = PINFO_ARRAY_OFFSET(rech_pinfo) + PINFO_RECL(rech_pinfo);
						else
							assert(PINFO_IS_FILE(pinfo));
					}
				}
				else if (rech_pinfo)
				{
					if (PINFO_IS_FILE(rech_pinfo))
						PINFO_ARRAY_OFFSET(rech_pinfo) += PINFO_RECL(rech_pinfo);
					else
						assert(PINFO_IS_FILE(rech_pinfo));
				}
				else if (pinfo)
				{
					if (PINFO_IS_FILE(pinfo))
						PINFO_ARRAY_OFFSET(pinfo) += PINFO_SUB_ARRAY_BYTES(pinfo);
					else
						assert(PINFO_IS_FILE(pinfo));
				}
			} /* if IS_DATA(PINFO_FORMAT(pinfo)) && PINFO_ARRAY_DONE(pinfo) && !IS_ARRAY(PINFO_FORMAT(pinfo)) */
		}

		pinfo_list = dll_next(pinfo_list);
		input_pinfo = FF_PI(pinfo_list);
	} /* End of format_list processing (while) */

	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbin_convert_data"

static int dbin_convert_data(FORMAT_DATA_MAPPING_PTR format_data_mapping)
{
	int error;
	
	long output_bytes = 0L;

	FF_VALIDATE(format_data_mapping);
			
	output_bytes = (format_data_mapping->input->data->bytes_used /
	                FORMAT_LENGTH(format_data_mapping->input->format)
	               ) * FORMAT_LENGTH(format_data_mapping->output->format);
	if ((unsigned long)output_bytes > (unsigned long)UINT_MAX)
		return(err_push(ERR_MEM_LACK, "reallocation size for output data is too big"));
					
	if ((FF_BSS_t)output_bytes >= format_data_mapping->output->data->total_bytes)
	{
		assert(!format_data_mapping->output->state.locked);
		
		if (ff_resize_bufsize(output_bytes + 1, &format_data_mapping->output->data))
			return(err_push(ERR_MEM_LACK, "reallocation of buffer"));
	}

	error = ff_process_format_data_mapping(format_data_mapping);

	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "convert_formats"

static int convert_formats
	(
	 DATA_BIN_PTR dbin,
	 PROCESS_INFO_LIST pinfo_list
	)
{ 
	int error = 0;
	
	PROCESS_INFO_PTR input_pinfo = NULL;
	
	FF_TYPES_t format_type = FFF_NULL;
	
	FF_VALIDATE(pinfo_list);
	
	pinfo_list = dll_first(pinfo_list);
	input_pinfo = FF_PI(pinfo_list);

	while (input_pinfo)
	{
		PROCESS_INFO_PTR pinfo = NULL;
		FF_VALIDATE(input_pinfo);

		pinfo = PINFO_MATE(input_pinfo);
		if (pinfo)
		{
			FF_VALIDATE(pinfo);
			
			if (PINFO_NEW_RECORD(input_pinfo))
			{
				error = db_do(dbin, DBDO_CONVERT_DATA, PINFO_FORMAT_MAP(pinfo));
				if (error)
					return(error);

				error = db_do(dbin, DBDO_BYTE_SWAP, PINFO_FD(pinfo));
				if (error)
					return(error);

				PINFO_NEW_RECORD(input_pinfo) = 0;
			}
		}

		pinfo_list = dll_next(pinfo_list);
		input_pinfo = FF_PI(pinfo_list);
	} /* while input_format_data and the right type */
	
	return(0);
}

#define IS_INPUT_FILE_HEADER(f) (IS_INPUT_TYPE(PINFO_TYPE(f)) && \
                                    IS_HEADER_TYPE(PINFO_TYPE(f)) && \
                                  IS_FILE_TYPE(PINFO_TYPE(f)) \
                                )
#define IS_INPUT_DATA(f) (IS_INPUT_TYPE(PINFO_TYPE(f)) && \
                          IS_DATA_TYPE(PINFO_TYPE(f)) \
                         )
#define WAIT_FOR_NEXT_RECORD_HEADER(f) (IS_INPUT_TYPE(PINFO_TYPE(f)) && \
                                        IS_HEADER_TYPE(PINFO_TYPE(f)) && \
                                        IS_REC_TYPE(PINFO_TYPE(f)) \
                                       )
static BOOLEAN need_to_read_next_record_header
	(
	 PROCESS_INFO_PTR pinfo,
	 DATA_BIN_PTR dbin
	)
{
	FF_VALIDATE(pinfo);
	FF_VALIDATE(dbin);

	if (IS_INPUT_TYPE(PINFO_TYPE(pinfo)) && 
	    IS_HEADER_TYPE(PINFO_TYPE(pinfo)) && 
	    IS_REC_TYPE(PINFO_TYPE(pinfo))
	   )
	{
		PROCESS_INFO_LIST pinfo_list = NULL;

		int error = 0;
		
		error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA, &pinfo_list);
		if (!error)
		{
 			PROCESS_INFO_PTR data_pinfo = NULL;
			BOOLEAN answer = FALSE;

			data_pinfo = FF_PI(dll_first(pinfo_list));

			if (PINFO_ARRAY_DONE(data_pinfo))
				answer = TRUE;

			ff_destroy_process_info_list(pinfo_list);

			return(answer);
		}
 	}
	else
	{
		return(FALSE);
	}

}

/*****************************************************************************
 * NAME:
 *
 * PURPOSE:
 *
 * USAGE:
 *
 * RETURNS:
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

static int check_for_array_EOF(PROCESS_INFO_LIST pinfo_list)
{
	PROCESS_INFO_PTR pinfo = NULL;

	FF_VALIDATE(pinfo_list);

	pinfo_list = dll_first(pinfo_list);
	pinfo      = FF_PI(pinfo_list);
	while (pinfo)
	{
		FF_VALIDATE(pinfo);

		if (!IS_ARRAY(PINFO_FORMAT(pinfo)) && IS_DATA(PINFO_FORMAT(pinfo)))
			return(0);

		if (!PINFO_ARRAY_DONE(pinfo))
			return(0);
			
		pinfo_list = dll_next(pinfo_list);
		pinfo      = FF_PI(pinfo_list);
	}

	return(EOF);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "read_formats"

/*****************************************************************************
 * NAME:
 *
 * PURPOSE:
 *
 * USAGE:
 *
 * RETURNS:
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

static int read_formats
	(
	 DATA_BIN_PTR dbin,
	 PROCESS_INFO_LIST pinfo_list
	)
{
	unsigned long bytes_to_read = 0;
	unsigned long bytes_read = 0;

	long count = 1L;
	
	int error = 0;

	PROCESS_INFO_PTR pinfo = NULL;

	FF_VALIDATE(pinfo_list);
	
	pinfo_list = dll_first(pinfo_list);
	pinfo = FF_PI(pinfo_list);
	while (pinfo && !error)
	{
		void *buffer = NULL;
		unsigned long bsize = 0;
		unsigned long bcapacity = 0;
		
		FF_VALIDATE(pinfo);
		
		if (PINFO_NEW_RECORD(pinfo) || !PINFO_LOCUS(pinfo) /* kludge */)
		{
			pinfo_list = dll_next(pinfo_list);
			pinfo = FF_PI(pinfo_list);
						
			continue;
		}

		error = ff_lock__(pinfo, &buffer, &bsize, &bcapacity);
		if (error)
			return(error);
		
		if (IS_INPUT_FILE_HEADER(pinfo))
		{
			if (PINFO_ARRAY_DONE(pinfo))
			{
				error = ff_unlock__(pinfo, &buffer, 0);
				if (error)
					return(error);
					
				pinfo_list = dll_next(pinfo_list);
				pinfo = FF_PI(pinfo_list);
						
				continue;
			}

			bytes_to_read = PINFO_RECL(pinfo);
			if (bytes_to_read >= PINFO_TOTAL_BYTES(pinfo))
			{
				error = ff_resize_bufsize(bytes_to_read + 1, &PINFO_DATA(pinfo));
				if (error)
					goto read_formats_exit;

				buffer = PINFO_BUFFER(pinfo);
			}

			bytes_read = ndarr_reorient(PINFO_ARRAY_MAP(pinfo),
			                            PINFO_ID(pinfo),
			                            PINFO_LOCUS(pinfo),
												 PINFO_IS_FILE(pinfo) ? PINFO_ARRAY_OFFSET(pinfo) : bytes_to_read,
			                            NDARRS_BUFFER,
			                            buffer,
			                            bytes_to_read,
			                            &PINFO_ARRAY_DONE(pinfo)
			                           );
			if (bytes_read != bytes_to_read)
			{
				error = err_push(ERR_READ_FILE, "Reading file header");
				goto read_formats_exit;
			}

			assert(PINFO_ARRAY_DONE(pinfo));

			ff_unlock__(pinfo, &buffer, (long)bytes_read);

			PINFO_BYTES_LEFT(pinfo) -= bytes_read;
			assert(PINFO_BYTES_LEFT(pinfo) == 0);

			error = db_do(dbin, DBDO_BYTE_SWAP, PINFO_FD(pinfo));
			if (error)
			{
				err_push(ERR_PROCESS_DATA, "Byte-swapping to native byte order");
				goto read_formats_exit;
			}
		} /* if input header file format */
		else if (need_to_read_next_record_header(pinfo, dbin))
		{
			bytes_to_read = PINFO_RECL(pinfo);
			if (bytes_to_read >= PINFO_TOTAL_BYTES(pinfo))
			{
				error = ff_resize_bufsize(bytes_to_read + 1, &PINFO_DATA(pinfo));
				if (error)
					goto read_formats_exit;

				buffer = PINFO_BUFFER(pinfo);
			}

			error = make_tabular_format_array_mappings(pinfo, 1, 1);
			if (error)
				goto read_formats_exit;
					
			assert(PINFO_BYTES_LEFT(pinfo) == PINFO_RECL(pinfo));
			if (PINFO_MATE(pinfo))
				assert(PINFO_MATE_BYTES_LEFT(pinfo) == PINFO_MATE_RECL(pinfo));

			bytes_read = ndarr_reorient(PINFO_ARRAY_MAP(pinfo),
			                            PINFO_ID(pinfo),
			                            PINFO_LOCUS(pinfo),
												 PINFO_IS_FILE(pinfo) ? PINFO_ARRAY_OFFSET(pinfo) : bytes_to_read,
			                            NDARRS_BUFFER,
			                            buffer,
			                            bytes_to_read,
			                            &PINFO_ARRAY_DONE(pinfo)
			                           );
			if (bytes_read != bytes_to_read)
			{
				if (bytes_read)
					error = err_push(ERR_READ_FILE, "Reading record header");
				else
				{
					error = EOF;
					PINFO_BYTES_LEFT(pinfo) -= PINFO_RECL(pinfo);
					PINFO_MATE_BYTES_LEFT(pinfo) -= PINFO_MATE_RECL(pinfo);
				}

				goto read_formats_exit;
			}

			assert(PINFO_ARRAY_DONE(pinfo));

			ff_unlock__(pinfo, &buffer, (long)bytes_read);

			if (IS_SEPARATE_TYPE(PINFO_TYPE(pinfo)))
			{
				if (PINFO_IS_FILE(pinfo))
					PINFO_ARRAY_OFFSET(pinfo) += bytes_read;
				else
					assert(PINFO_IS_FILE(pinfo));
			}
			/* else don't know next offset -- it will be set when handling data format */

			PINFO_BYTES_LEFT(pinfo) -= bytes_read;

			/* Must swap bytes before using record header */
			error = db_do(dbin, DBDO_BYTE_SWAP, PINFO_FD(pinfo));
			if (error)
			{
				err_push(ERR_PROCESS_DATA, "Byte-swapping to native byte order");
				goto read_formats_exit;
			}
		} /* else if need_to_read_next_record_header(pinfo, dbin) */
		else if (IS_INPUT_DATA(pinfo))
		{
			if (PINFO_ARRAY_DONE(pinfo) && IS_ARRAY(PINFO_FORMAT(pinfo)))
			{
				error = ff_unlock__(pinfo, &buffer, 0);
				if (error)
					return(error);
					
				pinfo_list = dll_next(pinfo_list);
				pinfo = FF_PI(pinfo_list);
						
				continue;
			}
			else if (PINFO_ARRAY_DONE(pinfo))
			{
				PROCESS_INFO_PTR rech_pinfo = FF_PI(dll_previous(pinfo_list));
				if (rech_pinfo && IS_REC_TYPE(PINFO_TYPE(rech_pinfo)) && IS_HEADER_TYPE(PINFO_TYPE(rech_pinfo)))
				{
					unsigned long record_count = 0;
					unsigned long previous_input_array_size = 0;

					if (nt_ask(dbin, FFF_REC, "count", FFV_LONG, &record_count) &&
						 nt_ask(dbin, NT_ANYWHERE, "record_count", FFV_LONG, &record_count)
						)
					{
						error = err_push(ERR_GENERAL, "Cannot get record count");
						goto read_formats_exit;
					}

					assert(record_count > 0);
					if (record_count <= 0)
					{
						error = err_push(ERR_PARAM_VALUE, "Expecting a positive record count (was %ld)", record_count);
						goto read_formats_exit;
					}
					
					if (PINFO_ARRAY_MAP(pinfo)) /* virginity clause */
						previous_input_array_size = PINFO_SUPER_ARRAY_BYTES(pinfo);
					else
						previous_input_array_size = 0;

					error = make_tabular_format_array_mappings(pinfo, record_count, record_count);
					if (error)
						goto read_formats_exit;
					
					assert(PINFO_BYTES_LEFT(pinfo) == record_count * PINFO_RECL(pinfo));
					if (PINFO_MATE(pinfo))
						assert(PINFO_MATE_BYTES_LEFT(pinfo) == record_count * PINFO_MATE_RECL(pinfo));

					if (IS_SEPARATE_TYPE(PINFO_TYPE(rech_pinfo)))
					{
						if (PINFO_IS_FILE(pinfo))
							PINFO_ARRAY_OFFSET(pinfo) += previous_input_array_size;
						else
							assert(PINFO_IS_FILE(pinfo));

						if (PINFO_IS_FILE(rech_pinfo))
							PINFO_ARRAY_OFFSET(rech_pinfo) += PINFO_RECL(rech_pinfo);
						else
							assert(PINFO_IS_FILE(rech_pinfo));
					}
					else
					{
						/* set file offset for data format's next read based on file offset of last read
							record header and its length
						*/
						if (PINFO_IS_FILE(pinfo) && PINFO_IS_FILE(rech_pinfo))
							PINFO_ARRAY_OFFSET(pinfo) = PINFO_ARRAY_OFFSET(rech_pinfo) + PINFO_RECL(rech_pinfo);
						else
							assert(PINFO_IS_FILE(pinfo) && PINFO_IS_FILE(rech_pinfo));

						/* set file offset for record header's next read based on its old
							file offset, its record length, and the data block length
						*/
						if (PINFO_IS_FILE(rech_pinfo))
							PINFO_ARRAY_OFFSET(rech_pinfo) += PINFO_RECL(rech_pinfo) + PINFO_BYTES_LEFT(pinfo);
						else
							assert(PINFO_IS_FILE(rech_pinfo));
					}
				} /* if rech_pinfo && IS_REC_TYPE(PINFO_TYPE(rech_pinfo)) && IS_HEADER_TYPE(PINFO_TYPE(rech_pinfo)) */
				else
				{
					error = EOF;
					goto read_formats_exit;
				}
			} /* if PINFO_ARRAY_DONE(pinfo) */

			bytes_to_read = min(PINFO_BYTES_LEFT(pinfo), bcapacity);
			count = bytes_to_read / PINFO_RECL(pinfo);
			bytes_to_read = (unsigned)(count * PINFO_RECL(pinfo));

			assert(bytes_to_read <= bcapacity);
			assert(bytes_to_read < PINFO_TOTAL_BYTES(pinfo));

			bytes_read = ndarr_reorient(PINFO_ARRAY_MAP(pinfo),
			                            PINFO_ID(pinfo),
			                            PINFO_LOCUS(pinfo),
												 PINFO_IS_FILE(pinfo) ? PINFO_ARRAY_OFFSET(pinfo) : bytes_to_read,
			                            NDARRS_BUFFER,
			                            buffer,
			                            bytes_to_read,
			                            &PINFO_ARRAY_DONE(pinfo)
			                           );
			if ((long)bytes_read <= 0)
			{
				if (bytes_read)
					error = err_push(ERR_READ_FILE, "Unable to read from %s", PINFO_FNAME(pinfo));
				else
					error = err_push(ERR_READ_FILE, "Unexpected end of file processing \"%s\"", PINFO_NAME(pinfo));

				goto read_formats_exit;
			}

			ff_unlock__(pinfo, &buffer, (long)bytes_read);

			PINFO_BYTES_LEFT(pinfo) -= bytes_read;

			error = db_do(dbin, DBDO_BYTE_SWAP, PINFO_FD(pinfo));
			if (error)
			{
				err_push(ERR_PROCESS_DATA, "Byte-swapping to native byte order");
				goto read_formats_exit;
			}
	
			if (dbin->eqn_info)
			{
				error = db_do(dbin, DBDO_FILTER_ON_QUERY);
				if (error)
					goto read_formats_exit;
			}
		} /* else if is input data */
		else if (WAIT_FOR_NEXT_RECORD_HEADER(pinfo))
		{
			/* Do nothing.  Wait until input data format is done cacheing */
			error = ff_unlock__(pinfo, &buffer, 0);
			if (error)
				return(error);
		}
		else
			assert(0);

		pinfo_list = dll_next(pinfo_list);
		pinfo = FF_PI(pinfo_list);
	} /* End of format_list processing (while) */

	error = check_for_array_EOF(pinfo_list);
	return(error);

read_formats_exit:

	(void)ff_unlock__(pinfo, NULL, 0);
	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "byte_swap"

/*****************************************************************************
 * NAME: byte_swap()
 *
 * PURPOSE:  To byte-swap various data types
 *
 * USAGE:  error = byte_swap(data_ptr, data_type);
 *
 * RETURNS:
 *
 * DESCRIPTION:  Data in a 1-2, 1-2-3-4, or 1-2-3-4-5-6-7-8 order become a 2-1, 4-3-2-1,
 * or 8-7-6-5-4-3-2-1 order, respectively.
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

static int byte_swap(char *dataptr, FF_TYPES_t var_type)
{
	int error = 0;
	size_t byte_size = ffv_type_size(var_type);

	switch ((int)byte_size)
	{
		char temp;
		
		case 1:
			/* do nothing */
		break;
		
		case 2:
			temp = *dataptr;
			*dataptr = *(dataptr + 1);
			*(dataptr + 1) = temp;
		break;
		
		case 4:
			temp = *dataptr;
			*dataptr = *(dataptr + 3);
			*(dataptr + 3) = temp;
			temp = *(dataptr + 1);
			*(dataptr + 1) = *(dataptr + 2);
			*(dataptr + 2) = temp;
		break;
		
		case 8:
			temp = *dataptr;
			*dataptr = *(dataptr + 7);
			*(dataptr + 7) = temp;
			temp = *(dataptr + 1);
			*(dataptr + 1) = *(dataptr + 6);
			*(dataptr + 6) = temp;
			temp = *(dataptr + 2);
			*(dataptr + 2) = *(dataptr + 5);
			*(dataptr + 5) = temp;
			temp = *(dataptr + 3);
			*(dataptr + 3) = *(dataptr + 4);
			*(dataptr + 4) = temp;
		break;
		
		default:
			assert(!ERR_SWITCH_DEFAULT);
			error = err_push(ERR_SWITCH_DEFAULT, "%d, %s:%d", (int)byte_size, os_path_return_name(__FILE__), __LINE__);
		break;
	}

	return(error);
}

static int dbdo_process_formats
	(
	 DATA_BIN_PTR dbin,
	 PROCESS_INFO_LIST pinfo_list
	)
{
	int error = 0;
	BOOLEAN done_reading = FALSE;

	FF_VALIDATE(dbin);
	assert(pinfo_list);

	error = db_do(dbin, DBDO_READ_FORMATS, pinfo_list);
	if (error == EOF)
	{
		error = 0;
		done_reading = TRUE;
	}

	if (!error)
		error = db_do(dbin, DBDO_CONVERT_FORMATS, pinfo_list);

	if (!error && done_reading)
		error = EOF;

	return(error);
}

#define IS_NATIVE_BYTE_ORDER(fd) (endian() == (BOOLEAN)(fd)->state.byte_order)

static int dbdo_byte_swap(FORMAT_DATA_PTR format_data)
{
	VARIABLE_PTR var	= NULL;
	VARIABLE_LIST	v_list = NULL;
	
	FF_VALIDATE(format_data);
	FF_VALIDATE(format_data->data);

	assert(format_data->data->bytes_used < format_data->data->total_bytes);
		
	/* Check to make sure the input format is binary */
	if (!IS_BINARY(format_data->format))
		return(0);

	/* check whether or not data is in the native format */
	if (IS_NATIVE_BYTE_ORDER(format_data))
		return(0);

	/* Byte-swap one variable at a time, down through columns */

	v_list = FFV_FIRST_VARIABLE(format_data->format);
	var    = FF_VARIABLE(v_list);
	while (var)
	{
		int error;
		long offset;

		offset = var->start_pos - 1;

		/* test for valid variable type before loop on second through remaining */
		error = byte_swap(format_data->data->buffer + offset, FFV_DATA_TYPE(var));
		if (error)
			return(error);

		offset += FORMAT_LENGTH(format_data->format);
		while ((FF_BSS_t)offset < format_data->data->bytes_used)
		{
			(void)byte_swap(format_data->data->buffer + offset, FFV_DATA_TYPE(var));
			offset += FORMAT_LENGTH(format_data->format);
		}

		v_list = dll_next(v_list);
		var    = FF_VARIABLE(v_list);
	}

	return(0);
}

static int dbdo_filter_on_query(DATA_BIN_PTR dbin)
{
	int error;
	FF_BSS_t count = 0;
	char HUGE *src_ptr = NULL;
	char HUGE *trg_ptr = NULL;

	PROCESS_INFO_LIST process_info_list = NULL;
	PROCESS_INFO_PTR pinfo = NULL;

	FF_VALIDATE(dbin);
	FF_VALIDATE(dbin->eqn_info);

	/* Check for preconditions */
	assert(dbin->eqn_info);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA, &process_info_list);
	if (!error)
	{
		pinfo = FF_PI(dll_first(process_info_list));

		if (ee_check_vars_exist(dbin->eqn_info, PINFO_FORMAT(pinfo)))
		{
			ff_destroy_process_info_list(process_info_list);
			return(err_push(ERR_EE_VAR_NFOUND, "In input format"));
		}

		error = 0;
		count = 0;
		src_ptr = trg_ptr = PINFO_BUFFER(pinfo);
		while ((char HUGE *)src_ptr < (char HUGE *)(PINFO_BUFFER(pinfo) + PINFO_BYTES_USED(pinfo)))
		{
			if (ee_set_var_values(dbin->eqn_info, src_ptr, PINFO_FORMAT(pinfo)))
			{
				ff_destroy_process_info_list(process_info_list);
				return(err_push(ERR_GEN_QUERY, "Seting equation variables"));
			}

			if (ee_evaluate_equation(dbin->eqn_info, &error))
			{
				count++;
				if ((char HUGE *)src_ptr != (char HUGE *)trg_ptr) 
				{
					memMemcpy(trg_ptr,
								 src_ptr,
								 PINFO_RECL(pinfo),
								 NO_TAG
								);
				}

				trg_ptr += PINFO_RECL(pinfo);
			}
			else if (error)
			{
				/* what goes here? */
			}
				
			src_ptr += PINFO_RECL(pinfo);
		}
			
		/* seting data_ptr to NULL allows VIEW_GET_DATA to once again
			start at the beginning of the cache (dview->first_pointer) */
		PINFO_BYTES_USED(pinfo) = (FF_BSS_t)count * PINFO_RECL(pinfo);

		if (!count)
			PINFO_NEW_RECORD(pinfo) = 0;

		ff_destroy_process_info_list(process_info_list);
	} /* if not error */

	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "db_do"

/*****************************************************************************
 * NAME:
 *
 * PURPOSE:
 *
 * USAGE:
 *
 * RETURNS:
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

int db_do(DATA_BIN_PTR dbin, int message, ...)
{
	va_list args;
	int error = 0;

	va_start(args, message);

	FF_VALIDATE(dbin);

	switch (message)
	{
		case DBDO_WRITE_FORMATS:       /* Argument: format type */
		{
			PROCESS_INFO_LIST pinfo_list = va_arg(args, PROCESS_INFO_LIST);

			error = write_formats(pinfo_list);
		}
		break;
			
		case DBDO_BYTE_SWAP:
		{
			FORMAT_DATA_PTR format_data = va_arg(args, FORMAT_DATA_PTR);

			error = dbdo_byte_swap(format_data);
		}
		break;

		case DBDO_CONVERT_DATA:
		{
			FORMAT_DATA_MAPPING_PTR format_data_mapping = NULL;

			format_data_mapping = va_arg(args, FORMAT_DATA_MAPPING_PTR);
				
			error = dbin_convert_data(format_data_mapping);
		}
		break;

		case DBDO_READ_FORMATS:       /* Argument: format type */
		{
			PROCESS_INFO_LIST pinfo_list = va_arg(args, PROCESS_INFO_LIST);

			error = read_formats(dbin, pinfo_list);
		}
		break;
			
		case DBDO_PROCESS_FORMATS:
		{
			PROCESS_INFO_LIST pinfo_list = va_arg(args, PROCESS_INFO_LIST);

			error = dbdo_process_formats(dbin, pinfo_list);
		}
		break;
			
		case DBDO_CONVERT_FORMATS:
		{
			PROCESS_INFO_LIST pinfo_list = va_arg(args, PROCESS_INFO_LIST);

			error = convert_formats(dbin, pinfo_list);
		}
		break;

		case DBDO_FILTER_ON_QUERY:

			error = dbdo_filter_on_query(dbin);
			
		break;

#ifdef DBDO_WRITE_OUTPUT_FMT_FILE
		case DBDO_WRITE_OUTPUT_FMT_FILE:
		{
			char *fname = va_arg(args, char *);

			error = write_output_fmt_file(dbin, fname);

		}
		break;
#endif /* WRITE_OUTPUT_FMT_FILE */

		default:
		{
			assert(!ERR_SWITCH_DEFAULT);
			error = err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d",  ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__);
		}
	}       /* End of Attribute Switch */

	va_end(args);
	return(error);
}


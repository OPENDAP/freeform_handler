/*
 * FILENAME:  db_set.c
 * CONTAINS:  
 * Public functions:
 *
 * db_set()
 *
 * Private functions:
 *
 * alphanum_type
 * ask_header_parts
 * change_input_img_format
 * change_var
 * check_file_exists
 * collect_record_formats
 * destroy_format_list
 * find_format_files
 * dbset_byte_order
 * dbset_cache_size
 * dbset_header_file_names
 * dbset_input_formats
 * dbset_input_header
 * dbset_read_eqv
 * dbset_output_formats
 * dbset_query_restriction
 * dbset_variable_restriction
 * find_files
 * header_to_format
 * make_contiguous_fomrat
 * set_flist_loci
 * set_format_locus
 * trim_format
 * find_dir_format_files
 * find_initialization_files
 * finishup_header
 * extract_format
 * get_default_var
 * make_format_data
 * make_format_eqv_list
 * set_var_info
 * set_var_typeprec
 * setup_header
 * strascii
 * text_delim_offset
 */

/*
 * NAME: db_set
 *              
 * PURPOSE: Initialize a data bin.
 *
 * USAGE: db_set(data_bin, message, ...)
 *
 * RETURNS:     An error code defined in err.h on failure, zero on success
 *
 * DESCRIPTION:  Initialize a data bin according to message code.
 * The message codes and their arguments are:
 *
 * DBSET_BYTE_ORDER: FF_TYPES_t format_type
 * DBSET_CACHE_SIZE: unsigned long cache_size
 * DBSET_HEADER_FILE_NAMES FF_TYPES_t io_type, char *data_file_name
 * DBSET_INPUT_FORMATS: char *input_data_file_name, char *output_data_file_name, char *format_file_name, char *format_buffer, char *title_specified, FORMAT_DATA_LIST_HANDLE format_data_list
 * DBSET_OUTPUT_FORMATS: char *input_data_file_name, char *output_data_file_name, char *format_file_name, char *format_buffer, char *title_specified, FORMAT_DATA_LIST_HANDLE format_data_list
 * DBSET_QUERY_RESTRICTION: char *query_file_name
 * DBSET_VARIABLE_RESTRICTION: char *variable_file_name, FORMAT_PTR output_format
 *
 * This function is typically called only from db_create.
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * AUTHOR:  Mark Ohrenschall, NGDC (303) 497-6124, mao@mail.ngdc.noaa.gov
 *
 * COMMENTS:
 *                                 
 * KEYWORDS:
 *
*/

#include <freeform.h>

/*
 * NAME:  trim_format
 *
 * PURPOSE:  Remove all but given variables from a format  
 *
 * USAGE:  error = trim_format(format, buffer);
 *
 * RETURNS:  ERR_GENERAL
 *
 * DESCRIPTION:  Deletes all variables from format whose names are not in buffer.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC (303) 497-6124, mao@mail.ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * COMMENTS:  
 *
 * KEYWORDS:
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "trim_format"

static int trim_format
	(
	 FORMAT_PTR format,
	 char *buffer
	)
{
	VARIABLE_LIST v_list = NULL;
	VARIABLE_PTR var = NULL;

	FF_VALIDATE(format);
	
	v_list = FFV_FIRST_VARIABLE(format);
	var = FF_VARIABLE(v_list);
	while (var)
	{
		BOOLEAN save_this_var = TRUE;
		char *save_var_name = NULL;

		FF_VALIDATE(var);

		v_list = dll_next(v_list);

		/* is var->name a substring of buffer? */
		save_var_name = strstr(buffer, var->name);
		if (save_var_name)
		{
			size_t vname_len = strlen(var->name);

			/* var->name is a substring, but does its match have trailing whitespace or ends the string? */
			if (isspace(save_var_name[vname_len]) || save_var_name[vname_len] == STR_END)
			{
				/* var->name is a substring, but does its match have leading whitespace or begins the string? */
				if (((char HUGE *)save_var_name == (char HUGE *)buffer) || isspace(save_var_name[-1]))
				{
					/* var->name has an exact match */
					save_this_var = TRUE;
				}
				else
					save_this_var = FALSE;
			}
			else
				save_this_var = FALSE;
		}
		else
			save_this_var = FALSE;

		if (!save_this_var)
		{
			dll_delete(dll_previous(v_list));
			format->num_vars--;
		}

		var = FF_VARIABLE(v_list);
	}

	var = FF_VARIABLE(FFV_FIRST_VARIABLE(format));
	if (!var)
		return(err_push(ERR_NO_VARIABLES, "All variables removed from \"%s\"", format->name));
	else
		return(0);
}

/*
 * NAME:  make_contiguous_format
 *
 * PURPOSE:  Realign start and end positions of variables to be contiguous.
 *
 * USAGE:  make_contiguous_format(format);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  Steps through each variable in format, reassigning the start
 * position to be one more than the end position of the previous variable, and
 * reassigning the end position to preserve the variable's field width.
 *
 * This is a companion function to trim_format.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC (303) 497-6118, mao@mail.ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * COMMENTS:
 *
 * KEYWORDS:
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "make_contiguous_format"

static void make_contiguous_format(FORMAT_PTR format)
{
	VARIABLE_LIST vlist = FFV_FIRST_VARIABLE(format);
	FF_NDX_t new_start_pos = 0;
	FF_NDX_t new_end_pos = 0;
	FF_NDX_t old_end_pos = 0;
	unsigned short i = 0;
	unsigned short buffer_char = 0;

	FF_VALIDATE(format);
  
   /*  if(IS_ASCII(format))buffer_char = 1;*/
  
  	/* check first variable */
	if (FF_VARIABLE(vlist)->start_pos != 1)
	{
		/* realign first variable */
		new_start_pos = 1;
		new_end_pos = new_start_pos + (FF_VARIABLE(vlist)->end_pos - 
		                               FF_VARIABLE(vlist)->start_pos);
		FF_VARIABLE(vlist)->start_pos = new_start_pos;
		FF_VARIABLE(vlist)->end_pos = new_end_pos;
	}
	old_end_pos = FF_VARIABLE(vlist)->end_pos ;
	
	for (i = 1; i < format->num_vars; i++)
	{ 
		vlist = dll_next(vlist);
		new_start_pos = old_end_pos + 1;
		new_end_pos = new_start_pos + (FF_VARIABLE(vlist)->end_pos - 
		                               FF_VARIABLE(vlist)->start_pos);
		FF_VARIABLE(vlist)->start_pos = new_start_pos;
		FF_VARIABLE(vlist)->end_pos = new_end_pos;		                              
		old_end_pos = new_end_pos;		                               
	}

	format->length = FF_VARIABLE(vlist)->end_pos;
	return;
}

/*****************************************************************************
 * NAME:  ask_header_parts
 *
 * PURPOSE:  get values for header keywords from name table
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "ask_header_parts"

static int ask_header_parts
	(
		FF_TYPES_t io_type,
		DATA_BIN_PTR dbin,
		char *file_name,
		char *header_file_name,
		char *header_file_path,
		char *header_file_ext
	)
{
	int error = 0;
	BOOLEAN hfn_defined = FALSE;

	if (nt_ask(dbin, (FFF_IO & io_type) | NT_ANYWHERE, "header_file_name", FFV_CHAR, header_file_name))
	{
		if (!file_name)
			return(err_push(ERR_FILE_DEFINED, IS_INPUT_TYPE(io_type) ? "Input data file" : "Output data file"));
	
		hfn_defined = FALSE;
		os_path_put_parts(header_file_name, NULL, file_name, NULL);
	}
	else
	{
		hfn_defined = TRUE;
		os_path_make_native(header_file_name, header_file_name);
	}
	
	if (hfn_defined && os_path_return_path(header_file_name))
	{
		os_path_get_parts(header_file_name, header_file_path, NULL, NULL);
		os_path_get_parts(header_file_name, NULL, header_file_ext, NULL);
		strcpy(header_file_name, header_file_ext);
	}
	else
	{
		if (nt_ask(dbin, (FFF_IO & io_type) | NT_ANYWHERE, "header_file_path", FFV_CHAR, header_file_path))
			header_file_path[0] = STR_END;
		else
			os_path_make_native(header_file_path, header_file_path);
	}
  
	if (hfn_defined && os_path_return_ext(header_file_name))
		os_path_get_parts(header_file_name, NULL, NULL, header_file_ext);
	else
	{
		if (nt_ask(dbin, (FFF_IO & io_type) | NT_ANYWHERE, "header_file_ext", FFV_CHAR, header_file_ext))
			strcpy(header_file_ext, "hdr");
		else
			os_path_make_native(header_file_ext, header_file_ext);
	}
	
	return(error);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "setup_header"

static int setup_header
	(
	 DATA_BIN_PTR dbin,
	 PROCESS_INFO_PTR pinfo
	)
{
	long bytes_to_read = 0;
	char temp_buf[MAX_PV_LENGTH];
	int error = 0;

	PROCESS_INFO_LIST id_pinfo_list = NULL;
	PROCESS_INFO_PTR id_pinfo = NULL;
	
	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA, &id_pinfo_list);
	if (error)
		return(error);
	
	id_pinfo = FF_PI(dll_first(id_pinfo_list));
	
	/* get the file header in */
	if (IS_EMBEDDED_TYPE(PINFO_TYPE(pinfo)))
	{
		if (IS_VARIED_TYPE(PINFO_TYPE(pinfo)))
		{
			if (nt_ask(dbin, NT_ANYWHERE, "header_length", FFV_USHORT, &temp_buf))
			{
				/* The default bytes to read is the entire file, or maximum cache size */
				bytes_to_read = (unsigned)min(FF_MAX_CACHE_SIZE, os_filelength(PINFO_FNAME(id_pinfo)));
			}
			else
				bytes_to_read = *(unsigned short *)temp_buf;
						
			/* This is a kludge so that READ_FORMATS can read the header,
			   so that we can parse it, so that we can know its max_length!
			*/
			PINFO_FORMAT(pinfo)->length = (FF_NDX_t)bytes_to_read;
			error = ff_resize_bufsize(PINFO_FORMAT(pinfo)->length + 1, &PINFO_DATA(pinfo));
			if (error)
				return(error);
		}
	} /* if IS_EMBEDDED */
	else if (IS_SEPARATE_TYPE(PINFO_TYPE(pinfo)))
	{
		if (IS_VARIED_TYPE(PINFO_TYPE(pinfo)))
		{
			assert(os_file_exist(PINFO_FNAME(pinfo)));
						
			/* This is a kludge so that READ_FORMATS can read the header,
			   so that we can parse it, so that we can know its max_length!
			*/
			PINFO_FORMAT(pinfo)->length = (FF_NDX_t)os_filelength(PINFO_FNAME(pinfo));
			error = ff_resize_bufsize(PINFO_FORMAT(pinfo)->length + 1, &PINFO_DATA(pinfo));
			if (error)
				return(error);
		}
	}
	else
		assert(0);

	ff_destroy_process_info_list(id_pinfo_list);

	return(0);
}

/*
 * NAME:        strascii        
 *              
 * PURPOSE:     To convert non-printable character to it's ASCII value. Only used in setdbin
 *
 * USAGE:       int strascii(char *ch);
 *                      *ch: pointer to the non-printable character 
 *
 * RETURNS:     ASCII code of the character
 *
 * DESCRIPTION: 
 *
 * SYSTEM DEPENDENT FUNCTIONS:  
 *
 * GLOBALS:     
 *
 * AUTHOR:      Liping Di 303-497-6284  lpd@mail.ngdc.noaa.gov
 *
 * COMMENTS:    
 *
 * KEYWORDS:    
 *
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "strascii"

static char strascii(char *ch)
{
	if (*ch != '\\')
		return(*ch);

	switch(*++ch)
	{
		case 'n':
			return('\n');
	
		case 't':
			return('\t');
	
		case '0':
			return(STR_END);
			
		case 'r':
			return('\r');
	
		default:
			return(*ch);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "text_delim_offset"

static BOOLEAN text_delim_offset
	(
	 char *text,
	 char *delim,
	 size_t *offset
	)
{
	size_t delim_offset;
	
	delim_offset = strcspn(text, delim);
	*offset = 0;
	while ((isprint(text[*offset]) || isspace(text[*offset])) &&
	       *offset < delim_offset)
		(*offset)++;

	if (text[*offset] == STR_END || strcspn(text + *offset, delim))
		return(FALSE);
	else
		return(TRUE);
}

/*
 * NAME:                alphanum_type
 *              
 * PURPOSE:     To parse a string to determine whether or not it is a
 *              numerical string.
 *
 * USAGE:       int alphanum_type(char *header, int start, int end)
 *
 * RETURNS:     The variable type of field, e.g., FFV_TEXT
 *
 * DESCRIPTION: This function uses a finite state technique to parse
 * a string to determine whether or not it is a numerical string.
 *
 * SYSTEM DEPENDENT FUNCTIONS:  none
 *
 * GLOBALS:     none
 *
 * AUTHOR:      Liping Di, NGDC, (303) 497 - 6284, lpd@mail.ngdc.noaa.gov
 *
 * COMMENTS:    
 *
 * KEYWORDS:    utility
 *
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "alphanum_type"

static FF_TYPES_t alphanum_type
	(
	 char *header,
	 FF_NDX_t start,
	 FF_NDX_t end
	)
{
	enum
	{
	 INITIAL     = 0,
	 SIGN        = 1,
	 DECIMAL     = 2,
	 INTEGER     = 3,
	 FLOAT       = 4,
	 ENOTATION   = 5,
	 END_INTEGER = 6,
	 END_FLOAT   = 7,
	 SIGNED_E    = 8,
	 DECIMAL_E   = 9,
	 END_E       = 10
	} status = INITIAL;

	/* states       =INITIAL, initial
			=SIGN, initial sign
			=DECIMAL, initial decimal point
			=INTEGER, integer &
			=FLOAT, float &
			=ENOTATION, has E part
			=END_INTEGER, finish interger &
			=END_FLOAT, finish float &
			=SIGNED_E, E part has sign
			=DECIMAL_E, E digital   &
			=END_E, E finish &
	*/

	while (start <= end)
	{
		switch (header[start - 1])
		{
			case '+':
			case '-':
	
				if (status == INITIAL)
					status = SIGN;
				else if (status == ENOTATION)
					status = SIGNED_E;
				else
					return(FFV_TEXT);
			break;
				
			case 'E':
			case 'e':
			
				if (status == INTEGER || status == FLOAT)
					status = ENOTATION;
				else
					return(FFV_TEXT);
			break;
	
			case '.':
				if (status == INITIAL || status == SIGN)
					status = DECIMAL;
				else if (status == INTEGER)
					status = FLOAT;
				else 
					return(FFV_TEXT);
			break;
	
			default:
				if (isdigit(header[start - 1]))
				{
					if (status == INITIAL || status == SIGN || status == INTEGER)
						status = INTEGER;
					else if (status == DECIMAL || status == FLOAT)
						status = FLOAT;
					else if (status == ENOTATION || status == SIGNED_E || status == DECIMAL_E)
						status = DECIMAL_E;
					else
						return(FFV_TEXT);
				}
				else if (isspace(header[start - 1]))
				{ /* space */
					if (status == INITIAL)
						;
					else if (status == INTEGER || status == END_INTEGER)
						status = END_INTEGER;
					else if (status == FLOAT || status == END_FLOAT)
						status = END_FLOAT;
					else if (status == DECIMAL_E || status == END_E)
						status = END_E;
					else 
						return(FFV_TEXT);
				}
				else
					return(FFV_TEXT);
		} /* switch header[start - 1] */
		start++;
	}

	if (status == END_INTEGER || status == INTEGER)
		return(FFV_INT32);
	else if (status == FLOAT || status == END_FLOAT)
		return(FFV_FLOAT64);
	else if (status == DECIMAL_E || status == END_E)
		return(FFV_ENOTE);
	else
		return(FFV_TEXT);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "set_var_typeprec"

static void set_var_typeprec
	(
	 char *header,
	 VARIABLE_PTR var
	)
{
	FF_TYPES_t type;
	short precision;
	
	type = alphanum_type(header, var->start_pos, var->end_pos);
	switch (type)
	{
		size_t v_shift;
		
		case FFV_TEXT:
			precision = 0;
		break;
        	
		case FFV_INT32:
			precision = 0;
		break;
        	
		case FFV_FLOAT64:
			
			text_delim_offset(header + var->start_pos - 1, ".", &v_shift);
			if (v_shift + (size_t)var->start_pos < (size_t)var->end_pos)
			{ /* count trailing characters after decimal point */
				precision = (short)(var->end_pos - var->start_pos - v_shift);
														
				assert(precision >= 0);
				assert((unsigned)precision < var->end_pos - var->start_pos + 1);
			}
			else
				precision = 0;

		break;
        	
		case FFV_ENOTE:
						
			text_delim_offset(header + var->start_pos - 1, ".", &v_shift);
			if (v_shift + (size_t)var->start_pos < (size_t)var->end_pos)
			{ /* count trailing characters after decimal point */
				precision = (short)(var->end_pos - var->start_pos - v_shift);
				text_delim_offset(header + var->start_pos - 1, "eE", &v_shift);
				if (v_shift + (size_t)var->start_pos < (size_t)var->end_pos)
				{
					/* E-notation characters were added into precision, subtract
					them off, including the 'E' or 'e' */
					precision = (short)(precision - ((size_t)var->end_pos - (size_t)var->start_pos - v_shift + (size_t)1));
				}
														
				assert(precision >= 0);
				assert((unsigned)precision < var->end_pos - var->start_pos + 1);
			}
			else
				precision = 0;

		break;
        	
		default:
			err_push(ERR_SWITCH_DEFAULT, "%ld, %s:%d", (long int)type, os_path_return_name(__FILE__), __LINE__);
		break;
	} /* switch alphanum_type() */

	var->type      = type;
	var->precision = precision;
}

/*
 * NAME:  set_var_info
 *              
 * PURPOSE:  Set variable attributes for a dynamically generated format
 *
 * USAGE:  error = set_var_info(header, item_name, value_terminator, name_terminator,
 * name_dist, &var);
 *
 * RETURNS:  If success, return 0, otherwise an error code
 *
 * DESCRIPTION:  header is a string with the following basic layout:
 *
 * item_name name_terminator item_value value_terminator (and repeats).
 *
 * Either or both item_name and name_terminator may be absent.  item_name
 * may either be known a priori (passed in as an argument) or unknown (NULL).
 * if item_name is present in the string, but not known a priori (NULL
 * argument value), and name_terminator is present, then name_dist
 * must be defined.
 *
 * value_terminator should always be present, but if missing, item_value is
 * taken as all characters up to the NULL-terminator, or up to the first
 * nonprintable character.
 *
 * I would like to implement the following, but cannot without breaking things:
 * {If item_name is passed as an argument, name_terminator is absent, and}
 * {name_dist is defined (not zero) then item_name and name_dist}
 * {have a cumulative effect.  In other words, name_dist is the offset}
 * {not from the "previous" value_terminator, but from the end of item_name.}
 *
 * The variable name is set to item_name (if present, otherwise "no name")
 * and the variable start and end positions are set according to item_value's
 * position.
 *
 * SYSTEM DEPENDENT FUNCTIONS:  
 *
 * GLOBALS:
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * COMMENTS:    
 *
 * KEYWORDS:    
 *
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "set_var_info"

static int set_var_info
	(
	 char *header,
	 char *item_name,
	 char *value_terminator_str,
	 char *name_terminator_str,
	 int name_dist,
	 VARIABLE_HANDLE hvar
	)
{
	int error = 0;

	BOOLEAN offset_found = FALSE;
	size_t offset = 0;
	
	unsigned long item_name_pos = 0;
	size_t start_pos = 0;
	size_t end_pos = 0;
	
	char *pname = NULL;
	
	assert(header);

	if (*hvar == NULL)
	{
		*hvar = ff_create_variable(item_name ? item_name : "");
		if (*hvar == NULL)
			return(err_push(ERR_MEM_LACK, NULL));
	}
  
	if (item_name)
	{
		pname = strstr(header, item_name);
		if (pname)
		{
			/* also use item_name_pos as a BOOLEAN, count from one */
			item_name_pos = (char HUGE *)pname - (char HUGE *)header + 1;
		}
		else
		{
			if (name_dist == 0 && name_terminator_str[0] == STR_END)
			{ /* item_names are not in header */
				pname = header;
			}
			else
			{
				ff_destroy_variable(*hvar);
				*hvar = NULL;
				return(err_push(ERR_MAKE_FORM, "Could not find variable \"%s\"", item_name));
			}
		}
	}
	else
		pname = header;
						
	if (name_dist || item_name_pos)
	{
		if (!item_name_pos)
		{
			start_pos = min(strlen(pname), (size_t)name_dist);

			if (strlen((*hvar)->name) < start_pos)
			{
				char *cp = memRealloc((*hvar)->name, start_pos + 1, "(*hvar)->name");
				if (!cp)
					return(err_push(ERR_MEM_LACK, "new variable name"));

				(*hvar)->name = cp;
			}

			strncpy((*hvar)->name, pname, start_pos - 1);
			(*hvar)->name[start_pos - 1] = STR_END;

			os_str_trim_whitespace((*hvar)->name, (*hvar)->name);
		}

		if (name_dist && item_name_pos)
		{
			/* name_dist is used to set start position */
			start_pos = (item_name_pos - 1) + name_dist;
		}
		else if (name_dist)
			start_pos = name_dist;
		else if (item_name_pos)
		{
			start_pos = FF_STRLEN(item_name) + (item_name_pos - 1);
			if (name_terminator_str[0] != STR_END)
			{
				text_delim_offset(header + start_pos, name_terminator_str, &offset);
				start_pos += offset;
			}

			/* skip repeated name terminators */
			start_pos += strspn(header + start_pos, name_terminator_str);
		}
	} /* if (name_dist || item_name_pos) */
	else
	{
		if (name_terminator_str[0] != STR_END)
			offset_found = text_delim_offset(pname, name_terminator_str, &start_pos);

		if (start_pos == 0 && name_terminator_str[0] != STR_END)
		{ /* string begins with name_terminator */
			start_pos = 1;

			error = new_name_string__("no name", &(*hvar)->name);
			if (error)
				return(error);
		}
		else if (!offset_found || name_terminator_str[0] == STR_END)
		{ /* string does not contain name_terminator, or name_terminator is NULL */
			start_pos = 0;
			
			if (!item_name)
			{
				error = new_name_string__("no name", &(*hvar)->name);
				if (error)
					return(error);
			}
		} /* (else) if start_pos == strlen(pname) */
		else
		{
			assert(pname[start_pos] == name_terminator_str[0]);
				
			/* Next clause to allow a "cumulative" effect */
			if (item_name_pos)
			{
				start_pos += (item_name_pos - 1);
				if (name_dist)
					start_pos += name_dist;
			}
			else
			{
				if (strlen((*hvar)->name) < start_pos)
				{
					char *cp = memRealloc((*hvar)->name, start_pos + 1, "(*hvar)->name");
					if (!cp)
						return(err_push(ERR_MEM_LACK, "new variable name"));

					(*hvar)->name = cp;
				}

				strncpy((*hvar)->name, pname, start_pos - 1);
				(*hvar)->name[start_pos - 1] = STR_END;
			
				os_str_trim_whitespace((*hvar)->name, (*hvar)->name);
			}
				
			start_pos += strspn(pname + start_pos, name_terminator_str);
		}
	} /* (else) if (name_dist || item_name_pos) */
	
	/* skip leading whitespace */
	start_pos += strspn(header + start_pos, WHITESPACE);
			
	/* "normalize" start_pos so as to start counting from one */
	start_pos += 1;

	offset_found = text_delim_offset(pname, value_terminator_str, &end_pos);

	if (end_pos == 0)
	{ /* string begins with value_terminator, or at end of string */
	}
	else if (!offset_found)
	{ /* string does not contain value_terminator, or value_terminator is NULL */
	}
	else
	{
		assert(pname[end_pos] == value_terminator_str[0]);
	}

	if (item_name_pos)
		end_pos += (item_name_pos - 1); /* was relative to pname, not header */

	/* Trim trailing whitespace */
	while (end_pos > 0 && isspace(header[end_pos - 1]))
		--end_pos;

	if (start_pos <= 0 || end_pos <= 0 || end_pos < start_pos)
	{
		ff_destroy_variable(*hvar);
		*hvar = NULL;
		return(ERR_MISSING_TOKEN);
	}

	(*hvar)->start_pos = start_pos;
	(*hvar)->end_pos   = end_pos;

	/* Set variable type and precision */
	set_var_typeprec(header, *hvar);

	return(0);
}
	
/*
 * NAME:                header_to_format
 *              
 * PURPOSE:     To make a FREEFORMAT list(actually the position) from
 *                      header contents
 *
 * USAGE:       BOOLEAN mkformat(FORMAT *format, char *header, char delim_item, char delim_value, int name_dist)
 *
 * RETURNS:     If success, return 0, otherwise return an error code
 *
 * DESCRIPTION: This function is used to prepare header format for
 * variable-length variable-position header which usually
 * has formats: variable-name delim_value variable-value delim_item, or
 * variable-name variable-value delim_item where the distance between 
 * variable-name and variable-value is name_dist, or variable-value delim_item.
 * The function selects format according to the passed arguments. The 
 * delim_item must be none-NULL character. If delim_value is NULL (STR_END)
 * and name_dist is not equal to zero, it is second format. if both delim_value== STR_END, 
 * and name_dist==0, the third format. 
 *                              
 * For the first two cases, if the fisrt variable name in the 
 * format is "create_format_from_data_file", this function will
 * create the format according to the data in the header.
 *
 * ERRORS:
 * Out of memory,"format->variables"
 * Out of memory,"var->next"
 * Out of memory,"can not create dll node"
 *
 * SYSTEM DEPENDENT FUNCTIONS:  none
 *
 * GLOBALS:     none
 *
 * AUTHOR:      Liping Di, NGDC, (303) 497 - 6284, lpd@mail.ngdc.noaa.gov
 *
 * COMMENTS:    
 *
 * KEYWORDS:    variable-length header, format
 *
 */
#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "header_to_format"

static int header_to_format
	(
	 FORMAT *format,
	 char *header,
	 int value_terminator,
	 int name_terminator,
	 int name_dist
	)
{
	int error;
	FF_NDX_t l_offset; /* line offset from start of header */

	VARIABLE *var;
	VARIABLE_LIST v_list;

	char value_terminator_str[2];
	char name_terminator_str[2];

	FF_VALIDATE(format);
	assert(header);
	assert(value_terminator);
		
	value_terminator_str[0] = (char)value_terminator;
	value_terminator_str[1] = STR_END;
		
	name_terminator_str[0] = (char)name_terminator;
	name_terminator_str[1] = STR_END;
		
	l_offset = 0;
	format->length = 0;
	format->num_vars = 0;

	v_list = FFV_FIRST_VARIABLE(format);
	var = FF_VARIABLE(v_list);
	if (var != NULL &&
	    (strcmp(var->name, "create_format_from_data_file") == 0 ||
	     strcmp(var->name, "create_format") == 0
	    )
	   )
	{
		dll_free_holdings(format->variables);
		format->variables = NULL;

		format->variables = dll_init();
		if (format->variables == NULL)
			return(err_push(ERR_MEM_LACK, "format variable list"));

		while (header[l_offset] != STR_END)
		{
			size_t offset = 0;
			
			var = NULL;
			error = set_var_info(header + l_offset, NULL, value_terminator_str, name_terminator_str, name_dist, &var);
			if (!error)
			{
				v_list = dll_add(format->variables);
				if (!v_list)
				{
					dll_free_holdings(format->variables);
					format->variables = NULL;   
					return(err_push(ERR_MEM_LACK, "Unable to add to format variable list"));
				}

				dll_assign(var, DLL_VAR, v_list);

				/* Variable start and end positions were relative */
				var->start_pos += l_offset;
				var->end_pos   += l_offset;

				format->length = max(format->length, var->end_pos);
				++format->num_vars;
			} /* if not error */

			if (text_delim_offset(header + l_offset, value_terminator_str, &offset))
				l_offset += offset + 1;
			else
				break;
		} /* header[l_offset] != STR_END */
	} /* if var && var->name is create_format... */
	else
	{
		var = FF_VARIABLE(v_list);
		while (var != NULL)
		{
			size_t offset = 0;
			
			error = set_var_info(header + l_offset, var->name, value_terminator_str, name_terminator_str, name_dist, &var);
			if (!error)
			{
				/* Variable start and end positions may be relative */
				var->start_pos += l_offset;
				var->end_pos   += l_offset;

				format->length = max(format->length, var->end_pos);
				++format->num_vars;
			}
			
			if ((char)name_terminator == STR_END && name_dist == 0)
			{ /* item_names are not in header */
				if (text_delim_offset(header + l_offset, value_terminator_str, &offset))
					l_offset += offset + 1;
			}

			v_list = dll_next(v_list);

			if (error)
				dll_delete_node(dll_previous(v_list));

			var = FF_VARIABLE(v_list);
		}
	} /* (else) if var && var->name is create_format... */

	if (format->num_vars == 0)
		return(err_push(ERR_NO_VARIABLES, "%s", format->name));
	else
		return(0);
}

static void make_Unix_newlines(char *s)
{
	s = strchr(s, '\r');
	while (s)
	{
		*s = '\n';
		s = strchr(s, '\r');
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "finishup_header"

static int finishup_header
	(
	 DATA_BIN_PTR dbin,
	 PROCESS_INFO_PTR pinfo
	)
{
	char temp_buf[MAX_PV_LENGTH];
	int delim_item, delim_value;
	int error = 0;
	short distance;
	
	/* make variable length, variable position header format, need to be modified */
	if (IS_VARIED_TYPE(PINFO_TYPE(pinfo)))
	{ /* get the delimiter 1, delimiter 2 and/or distance */
		error = nt_ask(dbin, NT_ANYWHERE, "delimiter_item", FFV_CHAR, temp_buf);
		if (error == ERR_NT_KEYNOTDEF)
		{
			error = 0;

			delim_item = '\n';
		}
		else if (error)
			return(err_push(error, "Badly formed keyword definition: delimiter_item"));
		else
			delim_item = strascii(temp_buf);

		if (delim_item == '\n')
		{
			if (IS_ASCII(PINFO_FORMAT(pinfo)))
				make_Unix_newlines(PINFO_BUFFER(pinfo));
		}
	
		distance = 0;
		error = nt_ask(dbin, NT_ANYWHERE, "_distance", FFV_SHORT, &distance);
		if (error == ERR_NT_KEYNOTDEF)
		{
			error = 0;

			error = nt_ask(dbin, NT_ANYWHERE, "delimiter_value", FFV_CHAR, temp_buf);
			if (error == ERR_NT_KEYNOTDEF)
			{
				error = 0;

				delim_value = '=';
			}
			else if (error)
				return(err_push(error, "Badly formed keyword definition: delimiter_value"));
			else
				delim_value = strascii(temp_buf);
		}
		else if (error)
			return(err_push(error, "Badly formed keyword definition: _distance"));
		else
			delim_value = STR_END;
	
		if (IS_VARIED_TYPE(PINFO_TYPE(pinfo)))
		{
			assert(PINFO_RECL(pinfo) <= PINFO_CACHEL(pinfo));
			PINFO_BUFFER(pinfo)[PINFO_RECL(pinfo)] = STR_END;
			error = header_to_format(PINFO_FORMAT(pinfo), PINFO_BUFFER(pinfo), delim_item, delim_value, distance);
			if (error)
				return(err_push(error, "Unable to create header format"));

			PINFO_BYTES_USED(pinfo) = FORMAT_LENGTH(PINFO_FORMAT(pinfo));
		}
					
		if (IS_EMBEDDED_TYPE(PINFO_TYPE(pinfo)))
		{
			error = ff_resize_bufsize(PINFO_RECL(pinfo) + 1, &PINFO_DATA(pinfo));
			if (error)
				return(error);
		}
	} /* if header is varied (separated or embedded) */
	
	return(0);
}

/*****************************************************************************
 * NAME:  check_file_exists() -- a module function of findfile.c
 *
 * PURPOSE:  Test existence for a file, composed from parts, and return name
 *
 * USAGE:  found = check_file_exists(trial_fname, search_dir, filebase, ext);
 *
 * RETURNS: 1 if file is found, zero otherwise
 *
 * DESCRIPTION:  search_dir, filebase, and ext are concatenated, with native
 * directory separator.  Variable trial_fname is set to point to an allocated
 * string containing the existing file name, otherwise trial_fname is set to
 * NULL.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC (303) 497-6124, mao@ngdc.noaa.gov
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
#define ROUTINE_NAME "check_file_exists"

static int check_file_exists
	(
	 char **fname,
	 char *search_dir,
	 char *filebase,
	 char *ext
	)
{
	char trial_fname[_MAX_PATH];
	
	(void)os_path_put_parts(trial_fname, search_dir, filebase, ext);
	if (os_file_exist(trial_fname))
	{
		*fname = memStrdup(trial_fname, "trial_fname");
		if (*fname == NULL)
		{
			err_push(ERR_MEM_LACK, NULL);
			return(0);
		}
		return(1);
	}
	else
		return(0);
}

/* NAME:        find_files
 *              
 * PURPOSE:     Search multiple directories for files matching the supplied
 *              file name and extension.
 *
 * USAGE:       int find_files(char *file_base, char *ext, char *first_dir,
 *                                      char ***targets)
 *
 * RETURNS:  number of files found -- string vector targets is filled with
 * found file names.
 *
 * DESCRIPTION:  file_base may include a path (which distinguishes the default
 * directory search from the file-directory search) and first_dir may be
 * NULL.  find_files() searches first in first_dir,
 * second in the default directory, and lastly in the file's home directory
 * (given by the path component of file_base) for files with the file name
 * component of file_base (i.e., not including the path component of file_base)
 * and the given extension ext and then for files with the file extension
 * component of file_base and the given extenstion ext.
 * 
 * In other words,
 * perform the following searches, where filename(file_base) is the file name
 * component of file_base, fileext(file_base) is the file extension component
 * of file_base, and path(file_base) is the directory path component of file_base.
 *
 * 1) Search first_dir for filename(file_base).ext
 * 2) Search first_dir for fileext(file_base).ext
 * 3) Search the default directory for filename(file_base).ext
 * 4) Search the default directory for fileext(file_base).ext
 * 5) Search the directory given by path(file_base) for filename(file_base).ext
 * 6) Search the directory given by path(file_base) for fileext(file_base).ext
 *
 * This means potentially six files can be found.
 *
 * For each file found, an element of the string vector targets
 * is set to point to an allocated string which is the found file name.
 * The parameter targets itself is an address of a string vector, like argv[].
 *
 * Once the calling routine is done with targets[], it should free the memory
 * according to the following paradigm:
 *
 * For each file found, free(targets[i]), then free(targets).
 *
 * ERRORS:
 *
 * SYSTEM DEPENDENT FUNCTIONS:  
 *
 * GLOBALS:
 *
 * AUTHOR:	Mark A. Ohrenschall, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * COMMENTS:    
 *
 * KEYWORDS:    
 *
 */

#define MAX_CAN_FIND 6

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "find_files"

#define IS_A_RELATIVE_PATH(path)	((path[0] == '.') ? TRUE : FALSE)

static int find_files
	(
	 char *file_base,
	 char *ext,
	 char *first_dir,
	 char ***targets
	)
{
	char *fileext = NULL; /* the extension on the search file name */
	char  filename[_MAX_PATH];
	char  home_dir[_MAX_PATH];
	int num_found = 0;
	char *filenames[MAX_CAN_FIND] = {NULL, NULL, NULL, NULL, NULL, NULL};

	assert(file_base);
	assert(FF_STRLEN(file_base));
	
	if (file_base == NULL || FF_STRLEN(file_base) == 0)
		return(0);

	/* overuse variable fileext to check out ext, which may be a filename.ext */
	fileext = os_path_return_ext(ext);
	if (fileext)
		ext = fileext;

	fileext = os_path_return_ext(file_base);
	os_path_get_parts(file_base, home_dir, filename, NULL);
	if (FF_STRLEN(filename) == 0)
		return(0);
	
	/* If first_dir is relative, check for filename.ext in directory given by
	   first_dir relative to home_dir, else if first_dir is absolute,
		just look for filename.ext in first_dir
	*/
	if (first_dir)
	{
		if (IS_A_RELATIVE_PATH(first_dir))
		{
			char partial_path[_MAX_PATH];

			os_path_put_parts(partial_path, first_dir, filename, NULL);

			/* Check for filename.ext in first_dir relative to home_dir */
			if (check_file_exists(&filenames[num_found], home_dir, partial_path, ext))
				++num_found;
		}
		else
		{
			/* Check for filename.ext in first_dir */
			if (first_dir && check_file_exists(&filenames[num_found], first_dir, filename, ext))
				++num_found;
		}
	}
	
	/* Check for filename's extension.ext in first_dir */
	if (fileext && first_dir &&
	    check_file_exists(&filenames[num_found], first_dir, fileext, ext))
		++num_found;

	/* Check for filename.ext in the current directory */
	if (check_file_exists(&filenames[num_found], NULL, filename, ext))
		++num_found;

	/* Check for filename's extension.ext in the current directory */
	if (fileext &&
	    check_file_exists(&filenames[num_found], NULL, fileext, ext))
		++num_found;
	
	/* Check for file_base.ext in data home directory */
	if (home_dir &&
	    check_file_exists(&filenames[num_found], home_dir, filename, ext))
		++num_found;

	/* Check for file_base's extension.ext in data home directory */
	if (fileext && home_dir &&
	    check_file_exists(&filenames[num_found], home_dir, fileext, ext))
		++num_found;
	
	if (num_found)
	{
		*targets = (char **)memMalloc(num_found * sizeof(char *), "*targets");
		if (!*targets)
		{
			err_push(ERR_MEM_LACK, NULL);
			return(0);
		}
		memMemcpy((char *)*targets, (char *)filenames, num_found * sizeof(char *), "*targets, filenames");
	}
	
	return(num_found);
}

/*****************************************************************************
 * NAME: find_dir_format_files
 *
 * PURPOSE:  Search the directory given by search_dir for format files
 *
 * USAGE: num_found = find_dir_format_files(input_file, format_dir, fileext, format_files);
 *
 * RETURNS:  0, 1 -- 0: no files found, 1: filename.fmt,
 * or ext.fmt found
 *
 * DESCRIPTION:  First looks in the supplied directory for filename.fmt,
 * then for ext.fmt.  Each successive search proceeds only if the
 * previous search failed.  See the DESCRIPTION for find_format_files() for
 * more information.
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "find_dir_format_files"

static int find_dir_format_files
	(
	 char *input_file,
	 char *search_dir,
	 char **targets
	)
{
	int num_found;
	char *fileext = os_path_return_ext(input_file);
	char filename[_MAX_PATH];

	os_path_get_parts(input_file, NULL, filename, NULL);
	if (FF_STRLEN(filename) == 0)
		return(0);

	/* Search search_dir for datafile.fmt */
	num_found = check_file_exists(&(targets[0]), search_dir, filename, "fmt");
	if (num_found == 1)
		return(1);
	
	/* Search search_dir for ext.fmt */
	if (num_found == 0)
		num_found = check_file_exists(&(targets[0]), search_dir, fileext, "fmt");

	return(num_found);
}

/* NAME:        find_format_files
 *              
 * PURPOSE:     To search for format files given a (data) file name.
 *
 * USAGE: number = find_format_files(dbin, dbin->file_name, &targets);
 *
 * RETURNS: Zero, one, or two:  the number of eligible format files found,
 * targets[0] is input format file name, targets[1] is output format file
 * name, if found.
 *
 * DESCRIPTION: The variable targets is a string vector, type (char (*)[]),
 * like argv.
 *
 * If a format file is not given explicitly on a command line then FreeForm
 * searches for default format files as detailed below.  FORMAT_DIR refers
 * to the directory given by the GeoVu keyword "format_dir".  "format_dir"
 * can also be defined in the operating system variable environment space.  The
 * default directory refers to the current working directory.  The file's home
 * directory is given by the path component of datafile, if any.  If there is
 * no path component, then the file's home directory search is not conducted.
 *
 * So, given the data file datafile.ext conduct the following searches for
 * the default format file(s):
 *
 * 1) Search FORMAT_DIR for datafile.fmt.
 *
 * 2) Search FORMAT_DIR for ext.fmt
 *
 * 3) Search the default directory for datafile.fmt.
 *
 * 4) Search the default directory for ext.fmt
 *
 * Steps 5 - 6 are conducted if datafile has a path component.
 *
 * 5) Search the data file's directory for datafile.fmt.
 *
 * 6) Search the data file's directory for ext.fmt
 *
 * Each successive search step is conducted only if the previous search fails.
 *
 * NOTE that in a Windows environment the default directory has the potential
 * to be changed at any time, and it need not be the same as the application
 * start-up directory.  If the calling routine is relying on formats to be
 * found in the default directory, it must ensure that the default directory
 * has been properly set.
 *
 * See the description for find_files() for an explanation of the parameter
 * targets.
 *
 * SYSTEM DEPENDENT FUNCTIONS:  
 *
 * AUTHOR:  Mark A. Ohrenschall, NGDC, (303) 497 - 6124, mao@ngdc.noaa.gov
 *
 * COMMENTS:
 *
 * KEYWORDS:    
 *
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "find_format_files"

static int find_format_files
	(
	 DATA_BIN_PTR dbin,
	 char *input_file,
	 char ***targets
	)
{
	enum {NUM_FMT_FILES = 2};
	char home_dir[_MAX_PATH];
	char format_dir[_MAX_PATH];
	char *format_files[NUM_FMT_FILES] = {NULL, NULL};
	int num_found;
	
	assert(input_file);
	assert(targets);
	
	if (!input_file || !targets)
		return(0);
	
	*targets = (char **)memCalloc(NUM_FMT_FILES, sizeof(char *), "*targets");
	if (!*targets)
	{
		err_push(ERR_MEM_LACK, NULL);
		return(0);
	}

	if (nt_ask(dbin, NT_TABLE, "format_dir", FFV_CHAR, format_dir))
		format_dir[0] = STR_END;
	
	os_path_get_parts(input_file, home_dir, NULL, NULL);

	/* Search format_dir first */
	num_found = find_dir_format_files(input_file, format_dir, format_files);

	/* Search default directory second */
	if (num_found == 0)
		num_found = find_dir_format_files(input_file, NULL, format_files);
		
	/* Search data file's directory last */
	if (FF_STRLEN(home_dir) && num_found == 0)
		num_found = find_dir_format_files(input_file, home_dir, format_files);

	if (num_found >= 1)
		(*targets)[0] = format_files[0];
	else
	{
		format_files[0] = NULL;
		memFree(*targets, "*targets");
	}

	return(num_found);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "find_initialization_files"

static int find_initialization_files
	(
	 DATA_BIN_PTR dbin,
	 char *input_data_file_name,
	 FORMAT_PTR format
	)
{
	char format_dir[_MAX_PATH];

	char *path      = NULL;

	VARIABLE_LIST v_list = FFV_FIRST_VARIABLE(format);
	VARIABLE_PTR         var = FF_VARIABLE(v_list);

	if (nt_ask(dbin, NT_ANYWHERE, "format_dir", FFV_CHAR, format_dir))
		format_dir[0] = STR_END;

	v_list = FFV_FIRST_VARIABLE(format);
	var = FF_VARIABLE(v_list);
	
	while (var)
	{
		if (IS_INITIAL_VAR(var))
		{
			os_path_find_parts(var->name, &path, NULL, NULL);
			if (path && *path)
			{
				if (!os_file_exist(var->name))
					return(err_push(ERR_OPEN_FILE, var->name));
			}
			else
			{
				char *ext          = NULL;
				char **found_files = NULL;
				char dfname[_MAX_PATH];
				
				int number;
				
				if (input_data_file_name)
				{
					os_path_get_parts(input_data_file_name, dfname, NULL, NULL);
					os_path_put_parts(dfname, dfname, NULL, NULL);

					os_path_find_parts(dfname, NULL, NULL, &ext);
					if (ext)
						ext[-1] = STR_END;
				}
				else
				{
					strcpy(dfname, ".");
					ext = NULL;
				}
				
				number = find_files(dfname, ext, format_dir, &found_files);
				if (number)
				{
					int i;
					

					var->name = found_files[0];
					for (i = 1; i < number; i++)
						memFree(found_files[i], "found_files[i]");
					
					memFree(found_files, "found_files");
				}
			} /* (else) if initialization file name (variable name) has a path */
		} /* if initialization variable */
		
		v_list = FFV_NEXT_VARIABLE(v_list);
		var = FF_VARIABLE(v_list);
  } /* while var */
  
	return(0);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "make_format_eqv_list"

static int make_format_eqv_list
	(
	 char *input_data_file_name,
	 FF_TYPES_t fff_iotype,
	 char *fmt_fname,
	 char *format_buffer,
	 FORMAT_LIST_HANDLE hf_list,
	 NAME_TABLE_LIST_HANDLE hnt_list,
	 DATA_BIN_PTR dbin
	)
{
	int number;
	char **found_files;
	int error = 0;
	
	*hf_list = NULL;
	number = 0; /* to hold return value of db_find_format_file() */

	if (!fmt_fname && !format_buffer)
	{
		if (input_data_file_name == NULL)
			return(err_push(ERR_FILE_DEFINED, "input data file name -- Cannot default format search"));
		else
		{
			number = find_format_files(dbin, input_data_file_name, &found_files);
			if (number)
				fmt_fname = found_files[0];
		}
	} /* if no file name and no buffer */
			
	if (fmt_fname || format_buffer)
	{
		PP_OBJECT pp_object;
		FF_BUFSIZE_PTR bufsize = NULL;
		
		if (fmt_fname)
		{
			error = ff_file_to_bufsize(fmt_fname, &bufsize);
			if (error)
				return(err_push(error, fmt_fname));
		}
		else
		{
			bufsize = ff_create_bufsize(strlen(format_buffer) + 1);
			if (!bufsize)
				return(ERR_MEM_LACK);

			bufsize->bytes_used = strlen(format_buffer) + 1;
			strcpy(bufsize->buffer, format_buffer);
		}

		pp_object.ppo_type = PPO_FORMAT_LIST;
		pp_object.u.hf_list = hf_list;
		
		error = ff_text_pre_parser(fmt_fname, bufsize, &pp_object);
		if (!error)
		{
			pp_object.ppo_type = PPO_NT_LIST;
			pp_object.u.nt_list.nt_io_type = fff_iotype;
			pp_object.u.nt_list.hnt_list = hnt_list;
				
			error = ff_text_pre_parser(fmt_fname, bufsize, &pp_object);
			if (error == ERR_NO_NAME_TABLE)
				error = 0;
		}
		
		if (number)
		{
			int i;
			
			for (i = 0; i < number; i++)
				memFree(found_files[i], "found_files[i]");
			memFree(found_files, "found_files");
		}

		ff_destroy_bufsize(bufsize);
	} /* if (fmt_fname || format_buffer) */
			
	if (error)
		return(error);
	else if (!*hf_list)
		return(ERR_GENERAL);
	else
		return(0);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "extract_format"

static void extract_format
	(
	 FORMAT_PTR format,
	 FORMAT_LIST f_list
	)
{
	f_list = dll_first(f_list);
	while (FF_FORMAT(f_list))
	{
		if (ff_format_comp(format, FF_FORMAT(f_list)))
			break;

		f_list = dll_next(f_list);
	}
	
	if (FF_FORMAT(f_list))
		dll_delete_node(f_list);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "make_format_data"

static int make_format_data
	(
	 char *dbin_file_name,
	 char *output_file_name,
	 char	*title_specified,
	 FF_TYPES_t format_type,
	 FORMAT_LIST f_list,
	 FORMAT_DATA_HANDLE hformat_data
	)
{
	int error = 0;
	FORMAT_PTR format = NULL;

	if (title_specified)
	{
		format = db_find_format(f_list, FFF_NAME_CASE, title_specified);
		if (format)
		{
			format->type &= ~FFF_IO;
			format->type |= (FFF_IO & format_type);
			extract_format(format, f_list);
		}
		else
			return(err_push(ERR_FIND_FORM, title_specified));
	} /* if title_specified */
	else
	{
		format = db_find_format(f_list, FFF_GROUP, format_type);
		if (!format)
		{
			db_format_list_mark_io(f_list, format_type, dbin_file_name, output_file_name);
			format = db_find_format(f_list, FFF_GROUP, format_type);
		}

		if (format)
			extract_format(format, f_list);
		else
			return(ERR_GENERAL);
	} /* else if title_specified */
				
	*hformat_data = fd_create_format_data(format, format->length + 1 + (IS_ASCII(format) ? NATIVE_EOL_LENGTH : 0), format->name);
	if (*hformat_data)
		return(0);
	else
	{
		ff_destroy_format(format);
		return(ERR_MEM_LACK);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "collect_record_formats"

static int collect_record_formats
	(
	 FORMAT_LIST f_list,
	 FORMAT_DATA_LIST format_data_list
	)
{
	FORMAT_PTR format = NULL;

	assert(format_data_list);
	if (!format_data_list)
		return(err_push(ERR_GENERAL, "Expecting a format list"));

	format = db_find_format(f_list, FFF_GROUP, FFF_RECORD);
	while (format)
	{
		FORMAT_DATA_PTR format_data = NULL;

		FF_VALIDATE(format);

		extract_format(format, f_list);
		format_data = fd_create_format_data(format, 1, format->name); /* something more elegant would be nice */
		if (!format_data)
		{
			ff_destroy_format(format);
			return(ERR_MEM_LACK);
		}

		format_data_list = dll_add(format_data_list);
		if (!format_data_list)
		{
			ff_destroy_format(format);
			return(ERR_MEM_LACK);
		}

		dll_assign(format_data, DLL_FD, format_data_list);

		format = db_find_format(f_list, FFF_GROUP, FFF_RECORD);
	} /* else if title_specified */

	return(0);
}

static void destroy_format_list(FORMAT_LIST f_list)
{
	dll_free_holdings(f_list);
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

/*
 * MESSAGE:     DBIN_CACHE_SIZE
 *
 * OBJECT TYPE: DATA_BIN
 *
 * ARGUMENTS:   (TO SET) long cache_size
 *
 * PRECONDITIONS:       None
 *
 * CHANGES IN STATE:	ALLOCATES MEMORY FOR CACHE
 *						Sets dbin->cache
 *						= dbin->cache_end
 *						= dbin->data_ptr
 *						Sets dbin->cache_size
 *						Sets dbin->records_in_cache 
 *						Sets dbin->state.cache_defined = 1
 *						Initialize first character of cache to STR_END
 *
 * DESCRIPTION: Create cache for dbin with a given size.
 * If an input or output format is defined, the cache size
 * is adjusted to be the largest integral multiple
 * of the record size of the larger of input or output format.
 *
 * If insufficient memory exists to create a new cache, smaller cache
 * sizes are tried until a cache is allocated, or no cache can be allocated
 * whatever the size.  If an existing cache is being resized, and
 * insufficient memory exists to resize it, then no such cache reduction
 * is attempted, instead, we return immediately with an error.  This
 * assumes that if the user (application programmer) is changing the
 * cache size, they want the exact size they are asking for.
 *
 * ERRORS:	Lack of Memory:
 *				Sets dbin->state.cache_defined = 0 
 *				Sets dbin->state.cache_filled  = 0 
 *
 * AUTHOR:      TH, NGDC,(303)497-6472, haber@ngdc.noaa.gov
 *
 * COMMENTS:	Why initialize first cache character to STR_END?
 *
 * KEYWORDS:    
 *
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_cache_size"  

static int dbset_cache_size
	(
	 DATA_BIN_PTR dbin,
	 unsigned long orig_cache_size
	)
{
	PROCESS_INFO_LIST pinfo_list = NULL;
	long count  = 1L;
	
	int error = 0;

	FF_VALIDATE(dbin);
	assert(orig_cache_size > 0);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA, &pinfo_list);
	if (!error)
	{
		PROCESS_INFO_PTR pinfo = NULL;

		pinfo_list = dll_first(pinfo_list);
		pinfo      = FF_PI(pinfo_list);
		while (pinfo)
		{
			unsigned long cache_size = 0;
			unsigned long max_record_length = 0;

			max_record_length = max(FORMAT_LENGTH(PINFO_FORMAT(pinfo)),
				                     (PINFO_MATE(pinfo) ? FORMAT_LENGTH(PINFO_MATE_FORMAT(pinfo)) : 0)
				                    );
			cache_size = max(orig_cache_size, max_record_length);

			if (PINFO_MATE(pinfo) && PINFO_BYTES_LEFT(pinfo))
			{
				unsigned long out_fsize; /* guess output file size */

				out_fsize = (PINFO_BYTES_LEFT(pinfo) / FORMAT_LENGTH(PINFO_FORMAT(pinfo)) *
					          FORMAT_LENGTH(PINFO_MATE_FORMAT(pinfo))
					         );
				if (cache_size > PINFO_BYTES_LEFT(pinfo) && cache_size > out_fsize)
					cache_size = max(PINFO_BYTES_LEFT(pinfo), out_fsize);
			}
			
			count = cache_size / max_record_length;
			cache_size = (count * FORMAT_LENGTH(PINFO_FORMAT(pinfo)));

			assert(cache_size < FF_MAX_CACHE_SIZE);
			cache_size = min(cache_size, FF_MAX_CACHE_SIZE);

			if (PINFO_DATA(pinfo))
			{
				if (cache_size >= PINFO_TOTAL_BYTES(pinfo))
				{
					error = ff_resize_bufsize(cache_size + 1, &PINFO_DATA(pinfo));
					if (error)
						break;
				}
			}
			else
			{
				PINFO_DATA(pinfo) = ff_create_bufsize(cache_size);
				if (!PINFO_DATA(pinfo))
				{
					error = ERR_MEM_LACK;
					break;
				}
			}

			pinfo_list = dll_next(pinfo_list);
			pinfo      = FF_PI(pinfo_list);
		} /* while pinfo */

		ff_destroy_process_info_list(pinfo_list);
	} /* if !error */
	
	return(error);
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

/*
 * MESSAGE:     DBIN_HEADER_FILE
 *
 * OBJECT TYPE: DATA_BIN
 *
 * ARGUMENTS:   FF_TYPES_t io_type, char *data_file_name
 *
 * PRECONDITIONS:       None
 *
 * CHANGES IN STATE:    ALLOCATES MEMORY FOR HEADER FILE NAME
 *
 * DESCRIPTION: Searches for an input header file,
 * or seeks to create an output header file.
 *
 * ERRORS:      Lack of Memory
 *
 * AUTHOR:      TH, NGDC,(303)497-6472, haber@ngdc.noaa.gov
 *
 * COMMENTS:    
 *
 * KEYWORDS:    
 *
 */
		 
#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_header_file_names"
		 
static int dbset_header_file_names
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t io_type,
	 char *data_file_name
	)
{
	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR  pinfo      = NULL;

	char header_file_name[_MAX_PATH];
	char header_file_path[_MAX_PATH];
	char header_file_ext[_MAX_PATH];
	int error = 0;
		
	FF_VALIDATE(dbin);

	error = db_ask(dbin, DBASK_PROCESS_INFO, io_type | FFF_HEADER, &pinfo_list);
	if (error == ERR_GENERAL)
		return(0);
	else if (error)
		return(error);
	
	pinfo_list = dll_first(pinfo_list);
	pinfo      = FF_PI(pinfo_list);
	while (pinfo)
	{
		FF_VALIDATE(pinfo);

		if (IS_EMBEDDED(PINFO_FORMAT(pinfo)))
		{
			pinfo_list = dll_next(pinfo_list);
			pinfo      = FF_PI(pinfo_list);
			continue;
		}

		error = ask_header_parts(IS_INPUT_TYPE(io_type) ? FFF_INPUT : FFF_OUTPUT,
			                      dbin,
			                      data_file_name,
			                      header_file_name,
			                      header_file_path,
			                      header_file_ext
			                     );
		if (error)
			break;

		if (IS_INPUT_TYPE(io_type))
		{
			int number = 0;
			char **found_files = NULL;
				
			number = 0; /* number of default files found */
			
			number = find_files(header_file_name, header_file_ext, header_file_path, &found_files);
			if (number)
			{
				if (PINFO_IS_FILE(pinfo))
					memFree(PINFO_FNAME(pinfo), "PINFO_FNAME(pinfo)");

				PINFO_FNAME(pinfo) = (char *)memStrdup(found_files[0], "PINFO_FNAME(pinfo)");
				if (!PINFO_FNAME(pinfo))
					error = err_push( ERR_MEM_LACK, NULL);

				while (number > 0)
				{
					--number;
					memFree(found_files[number], "found_files[]");
				}
				memFree(found_files, "found_files");
			}
			else
				error = err_push(ERR_FIND_FILE, "Input header file");
		}
		else if (IS_OUTPUT_TYPE(io_type))
		{
			os_path_put_parts(header_file_name,
							  header_file_path,
							  header_file_name,
							  header_file_ext
							 );
				
			if (PINFO_IS_FILE(pinfo) && PINFO_FNAME(pinfo))
				memFree(PINFO_FNAME(pinfo), "PINFO_FNAME(pinfo)");

			PINFO_FNAME(pinfo) = (char *)memStrdup(header_file_name, "header_file_name");
			if (!PINFO_FNAME(pinfo))
				error = err_push(ERR_MEM_LACK, "");

			PINFO_ID(pinfo) = NDARRS_FILE | NDARRS_UPDATE;
		} /* else if IS_OUTPUT_TYPE(io_type) */

		pinfo_list = dll_next(pinfo_list);
		pinfo      = FF_PI(pinfo_list);
	}

	ff_destroy_process_info_list(pinfo_list);

	return(error);
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

/*
 * MESSAGE:     DEFINE_INPUT_HEADER
 *
 * OBJECT TYPE: DATA_BIN
 *
 * ARGUMENTS:   (TO SET)char *file_name char * header_file_name
 *
 * PRECONDITIONS:       None
 *
 * CHANGES IN STATE:    Checks for delimited fields in header.
 *		                Sets dbin->cache_bytes_left = 0 if a record header        
 *		                Sets dbin->state.header_defined = 1
 *		                Sets dbin->state.header_delimited = 1
 *
 * DESCRIPTION: Defines a header for a data bin.
 *				The FREEFORM system presently recognizes three
 *				types of headers:
 *				File Headers:
 *			        Only one file header can be associated with
 *			        a DATA_BIN.
 *			        It either occurs at the beginning of the data
 *			        file or in a file of it's own (SEPARATE).
 *				Record Headers:
 *			        Record headers occur once for every record
 *			        of a data file. They can either be interspersed
 *			        with the data or in a file of their own (SEPARATE);
 *				Index Headers:
 *			        see index.h for format
 *				At present only one INPUT and OUTPUT header format is
 *					allowed / format_list.
 *				The HEADER event processing includes the following
 *					steps:
 *		        1) Check for header format in format list.
 *	                If list exists with no header format: ERROR (NO!!!)
 *		        2) Make header format from file given as first
 *	                argument.
 *		        3) Define header file (second argument if SEPARATE).
 *		        3) Read Header if it is file header
 *
 *				In order that READ_FORMAT_LIST can process both
 *				a file header and record headers:
 *		        1) make sure there is enough room in the header buffer
 *
 *
 * ERRORS:
 *                      both the filename and header being defined
 *                      Format list with no header format
 *                      Problems making header format
 *                      No Header file found
 *                      Lack of memory for header
 *
 * AUTHOR:      LPD,TH, NGDC,(303)497-6472, haber@ngdc.noaa.gov
 *
 * COMMENTS:    This call will become vestigial when the memory
 *				management moves into the bin library.
 *
 * KEYWORDS:    
 *
 */                 		 
/*
 * HISTORY:
 *	Rich Fozzard	9/26/95		-rf01
 *		make header_file_path native before using
*/

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_input_header"

static int dbset_input_header(DATA_BIN_PTR dbin)
{
	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR pinfo = NULL;

	int error = 0;

	FF_VALIDATE(dbin);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_FILE | FFF_HEADER, &pinfo_list);
	if (!error)
	{
		pinfo = FF_PI(dll_first(pinfo_list));
		FF_VALIDATE(pinfo);

		error = setup_header(dbin, pinfo);
		if (error)
			return(error);
			
		error = db_set(dbin, DBSET_INIT_CONDUITS, FFF_FILE | FFF_HEADER, 0);
		if (error)
			return(error);

		error = db_do(dbin, DBDO_READ_FORMATS, pinfo_list);
		if (error && error != EOF)
			return(error);
	/*
		error = db_clear_array_mappings(dbin);
		if (error)
			return(error);
 
		error = db_create_array_mappings(dbin);
		if (error)
			return(error);
	  */
		error = finishup_header(dbin, pinfo);

		ff_destroy_process_info_list(pinfo_list);
		if (error)
			return(error);

		db_set(dbin, DBSET_BYTE_ORDER, FFF_INPUT | FFF_DATA);
 	}
	else if (error == ERR_GENERAL)
		error = 0;

	if (!error)
	{
		error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_REC | FFF_HEADER, &pinfo_list);
		if (!error)
		{
			PROCESS_INFO_LIST data_pinfo_list = NULL;

			pinfo = FF_PI(dll_first(pinfo_list));
			FF_VALIDATE(pinfo);

			assert(!IS_VARIED(PINFO_FORMAT(pinfo)));

			error = setup_header(dbin, pinfo);
			if (error)
				return(error);

			error = db_set(dbin, DBSET_INIT_CONDUITS, FFF_REC | FFF_HEADER, 0);
			if (error)
				return(error);

			error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT | FFF_DATA, &data_pinfo_list);
			if (error)
			{
				/* Disaster! */
				assert(0);
			}
			else
			{
				PROCESS_INFO_PTR data_pinfo = FF_PI(dll_first(data_pinfo_list));

				PINFO_ARRAY_DONE(data_pinfo) = 1;

				if (PINFO_MATE(data_pinfo))
					PINFO_MATE_ARRAY_DONE(data_pinfo) = 1;

				ff_destroy_process_info_list(data_pinfo_list);
			}

			error = db_do(dbin, DBDO_READ_FORMATS, pinfo_list);
			if (error && error != EOF)
				return(error);
		/*
			error = db_clear_array_mappings(dbin);
			if (error)
				return(error);
 
			error = db_create_array_mappings(dbin);
			if (error)
				return(error);
		  */

			error = finishup_header(dbin, pinfo);

			ff_destroy_process_info_list(pinfo_list);
			if (error)
				return(error);

			db_set(dbin, DBSET_BYTE_ORDER, FFF_INPUT | FFF_REC);
		} /* if not error */
		else if (error == ERR_GENERAL)
			error = 0;
	} /* if not error */

	return(error);
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

/*
* MESSAGE:     SET_INPUT_FORMAT_DATA_LIST
*
* OBJECT TYPE: DATA_BIN
*
* ARGUMENTS:   (TO SET)char *file_name, char *format_buffer
*
* file_name is the name of a format file (only .fmt
* extensions allowed) and format_buffer is a character pointer to text
* containing a format description block (i.e., the contents of a .fmt
* file).  If both arguments are NULL, this provokes a
* search for defaulting format (detailed under DESCRIPTION).  If both
* arguments are non-NULL, then format_buffer is taken not as a format
* description block, but as the desired format title (assuming that more
* than one format description in the located format file is eligible).
*
* If file_name is set to FORMAT_BUFFER_IS_A_TITLE 
* then a search for a defaulting format file ensues, with the contents
* of format_buffer taken as the desired format title.
*
 * PRECONDITIONS: dbin->file_name must be defined for default searches
 *
 * CHANGES IN STATE: 
 *                
 * DESCRIPTION: 
 *
 * If file_name is NULL and format_buffer is not, then format_buffer is
 * assumed to contain a format description block (i.e., the contents of
 * a .fmt file).
 * format_buffer containing a format description is used to create
 * a format, which is added into dbin->format_list and assigned to
 * dbin->input_format.
 * Currently the format created in this way is automatically given the
 * binary file
 * format descriptor type.  This apparently is an oversight and should be
 * corrected.  My suggestion (MAO) is to use the dbin->file_name extension
 * and the default extension rules, (i.e., .dat means ASCII, .dab means
 * dBASE, and anything else means binary).
 *
 * If both file_name and format_buffer are non-NULL (and file_name
 * has not been set to FORMAT_BUFFER_IS_A_TITLE) then file_name is
 * assumed to be the name of a .fmt file and format_buffer the desired
 * format title.  The contents of the format file are used to create a
 * temporary format list, but all data formats except the first occurring
 * format matching the given format title are deleted.  The first matching
 * format is added into dbin->format_list and assigned to dbin->input_format.
 *
 * If file_name has been set to FORMAT_BUFFER_IS_A_TITLE and format_buffer
 * is not NULL then the format_buffer is assumed to be the desired format
 * title.  Even though file_name is set to FORMAT_BUFFER_IS_A_TITLE the
 * menu is searched first for formats.  This means that format titles can
 * be applied to format sections in menu files.  If the menu does not
 * produce a format then
 * a default file search is conducted.  If a format file is found, then the
 * steps above for file_name and format_buffer being non-NULL are
 * performed.
 *
 * If file_name is non-NULL and format_buffer is NULL, then file_name
 * is assumed to be the name of a format file (.fmt).  The format is added into
 * dbin->format_list and assigned to dbin->input_format.  If a .fmt file,
 * then the contents are used to create a temporary format list.
 * 
 * If both file_name and format_buffer are NULL then the GeoVu menu file
 * is searched.  See the DESCRIPTION for dbs_search_menu_formats() for
 * more information.
 *
 * If both file_name and format_buffer are NULL and the menu search fails
 * then a default format
 * file search is conducted.  If a format file is found, then the contents
 * are used to create a temporary format list, and the first input format
 * is selected to be added into dbin->format_list and assigned to
 * dbin->input_format.  All output and read/write ambiguous data formats are
 * deleted from the temporary format list.  All other input formats which
 * do not have the same format title also deleted.  If multiple input
 * formats with identical titles exist in the same .fmt file, problems
 * may result, and no measures are taken to detect or prevent this.  If no
 * input format exists, then ambiguous formats are marked as either input
 * or output according to the extension of dbin->file_name and the format
 * file type (i.e., .dat/ASCII => input, .dat/binary/dBASE => output,
 * .bin/binary => input, .bin/ASCII/dBASE => output, .dab/dBASE => input,
 * .dab/ASCII/binary => output).  After this ambiguity resolution, the
 * first input format is selected to be added into dbin->format_list and
 * assigned to dbin->input_format.  All other data formats are deleted from
 * the temporary format list.
 * 
 * If an input format cannot be created above, this event returns an error
 * code.  Pending events are NOT processed.
 *
 * However, if an input format has been created in one of the above
 * scenarios, it is then checked for an "old style" header format variable.
 * If such exists, the header format is created via the input format and
 * added into the format list.
 *
 * Whether an old style header variable exists in the input format
 * or not, the DEFINE_INPUT_HEADER event is then called.  After the return of
 * this event, the input format is checked to see if it is a delimited
 * format (meaning that the variable start and end positions must be
 * determined dynamically).
 *
 * ERRORS:      Lack of memory for temporary file name
 *                      Problems making input format
 *
 * AUTHOR:      TH, NGDC,(303)497-6472, haber@ngdc.noaa.gov
 * modified substantially by: Kevin Frender, kbf@ngdc.noaa.gov
 *
 * COMMENTS:    Things are written to ch_ptr without checking to see if
 *				it is initialized. This seems unsafe.
 *
 * KEYWORDS:    
 *
 */
	 
#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_input_formats"

static int dbset_input_formats
	(
	 DATA_BIN_PTR dbin,
	 char *input_data_file_name,
	 char *output_data_file_name,
	 char *format_file_name,
	 char *format_buffer,
	 char *title_specified,
	 FORMAT_DATA_LIST_HANDLE format_data_list
	)
{
	FORMAT_DATA_PTR id_fd  = NULL; /* input data */
	FORMAT_DATA_PTR fh_fd = NULL; /* file header */
	FORMAT_DATA_PTR rh_fd = NULL; /* record header */

	FORMAT_DATA_LIST fdl = NULL;
	FORMAT_LIST f_list = NULL;

	int error = 0;

	assert(!(format_file_name && format_buffer));

	FF_VALIDATE(dbin);
		
	error = make_format_eqv_list(input_data_file_name,
		                          FFF_INPUT,
		                          format_file_name,
		                          format_buffer,
		                          &f_list,
		                          &(dbin->table_list),
		                          dbin
		                         );
	if (error)
		return(error);
			
	error = make_format_data(input_data_file_name,
		                      output_data_file_name,
		                      NULL,
		                      FFF_INPUT | FFF_HEADER | FFF_FILE,
		                      f_list,
		                      &fh_fd
		                     );
	if (error == ERR_GENERAL)
		error = 0;
	else if (error)
		return(error);

	if (fh_fd)
	{
		if (!*format_data_list)
		{
			*format_data_list = dll_init();
			if (!*format_data_list)
				return(ERR_MEM_LACK);
		}
			
		fdl = dll_add(*format_data_list);
		if (!fdl)
			return(ERR_MEM_LACK);
		else
			dll_assign(fh_fd, DLL_FD, fdl);
	}
		
	error = make_format_data(input_data_file_name,
		                      output_data_file_name,
		                      NULL,
		                      FFF_INPUT | FFF_HEADER | FFF_REC,
		                      f_list,
		                      &rh_fd
		                     );
	if (error == ERR_GENERAL)
		error = 0;
	else if (error)
		return(error);

	if (rh_fd)
	{
		if (!*format_data_list)
		{
			*format_data_list = dll_init();
			if (!*format_data_list)
				return(ERR_MEM_LACK);
		}
			
		fdl = dll_add(*format_data_list);
		if (!fdl)
			return(ERR_MEM_LACK);
		else
			dll_assign(rh_fd, DLL_FD, fdl);
	}
		
	error = make_format_data(input_data_file_name,
		                      output_data_file_name,
		                      title_specified,
		                      FFF_INPUT | FFF_DATA,
		                      f_list,
		                      &id_fd
		                     );
		
	if (error == ERR_GENERAL)
		error = 0;
	else if (error)
	{
		destroy_format_list(f_list);
		return(error);
	}

	if (id_fd)
	{
		if (!*format_data_list)
		{
			*format_data_list = dll_init();
			if (!*format_data_list)
			{
				destroy_format_list(f_list);
				return(ERR_MEM_LACK);
			}
		}
			
		fdl = dll_add(*format_data_list);
		if (!fdl)
		{
			destroy_format_list(f_list);
			return(ERR_MEM_LACK);
		}
		else
			dll_assign(id_fd, DLL_FD, fdl);
	}

	error = collect_record_formats(f_list, *format_data_list);
	destroy_format_list(f_list);
	if (error)
		return(error);

	if (!id_fd)
		return(ERR_MAKE_FORM);

	return(error);
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

/*
 * MESSAGE:     MAKE_NAME_TABLE
 *
 * OBJECT TYPE: DATA_BIN
 *
 * ARGUMENTS:   
 *
 * PRECONDITIONS:       dbin->buffer must be defined
 *
 * CHANGES IN STATE:    Name table is created
 *
 * DESCRIPTION: Attempts to create a name table.  If the supplied
 * filename argument is not NULL, this file is treated as a .eqv file,
 * and the dbin's name table is created from this file.  If (and only if)
 * the filename argument is NULL, then a default .eqv file search is
 * conducted.  This follows the same search criteria as for a .fmt file,
 * as in the SET_INPUT_FORMAT and SET_OUTPUT_FORMAT events.  (In fact, even the
 * environment variable FORMAT_DIR is searched for the .eqv file --
 * perhaps we need to make up an environment variable EQV_DIR?)  If a file
 * is found, it is used to create the dbin's name table.  If this search
 * fails, and the calling application is GeoVu, then the relevent menu's
 * eqv sections are searched.  (To wit:  default_eqv, merged into any
 * pattern-matching eqv's, merged into the file-level eqv.)
 *
 * ERRORS:      Lack of memory for temporary file name
 *                      Problems making name table (in nt_create)
 *
 * AUTHOR: Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * COMMENTS:    
 *
 * KEYWORDS:    
 *
 */
	 
#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_read_eqv"

static int dbset_read_eqv
	(
	 DATA_BIN_PTR dbin,
	 char *input_data_file_name
	)
{
	int number = 0;
	char **found_files = NULL;
				
	NAME_TABLE_PTR table = NULL;
	int error = 0;

	FF_VALIDATE(dbin);

	if (!input_data_file_name)
		return(0);

	number = find_files(input_data_file_name, "eqv", NULL, &found_files);
	if (number)
	{
		char *file_name = NULL;

		file_name = found_files[0];

		if (os_file_exist(file_name))
		{
			FF_BUFSIZE_PTR bufsize = NULL;

			error = ff_file_to_bufsize(file_name, &bufsize);
			if (error)
				return(error);

			error = nt_parse(file_name, bufsize, &table);
			ff_destroy_bufsize(bufsize);
			if (!error)
				error = nt_merge_name_table(&dbin->table_list, table);

			while (number)
				memFree(found_files[--number], "found_files[]");

			memFree(found_files, "found_files");
		} /* if os_file_exist(file_name) */
	} /* if number */

	return(error);
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

/*
 * MESSAGE:     DBSET_BYTE_ORDER
 *
 * OBJECT TYPE: DATA_BIN
 *
 * ARGUMENTS:   FF_TYPES_t format_type
 *
 * PRECONDITIONS:	Should only be called after ff_init_array_conduit_list.
 *
 * CHANGES IN STATE: 
 *                                              
 * DESCRIPTION:  Sets the format-data's byte_order state vector according
 * to the user-defined value of "data_byte_order".
 *
 * Calls nt_ask to search header, name table, and environment for
 * data_byte_order.  If found, the dbin byte_order state vector is set to
 * 0 for a value of "little_endian", 1 for a value of "big_endian", or
 * return with error if the value of data_byte_order is neither.
 *
 * ERRORS:                   
 *
 * AUTHOR: Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * COMMENTS:
 *
 * KEYWORDS:    
 *
 */
	 
/*****************************************************************************
 * NAME: dbset_byte_order
 *
 * PURPOSE:  Determine the data byte order for each format of format_type
 *
 * USAGE:  error = dbset_byte_order(dbin, format_type);
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
 * KEYWORDS:  data_byte_order
 *
 * ERRORS:
 ****************************************************************************/

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_byte_order"

static int dbset_byte_order
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t format_type
	)
{
	char data_byte_order[MAX_PV_LENGTH];
	int error = 0;

	assert(format_type);

	FF_VALIDATE(dbin);
			
	if (!nt_ask(dbin, (FFF_IO & format_type) | NT_ANYWHERE, "data_byte_order", FFV_CHAR, data_byte_order))
	{
		PROCESS_INFO_LIST pinfo_list = NULL;
		PROCESS_INFO_PTR pinfo = NULL;
		
		error = db_ask(dbin, DBASK_PROCESS_INFO, format_type, &pinfo_list);
		if (error)
			return(error);
			
		pinfo_list = dll_first(pinfo_list);
		pinfo = FF_PI(pinfo_list); 
		while (pinfo) /* not necessarily only input */
		{
			FF_VALIDATE(pinfo);

			if (os_strcmpi(data_byte_order, "DOS") == 0)
				PINFO_BYTE_ORDER(pinfo) = 0;
			else if (os_strcmpi(data_byte_order, "UNIX") == 0)
				PINFO_BYTE_ORDER(pinfo) = 1;
			else if (os_strcmpi(data_byte_order, "MAC") == 0 || os_strcmpi(data_byte_order, "MACOS") == 0)
				PINFO_BYTE_ORDER(pinfo) = 1;
			else if (os_strcmpi(data_byte_order, "LINUX") == 0)
				PINFO_BYTE_ORDER(pinfo) = 0;
			else if (!os_strcmpi(data_byte_order, "little_endian"))
				PINFO_BYTE_ORDER(pinfo) = 0;
			else if (!os_strcmpi(data_byte_order, "big_endian"))
				PINFO_BYTE_ORDER(pinfo) = 1;
			else
			{
				error = err_push(ERR_PARAM_VALUE, data_byte_order);
				break;
			}

			pinfo_list = dll_next(pinfo_list);
			pinfo = FF_PI(pinfo_list);
		}
		
		ff_destroy_process_info_list(pinfo_list);
	}

	return(error);
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

/*
 * MESSAGE:     SET_OUTPUT_FORMAT
 *
 * OBJECT TYPE: DATA_BIN
 *
 * ARGUMENTS:   (TO SET)char *file_name, char *format_buffer
 *				If file name and buffer are NULL, use dbin file name to create
 *				format from file_name.fmt.
 *              Revision by KBF, 3/29/95: if file_name AND format_buffer are
 *              both defined, format_buffer is assumed to be the title of the output format
 *              to be used.  If file_name is a ".fmt" file, and is different from the 
 *              input format file, the contents of file_name are appended to the
 *              dbin->format_list, with all formats specifically designated to be input
 *              removed.  The argument FORMAT_BUFFER_IS_A_TITLE can be sent in 
 *				as the file name if a format title, but not a format file,
 *				was specified by the user.  The FORMAT_BUFFER_IS_A_TITLE argument
 *				is essentially treated as if no filename were sent in.
 *
 * PRECONDITIONS:       dbin->file_name must be defined for default to work.
 *
 * CHANGES IN STATE:    ALLOCATES MEMORY FOR TEMPORARY FILE NAME
 *                
 * DESCRIPTION: Defines an output format for a data bin.  The output
 * format search parallels the search for an input format.
 * See the DESCRIPTION for SET_INPUT_FORMAT for more information.
 *
 * ERRORS:      Lack of memory for temporary file name
 *                      Problems making input format
 *
 * AUTHOR:      TH, NGDC,(303)497-6472, haber@ngdc.noaa.gov
 *
 * COMMENTS:
 * MAO: I am uncertain that appropriate extensions are being looked for.
 *
 * KEYWORDS:    
 *
 */
	 
#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_output_formats"

static int dbset_output_formats
	(
	 DATA_BIN_PTR dbin,
	 char *input_data_file_name,
	 char *output_data_file_name,
	 char *file_name,
	 char *format_buffer,
	 char *title_specified,
	 FORMAT_DATA_LIST_HANDLE format_data_list
	)
{
	FORMAT_DATA_PTR od_fd  = NULL; /* input data */
	FORMAT_DATA_PTR fh_fd = NULL; /* file header */
	FORMAT_DATA_PTR rh_fd = NULL; /* record header */

	FORMAT_DATA_LIST fdl = NULL;
	FORMAT_LIST f_list = NULL;

	int error = 0;

	FF_VALIDATE(dbin);

	assert(!(file_name && format_buffer));
		
	error = make_format_eqv_list(input_data_file_name,
		                          FFF_OUTPUT,
		                          file_name,
		                          format_buffer,
		                          &f_list,
		                          &(dbin->table_list),
		                          dbin
		                         );
	if (error)
		return(error);
			
	error = make_format_data(input_data_file_name,
		                      output_data_file_name,
		                      NULL,
		                      FFF_OUTPUT | FFF_HEADER | FFF_FILE,
		                      f_list,
		                      &fh_fd
		                     );
	if (error == ERR_GENERAL)
	{
		error = 0;
	}
	else if (error)
		return(error);

	if (fh_fd)
	{
		if (!*format_data_list)
		{
			*format_data_list = dll_init();
			if (!*format_data_list)
				return(ERR_MEM_LACK);
		}
			
		fdl = dll_add(*format_data_list);
		if (!fdl)
			return(ERR_MEM_LACK);
		else
			dll_assign(fh_fd, DLL_FD, fdl);
			
		error = find_initialization_files(dbin, input_data_file_name, fh_fd->format);
		if (error)
			return(error);
	}
		
	error = make_format_data(input_data_file_name,
		                      output_data_file_name,
		                      NULL,
		                      FFF_OUTPUT | FFF_HEADER | FFF_REC,
		                      f_list,
		                      &rh_fd
		                     );
	if (error == ERR_GENERAL)
	{
		error = 0;
	}
	else if (error)
		return(error);

	if (rh_fd)
	{
		if (!*format_data_list)
		{
			*format_data_list = dll_init();
			if (!*format_data_list)
				return(ERR_MEM_LACK);
		}
			
		fdl = dll_add(*format_data_list);
		if (!fdl)
			return(ERR_MEM_LACK);
		else
			dll_assign(rh_fd, DLL_FD, fdl);
			
		error = find_initialization_files(dbin, input_data_file_name, rh_fd->format);
		if (error)
			return(error);
	}
		
	error = make_format_data(input_data_file_name,
		                      output_data_file_name,
		                      title_specified,
		                      FFF_OUTPUT | FFF_DATA,
		                      f_list,
		                      &od_fd
		                     );
		
	destroy_format_list(f_list);
		
	if (error == ERR_GENERAL)
	{
		error = 0;
	}
	else if (error)
		return(error);

	if (od_fd)
	{
		if (!*format_data_list)
		{
			*format_data_list = dll_init();
			if (!*format_data_list)
				return(ERR_MEM_LACK);
		}
			
		fdl = dll_add(*format_data_list);
		if (!fdl)
			return(ERR_MEM_LACK);
		else
			dll_assign(od_fd, DLL_FD, fdl);
			
		error = find_initialization_files(dbin, input_data_file_name, od_fd->format);
		if (error)
			return(error);
	}

	if (!(od_fd || fh_fd || rh_fd))
		return(ERR_MAKE_FORM);

	return(error);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_query_restriction"

static int dbset_query_restriction
	(
	 DATA_BIN_PTR dbin,
	 char *query_file_name
	)
{
	FF_BUFSIZE_PTR bufsize = NULL;
	char *ch = NULL;

	FORMAT_DATA_PTR input = NULL;
	int error = 0;

	FF_VALIDATE(dbin);
		
	assert(query_file_name);

	error = ff_file_to_bufsize(query_file_name, &bufsize);
	if (error)
		return(error);
		
	ch = bufsize->buffer - 1;
	while (++ch)
	{
		if (ch[0] < ' ')
		{
			if (ch[0] == '\0')
				break;
	
			/* End-of-line character */
			ch[0] = '\0';
			break;
		}
	}

	input = fd_get_data(dbin, FFF_INPUT);
	if (!input)
		return(err_push(ERR_GENERAL, "Input format must be defined for query"));

	dbin->eqn_info = ee_make_std_equation(bufsize->buffer, input->format);
	ff_destroy_bufsize(bufsize);
	if (!dbin->eqn_info)
		return(err_push(ERR_GEN_QUERY, "Setting up the query"));

	return(error);
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

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dbset_variable_restriction"

static int dbset_variable_restriction
	(
	 char *variable_file_name,
	 FORMAT_PTR output_format
	)
{
	int error = 0;
	FF_BUFSIZE_PTR bufsize = NULL;

	FF_VALIDATE(output_format);

	error = ff_file_to_bufsize(variable_file_name, &bufsize);
	if (error)
		return(error);

	error = trim_format(output_format, bufsize->buffer);
	if (!error)
		make_contiguous_format(output_format);
	
	ff_destroy_bufsize(bufsize);

	return(error);
}

/*
 * NAME:	dynamic_update_var
 *		
 * PURPOSE:	 Change the variable info according to the new data_type and bytes_per_pixel
 *
 * USAGE:	
 *
 * RETURNS:	 Zero on success, an error code on failure.
 *
 * DESCRIPTION:  Update the start and end positions for each variable affected by the
 * change, and update the format's max_length.  Update the given variable's information,
 * namely:  1) type (to the new data_type), 2) end_pos, and 3) array_desc_str.
 *
 * SYSTEM DEPENDENT FUNCTIONS:	none
 *
 * GLOBALS:	none
 *
 * AUTHOR:
 *
 * ERRORS:
 *
 * COMMENTS:	
 *
 * KEYWORDS:
 *
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dynamic_update_var"

static int dynamic_update_var
	(
	 VARIABLE_PTR var,
	 FF_TYPES_t data_type,
	 short bytes_per_pixel,
	 short *diff
	)
{
	int error = 0;

	VARIABLE_LIST vlist = NULL;
	VARIABLE_PTR vstepper = NULL;

	FF_VALIDATE(var);

	*diff = (short)(bytes_per_pixel - FF_VAR_LENGTH(var));

	var->end_pos += *diff;

	var->type &= ~(FFV_DATA_TYPES);
	var->type |= data_type;

	if (IS_ARRAY(var))
	{
		sprintf(var->array_desc_str + strlen(var->array_desc_str) - FF_ARRAY_ELEMENT_SIZE_WIDTH,
		        "%*d", FF_ARRAY_ELEMENT_SIZE_WIDTH, (int)bytes_per_pixel
		       );
	}
		
	return(error);
}

/*
 * NAME:	dynamic_update_var
 *		
 * PURPOSE:	 
 *
 * USAGE:	
 *
 * RETURNS:	 Zero on success, an error code on failure.
 *
 * DESCRIPTION:  
 *
 * SYSTEM DEPENDENT FUNCTIONS:	none
 *
 * GLOBALS:	none
 *
 * AUTHOR:
 *
 * ERRORS:
 *
 * COMMENTS:	
 *
 * KEYWORDS:
 *
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "dynamic_update_format"

static int dynamic_update_format
	(
	 VARIABLE_PTR var,
	 FORMAT_PTR format,
	 short diff
	)
{
	int error = 0;

	VARIABLE_LIST vlist = NULL;
	VARIABLE_PTR vstepper = NULL;

	FF_VALIDATE(format);
	FF_VALIDATE(var);

	var->end_pos += diff;

	/* change the position of the variables after the current variable var */
	vlist = FFV_FIRST_VARIABLE(format);
	vstepper = FF_VARIABLE(vlist);
	while (vstepper)
	{
		FF_VALIDATE(vstepper);
		
		if (vstepper->start_pos > var->start_pos)
		{
			vstepper->start_pos += diff;
			vstepper->end_pos += diff;
		}

		vlist = dll_next(vlist);
		vstepper = FF_VARIABLE(vlist);
	}

	format->length += diff;

	return(error);
}

/*
 * NAME:		get_default_var
 *		
 * PURPOSE:	To get the default variable for image display
 *
 * USAGE:	VARIABLE_LIST *get_default_var(FORMAT_PTR form)
 *
 * RETURNS:	the variable to be display. NULL if can be found
 *
 * DESCRIPTION:	The function first checks whether or not the variables
 *				bsq, bil, bip2, or data exist. If no, the function will
 *				try to find the variable with numerical data type. 
 *
 * SYSTEM DEPENDENT FUNCTIONS:	none
 *
 * GLOBAL:	none
 *
 * AUTHOR:	Liping Di, NGDC, (303) 497 - 6284, lpd@mail.ngdc.noaa.gov
 *
 * COMMENTS:	
 *
 * KEYWORDS:	
 *
 */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "get_default_var"

static VARIABLE *get_default_var(FORMAT_PTR form)
{
 	VARIABLE_PTR var;
	VARIABLE_LIST v_list;
	
	FF_VALIDATE(form);
	
	var = ff_find_variable("bsq",form);
	if (!var)
		var = ff_find_variable("bil",form);

	if (!var)
		var = ff_find_variable("bip2",form);

	if (!var)
		var = ff_find_variable("data",form);

	if (!var)
	{
		v_list = FFV_FIRST_VARIABLE(form);
		var = FF_VARIABLE(v_list);
		while (var)
		{
			if (!IS_TEXT(var))
				break;

			v_list = dll_next(v_list);
			var = FF_VARIABLE(v_list);
		}
	}
	
	return(var);
}

/*****************************************************************************
 * NAME:  change_input_img_format()
 *
 * PURPOSE:  Dynamically change input format according to data_representation
 * and bytes_per_pixel
 *
 * USAGE:  change_input_img_format(data_bin);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  Sets the data bin image parameters data_type according to
 * value of data_representation, and bytes_per_pixel according to namesake.
 * Calls dynamic_update_var() to change variable type and size for the qualifying
 * variable in the data bin input format.
 *
 * If input format is NULL nothing is done.
 *
 * AUTHOR:  (edited by) Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:  This function needs some error handling
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "change_input_img_format"

static int change_input_img_format
	(
	 DATA_BIN_PTR dbin,
	 FORMAT_PTR input
	)
{
	int error = 0;

	char data_rep[MAX_PV_LENGTH] = {""};
	short bytes_per_pixel;
	VARIABLE_PTR var = NULL;
	FF_TYPES_t new_data_type;

	FF_VALIDATE(dbin);

	error = nt_ask(dbin, NT_ANYWHERE, "data_representation", FFV_CHAR, data_rep);
	if (error == ERR_NT_KEYNOTDEF)
		error = 0;
	else if (!error)
	{
		new_data_type = ff_lookup_number(variable_types, data_rep);
		if (new_data_type == FF_VAR_TYPE_FLAG)
		{
			error = err_push(ERR_UNKNOWN_VAR_TYPE, "Defined for data_representation (\"%s\")", data_rep);
		}
		else
		{
			/* change bytes_per_pixel due to the change of data_type */
			if (IS_BINARY(input))
				bytes_per_pixel = (short)ffv_type_size(new_data_type);
			else
				error = nt_ask(dbin, NT_ANYWHERE, "bytes_per_pixel", FFV_SHORT, &bytes_per_pixel);
			
			if (!error)
			{
				var = get_default_var(input);
				if (var)
				{
					short diff = 0;

					/* Does var have a precision of zero and are we changing the data type
						from an integer to a float or a double?  If so, change the precision to
						a magic number like five or nine.
					*/
							
					if (var->precision == 0 && IS_INTEGER(var) && IS_REAL_TYPE(new_data_type))
					{
						if (IS_FLOAT32_TYPE(new_data_type))
							var->precision = 5;
						else
							var->precision = 9;
					}
					
					error = dynamic_update_var(var, new_data_type, bytes_per_pixel, &diff);
					if (!error)
						error = dynamic_update_format(var, input, diff);
				}
			} /* if bytes_per_pixel */
		} /* if data_type != FF_VAR_TYPE_FLAG */
	} /* if nt_ask() for data_representation */
	
	return(error);
}

/*****************************************************************************
 * NAME: ask_change_input_img_format()
 *
 * PURPOSE:  Change the input format according to data_representation and
 * bytes_per_pixel, if data file is an image file.
 *
 * USAGE:  error = ask_change_input_img_format(dbin);
 *
 * RETURNS: Zero on success, ERR_MEM_LACK on failure
 *
 * DESCRIPTION:  Checks value
 * of GeoVu keyword "data_type" for a substring containing "image", "raster",
 * or "grid".  Initializes dbin->data_parameters to an
 * IMAGE structure (remembering the previous pointer, to be restored later),
 * and calls change_input_img_format().
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:  I can't distinguish between newform/checkvar calling this event,
 * or GeoVu, so there is redundancy when this function gets called again in
 * set_image_limit().  However, this call cannot be removed from set_image_limit(),
 * as bytes_per_pixel get set in other ways by that calling code.  This also
 * precludes newform() and checkvar() running on .pcx files.  This can be fixed
 * by making this function more intelligent (set_image_limit() less?).
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "ask_change_input_img_format"

static BOOLEAN ask_change_input_img_format
	(
	 DATA_BIN_PTR dbin
	)
{
	char data_type[MAX_PV_LENGTH] = {""};

	FF_VALIDATE(dbin);
	
	if (!nt_ask(dbin, NT_ANYWHERE, "data_type", FFV_CHAR, data_type))
	{
		if (strstr(data_type, "image") || strstr(data_type, "raster") || strstr(data_type, "grid"))
			return(TRUE);
	}

	return(FALSE);
}

static int check_vars_types_for_keywords
	(
	 DATA_BIN_PTR dbin,
	 FORMAT_PTR format
	)
{
	int error_return = 0;
	int error = 0;

	VARIABLE_PTR var = NULL;
	VARIABLE_LIST vlist = NULL;

	FF_VALIDATE(dbin);
	FF_VALIDATE(format);

	vlist = FFV_FIRST_VARIABLE(format);
	var = FF_VARIABLE(vlist);
	while (var)
	{
		if (!FFV_DATA_TYPE(var) && !IS_CONSTANT_VAR(var) && !IS_INITIAL_VAR(var))
		{
			char variable_type[MAX_PV_LENGTH] = {""};

			if (!IS_BINARY(format))
				return(err_push(ERR_GENERAL, "Keyworded variable types only supported for binary formats (\"%s\")", format->name));

			assert(var->record_title);

			error = nt_ask(dbin, NT_ANYWHERE, var->record_title, FFV_TEXT, variable_type);
			if (error)
				err_push(error, "Keyworded variable type (\"%s\") not defined for %s", var->record_title, var->name);
			else
			{
				FF_TYPES_t data_type = FFV_NULL;

				data_type = ff_lookup_number(variable_types, variable_type);
				if (data_type == FF_VAR_TYPE_FLAG)
				{
					error = err_push(ERR_VARIABLE_DESC, "Unknown variable type for \"%s\"", var->name);
				}
				else
				{
					short diff = 0;
					short bytes_per_pixel = (short)ffv_type_size(data_type);

					error = dynamic_update_var(var, data_type, bytes_per_pixel, &diff);
					if (!error && diff)
						error = dynamic_update_format(var, format, diff);
				}
			}
		} /* if (!FFV_DATA_TYPE(var) && !IS_CONSTANT_VAR(var) && !IS_INITIAL_VAR(var)) */

		if (error)
			error_return = error;

		vlist = FFV_NEXT_VARIABLE(vlist);
		var = FF_VARIABLE(vlist);
	}

	return(error_return);
}

static int dbset_user_update_formats
	(
	 DATA_BIN_PTR dbin
	)
{
	int error = 0;

	FORMAT_DATA_PTR input_fd = NULL;
	FORMAT_DATA_PTR output_fd = NULL;

	FF_VALIDATE(dbin);

	input_fd = fd_get_data(dbin, FFF_INPUT);
	if (input_fd)
	{
 		FF_VALIDATE(input_fd);

		error = check_vars_types_for_keywords(dbin, input_fd->format);
		if (error)
			return(error);

		if (ask_change_input_img_format(dbin))
		{
			error = change_input_img_format(dbin, input_fd->format);
			if (error)
				return(error);
		}
	}

	output_fd = fd_get_data(dbin, FFF_OUTPUT);
	if (output_fd)
	{
		FF_VALIDATE(output_fd);

		error = check_vars_types_for_keywords(dbin, output_fd->format);
	}

	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif 
#define ROUTINE_NAME "copy_variable_list"

/*****************************************************************************
 * NAME: copy_variable_list
 *
 * PURPOSE:  Duplicate a format's variable list
 *
 * USAGE:  error = copy_variable_list(source_list, target_LIST)
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
 * COMMENTS:
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int copy_variable_list
	(
	 VARIABLE_LIST vsource,
	 VARIABLE_LIST_HANDLE hvtarget
	)
{
	VARIABLE_PTR source_var = NULL;
	VARIABLE_PTR new_var = NULL;
	VARIABLE_LIST source_list = NULL;
	VARIABLE_LIST new_list = NULL;
	
	int error;

	assert(vsource);
	assert(hvtarget);
	
	*hvtarget = new_list = dll_init();
	if (!new_list)
		return(ERR_MEM_LACK);

	source_list = dll_first(vsource);
	source_var  = FF_VARIABLE(source_list);
	while (source_var != NULL)
	{
		new_var = ff_create_variable(source_var->name);
		if (!new_var)
		{
			dll_free_holdings(*hvtarget);
			return(ERR_MEM_LACK);
		}

		error = ff_copy_variable(source_var, new_var);
		if (error)
		{
			ff_destroy_variable(new_var);
			dll_free_holdings(*hvtarget);
			return(error);
		}

		new_list = dll_add(*hvtarget);
		if (!new_list)
		{
			ff_destroy_variable(new_var);
			dll_free_holdings(*hvtarget);
			return(ERR_MEM_LACK);
		}

		dll_assign(new_var, DLL_VAR, new_list);
		source_list = dll_next(source_list);
		source_var  = FF_VARIABLE(source_list);
	}

	return(0);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif 
#define ROUTINE_NAME "copy_format"

/*
 * NAME:	copy_format
 *
 * PURPOSE:	To create a format in memory.
 *
 * USAGE: format_copy = copy_format(source->format);
 *
 * RETURNS:	A pointer to the new format, else NULL
 *
 * DESCRIPTION:	This function makes a duplicate of an existing format
 *
 * AUTHOR:	T. Habermann, NGDC, (303) 497 - 6472, haber@mail.ngdc.noaa.gov
 *
 * COMMENTS:	
 *
 * ERRORS:
 *
 * SYSTEM DEPENDENT FUNCTIONS: 
 *
 * KEYWORDS:
 *
 */
 
static FORMAT_PTR copy_format(FORMAT_PTR format)
{
	FORMAT_PTR new_format = NULL;
	
	FF_VALIDATE(format);
	
	if (!format)
		return(NULL);

	new_format = ff_create_format(format->name, format->locus);
	if (new_format == NULL)
		return(NULL);

	if (format->variables)
	{
		int error = 0;

		error = copy_variable_list(format->variables, &new_format->variables);
		if (error)
		{
			ff_destroy_format(new_format);
			return(NULL);
		}
	}

	if (new_name_string__(format->name, &new_format->name))
		return(NULL);

	new_format->type = format->type;
	new_format->num_vars = format->num_vars;
	new_format->length = format->length;

	return(new_format);
}

static int spawn_record_array_format
	(
	 FORMAT_PTR format,
	 FORMAT_DATA_PTR fd,
	 VARIABLE_PTR record_var,
	 FORMAT_HANDLE spawn_format_h
	)
{
	VARIABLE_PTR var = NULL;

	char *spawn_name = NULL;

	FF_VALIDATE(format);
	FF_VALIDATE(record_var);

	*spawn_format_h = copy_format(fd->format);
	if (!*spawn_format_h)
		return(ERR_MEM_LACK);

	if ((*spawn_format_h)->name)
		memFree((*spawn_format_h)->name, "(*spawn_format_h)->name");

	spawn_name = (char *)memMalloc(strlen(format->name) + strlen(record_var->name) + 2 + 1, "spawn_name");
	if (!spawn_name)
		return(err_push(ERR_MEM_LACK, ""));

	sprintf(spawn_name, "%s::%s", format->name, record_var->name);
	(*spawn_format_h)->name = spawn_name;

	FFF_TYPE(*spawn_format_h) = FFF_TYPE(format) | FFF_RECORD;

	var = FF_VARIABLE(FFV_FIRST_VARIABLE((*spawn_format_h)));
	if (var)
	{
		FF_VALIDATE(var);

		assert(!var->array_desc_str);
		var->array_desc_str = (char *)memStrdup(record_var->array_desc_str, "var->array_desc_str");
		if (!var->array_desc_str)
		{
			ff_destroy_format(*spawn_format_h);
			return(err_push(ERR_MEM_LACK, ""));
		}
	}
	else
		assert(var);

	return(0);
}

static int spawn_array_format
	(
	 FORMAT_PTR format,
	 VARIABLE_PTR var,
	 FORMAT_HANDLE spawn_format_h
	)
{
	VARIABLE_PTR spawn_var = NULL;
 	char *spawn_name = NULL;

	int error = 0;

	FF_VALIDATE(var);

	*spawn_format_h = ff_create_format(NULL, format->locus);
	if (!*spawn_format_h)
		return(err_push(ERR_MEM_LACK, ""));

	spawn_name = (char *)memMalloc(strlen(format->name) + strlen(var->name) + 2 + 1, "spawn_name");
	if (!spawn_name)
		return(err_push(ERR_MEM_LACK, ""));

	sprintf(spawn_name, "%s::%s", format->name, var->name);
	(*spawn_format_h)->name = spawn_name;

	(*spawn_format_h)->variables = dll_init();
	if (!(*spawn_format_h)->variables)
		return(err_push(ERR_MEM_LACK, ""));

	if (!dll_add((*spawn_format_h)->variables))
		return(err_push(ERR_MEM_LACK, ""));

	spawn_var = ff_create_variable(var->name);
	if (!spawn_var)
		return(err_push(ERR_MEM_LACK, ""));

	dll_assign(spawn_var, DLL_VAR, dll_last((*spawn_format_h)->variables));

	error = ff_copy_variable(var, spawn_var);
	if (error)
		return(error);

	(*spawn_format_h)->type = format->type;
	(*spawn_format_h)->num_vars = 1;
	(*spawn_format_h)->length = FF_VAR_LENGTH(var);

	return(0);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "spawn_array_format_data"

static int spawn_array_format_data
	(
	 FORMAT_DATA_LIST format_data_list,
	 FORMAT_PTR format,
	 VARIABLE_PTR var,
	 FORMAT_DATA_HANDLE fdh
	)
{
	int error = 0;

	FORMAT_PTR spawn_format = NULL;

	FF_VALIDATE(format);
	FF_VALIDATE(var);

	assert(fdh);

	/* a record array might in fact just be a simple array with a keyword as the variable type */
	if (IS_RECORD_VAR(var))
	{
		FORMAT_DATA_PTR fd = NULL;

		fd = fd_find_format_data(format_data_list, FFF_NAME_CASE, var->record_title);
		if (fd)
		{
			if (!IS_RECORD_FORMAT(fd->format))
				return(err_push(ERR_VARIABLE_DESC, "Variable type of %s must refer to a RECORD format", var->name));

			error = spawn_record_array_format(format, fd, var, &spawn_format);
			if (error)
				return(error);
		}
		else
		{
			/* Treat this as a keyword variable type in a format description */

			FFV_TYPE(var) &= ~FFV_RECORD;
			FFV_TYPE(var) |= FF_ARRAY;
		}
	}

	/* var might have been RECORD but has changed to ARRAY */
	if (!IS_RECORD_VAR(var))
	{
		error = spawn_array_format(format, var, &spawn_format);
		if (error)
		{
			ff_destroy_format(spawn_format);
			return(error);
		}
	}

	*fdh = fd_create_format_data(spawn_format, FORMAT_LENGTH(spawn_format) + 1, NULL);
	if (!*fdh)
	{
		ff_destroy_format(spawn_format);
		return(err_push(ERR_MEM_LACK, ""));
	}

	return(0);
}

static void set_array_offsets
	(
	 FF_ARRAY_CONDUIT_PTR array_conduit
	)
{
	FORMAT_PTR input_format = NULL;
	VARIABLE_PTR input_var = NULL;

	FORMAT_PTR output_format = NULL;
	VARIABLE_PTR output_var = NULL;

	input_format = array_conduit->input->fd->format;
	input_var = FF_VARIABLE(FFV_FIRST_VARIABLE(input_format));

	if (array_conduit->output)
	{
		output_format = array_conduit->output->fd->format;
		output_var = FF_VARIABLE(FFV_FIRST_VARIABLE(output_format));
	}

	if (array_conduit->input->connect.id & NDARRS_FILE)
		array_conduit->input->connect.file_info.array_offset = input_var->start_pos - 1;
	else
		array_conduit->input->connect.file_info.array_offset = 0;

	if (array_conduit->output->connect.id & NDARRS_FILE)
		array_conduit->output->connect.file_info.array_offset = output_var->start_pos - 1;
	else
		array_conduit->output->connect.file_info.array_offset = 0;

	input_var->end_pos = FF_VAR_LENGTH(input_var);
	input_var->start_pos = 1;

	if (output_format)
	{
		output_var->end_pos = FF_VAR_LENGTH(output_var);
		output_var->start_pos = 1;
	}
}

static void set_record_array_offsets
	(
	 VARIABLE_PTR in_var,
	 VARIABLE_PTR out_var,
	 FF_ARRAY_CONDUIT_PTR array_conduit
	)
{
	FF_VALIDATE(in_var);
	
	if (array_conduit->input->connect.id & NDARRS_FILE)
		array_conduit->input->connect.file_info.array_offset = in_var->start_pos - 1;
	else
		array_conduit->input->connect.file_info.array_offset = 0;

	if (out_var)
	{
		FF_VALIDATE(out_var);

		if (array_conduit->output->connect.id & NDARRS_FILE)
			array_conduit->output->connect.file_info.array_offset = out_var->start_pos - 1;
		else
			array_conduit->output->connect.file_info.array_offset = 0;
	}
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "create_array_pole"

/*****************************************************************************
 * NAME: create_array_pole
 *
 * PURPOSE: To create an array pole
 *
 * USAGE:  create_array_pole();
 *
 * RETURNS:  NULL on error, otherwise a pointer to an array pole
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

static int create_array_pole
	(
	 char *name,
	 FORMAT_DATA_PTR format_data,
	 NDARR_SOURCE id,
	 char *filename,
	 FF_BUFSIZE_PTR bufsize,
	 FF_ARRAY_DIPOLE_HANDLE pole_h
	)
{
	int error = 0;

	FF_VALIDATE(format_data);
	FF_VALIDATE(format_data->format);
	FF_VALIDATE(format_data->data);

	assert(id & (NDARRS_BUFFER | NDARRS_FILE));
	
	if (bufsize)
		FF_VALIDATE(bufsize);
	
	assert(pole_h);

	*pole_h = (FF_ARRAY_DIPOLE_PTR)memMalloc(sizeof(FF_ARRAY_DIPOLE), "*pole_h");
	if (*pole_h)
	{
#ifdef FF_CHK_ADDR
		(*pole_h)->check_address = (void *)*pole_h;
#endif
		(*pole_h)->mate = NULL;
		(*pole_h)->format_data_mapping = NULL;

		(*pole_h)->name = (char *)memStrdup(name, "(*pole_h)->name");
		if (!(*pole_h)->name)
		{
			memFree(*pole_h, "*pole_h");
			*pole_h = NULL;
			return(err_push(ERR_MEM_LACK, ""));
		}

		(*pole_h)->fd = format_data;

		(*pole_h)->array_mapping = NULL;

		(*pole_h)->connect.id = id;
		
		(*pole_h)->connect.file_info.array_offset = 0;

		(*pole_h)->connect.locus.filename = NULL;
		(*pole_h)->connect.locus.bufsize = NULL;

		if ((id & NDARRS_FILE) && filename)
		{
			(*pole_h)->connect.id |= NDARRS_UPDATE;

			(*pole_h)->connect.locus.filename = memStrdup(filename, "filename");
			if (!(*pole_h)->connect.locus.filename)
			{
				memFree(*pole_h, "*pole_h");
				*pole_h = NULL;
				return(err_push(ERR_MEM_LACK, NULL));
			}
		}
		else if ((id & NDARRS_BUFFER) && bufsize)
			(*pole_h)->connect.locus.bufsize = bufsize;
		else if (!(id & (NDARRS_BUFFER | NDARRS_FILE)))
			return(err_push(ERR_API, "Calling create_array_pole with with incorrect ID"));

		(*pole_h)->connect.array_done = 0;
		(*pole_h)->connect.bytes_left = 0;
	} /* if *pole_h */
	else
		return(err_push(ERR_MEM_LACK, NULL));
	
	return(0);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "create_array_conduit"

/*****************************************************************************
 * NAME: create_array_conduit
 *
 * PURPOSE: To create an array conduit
 *
 * USAGE:  create_array_conduit(conduit_ptr);
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

static int create_array_conduit
	(
	 char *name,
	 FF_ARRAY_DIPOLE_PTR input,
	 FF_ARRAY_DIPOLE_PTR output,
	 FF_ARRAY_CONDUIT_HANDLE conduit_h
	)
{
	int error = 0;

	assert(conduit_h);

	*conduit_h = (FF_ARRAY_CONDUIT_PTR)memMalloc(sizeof(FF_ARRAY_CONDUIT), "*conduit_h");
	if (*conduit_h)
	{
#ifdef FF_CHK_ADDR
		(*conduit_h)->check_address = (void *)*conduit_h;
#endif
		
		strncpy((*conduit_h)->name, name, sizeof((*conduit_h)->name) - 1);
		(*conduit_h)->name[sizeof((*conduit_h)->name) - 1] = STR_END;
		
		if (input)
		{
			FF_VALIDATE(input);
			(*conduit_h)->input = input;

			if (output)
				input->mate = output;
		}
		else
			(*conduit_h)->input = NULL;
		
		if (output)
		{
			FF_VALIDATE(output);
			(*conduit_h)->output = output;

			if (input)
				output->mate = input;
		}
		else
			(*conduit_h)->output = NULL;
	}
	
	return(error);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "make_tabular_array_conduit"

/*****************************************************************************
 * NAME:  make_tabular_array_conduit
 *
 * PURPOSE:  Take two traditional FreeForm formats and initialize an array conduit,
 * adding it to the array conduit list.
 *
 * USAGE:
 *
 * RETURNS:  Zero on success, an error code on failure
 *
 * DESCRIPTION:  Allocate memory for header formats.  For data formats,
 * graft the passed parameters input_data and output_data bufsizes.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:  For now input_src and output_src are taken as input and output
 * data file names.
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int make_tabular_array_conduit
	(
	 FORMAT_DATA_PTR input,
	 FORMAT_DATA_PTR output,
	 FF_STD_ARGS_PTR std_args,
	 FF_ARRAY_CONDUIT_LIST array_conduit_list
	)
{
	int error = 0;
	
	FF_ARRAY_CONDUIT_PTR array_conduit = NULL;
	FF_ARRAY_DIPOLE_PTR input_pole = NULL;
	FF_ARRAY_DIPOLE_PTR output_pole = NULL;
	
	FF_VALIDATE(input);
	FF_VALIDATE(std_args);
	
	error = create_array_pole(input->format->name,
	                             input,
	                             (NDARR_SOURCE)(std_args->input_file ?
	                             NDARRS_FILE:
	                             NDARRS_BUFFER),
	                             std_args->input_file,
	                             std_args->input_bufsize,
	                             &input_pole
	                            );
	if (error)
		goto make_tabular_array_conduit_exit;
	
	if (output)
	{
		FF_VALIDATE(output);
	
		error = create_array_pole(output->format->name,
		                             output,
		                             (NDARR_SOURCE)(std_args->output_file ?
		                             NDARRS_FILE :
		                             NDARRS_BUFFER),
		                             std_args->output_file,
		                             std_args->output_bufsize,
		                             &output_pole
		                            );
		if (error)
			goto make_tabular_array_conduit_exit;
	}
	else
		output_pole = NULL;
	
	error = create_array_conduit("tabular", input_pole, output_pole, &array_conduit);
	if (error)
	{
		goto make_tabular_array_conduit_exit;
	}
	
	assert(!FF_AC(array_conduit_list));
	
	array_conduit_list = dll_add(array_conduit_list);
	if (!array_conduit_list)
		error = err_push(ERR_MEM_LACK, "");
	else
		dll_assign(array_conduit, DLL_AC, array_conduit_list);
	
make_tabular_array_conduit_exit:
	if (error)
	{
		if (array_conduit)
			ff_destroy_array_conduit(array_conduit);
		else
		{
			if (input_pole)
				ff_destroy_array_pole(input_pole);
			else if (input)
				fd_destroy_format_data(input);

			if (output_pole)
				ff_destroy_array_pole(output_pole);
			else if (output)
				fd_destroy_format_data(output);
		}
	}
	
	return(error);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "spawn_array_conduits"

/*****************************************************************************
 * NAME:  spawn_array_conduits
 *
 * PURPOSE:  Take two ARRAY FreeForm formats and spawns array conduits for each
 * matching array variable in the formats.
 *
 * USAGE:
 *
 * RETURNS:  Zero on success, an error code on failure
 *
 * DESCRIPTION:  Allocate memory for header formats.  For data formats,
 * graft the passed parameters input_data and output_data bufsizes.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:  For now input_src and output_src are taken as input and output
 * data file names.
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

static int spawn_array_conduits
	(
	 FORMAT_DATA_PTR input,
	 FORMAT_DATA_PTR output,
	 FF_STD_ARGS_PTR std_args,
	 FF_ARRAY_CONDUIT_LIST array_conduit_list,
	 FORMAT_DATA_LIST format_data_list
	)
{
	FF_BUFSIZE_PTR new_id = NULL;
	FF_BUFSIZE_PTR new_od = NULL;

	VARIABLE_LIST variables = NULL;
	VARIABLE_PTR in_var = NULL;

	int error = 0;
	
	FF_VALIDATE(input);
	FF_VALIDATE(std_args);

	if (output)
		FF_VALIDATE(output);
	else
		return(0);

	variables = FFV_FIRST_VARIABLE(input->format);
	in_var    = FF_VARIABLE(variables);
	while (in_var)
	{
		FORMAT_DATA_PTR new_ifd = NULL;
		FORMAT_DATA_PTR new_ofd = NULL;

		VARIABLE_PTR out_var = NULL;

		if (!IS_ARRAY(in_var) && !IS_RECORD_VAR(in_var))
		{
			error = err_push(ERR_GENERAL, "%s: A non-array variable cannot occur with array variables", in_var->name);
			return(error);
		}

		out_var = ff_find_variable(in_var->name, output->format);
		if (out_var)
		{
			error = spawn_array_format_data(format_data_list, input->format, in_var, &new_ifd);
			if (error)
				break;

			error = spawn_array_format_data(format_data_list, output->format, out_var, &new_ofd);
			if (error)
				break;

			error = make_tabular_array_conduit(new_ifd, new_ofd, std_args, array_conduit_list);
			if (error)
				break;

			if (IS_RECORD_VAR(in_var))
				set_record_array_offsets(in_var, out_var, FF_AC(dll_last(array_conduit_list)));
			else
				set_array_offsets(FF_AC(dll_last(array_conduit_list)));
		}

		variables = FFV_NEXT_VARIABLE(variables);
		in_var    = FF_VARIABLE(variables);
	} /* while var */

	return(error);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "graft_format_data"

/*****************************************************************************
 * NAME:  graft_format_data()
 *
 * PURPOSE:  Create an FORMAT_DATA object
 *
 * USAGE:  format_data = graft_format_data(format, bufsize_ptr);
 *
 * RETURNS:  NULL if memory allocations failed, otherwise a pointer to an
 * FORMAT_DATA structure
 *
 * DESCRIPTION:  Similar to fd_create_format_data, except that bufsize_ptr
 * points to a previously created bufsize.  Two or more format_data's will
 * share a common bufsize.
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

static FORMAT_DATA_PTR graft_format_data
	(
	 FORMAT_PTR format,
	 FF_BUFSIZE_PTR bufsize,
	 char *name
	)
{
	FORMAT_DATA_PTR format_data = NULL;

	FF_VALIDATE(bufsize);
	
	format_data = (FORMAT_DATA_PTR)memMalloc(sizeof(FORMAT_DATA), "format_data");
	if (format_data)
	{
#ifdef FF_CHK_ADDR
		format_data->check_address = (void *)format_data;
#endif
		
		/* default data byte_order=native order = endian()
			1=big endian, 0=little endian */
		format_data->state.byte_order = (unsigned char)endian();
		format_data->state.new_record = 0;
		format_data->state.locked     = 0;
		format_data->state.unused = 0;
		
		assert(bufsize->usage < USHRT_MAX);

		format_data->data = bufsize;
		format_data->data->usage++;

		if (format)
		{
			FF_VALIDATE(format);
			format_data->format = format;
		}
		else
		{
			format_data->format = ff_create_format(name, NULL);
			if (!format_data->format)
				err_push(ERR_MEM_LACK, "new format-data");
		}
	}
	else
		err_push(ERR_MEM_LACK, "new format-data");
	
	return(format_data);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "reproduce_format_data"

static int reproduce_format_data
	(
	 FORMAT_DATA_PTR source,
	 FORMAT_DATA_HANDLE target
	)
{
	FORMAT_PTR format = NULL;

	format = copy_format(source->format);
	if (!format)
		return(ERR_MEM_LACK);

	*target = graft_format_data(format, source->data, NULL);
	if (!*target)
		return(ERR_MEM_LACK);

	return(0);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "dbset_create_conduits_core"

static int dbset_create_conduits_core
	(
	 FORMAT_DATA_LIST format_data_list,
	 FF_STD_ARGS_PTR std_args,
	 FF_ARRAY_CONDUIT_LIST array_conduit_list
	)
{
#ifdef FF_DBG
	FORMAT_DATA_PTR input;
#endif
	FORMAT_DATA_PTR output;
	
	FORMAT_DATA_LIST fdlist; /* walk-through list */
	FORMAT_DATA_PTR fd;     /* walk-through format */
	
	FF_TYPES_t format_type;
	
	int error = 0;
	
	assert(format_data_list);
	
	FF_VALIDATE(std_args);
	
	fdlist = dll_first(format_data_list);
	fd = FD_FORMAT_DATA(fdlist);
	while (fd)
	{
		FF_VALIDATE(fd);
		
		if (IS_OUTPUT(fd->format) || IS_RECORD_FORMAT(fd->format))
		{
			fdlist = dll_next(fdlist);
			fd = FD_FORMAT_DATA(fdlist);
			
			continue;
		}
		
		format_type = (FFF_TYPE(fd->format) & ~FFF_IO) & FFF_DATA_TYPES;

#ifdef FF_DBG
		input = fd_find_format_data(format_data_list, FFF_GROUP, FFF_INPUT | format_type);
		assert(input == fd);
#endif

		output = fd_find_format_data(format_data_list, FFF_GROUP, FFF_OUTPUT | format_type);

		if (output)
			FF_VALIDATE(output);

		if (IS_ARRAY(fd->format))
		{
			if (output && !IS_ARRAY(output->format))
				return(err_push(ERR_GEN_ARRAY, "Array/Tabular Format mismatch"));

			error = spawn_array_conduits(fd, output, std_args, array_conduit_list, format_data_list);
			if (error)
				return(error);
		}
		else if (!IS_ARRAY(fd->format))
		{
			FORMAT_DATA_PTR input_copy = NULL;
			FORMAT_DATA_PTR output_copy = NULL;

			if (output && IS_ARRAY(output->format))
				return(err_push(ERR_GEN_ARRAY, "Array/Tabular Format mismatch"));

			error = reproduce_format_data(fd, &input_copy);
			if (error)
				return(error);

			if (output)
			{
				error = reproduce_format_data(output, &output_copy);
				if (error)
				{
					fd_destroy_format_data(input_copy);
					return(error);
				}
			}

			error = make_tabular_array_conduit(input_copy, output_copy, std_args, array_conduit_list);
			if (error)
				return(error);
		}
		else
		{
		}

		fdlist = dll_next(fdlist);
		fd = FD_FORMAT_DATA(fdlist);
	}
	
	return(error);
}

/*****************************************************************************
 * NAME: dbset_create_conduits
 *
 * PURPOSE: To initialize an array conduit list
 *
 * USAGE:  dbset_create_conduits();
 *
 * RETURNS:  int
 *
 * DESCRIPTION:  Associate output array variables with input array variables.
 * For each pairing, create the two memory formats (discussed in detail later).
 *
 * Allocate memory for input and output formats.  Header formats will each
 * get their own memory, but input data formats will share common memory
 * and output data formats will likewise share common memory.  This assumes
 * that input and output data formats are processed sequentially, but all
 * headers are processed in parallel with data.
 *
 * Allocation sizes must be determined as appropriate.  Headers will be
 * allocated their record size (maybe plus some extra if GeoVu compatibility
 * is needed -- GeoVu can enlarge the header, see Edit Data Header) but
 * input and output data formats will be allocated a common multiple of the
 * largest record size.  The input and output allocation sizes must allow for
 * the same number of records for the benefit of ff_process_format_data_mapping.
 *
 * AUTHOR:  Mark Ohrenschall, NGDC, (303) 497-6124, mao@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:
 *
 * Header formats may legitimately have no output format, and while less
 * likely, data formats may also have no output format.
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

#undef ROUTINE_NAME
#define ROUTINE_NAME "dbset_create_conduits"

static int dbset_create_conduits
	(
	 FORMAT_DATA_LIST format_data_list,
	 FF_STD_ARGS_PTR std_args,
	 FF_ARRAY_CONDUIT_LIST_HANDLE array_conduit_list_handle
	)
{
	int error;
	
	assert(array_conduit_list_handle);

	if (!array_conduit_list_handle)
		return(err_push(ERR_API, "NULL FF_ARRAY_CONDUIT_LIST_HANDLE in %s", ROUTINE_NAME));
	
	if (!*array_conduit_list_handle)
	{
		*array_conduit_list_handle = dll_init();
		if (!*array_conduit_list_handle)
			return(ERR_MEM_LACK);
	}

	error = dbset_create_conduits_core(format_data_list, std_args, *array_conduit_list_handle);
	if (error)
	{
		ff_destroy_array_conduit_list(*array_conduit_list_handle);
		*array_conduit_list_handle = NULL;
	}
	
	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "ask_EOL_string"

/*****************************************************************************
 * NAME:  ask_EOL_string
 *
 * PURPOSE:  Determine EOL length from the input or output name table.
 *
 * USAGE:  error = ask_EOL_string(dbin, io_type, EOL_length);
 *
 * RETURNS:  Zero or an error code.  If length and string are undetermined,
 * zero is still returned.
 *
 * DESCRIPTION:  Asks the input or output name table, according to io_type,
 * for the value of "EOL_type".  If defined, EOL_string is set accordingly.
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

static int ask_EOL_string
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t io_type,
	 char *EOL_string
	)
{
	char EOL_type[MAX_PV_LENGTH];
	
	int error = 0;
	
	FF_VALIDATE(dbin);

	assert(EOL_string);
	
	*EOL_string = STR_END;

	error = nt_ask(dbin, io_type | NT_ANYWHERE, "EOL_type", FFV_CHAR, EOL_type);
	if (!error)
	{
		if (os_strcmpi(EOL_type, "DOS") == 0)
			strcpy(EOL_string, DOS_EOL_STRING);
		else if (os_strcmpi(EOL_type, "UNIX") == 0)
			strcpy(EOL_string, UNIX_EOL_STRING);
		else if (os_strcmpi(EOL_type, "MAC") == 0 || os_strcmpi(EOL_type, "MACOS") == 0)
			strcpy(EOL_string, MAC_EOL_STRING);
		else if (os_strcmpi(EOL_type, "LINUX") == 0)
			strcpy(EOL_string, UNIX_EOL_STRING);
		else
			error = err_push(ERR_PARAM_VALUE, "Invalid operating system given for EOL_type");
	}
	
	if (error == ERR_NT_KEYNOTDEF)
		return(0);
	else
		return(error);
}

/*
 * NAME:        get_buffer_eol_str
 *              
 * PURPOSE:     to determine the EOL sequence for a given buffer
 *
 * USAGE:       int get_buffer_eol_str(char *buffer, char *buffer_eol_str)
 *
 * RETURNS:     0 if all is OK, >< 0 on error
 *
 * DESCRIPTION: Determines the EOL sequence for a given file.  If no EOL
 *				sequence is observed, buffer_eol_str is simply set to '\0';
 *				otherwise the first characters of buffer_eol_str are set to 
 *				the EOL sequence (buffer_eol_str must be at least 3 bytes)
 *
 * SYSTEM DEPENDENT FUNCTIONS:  none- completely portable
 *
 * AUTHOR:      Kevin Frender kbf@ngdc.noaa.gov
 *
 * COMMENTS:
 *          
 * KEYWORDS: EOL
 *
 */
#undef ROUTINE_NAME
#define ROUTINE_NAME "get_buffer_eol_str"

static int get_buffer_eol_str(char *buffer, char *buffer_eol_str)
{
	enum {GBEOLS_EOL_LF = 10, GBEOLS_EOL_CR = 13};
	char *c;
	
	assert(buffer && buffer_eol_str);
	
	c = buffer;
	
	while(c[0]){
		if(c[0] == GBEOLS_EOL_LF){ /* LF */
			/* Must be a unix file */
			buffer_eol_str[0] = (char)GBEOLS_EOL_LF;
			buffer_eol_str[1] = '\0';
			return(0);
		}
		if(c[0] == GBEOLS_EOL_CR){ /* CR */
			c++;
			if(c[0] == GBEOLS_EOL_LF){ /* CR-LF */
				/* Must be a DOS file */
				buffer_eol_str[0] = (char)GBEOLS_EOL_CR;
				buffer_eol_str[1] = (char)GBEOLS_EOL_LF;
				buffer_eol_str[2] = '\0';
				return(0);
			}
			/* Must be a MAC file */
			buffer_eol_str[0] = (char)GBEOLS_EOL_CR;
			buffer_eol_str[1] = '\0';
			return(0);
		}
		c++;
	}
	
	/* Couldn't find any EOL chars */
	buffer_eol_str[0] = '\0';
	return(0);
}

#define MAX_EOL_LENGTH DOS_EOL_LENGTH

static int search_for_EOL
	(
	 FILE *file,
	 char *fname,
	 char *EOL_string
	)
{
	unsigned long file_offset = 0;
	unsigned long file_length = os_filelength(fname);

	while (file_offset < file_length)
	{
		char buffer[MAX_EOL_LENGTH];
		unsigned int num_to_read = 0;
		int num_read = 0;

		num_to_read = min(MAX_EOL_LENGTH, (int)(file_length - file_offset));

		num_read = fread(buffer, sizeof(char), num_to_read, file);
		if (num_read != (int)num_to_read)
			return(err_push(ERR_READ_FILE, fname));

		get_buffer_eol_str(buffer, EOL_string);
		if (strlen(EOL_string))
			break;

		file_offset = ftell(file);
		if (file_offset == -1)
			return(err_push(ERR_READ_FILE, fname));
	}

	return(0);
}

static VARIABLE_PTR find_EOL_var
	(
	 FORMAT_PTR format
	)
{
	VARIABLE_LIST vlist = NULL;
	VARIABLE_PTR var = NULL;

	FF_VALIDATE(format);

	vlist = FFV_FIRST_VARIABLE(format);
	var   = FF_VARIABLE(vlist);
	while (var)
	{
		FF_VALIDATE(var);

		if (IS_EOL_VAR(var))
			break;

		vlist = FFV_NEXT_VARIABLE(vlist);
		var   = FF_VARIABLE(vlist);
	}

	return(var);
}

static void get_relative_file_offset
	(
	 FORMAT_PTR format,
	 unsigned long *file_offset
	)
{
	VARIABLE_PTR variable = NULL;
	
	FF_VALIDATE(format);
	
	variable = find_EOL_var(format);
	if (variable)
		*file_offset = variable->start_pos - 1;
	else
		*file_offset = format->length;
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "read_EOL_from_file"

static int read_EOL_from_file
	(
	 PROCESS_INFO_PTR pinfo,
	 char *EOL_string
	)
{
	FILE *file = NULL;
	unsigned long file_offset = 0;
	char buffer[MAX_EOL_LENGTH];

	int error = 0;

	FF_VALIDATE(pinfo);

	file = fopen(PINFO_FNAME(pinfo), "rb");
	if (file == NULL)
		return(err_push(ERR_OPEN_FILE, PINFO_FNAME(pinfo)));
	
	if (IS_VARIED(PINFO_FORMAT(pinfo)))
		file_offset = 0;
	else
		get_relative_file_offset(PINFO_FORMAT(pinfo), &file_offset);

	if (PINFO_IS_FILE(pinfo))
		file_offset += PINFO_ARRAY_OFFSET(pinfo);
	else
		assert(PINFO_IS_FILE(pinfo));
	
	if (fseek(file, file_offset, SEEK_SET))
		error = err_push(ERR_READ_FILE, PINFO_FNAME(pinfo));

	if (!error && !IS_VARIED(PINFO_FORMAT(pinfo)))
	{
		unsigned int num_to_read = 0;
		int num_read = 0;

		num_to_read = min(MAX_EOL_LENGTH, (int)(os_filelength(PINFO_FNAME(pinfo)) - file_offset));

		num_read = fread(buffer, sizeof(char), num_to_read, file);
		if (num_read != (int)num_to_read)
			error = err_push(ERR_READ_FILE, PINFO_FNAME(pinfo));
	}
		
	if (!error)
	{
		if (IS_VARIED(PINFO_FORMAT(pinfo)))
		{
			search_for_EOL(file, PINFO_FNAME(pinfo), EOL_string);
			assert(strlen(EOL_string));
		}
		else
		{
			error = get_buffer_eol_str(buffer, EOL_string);
			if (!error)
			{
				if (!strlen(EOL_string))
					error = err_push(ERR_NO_EOL, "At position %lu in %s (\"%s\")", (unsigned long)file_offset + 1, os_path_return_name(PINFO_FNAME(pinfo)), PINFO_NAME(pinfo));
			}
		}
	}
	
	fclose(file);
	
	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "read_EOL_string"

/* non-file data assumes single-object data stream */

static int read_EOL_string
	(
	 PROCESS_INFO_PTR pinfo,
	 char *EOL_string
	)
{
	int error = 0;

	FF_VALIDATE(pinfo);
	FF_VALIDATE(pinfo->pole);
	FF_VALIDATE(pinfo->pole->fd);

	if (PINFO_IS_FILE(pinfo))
	{
		error = read_EOL_from_file(pinfo, EOL_string);
	}
	else if (PINFO_LOCUS_BUFSIZE(pinfo))
	{
		error = get_buffer_eol_str(PINFO_LOCUS_BUFFER(pinfo), EOL_string);
		if (!strlen(EOL_string))
			error = err_push(ERR_MAKE_FORM, "Cannot determine type of newline for \"%s\"", PINFO_NAME(pinfo));
	}
	else
		strcpy(EOL_string, NATIVE_EOL_STRING);

	return(error);
}

static int resize_for_EOL
	(
	 FORMAT_PTR format,
	 char *EOL_string
	)
{
	int error = 0;

	VARIABLE_LIST v_list = FFV_FIRST_VARIABLE(format);
	VARIABLE_PTR     var = FF_VARIABLE(v_list);
	
	int adjust = 0;
	size_t EOL_length = strlen(EOL_string);
	
	if (IS_BINARY(format) || IS_VARIED(format))
		return(0);
	
	format->length = 0;

	while (var)
	{
		int EOL_delta;
		
		EOL_delta = 0;

		if (IS_EOL_VAR(var))
		{
			error = new_name_string__(EOL_string, &var->name);
			if (error)
				break;

			EOL_delta = EOL_length - FF_VAR_LENGTH(var);
		}
			
		var->start_pos += adjust;
		adjust += EOL_delta;
		var->end_pos += adjust;
				
		format->length = max(format->length, var->end_pos);

		v_list = FFV_NEXT_VARIABLE(v_list);
		var    = FF_VARIABLE(v_list);
	}

	return(error);
}

static BOOLEAN format_has_newlines
	(
	 PROCESS_INFO_PTR pinfo
	)
{
	BOOLEAN format_has_newlines = FALSE;

	FF_VALIDATE(pinfo);

	if (IS_ASCII(PINFO_FORMAT(pinfo)) || IS_DBASE(PINFO_FORMAT(pinfo)))
	{
		if (find_EOL_var(PINFO_FORMAT(pinfo)))
			format_has_newlines = TRUE;
	}

	return(format_has_newlines);
}

/*****************************************************************************
 * NAME:  set_formats_EOL_length
 *
 * PURPOSE:  Sets a dbin's formats' EOL_length
 *
 * USAGE:  set_formats_EOL_length(dbin);
 *
 * RETURNS:  Zero on success, error if an invalid EOL type is detected
 *
 * DESCRIPTION:  Input formats are set according to one of two factors:  1) the
 * values of the input equivalence section keywords "EOL_type" or "EOL_string",
 * or 2) the first EOL string found in the file.  Output formats are also
 * set according to one of two factors:  1) the values of the output equivalence
 * section keywords "EOL_type" or "EOL_string", or the native EOL length.
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

static int determine_EOL_by_type
	(
	 PROCESS_INFO_PTR pinfo,
	 char *EOL_string
	)
{
	int error = 0;

	FF_VALIDATE(pinfo);
	FF_VALIDATE(pinfo->pole);
	FF_VALIDATE(pinfo->pole->fd);

	if (format_has_newlines(pinfo))
	{
		if (!strlen(EOL_string))
		{
			if (IS_INPUT(PINFO_FORMAT(pinfo)))
			{
				error = read_EOL_string(pinfo, EOL_string);
				if (error)
					return(error);
			}
			else if (IS_OUTPUT(PINFO_FORMAT(pinfo)))
			{
				strcpy(EOL_string, NATIVE_EOL_STRING);
			}
			else
				assert(IS_INPUT(PINFO_FORMAT(pinfo)) || IS_OUTPUT(PINFO_FORMAT(pinfo)));
		}

		if (!IS_VARIED(PINFO_FORMAT(pinfo)))
			error = resize_for_EOL(PINFO_FORMAT(pinfo), EOL_string);
	} /* if format_has_newlines(pinfo) */

	*EOL_string = STR_END;
	return(error);
}

static int determine_EOLs
	(
	 DATA_BIN_PTR dbin,
	 PROCESS_INFO_PTR pinfo
	)
{
	int error = 0;

	char EOL_string[MAX_PV_LENGTH];

	FF_VALIDATE(dbin);
	FF_VALIDATE(pinfo);

	error = ask_EOL_string(dbin, FFF_INPUT, EOL_string);
	if (!error)
		error = determine_EOL_by_type(pinfo, EOL_string);

	if (error)
		return(error);

	error = ask_EOL_string(dbin, FFF_OUTPUT, EOL_string);
	if (!error && PINFO_MATE(pinfo))
		error = determine_EOL_by_type(PINFO_MATE(pinfo), EOL_string);

	if (error)
		return(error);

	return(error);
}

static BOOLEAN check_index_for_token(char *s)
{
	char *endptr = NULL;

	(void)strtol(s, &endptr, 10);
	if (endptr && strlen(endptr) && !isspace(*endptr) && *endptr != ']')
		return(TRUE);
	else
		return(FALSE);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "literal_arr_str_copy"

/*****************************************************************************
 * NAME: literal_arr_str_copy
 *
 * PURPOSE: Copy array descriptor string and replace dummy indices with literals
 *
 * USAGE:  array_desc_str_copy = literal_arr_str_copy(dbin, array_desc_str, format);
 *
 * RETURNS:  A copy of the array descriptor string, or NULL on failure
 *
 * DESCRIPTION:  Parse array descriptor string for non-numeric indices.  For all
 * so found, call nt_ask on the token, and if successful, write the token's value
 * in its place in the string copy.
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

static int literal_arr_str_copy
	(
	 DATA_BIN_PTR dbin,
	 char *array_desc_str,
	 FORMAT_PTR format,
	 char **desc_str_copy_h
	)
{
	size_t fudge_room = strlen(array_desc_str);
	size_t desc_str_size = (2 * fudge_room) + FF_ARRAY_ELEMENT_SIZE_WIDTH + 2;

	FF_VALIDATE(dbin);
	FF_VALIDATE(format);

	*desc_str_copy_h = (char *)memMalloc(desc_str_size, "desc_str_copy");
	if (*desc_str_copy_h)
	{
		enum
		{
			NO_MODE = 1,
			BEGIN_SUBSCRIPT = 2,
			END_SUBSCRIPT = 3,
			BEGIN_ARRAY_NAME = 4,
			END_ARRAY_NAME = 5,
			BEGIN_START_INDEX = 6,
			END_START_INDEX = 7,
			BEGIN__TO_ = 8,
			END__TO_ = 9,
			BEGIN_FINISH_INDEX = 10,
			END_FINISH_INDEX = 11
		} mode = NO_MODE;

		size_t desc_index = 0;
		size_t copy_index = 0;

		while (array_desc_str[desc_index])
		{
			BOOLEAN sub_token = FALSE;
			if (isspace(array_desc_str[desc_index]))
			{
				if (mode == BEGIN_START_INDEX)
					mode = END_START_INDEX;
				else if (mode == BEGIN__TO_)
					mode = END__TO_;
				else if (mode == BEGIN_FINISH_INDEX)
					mode = END_FINISH_INDEX;
			}
			else if (array_desc_str[desc_index] == '[' && (mode == END_SUBSCRIPT || mode == NO_MODE))
				mode = BEGIN_SUBSCRIPT;
			else if (array_desc_str[desc_index] == '"' && mode == BEGIN_SUBSCRIPT)
				mode = BEGIN_ARRAY_NAME;
			else if (array_desc_str[desc_index] == '"' && mode == BEGIN_ARRAY_NAME)
				mode = END_ARRAY_NAME;
			else if (os_strncmpi(array_desc_str + desc_index, "TO", 2) == 0 && mode == END_START_INDEX)
				mode = BEGIN__TO_;
			else if (array_desc_str[desc_index] == ']' && mode == BEGIN_FINISH_INDEX)
				mode = END_SUBSCRIPT;
			else 
			{
				if (mode == END_ARRAY_NAME)
					mode = BEGIN_START_INDEX;
				else if (mode == END__TO_)
					mode = BEGIN_FINISH_INDEX;
			}

			if (mode == BEGIN_START_INDEX || mode == BEGIN_FINISH_INDEX)
			{
				if (check_index_for_token(array_desc_str + desc_index))
					sub_token = TRUE;
			}

			if (sub_token)
			{
				int error = 0;

				unsigned long index = 0;
				char save_char = STR_END;
				size_t save_char_pos = 0;

				save_char_pos = strcspn(array_desc_str + desc_index, " \t]");
				save_char_pos += desc_index;
				save_char = *(array_desc_str + save_char_pos);
				*(array_desc_str + save_char_pos) = STR_END;

				error = nt_ask(dbin, NT_ANYWHERE, array_desc_str + desc_index, FFV_ULONG, &index);
				if (error)
				{
					return(err_push(error, "Unrecognized keyword \"%s\"", array_desc_str + desc_index));
				} /* error: nt_ask */
				else
				{
					signed short fudge = 0;
					char index_str[11]; /* 10 digits for ULONG_MAX + 1 for STR_END */

					sprintf(index_str, "%lu", index);
					fudge = (signed short)(strlen(index_str) - strlen(array_desc_str + desc_index));
					if (fudge > 0)
					{
						if ((size_t)fudge < fudge_room)
							fudge_room -= fudge;
						else
						{
							char *temp = NULL;

							fudge_room = max(strlen(array_desc_str), (size_t)fudge);
							desc_str_size += fudge_room;

							temp = memRealloc(*desc_str_copy_h, desc_str_size, "*desc_str_copy_h");
							if (temp)
								*desc_str_copy_h = temp;
							else
							{
								memFree(*desc_str_copy_h, "*desc_str_copy_h");
								return(err_push(ERR_MEM_LACK, ""));
							}
						}
					} /* fudge > 0 */

					strcpy(*desc_str_copy_h + copy_index, index_str);
					copy_index += strlen(index_str);

					if (fudge > 0)
					{
						desc_index += strlen(array_desc_str + desc_index);
					}
					else
					{
						desc_index += strlen(index_str);

						fudge = (short)-fudge;
						for (; fudge; fudge--, copy_index++, desc_index++)
							(*desc_str_copy_h)[copy_index] = ' ';
					} /* else fudge > 0 */

					(*desc_str_copy_h)[copy_index] = save_char;
				}  /* else error: nt_ask */

				*(array_desc_str + save_char_pos) = save_char;

				sub_token = FALSE;
			}
			else
			{
				(*desc_str_copy_h)[copy_index] = array_desc_str[desc_index];
				copy_index++;
				desc_index++;
			}
		} /* while array_desc_str[desc_index] */

		(*desc_str_copy_h)[copy_index] = STR_END;

		if (IS_RECORD_FORMAT(format))
			sprintf(*desc_str_copy_h + strlen(*desc_str_copy_h) - FF_ARRAY_ELEMENT_SIZE_WIDTH, "%*d", (int)FF_ARRAY_ELEMENT_SIZE_WIDTH, (int)FORMAT_LENGTH(format));
	}
	else
		return(err_push(ERR_MEM_LACK, ""));

	return(0);
}

/* Remove separation and grouping from all dimensions, change output element size to input's */
static void make_inputs_output_desc_str
	(
	 ARRAY_DESCRIPTOR_PTR input_desc,
	 ARRAY_DESCRIPTOR_PTR temp_desc,
	 char *output_desc_str
	)
{
	int i;

	output_desc_str[0] = STR_END;
	for (i = 0; i < temp_desc->num_dim; i++)
	{
		sprintf(output_desc_str + strlen(output_desc_str), "[\"%s\" %ld to %ld by %ld] ", temp_desc->dim_name[i], (long)temp_desc->start_index[i], (long)temp_desc->end_index[i], (long)temp_desc->granularity[i]);
	}

	sprintf(output_desc_str + strlen(output_desc_str), "%*d", FF_ARRAY_ELEMENT_SIZE_WIDTH, (int)input_desc->element_size);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "make_input_array_mapping"

static int make_input_array_mapping
	(
	 DATA_BIN_PTR dbin,
	 FF_ARRAY_DIPOLE_PTR pole
	)
{
	int error = 0;

	ARRAY_DESCRIPTOR_PTR input_desc = NULL;
	ARRAY_DESCRIPTOR_PTR output_desc = NULL;

	ARRAY_DESCRIPTOR_PTR temp_desc = NULL;
	
	char *input_desc_str = NULL;
	char *output_desc_str = NULL;

	VARIABLE_PTR invar  = FF_VARIABLE(FFV_FIRST_VARIABLE(pole->fd->format));
	VARIABLE_PTR outvar = FF_VARIABLE(FFV_FIRST_VARIABLE(pole->mate->fd->format));

	FF_VALIDATE(dbin);
	FF_VALIDATE(pole);

	error = literal_arr_str_copy(dbin, invar->array_desc_str, pole->fd->format, &input_desc_str);
	if (error)
		return(err_push(ERR_NDARRAY, "Problem with variable \"%s\"", invar->name));

	input_desc = ndarr_create_from_str(input_desc_str);
	memFree(input_desc_str, "input_desc_str");
	if (!input_desc)
		return(ERR_GEN_ARRAY);

	error = literal_arr_str_copy(dbin, outvar->array_desc_str, pole->fd->format, &output_desc_str);
	if (error)
	{
		ndarr_free_descriptor(input_desc);
		return(err_push(ERR_NDARRAY, "Problem with variable \"%s\"", outvar->name));
	}

	temp_desc = ndarr_create_from_str(output_desc_str);
	if (!temp_desc)
	{
		memFree(input_desc_str, "input_desc_str");
		memFree(output_desc_str, "output_desc_str");
		ndarr_free_descriptor(input_desc);
		return(ERR_GEN_ARRAY);
	}
			
	make_inputs_output_desc_str(input_desc, temp_desc, output_desc_str); /* versus make_outputs_input_desc_str */
	ndarr_free_descriptor(temp_desc);

	output_desc = ndarr_create_from_str(output_desc_str);
	if (!output_desc)
	{
		memFree(input_desc_str, "input_desc_str");
		memFree(output_desc_str, "output_desc_str");
		ndarr_free_descriptor(input_desc);
		ndarr_free_descriptor(temp_desc);
		return(ERR_GEN_ARRAY);
	}
			
	memFree(output_desc_str, "output_desc_str");
	pole->array_mapping = ndarr_create_mapping(output_desc, input_desc);
	if (!pole->array_mapping)
		return(ERR_GEN_ARRAY);
	
	return(0);
}

/* Remove separation and grouping from all dimensions */
static void make_outputs_input_desc_str
	(
	 ARRAY_DESCRIPTOR_PTR temp_desc,
	 char *input_desc_str
	)
{
	int i;

	input_desc_str[0] = STR_END;
	for (i = 0; i < temp_desc->num_dim; i++)
	{
		sprintf(input_desc_str + strlen(input_desc_str), "[\"%s\" %ld to %ld by %ld] ", temp_desc->dim_name[i], (long)temp_desc->start_index[i], (long)temp_desc->end_index[i], (long)temp_desc->granularity[i]);
	}

	sprintf(input_desc_str + strlen(input_desc_str), "%*d", FF_ARRAY_ELEMENT_SIZE_WIDTH, (int)temp_desc->element_size);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "make_output_array_mapping"

static int make_output_array_mapping
	(
	 DATA_BIN_PTR dbin,
	 FF_ARRAY_DIPOLE_PTR pole
	)
{
	int error = 0;

	ARRAY_DESCRIPTOR_PTR input_desc = NULL;
	ARRAY_DESCRIPTOR_PTR output_desc = NULL;

	ARRAY_DESCRIPTOR_PTR temp_desc = NULL;
	
	char *input_desc_str = NULL;
	char *output_desc_str = NULL;

	VARIABLE_PTR var = NULL;

	if (!pole)
		return(0);

	FF_VALIDATE(dbin);
	FF_VALIDATE(pole);

	var = FF_VARIABLE(FFV_FIRST_VARIABLE(pole->fd->format));

	FF_VALIDATE(pole);

	error = literal_arr_str_copy(dbin, var->array_desc_str, pole->fd->format, &output_desc_str);
	if (error)
		return(err_push(ERR_NDARRAY, "Problem with variable \"%s\"", var->name));

	output_desc = ndarr_create_from_str(output_desc_str);
	memFree(output_desc_str, "input_desc_str");
	if (!output_desc)
		return(ERR_GEN_ARRAY);

	error = literal_arr_str_copy(dbin, var->array_desc_str, pole->fd->format, &input_desc_str);
	if (error)
	{
		ndarr_free_descriptor(output_desc);
		return(err_push(ERR_NDARRAY, "Problem with variable \"%s\"", var->name));
	}

	temp_desc = ndarr_create_from_str(input_desc_str);
	if (!temp_desc)
	{
		memFree(input_desc_str, "input_desc_str");
		memFree(output_desc_str, "output_desc_str");
		ndarr_free_descriptor(output_desc);
		return(ERR_GEN_ARRAY);
	}
			
	make_outputs_input_desc_str(temp_desc, input_desc_str); /* versus make_inputs_output_desc_str */
	ndarr_free_descriptor(temp_desc);

	input_desc = ndarr_create_from_str(input_desc_str);
	if (!input_desc)
	{
		memFree(input_desc_str, "input_desc_str");
		memFree(output_desc_str, "output_desc_str");
		ndarr_free_descriptor(output_desc);
		ndarr_free_descriptor(temp_desc);
		return(ERR_GEN_ARRAY);
	}
			
	memFree(input_desc_str, "output_desc_str");
	pole->array_mapping = ndarr_create_mapping(output_desc, input_desc);
	if (!pole->array_mapping)
		return(ERR_GEN_ARRAY);
	
	return(0);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "make_array_mappings"

static int make_array_mappings
	(
	 DATA_BIN_PTR dbin,
	 PROCESS_INFO_PTR pinfo
	)
{
	int error = 0;

	FF_VALIDATE(dbin);
	FF_VALIDATE(pinfo);

	error = make_input_array_mapping(dbin, PINFO_POLE(pinfo));
	if (!error)
		error = make_output_array_mapping(dbin, PINFO_MATE_POLE(pinfo));

	return(error);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "set_array_mappings"

/*****************************************************************************
 * NAME:  set_array_mappings
 *
 * PURPOSE: Set the ARRAY_MAPPING_PTR's of the input and output ARRAY_DIPOLE's
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

static int set_array_mappings
	(
	 DATA_BIN_PTR dbin,
	 PROCESS_INFO_PTR pinfo,
	 long records_to_do
	)
{
	int error = 0;

	FF_VALIDATE(dbin);
	FF_VALIDATE(pinfo);
	
	if (IS_ARRAY(PINFO_FORMAT(pinfo)))
	{
		error = make_array_mappings(dbin, pinfo);
		if (error)
			return(error);

		PINFO_BYTES_LEFT(pinfo) = PINFO_SUB_ARRAY_BYTES(pinfo);

		if (PINFO_MATE(pinfo))
			PINFO_MATE_BYTES_LEFT(pinfo) = PINFO_MATE_SUB_ARRAY_BYTES(pinfo);
	}
	else
	{
		if (IS_HEADER(PINFO_FORMAT(pinfo)))
		{
			error = make_tabular_format_array_mappings(pinfo, 1, 1);
			if (error)
				return(error);

			if (PINFO_BYTES_LEFT(pinfo) != PINFO_RECL(pinfo))
				error = err_push(ERR_FILE_LENGTH, "Using \"%s\"", PINFO_NAME(pinfo));

			if (PINFO_MATE(pinfo) && PINFO_MATE_BYTES_LEFT(pinfo) != PINFO_MATE_RECL(pinfo))
				error = err_push(ERR_FILE_LENGTH, "Using \"%s\"", PINFO_MATE_NAME(pinfo));
		}
		else if (IS_DATA(PINFO_FORMAT(pinfo)))
		{
			if (!fd_get_header(dbin, FFF_REC))
			{
				unsigned long num_records = 0;

				if (PINFO_IS_FILE(pinfo))
				{
					unsigned long bytes_left = 0;

					bytes_left = (os_filelength(PINFO_FNAME(pinfo)) - PINFO_ARRAY_OFFSET(pinfo));
					num_records = bytes_left / PINFO_RECL(pinfo);

					if (bytes_left % PINFO_RECL(pinfo))
						error = err_push(ERR_FILE_LENGTH, "Using \"%s\"", PINFO_NAME(pinfo));
					else
						error = make_tabular_format_array_mappings(pinfo, num_records, records_to_do ? records_to_do : num_records);

#if 0	/* I don't remember what this is supposed to do and it doesn't seem to work right! */
					if (!records_to_do)
					{
						if (PINFO_BYTES_LEFT(pinfo) != bytes_left - PINFO_ARRAY_OFFSET(pinfo))
							error = err_push(ERR_FILE_LENGTH, "Using \"%s\"", PINFO_NAME(pinfo));

						if (PINFO_MATE(pinfo) && PINFO_MATE_BYTES_LEFT(pinfo) != num_records * PINFO_MATE_RECL(pinfo))
							error = err_push(ERR_FILE_LENGTH, "Using \"%s\"", PINFO_MATE_NAME(pinfo));
					}
#endif
				}
				else
				{
					num_records = 1;
					error = make_tabular_format_array_mappings(pinfo, num_records, records_to_do ? records_to_do : num_records);
				}
			} /* if !fd_get_header(dbin, FFF_REC) */
		} /* else if IS_DATA(PINFO_FORMAT(pinfo)) */
		else
			assert(0);
	} /* (else) if IS_ARRAY(PINFO_FORMAT(pinfo)) */

	return(error);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "update_sizes"

/*****************************************************************************
 * NAME: update_sizes
 *
 * PURPOSE:  Update the size field of ARRAY_DIPOLE's
 *
 * USAGE:
 *
 * RETURNS:
 *
 * DESCRIPTION:  Increments the size_offset field of each file array dipole
 * to the cumulative sum of preceding file array dipoles that have the same
 * file name.
 *
 * This is done with doubly-nested while loops, in which the output while
 * loop steps through the list of array conduits once, and the inner while
 * loop steps through the following array conduits once.
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

static long length_in_file(PROCESS_INFO_PTR pinfo)
{
	if (pinfo && PINFO_IS_FILE(pinfo))
	{
		if (IS_HEADER(PINFO_FORMAT(pinfo)))
			return(PINFO_RECL(pinfo));
		else if (IS_DATA(PINFO_FORMAT(pinfo)))
			return(PINFO_BYTES_LEFT(pinfo));
		else
			assert(0);
	}
	else
		return(0);
}

static int update_following_offsets_or_size
	(
	 PROCESS_INFO_PTR updater,
	 PROCESS_INFO_LIST updater_list
	)
{
	int error = 0;

	if (PINFO_IS_FILE(updater))
	{
		char *input_file_name = NULL;
		long input_file_length = 0; 
			
		char *output_file_name = NULL;
		long output_file_length = 0; 
			
		PROCESS_INFO_PTR pinfo = NULL;

		input_file_name = PINFO_FNAME(updater);
		input_file_length = length_in_file(updater);

		if (PINFO_MATE(updater))
		{
			output_file_name = PINFO_MATE_FNAME(updater);
			output_file_length = length_in_file(PINFO_MATE(updater));
		}

		updater_list = dll_next(updater_list); /* set sub_list to next node in super_list */
		pinfo        = FF_PI(updater_list);
		while (pinfo)
		{
			if (PINFO_IS_FILE(pinfo) && input_file_name)
			{
				if (!strcmp(input_file_name, PINFO_FNAME(pinfo)))
					PINFO_ARRAY_OFFSET(pinfo) += input_file_length;
			}
			else
			{
				/* else what? */
			}

			if (PINFO_MATE(pinfo))
			{
				if (PINFO_MATE_IS_FILE(pinfo) && output_file_name)
				{
					if (!strcmp(output_file_name, PINFO_MATE_FNAME(pinfo)))
						PINFO_MATE_ARRAY_OFFSET(pinfo) += output_file_length;
				}
				else
				{
					/* else what? */
				}
			}

			updater_list = dll_next(updater_list);
			pinfo        = FF_PI(updater_list);
		} /* while array_conduit -- in updater_list list */
	} /* if PINFO_IS_FILE(pinfo) */
	else
	{
		/* else what? */
	}
	
	return(0);
}

#undef ROUTINE_NAME
#define ROUTINE_NAME "dbset_init_conduits"

static int dbset_init_conduits
	(
	 DATA_BIN_PTR dbin,
	 FF_TYPES_t ftype,
	 long records_to_do
	)
{
	int error = 0;

	PROCESS_INFO_LIST pinfo_list = NULL;
	PROCESS_INFO_PTR  pinfo = NULL;

	FF_VALIDATE(dbin);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &pinfo_list);
	if (error)
		return(err_push(ERR_GENERAL, "Nothing to process"));

	pinfo_list = dll_first(pinfo_list);
	pinfo = FF_PI(pinfo_list);
	while (pinfo)
	{
		if ((PINFO_TYPE(pinfo) & ftype) == ftype)
		{
			error = determine_EOLs(dbin, pinfo);
			if (error)
				break;

			error = set_array_mappings(dbin, pinfo, records_to_do);
			if (error)
				break;

			if (!IS_ARRAY(PINFO_FORMAT(pinfo)))
			{
				error = update_following_offsets_or_size(pinfo, pinfo_list);
				if (error)
					break;
			}
		}

		pinfo_list = dll_next(pinfo_list);
		pinfo = FF_PI(pinfo_list);
	}

	ff_destroy_process_info_list(pinfo_list);

	return(error);
}

static int dbset_format_mappings
	(
	 DATA_BIN_PTR dbin
	)
{
	int error = 0;
	
	PROCESS_INFO_LIST pinfo_list  = NULL;
	PROCESS_INFO_PTR  input_finfo = NULL;

	FF_VALIDATE(dbin);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &pinfo_list);
	if (error)
		return(error);

	error = db_do(dbin, DBDO_READ_FORMATS, pinfo_list);
	if (!error || error == EOF)
	{
		pinfo_list  = dll_first(pinfo_list);
		input_finfo = FF_PI(pinfo_list);
		while (input_finfo)
		{
			if (PINFO_IS_FILE(input_finfo) || PINFO_LOCUS_BUFSIZE(input_finfo))
			{
				PROCESS_INFO_PTR finfo = NULL;

				FF_VALIDATE(input_finfo);

				finfo = PINFO_MATE(input_finfo);
				if (finfo)
				{
					if (PINFO_FORMAT_MAP(finfo))
						ff_destroy_format_data_mapping(PINFO_FORMAT_MAP(finfo));

					error = ff_create_format_data_mapping(PINFO_FD(input_finfo), PINFO_FD(finfo), &PINFO_FORMAT_MAP(finfo));
					if (error && error < ERR_WARNING_ONLY)
						break;
					else if (error)
						error = 0;
				}
			}

			pinfo_list  = dll_next(pinfo_list);
			input_finfo = FF_PI(pinfo_list);
		}
	}

	ff_destroy_process_info_list(pinfo_list);
	return(error);
}

static int dbset_equation_variables(DATA_BIN_PTR dbin)
{
	PROCESS_INFO_LIST process_info_list = NULL;
	PROCESS_INFO_PTR output_pinfo = NULL;

	int error = 0;

	FF_VALIDATE(dbin);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_OUTPUT, &process_info_list);
	if (!error)
	{
		process_info_list = dll_first(process_info_list);
		output_pinfo = FF_PI(process_info_list);
		while (output_pinfo)
		{
			VARIABLE_LIST vlist = NULL;
			VARIABLE_PTR var = NULL;

			vlist = FFV_FIRST_VARIABLE(PINFO_FORMAT(output_pinfo));
			var = FF_VARIABLE(vlist);
			while (var)
			{
				size_t nlen = strlen(var->name);

				if (nlen > 4)
					nlen -= 4;
				else
					nlen = 0;

				if (nlen && !os_strncmpi(var->name + nlen, "_eqn", 4))
					var->type |= FFV_EQN;

				if (IS_EQN(var))
				{
					char *ch = NULL;
					char eqn_string[MAX_PV_LENGTH];

					error = nt_ask(dbin, NT_ANYWHERE, var->name, FFV_TEXT, eqn_string);
					if (error)
					{
						err_push(ERR_EQN_SET, var->name);
						ff_destroy_process_info_list(process_info_list);
						return(error);
					}

					ch = eqn_string - 1;
					while (++ch)
					{
						if (ch[0] < ' ')
						{
							if (ch[0] == '\0')
								break;
					
							/* End-of-line character */
							ch[0] = '\0';
							break;
						}
					}

					var->eqn_info = ee_make_std_equation(eqn_string, PINFO_MATE_FORMAT(output_pinfo));
					if (!var->eqn_info)
					{
						ff_destroy_process_info_list(process_info_list);
						return(err_push(ERR_EQN_SET, "Setting up equation variable %s", var->name));
					}
				} /* if IS_EQN(var) */

				vlist = dll_next(vlist);
				var = FF_VARIABLE(vlist);
			} /* while var */

			process_info_list = dll_next(process_info_list);
			output_pinfo = FF_PI(process_info_list);
		} /* while output_pinfo */

		ff_destroy_process_info_list(process_info_list);
	} /* if !error */

	return(0);
}

static int dbset_setup_stdin(DATA_BIN_PTR dbin, FF_STD_ARGS_PTR std_args)
{
	int error = 0;
	PROCESS_INFO_LIST plist = NULL;
	PROCESS_INFO_PTR pinfo = NULL;

	FF_VALIDATE(dbin);
	FF_VALIDATE(std_args);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &plist);
	if (!error)
	{
		size_t bytes_to_read = 0;
		size_t bytes_to_alloc = 0;

		plist = dll_first(plist);
		pinfo = FF_PI(plist);
		while (pinfo)
		{
			if (IS_HEADER(PINFO_FORMAT(pinfo)) || IS_ARRAY(PINFO_FORMAT(pinfo)))
				error = err_push(ERR_GENERAL, "\"%s\": Ineligible format when redirecting standard input", PINFO_NAME(pinfo));

			bytes_to_read += PINFO_RECL(pinfo);

			plist = dll_next(plist);
			pinfo = FF_PI(plist);
		}

		if (!error)
		{
			if (std_args->cache_size)
				bytes_to_alloc = max(bytes_to_read, std_args->cache_size);
			else
				bytes_to_alloc = max(bytes_to_read, DEFAULT_CACHE_SIZE);

			std_args->input_bufsize = ff_create_bufsize(bytes_to_alloc);
			if (!std_args->input_bufsize)
				error = err_push(ERR_MEM_LACK, "");
		}

		ff_destroy_process_info_list(plist);

		if (!error)
		{
			size_t bytes_read = 0;

			ff_destroy_array_conduit_list(dbin->array_conduit_list);
			dbin->array_conduit_list = NULL;

#if FF_OS == FF_OS_DOS || FF_OS == FF_OS_MACOS
			setmode(fileno(stdin), O_BINARY);
#endif
			bytes_read = fread(std_args->input_bufsize->buffer, 1, bytes_to_read, stdin); 
			if (bytes_read != bytes_to_read)
				error = err_push(ERR_READ_FILE, "Only read %lu of %lu bytes from standard input", (unsigned long)bytes_read, (unsigned long)bytes_to_read);
			else
			{
				std_args->input_bufsize->bytes_used = bytes_read;

				/* Was an output file created by the previous db_init? */
				if (std_args->output_file && os_file_exist(std_args->output_file))
					remove(std_args->output_file);

				error = db_init(std_args, &dbin, NULL);
				if (!error || error > ERR_WARNING_ONLY)
				{
					error = db_set(dbin, DBSET_CACHE_SIZE, (unsigned long)bytes_to_alloc);
					if (!error)
					{
						error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &plist);
						if (!error)
						{
							plist = dll_first(plist);
							pinfo = FF_PI(plist);
							while (pinfo)
							{
								size_t records_in_buffer = 0;

								records_in_buffer = PINFO_CACHEL(pinfo) / PINFO_RECL(pinfo);

								PINFO_LOCUS_FILLED(pinfo) = records_in_buffer * PINFO_RECL(pinfo);

								plist = dll_next(plist);
								pinfo = FF_PI(plist);
							} /* while pinfo */

							ff_destroy_process_info_list(plist);
						} /* if !error (db_ask) */
					} /* if !error (db_set) */
				} /* if !error (db_init) */
			} /* else (if bytes_read != bytes_to_read) */
		} /* if not error (malloc) */
	} /* if !error (db_ask) */

	return(error);
}

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "db_set"

int db_set(DATA_BIN_PTR dbin, int message, ...)
{
	int error = 0;
	va_list args;

	if (!dbin)
		return(err_push(ERR_SET_DBIN, "data bin has not been defined"));

	if (!message)
		return(err_push(ERR_GENERAL, "message code not specified"));

	FF_VALIDATE(dbin);
	
	va_start(args, message);

	switch (message)
	{
		case DBSET_CACHE_SIZE: 
		{
			unsigned long cache_size = va_arg(args, unsigned long);

			error = dbset_cache_size(dbin, cache_size);
		}

		break;

		case DBSET_HEADER_FILE_NAMES:
		{
			FF_TYPES_t io_type = va_arg(args, FF_TYPES_t);
			char *file_name = va_arg(args, char *);

			error = dbset_header_file_names(dbin, io_type, file_name);
		}

		break;

		case DBSET_INPUT_HEADER:

			error = dbset_input_header(dbin);

		break;

		case DBSET_INPUT_FORMATS:
		{
			char *input_data_file_name = va_arg(args, char *);
			char *output_data_file_name = va_arg(args, char *);
			char *file_name = va_arg(args, char *);
			char *format_buffer = va_arg(args, char *);
			char *title_specified = va_arg(args, char *);
			FORMAT_DATA_LIST_HANDLE format_data_list = va_arg(args, FORMAT_DATA_LIST_HANDLE);
		
			error = dbset_input_formats(dbin, input_data_file_name, output_data_file_name, file_name, format_buffer, title_specified, format_data_list);
		}
		
		break;

		case DBSET_READ_EQV:
		{
			char *input_data_file_name = va_arg(args, char *);
		
			error = dbset_read_eqv(dbin, input_data_file_name);
		}
						
		break;
            
		case DBSET_BYTE_ORDER:
		{
			FF_TYPES_t format_type = va_arg(args, FF_TYPES_t);
				
			error = dbset_byte_order(dbin, format_type);
		}
		break;
					
		case DBSET_OUTPUT_FORMATS:
		{
			char *input_data_file_name = va_arg(args, char *);
			char *output_data_file_name = va_arg(args, char *);
			char *file_name = va_arg(args, char *);
			char *format_buffer = va_arg(args, char *);
			char *title_specified = va_arg(args, char *);
			FORMAT_DATA_LIST_HANDLE format_data_list = va_arg(args, FORMAT_DATA_LIST_HANDLE);

			error = dbset_output_formats(dbin, input_data_file_name, output_data_file_name, file_name, format_buffer, title_specified, format_data_list);
		}
		
		break;

		case DBSET_QUERY_RESTRICTION:
		{
			char *file_name = va_arg(args, char *);

			error = dbset_query_restriction(dbin, file_name);
		}
		
		break;
		
		case DBSET_VARIABLE_RESTRICTION:
		{
			char *variable_file = va_arg(args, char *);
			FORMAT_PTR output_format = va_arg(args, FORMAT_PTR);

			error = dbset_variable_restriction(variable_file, output_format);
		}

		break;

		case DBSET_CREATE_CONDUITS:
		{
			FF_STD_ARGS_PTR std_args = va_arg(args, FF_STD_ARGS_PTR);
			FORMAT_DATA_LIST format_data_list = va_arg(args, FORMAT_DATA_LIST);

			error = dbset_create_conduits(format_data_list, std_args, &(dbin->array_conduit_list));
		}

		break;

		case DBSET_USER_UPDATE_FORMATS:
		{
			error = dbset_user_update_formats(dbin);
		}

		break;

		case DBSET_INIT_CONDUITS:
		{
			FF_TYPES_t conduit_type = va_arg(args, FF_TYPES_t);
			long records_to_do = va_arg(args, long);

			error = dbset_init_conduits(dbin, conduit_type, records_to_do);
		}

		break;

		case DBSET_FORMAT_MAPPINGS:
		{
			error = dbset_format_mappings(dbin);
		}

		break;

		case DBSET_EQUATION_VARIABLES:
		{
			error = dbset_equation_variables(dbin);
		}

		break;

		case DBSET_SETUP_STDIN:
		{
			FF_STD_ARGS_PTR std_args = va_arg(args, FF_STD_ARGS_PTR);

			error = dbset_setup_stdin(dbin, std_args);
		}

		break;

		default:
		{
			assert(!ERR_SWITCH_DEFAULT);
			return(err_push(ERR_SWITCH_DEFAULT, "%s, %s:%d", ROUTINE_NAME, os_path_return_name(__FILE__), __LINE__));
		}
	} /* switch (attribute) */

	va_end(args);

	return(error);
}

/*
 * NAME:	ff_afm2bfm
 *		
 * PURPOSE:	Converts ASCII format to binary
 *
 * USAGE:	FORMAT_PTR ff_afm2bfm(FORMAT_PTR format)
 *
 * RETURNS:	FORMAT_PTR if successful, NULL if not
 *
 * DESCRIPTION:	This function takes a pointer to a format and converts
 *				a binary format with the same variable types and
 *				offsets based on the variable types.
 *
 *
 * AUTHOR:	T. Habermann, NGDC, (303) 497 - 6472, haber@ngdc.noaa.gov
 *
 * ERRORS:	
 *
 * COMMENTS:  Any array descriptor strings are copied verbatim.  Separation
 * does not change with changes to field size.
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * KEYWORDS:
 *
 */
 
#include <freeform.h>

#undef ROUTINE_NAME
#define ROUTINE_NAME "ff_afm2bfm"

FORMAT_PTR ff_afm2bfm(FORMAT_PTR format)
{
	FORMAT_PTR new_format = NULL;
	VARIABLE_PTR old_var;
	VARIABLE_PTR new_var;
	VARIABLE_LIST old_v_list;
	VARIABLE_LIST new_v_list;

	FF_NDX_t offset = 1;

	new_format = ff_create_format("say something intelligent", NULL);
	if (new_format == NULL)
	{
		err_push(ERR_MEM_LACK,"Creating New Binary Format");
		return(NULL);
	}

	new_format->type = format->type;

	/* Change from ASCII or FLAT to BINARY */
	new_format->type &= ~(FFF_FLAT | FFF_ASCII);
	new_format->type |=  FFF_BINARY;

	new_format->variables = new_v_list = dll_init();

	if (new_format->variables == NULL)
	{
		err_push(ERR_MEM_LACK,"Creating New Variable List Header");
		return(NULL);
	}
	
	old_v_list = FFV_FIRST_VARIABLE(format);
	old_var = FF_VARIABLE(old_v_list);
	while (old_var)
	{
		old_var = FF_VARIABLE(old_v_list);

		if (IS_EOL(old_var))
		{
			old_v_list = dll_next(old_v_list);
			old_var = FF_VARIABLE(old_v_list);

			continue;
		}

		/* Add Member to list for new variable */
		new_v_list = dll_add(dll_last(new_format->variables));
		if(!new_v_list)
		{
			err_push(ERR_MEM_LACK,"Creating New Variable List Node");
			ff_destroy_format(new_format);
			return(NULL);
		}

		new_var = ff_create_variable(old_var->name);
		if (!new_var)
		{
			err_push(ERR_MEM_LACK,old_var->name);
			ff_destroy_format(new_format);
			return(NULL);
		}

		new_var->start_pos = offset;
		new_var->type = old_var->type;
		new_var->precision = old_var->precision;

		if (IS_TEXT(old_var) || IS_CONSTANT(old_var) || IS_INITIAL(old_var))
			offset += FF_VAR_LENGTH(old_var) - 1;
		else
			offset += ffv_type_size(FFV_TYPE(old_var)) - 1;

		if (old_var->array_desc_str)
		{
			new_var->array_desc_str = (char *)memStrdup(old_var->array_desc_str, "new_var->array_desc_str");
			if (!new_var->array_desc_str)
			{
				err_push(ERR_MEM_LACK,old_var->name);
				ff_destroy_format(new_format);
				return(NULL);
			}
		}

		new_var->end_pos = offset++;
		++new_format->num_vars;

		dll_assign(new_var, DLL_VAR, new_v_list);

		old_v_list = dll_next(old_v_list);
		old_var = FF_VARIABLE(old_v_list);
	}

	new_format->length = offset - 1;

	return(new_format);
}

size_t ffv_ascii_type_size(VARIABLE_PTR var)
{
	size_t field_size = 0;

	FF_VALIDATE(var);
	
	switch (FFV_DATA_TYPE(var))
	{
		case FFV_TEXT:
			field_size = 1; /* unknown, determine elsehow */
		break;
		
		case FFV_INT8:
			field_size = 4;
		break;
					
		case FFV_UINT8:
			field_size = 3;
		break;
					
		case FFV_INT16:
			field_size = 6;
		break;
					
		case FFV_UINT16:
			field_size = 5;
		break;
					
		case FFV_INT32:
			field_size = 11;
		break;
					
		case FFV_UINT32:
			field_size = 10;
		break;
					
		case FFV_INT64:
			field_size = 21;
		break;
					
		case FFV_UINT64:
			field_size = 20;
		break;
					
		case FFV_FLOAT32:
			field_size = FLT_DIG + 1 + var->precision;
		break;
					
		case FFV_FLOAT64:
			field_size = DBL_DIG + 1 + var->precision;
		break;
		
		case FFV_ENOTE:
			field_size = DBL_DIG + 1 + var->precision; /* watch this one */
		break;
					
		default:
			assert(!ERR_SWITCH_DEFAULT);
			err_push(ERR_SWITCH_DEFAULT, "%d, %s:%d", (int)FFV_DATA_TYPE(var), os_path_return_name(__FILE__), __LINE__);
			field_size = 0;
		break;
	}
	
	return(field_size);
}

FORMAT_PTR ff_bfm2dfm(FORMAT_PTR format)
{
	FORMAT_PTR new_format = NULL;
	VARIABLE_PTR old_var;
	VARIABLE_PTR new_var;
	VARIABLE_LIST old_v_list;
	VARIABLE_LIST new_v_list;

	FF_NDX_t offset = 1;

	new_format = ff_create_format("say something intelligent", NULL);
	if (new_format == NULL)
	{
		err_push(ERR_MEM_LACK,"Creating New Binary Format");
		return(NULL);
	}

	new_format->type = format->type;

	/* Change from ASCII or FLAT to BINARY */
	new_format->type &= ~FFF_BINARY;
	new_format->type |= FFF_FLAT;

	new_format->variables = new_v_list = dll_init();

	if (new_format->variables == NULL)
	{
		err_push(ERR_MEM_LACK,"Creating New Variable List Header");
		return(NULL);
	}
	
	old_v_list = FFV_FIRST_VARIABLE(format);
	old_var = FF_VARIABLE(old_v_list);
	while (old_var)
	{
		old_var = FF_VARIABLE(old_v_list);

		/* Add Member to list for new variable */
		new_v_list = dll_add(dll_last(new_format->variables));
		if (!new_v_list)
		{
			err_push(ERR_MEM_LACK,"Creating New Variable List Node");
			ff_destroy_format(new_format);
			return(NULL);
		}

		new_var = ff_create_variable(old_var->name);
		if (!new_var)
		{
			err_push(ERR_MEM_LACK,old_var->name);
			ff_destroy_format(new_format);
			return(NULL);
		}

		new_var->start_pos = offset;
		new_var->type = old_var->type;
		new_var->precision = old_var->precision;

		if (IS_TEXT(old_var) || IS_CONSTANT(old_var) || IS_INITIAL(old_var))
			offset += FF_VAR_LENGTH(old_var) - 1;
		else
			offset += ffv_ascii_type_size(old_var) - 1;

		if (old_var->array_desc_str)
		{
			new_var->array_desc_str = (char *)memStrdup(old_var->array_desc_str, "new_var->array_desc_str");
			if (!new_var->array_desc_str)
			{
				err_push(ERR_MEM_LACK,old_var->name);
				ff_destroy_format(new_format);
				return(NULL);
			}
		}

		new_var->end_pos = offset++;
		++new_format->num_vars;

		dll_assign(new_var, DLL_VAR, new_v_list);

		old_v_list = dll_next(old_v_list);
		old_var = FF_VARIABLE(old_v_list);
	}

	new_format->length = offset - 1;

	return(new_format);
}

static int variable_format_control_string_to_DDR
	(
	 FF_TYPES_t format_type,
	 VARIABLE_PTR var,
	 FF_BUFSIZE_PTR ddr
	)
{
#ifdef _DEBUG
	enum {IS_EXTENDED_SDTS = TRUE};
#else
	enum {IS_EXTENDED_SDTS = FALSE};
#endif

	int error = 0;

	FF_VALIDATE(ddr);
	FF_VALIDATE(var);

	if (IS_BINARY_TYPE(format_type))
	{
		if (IS_TEXT(var))
			sprintf(ddr->buffer + ddr->bytes_used, "A(%d)", (int)FF_VAR_LENGTH(var));
		else
		{
			strcpy(ddr->buffer + ddr->bytes_used, "B");

			if (IS_EXTENDED_SDTS)
			{
				if (IS_INTEGER(var))
				{
					if (IS_UNSIGNED(var))
					{
						strcpy(ddr->buffer + ddr->bytes_used + strlen(ddr->buffer + ddr->bytes_used),
						       "U");
					}

					strcpy(ddr->buffer + ddr->bytes_used + strlen(ddr->buffer + ddr->bytes_used),
					       "I");
				}
				else
				{
					strcpy(ddr->buffer + ddr->bytes_used + strlen(ddr->buffer + ddr->bytes_used),
					       "FP");
				}
			}

			sprintf(ddr->buffer + ddr->bytes_used + strlen(ddr->buffer + ddr->bytes_used),
			        "(%d)", 8 * (int)FF_VAR_LENGTH(var));
		}
	}
	else
	{
		if (IS_TEXT(var))
			sprintf(ddr->buffer + ddr->bytes_used, "A(%d)", (int)FF_VAR_LENGTH(var));
		else if (IS_INTEGER(var))
			sprintf(ddr->buffer + ddr->bytes_used, "I(%d)", (int)FF_VAR_LENGTH(var));
		else if (IS_ENOTE(var))
			sprintf(ddr->buffer + ddr->bytes_used, "S(%d)", (int)FF_VAR_LENGTH(var));
		else
			sprintf(ddr->buffer + ddr->bytes_used, "R(%d)", (int)FF_VAR_LENGTH(var));
	}

	ddr->bytes_used += strlen(ddr->buffer + ddr->bytes_used);

	return error;
}


static int variable_list_to_DDR
	(
	 FORMAT_PTR format,
	 FF_BUFSIZE_PTR ddr
	)
{
	int error = 0;

	VARIABLE_LIST vlist = NULL;
	VARIABLE_PTR var = NULL;

	FF_VALIDATE(format);
	FF_VALIDATE(ddr);

	vlist = FFV_FIRST_VARIABLE(format);
	var = FF_VARIABLE(vlist);
	while (var)
	{
		if (IS_EOL(var) || IS_INTERNAL_VAR(var))
		{
			vlist = dll_next(vlist);
			var = FF_VARIABLE(vlist);

			continue;
		}

		strcpy(ddr->buffer + ddr->bytes_used, var->name);
		ddr->bytes_used += strlen(var->name);

		strcpy(ddr->buffer + ddr->bytes_used, "!");
		ddr->bytes_used++;

		if (ddr->bytes_used + SCRATCH_QUANTA > ddr->total_bytes)
		{
			error = ff_resize_bufsize(ddr->total_bytes + SCRATCH_QUANTA, &ddr);
			if (error)
				return error;
		}

		vlist = dll_next(vlist);
		var = FF_VARIABLE(vlist);
	}

	/* Back up over extra exclamation (!) */
	--ddr->bytes_used;

	strcpy(ddr->buffer + ddr->bytes_used, "&(");
	ddr->bytes_used += 2;

	vlist = FFV_FIRST_VARIABLE(format);
	var = FF_VARIABLE(vlist);
	while (var)
	{
		if (IS_EOL(var) || IS_INTERNAL_VAR(var))
		{
			vlist = dll_next(vlist);
			var = FF_VARIABLE(vlist);

			continue;
		}

		variable_format_control_string_to_DDR(format->type, var, ddr);

		strcpy(ddr->buffer + ddr->bytes_used, ",");
		ddr->bytes_used++;

		if (ddr->bytes_used + SCRATCH_QUANTA > ddr->total_bytes)
		{
			error = ff_resize_bufsize(ddr->total_bytes + SCRATCH_QUANTA, &ddr);
			if (error)
				return error;
		}

		vlist = dll_next(vlist);
		var = FF_VARIABLE(vlist);
	}

	/* Back up over extra comma (,) */
	--ddr->bytes_used;

	strcpy(ddr->buffer + ddr->bytes_used, ");\n");
	ddr->bytes_used += 3;

	return error;
}

/*****************************************************************************
 * NAME:  format_to_ISO8211DDR
 *
 * PURPOSE:  Translate a format to an Attribute Primary Data Description Record
 *
 * USAGE:
 *
 * RETURNS:
 *
 * DESCRIPTION:  Writes an ISO 8211 Data Description Record for an Attribute
 * Primary module, according to Spatial Data Transfer Standard (SDTS) - Part
 * 3, ISO 8211 Encoding.  This will look something like this:
 *
 * "0000 0000;&RESERVED;\n"
 * "0001 0100;&DDF RECORD IDENTIFIER;\n"
 * "ATPR 1600;&ATTRIBUTE PRIMARY&MODN!RCID&(A,I);\n"
 * "OBID 1600;&SPATIAL OBJECT ID&MODN!RCID&(A,I);\n"
 * "ATTP 1600;&PRIMARY ATTRIBUTES&attribute1!attribute2!...!attributen&(z,z...);\n"
 *
 * where attribute1, attribute2, etc. will be replaced with variable names, and the
 * parenthesized z's will be replaced with format control strings (determined by
 * variable types, with necessary ambiguity and vagueness for binary formats).
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

int format_to_ISO8211DDR
	(
	 FORMAT_PTR format,
	 const char *first_fields,
	 FF_BUFSIZE_HANDLE ddr
	)
{
	int error = 0;

	FF_VALIDATE(format);

	*ddr = ff_create_bufsize(2 * SCRATCH_QUANTA);
	if (!*ddr)
		return ERR_MEM_LACK;

	strcpy((*ddr)->buffer, first_fields);

	(*ddr)->bytes_used = strlen((*ddr)->buffer);

	if ((*ddr)->bytes_used + SCRATCH_QUANTA > (*ddr)->total_bytes)
	{
		error = ff_resize_bufsize((*ddr)->total_bytes + SCRATCH_QUANTA, ddr);
		if (error)
			return error;
	}

	error = variable_list_to_DDR(format, *ddr);

	return error;
}

/*
 *	FILENAME: name_tab.h
 *
 *	CONTAINS: This is the include file for much of the name table
 *		  code: symbols, structures, and prototypes. 
 *			More is found in include file
 *		  databin.h, which includes this file.
 *
 *
 */
 
#ifndef NAME_TAB_H__
#define NAME_TAB_H__

#define NTKN_INPUT_EQV         "input_eqv"
#define NTKN_OUTPUT_EQV        "output_eqv"
#define NTKN_BEGIN_CONSTANT    "begin constant"
#define NTKN_BEGIN_NAME_EQUIV  "begin name_equiv"
#define NTKN_END_CONSTANT      "end constant"
#define NTKN_END_NAME_EQUIV    "end name_equiv"

/* NAME_TABLE values' buffer is in multiples of 256 */
#define NAME_TABLE_QUANTA 256

#define INPUT_NAME_TABLE_EXISTS(dbin) ( (dbin) ? !fd_get_format_data(dbin->table_list, FFF_INPUT | FFF_TABLE, NULL) : 0)

/* define the conversion between user's variable name and value to geovu name and value */

typedef struct t_table /* define value translation table */
{
#ifdef FF_CHK_ADDR
	void           *check_address;
#endif
	FF_TYPES_t    gtype;  /* corresponding geovu data type */
	void           *gvalue;	/* corresponding geovu data value */
	FF_TYPES_t    utype;  /* define the user's data type */
	void           *uvalue; /* define the user's data value */
	struct t_table *next;
} TRANSLATOR, *TRANSLATOR_PTR;

/* Name Table Prototypes */
int nt_parse(char *origin, FF_BUFSIZE_PTR, NAME_TABLE_HANDLE);
NAME_TABLE_PTR nt_create(char *origin);

int nt_merge_name_table(NAME_TABLE_LIST_HANDLE, NAME_TABLE_PTR);

void nt_free_trans(TRANSLATOR_PTR trans);

BOOLEAN nt_copy_translator_sll(VARIABLE_PTR source_var, VARIABLE_PTR target_var);
BOOLEAN nt_comp_translator_sll(VARIABLE_PTR var1, VARIABLE_PTR var2);

#endif /* (NOT) NAME_TAB_H__ */


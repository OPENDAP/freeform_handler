
/* 
   (c) COPYRIGHT URI/MIT 1997-98
   Please read the full copyright statement in the file COPYRIGH.  

   Authors: reza (Reza Nekovei)
*/

/*
 * $Log: ff_read.c,v $
 * Revision 1.3  1998/04/21 17:14:09  jimg
 * Fixes for warnings, etc
 *
 * Revision 1.2  1998/04/16 18:11:24  jimg
 * Sequence support added by Reza
 */

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: ff_read.c,v 1.3 1998/04/21 17:14:09 jimg Exp $"};

#include <freeform.h>

static BOOLEAN 
is_separation(PROCESS_INFO_PTR finfo)
{
    FF_VALIDATE(finfo);

    if (PINFO_ARRAY_MAP(finfo)->subsep)
	return(TRUE);
    else
	return(FALSE);
}

static BOOLEAN 
is_offset(PROCESS_INFO_PTR finfo)
{
    FF_VALIDATE(finfo);

    if (PINFO_ARRAY_OFFSET(finfo) != 0)
	return(TRUE);
    else
	return(FALSE);
}

/* Check to see if any fseek's will be done on output. This will happen if an
   array has separation or has an offset other than zero. Basically, output
   formats with only a single array will not require any fseek's on output.

   Also check for any separate headers */

static int 
check_stdout_contiguity(DATA_BIN_PTR dbin)
{
    PROCESS_INFO_LIST finfo_list = NULL;
    PROCESS_INFO_PTR  finfo = NULL;
    int error = 0;

    FF_VALIDATE(dbin);

    error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_OUTPUT, &finfo_list);
    if (!error) {
	finfo_list = dll_first(finfo_list);
	finfo = FF_PI(finfo_list);
	while (finfo) {
	    FF_VALIDATE(finfo);

	    if (is_separation(finfo) || is_offset(finfo) ||
		(IS_HEADER(PINFO_FORMAT(finfo)) &&
		 IS_SEPARATE(PINFO_FORMAT(finfo)))) {
		error = err_push(ERR_GENERAL, "You must use -o with this output format: %s", PINFO_NAME(finfo));
		break;
	    }

	    finfo_list = dll_next(finfo_list);
	    finfo = FF_PI(finfo_list);
	}

	ff_destroy_process_info_list(finfo_list);
    }
    else
	error = err_push(ERR_GENERAL, "Nothing to redirect as no output formats have been specified");

    return(error);
}

static int 
check_standard_input(FF_STD_ARGS_PTR std_args, DATA_BIN_PTR dbin,
		     BOOLEAN *done_processing)
{
    int error = 0;

    FF_VALIDATE(std_args);
    FF_VALIDATE(dbin);

    if (std_args->user.is_stdin_redirected) {
	size_t bytes_read = 0;
	size_t bytes_to_read = 0;
	PROCESS_INFO_LIST plist = NULL;

	if (feof(stdin)) {
	    *done_processing = TRUE;
	    return(0);
	}

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &plist);
	if (!error) {
	    PROCESS_INFO_PTR pinfo = NULL;

	    plist = dll_first(plist);
	    pinfo = FF_PI(plist);
	    while (pinfo) {
		size_t records_to_read = 0;

		records_to_read = PINFO_BUFFER_SIZE(pinfo) / PINFO_RECL(pinfo);
		/* records_to_read = PINFO_LOCUS_SIZE(pinfo) /
		   PINFO_RECL(pinfo); */
		bytes_to_read = records_to_read * PINFO_RECL(pinfo);

		bytes_read = fread(std_args->input_buffer, 1,
				   bytes_to_read, stdin); 

		if (bytes_read != bytes_to_read) {
		    if (!feof(stdin))
			error = err_push(ERR_READ_FILE, "...from standard input");
		}
		else
		    *done_processing = FALSE;

		if (!error) {
		    size_t records_read = 0;

		    records_read = bytes_read / PINFO_RECL(pinfo);

		    if (bytes_read % PINFO_RECL(pinfo))
			error = err_push(ERR_FILE_LENGTH, "...from standard input");
		    else
			error = make_tabular_format_array_mappings(pinfo, records_read, 0);
		}

		if (error)
		    break;

		plist = dll_next(plist);
		pinfo = FF_PI(plist);
	    }

	    ff_destroy_process_info_list(plist);
	}
    }

    return(error);
}

static long 
bytes_to_process(PROCESS_INFO_LIST finfo_list)
{
    long bytes_to_process = 0;
    PROCESS_INFO_PTR finfo = NULL;

    finfo_list = dll_first(finfo_list);
    finfo = FF_PI(finfo_list);
    while (finfo) {
	if (PINFO_MATE(finfo))
	    bytes_to_process += PINFO_MATE_BYTES_LEFT(finfo);

	finfo_list = dll_next(finfo_list);
	finfo      = FF_PI(finfo_list);
    }

    return(bytes_to_process);
}

int 
convert_in(FF_STD_ARGS_PTR std_args, DATA_BIN_PTR dbin)
{
    int error = 0;

    long bytes_remaining = 0;
    long total_bytes = 0L;

    BOOLEAN done_processing = FALSE;

    PROCESS_INFO_LIST finfo_list = NULL;

    FF_VALIDATE(dbin);
	
    error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &finfo_list);
    if (error)
	return(error);
	
    total_bytes = bytes_to_process(finfo_list);

    while (!error && !done_processing) {
	error = db_do(dbin, DBDO_PROCESS_FORMATS, finfo_list);
	if (error == EOF) {
	    error = 0;
	    done_processing = TRUE;
	}

	if (!error)
	    error = db_do(dbin, DBDO_WRITE_FORMATS, finfo_list);

	if (!error)
	    error = check_standard_input(std_args, dbin, &done_processing);
    }
	
    /* End Processing */
	
    bytes_remaining = bytes_to_process(finfo_list);
    if (bytes_remaining)
	error = err_push(ERR_PROCESS_DATA + ERR_WARNING_ONLY, 
			 "%ld BYTES OF DATA NOT PROCESSED.", bytes_remaining);

    ff_destroy_process_info_list(finfo_list);

    return(error);
} 

#define DODS_DATA_PRX "dods-"
	
long
read_ff(char *dataset, char *if_file, char *o_format, char *o_buffer, 
	unsigned long size)
{
    char *tmp_file = 0;
    FF_BUFSIZE_PTR bufsize = NULL;
  
    DATA_BIN_PTR dbin = NULL;
    int error = 0;
    FF_STD_ARGS_PTR std_args = NULL;

    /*  char * el = "error.log"; */

    std_args = ff_create_std_args();
    if (!std_args) {
	error = ERR_MEM_LACK;
	goto main_exit;
    }

    /*
      std_args->output_bufsize->buffer = o_buffer;  
      std_args->output_bufsize->bytes_used = size;  
    */
    /* Can not get it to work ? Core dump in FF library
       using a temporary file for now, Reza     */
  
    tmp_file = tempnam(NULL, DODS_DATA_PRX);		       
    std_args->output_file = tmp_file;                  

    std_args->user.is_stdin_redirected = 0;
    std_args->input_file = dataset;
    std_args->input_format_file = if_file;
    std_args->output_format_buffer = o_format;

    /*	std_args->error_log = el;*/

    /*	std_args->cache_size = size;  */

    error = db_init(std_args, &dbin, NULL);
    if (error && error < ERR_WARNING_ONLY)
	goto main_exit;
    else if (error)
	error = 0;
  
    if (!isatty(fileno(stdout))) {
#if FF_OS == FF_OS_DOS || FF_OS == FF_OS_MACOS
	setmode(fileno(stdout), O_BINARY);
#endif
	error = check_stdout_contiguity(dbin);
	if (error)
	    goto main_exit;
    }
  
    error = convert_in(std_args, dbin);
  
 main_exit:

    if (dbin)
	db_destroy(dbin);
  
    if (error || err_state())
	err_disp(std_args);
  
    if (std_args)
	ff_destroy_std_args(std_args);
  
    /*	memExit(error ? EXIT_FAILURE : EXIT_SUCCESS, "main"); */
  
    error = ff_file_to_bufsize(tmp_file, &bufsize); /* check the error
						       here */
    unlink(tmp_file); 
    memcpy(o_buffer, bufsize->buffer,(size_t) bufsize->bytes_used); 	
  
    /* return size; */
    return bufsize->bytes_used;
}

#ifdef TEST

int main(int argc, char *argv[])
{
    char *ds;
    char *iff;
    char *of;
    unsigned long buffer_size;
    char *data;
    unsigned long bytes;

    ds = malloc(strlen(argv[1]) + 1);
    iff = malloc(strlen(argv[2]) + 1); 
    of = malloc(strlen(argv[3]) + 1);
    buffer_size = atoi(argv[4]);
    data = malloc(buffer_size);

    strcpy(ds, argv[1]);
    strcpy(iff, argv[2]);
    strcpy(of, argv[3]);

    bytes = read_ff(ds, iff, of, data, buffer_size);

    printf("Bytes read: %d\n", bytes);
    if (bytes > 0)
        fwrite(data, 1, bytes, stdout);

    exit(0);
}

#endif


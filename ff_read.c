/* 
   (c) COPYRIGHT URI/MIT 1997-98
   Please read the full copyright statement in the file COPYRIGHT.
   Authors: reza (Reza Nekovei)
*/

#include "config_ff.h"

static char rcsid[] not_used ={"$Id$"};

#include <freeform.h>

#include "debug.h"


long
read_ff(char *dataset, char *if_file, char *o_format, char *o_buffer, 
	unsigned long bsize)
{
    int error = 0;
    FF_BUFSIZE_PTR newform_log = NULL;
    FF_BUFSIZE_PTR bufsz = NULL;
    FF_STD_ARGS_PTR std_args = NULL;
  
    std_args = ff_create_std_args();
    if (!std_args) {
	error = ERR_MEM_LACK;
	goto main_exit;
    }

    /* set the std_arg structure values **/
    std_args->error_prompt = FALSE;
    std_args->user.is_stdin_redirected = 0;
    std_args->input_file = dataset;
    std_args->input_format_file = if_file;
    std_args->output_file = NULL;
    std_args->output_format_buffer = o_format;
    std_args->log_file = "/dev/null"; 
    /* Define DBG (as per dap/debug.h) to get a log file from FreeForm. 9/8/98
       jhrg */
    DBG(std_args->log_file = "/tmp/ffdods.log"); 

    bufsz = (FF_BUFSIZE_PTR)memMalloc(sizeof(FF_BUFSIZE), "bufsz");
    if (!bufsz) {
	error = ERR_MEM_LACK;
	goto main_exit;
    }
    
    bufsz->usage = 1;
    bufsz->buffer = o_buffer;
    bufsz->total_bytes = (FF_BSS_t)bsize;
    bufsz->bytes_used = (FF_BSS_t)0;
    std_args->output_bufsize = bufsz; 
  
    newform_log = ff_create_bufsize(SCRATCH_QUANTA);
    if (!newform_log) {
	error = ERR_MEM_LACK;
	goto main_exit;
    }
  
    error = newform(std_args, newform_log, stderr);

#ifdef TEST_LOGGING
    char log_file_write_mode[4];
    /* Is user asking for both error logging and a log file? */
    if (std_args->error_log && newform_log)  {
	if (strcmp(std_args->error_log, std_args->log_file))
	    strcpy(log_file_write_mode, "w");
	else
	    strcpy(log_file_write_mode, "a");
      
    }
    else if (newform_log)
	strcpy(log_file_write_mode, "w");
    
    FILE *fp = NULL;
      
    fp = fopen(std_args->log_file, log_file_write_mode);
    if (fp) {
	size_t bytes_written = fwrite(newform_log->buffer, 1, 
				      (size_t)newform_log->bytes_used, fp);
      
	if (bytes_written != (size_t)newform_log->bytes_used)
	    error = err_push(ERR_WRITE_FILE, "Wrote %d bytes of %d to %s", 
			     (int)bytes_written, 
			     (int)newform_log->bytes_used, std_args->log_file);
      
	fclose(fp);
    }
    else
	error = err_push(ERR_CREATE_FILE, std_args->log_file);
  
    if (std_args->user.is_stdin_redirected)
	ff_destroy_bufsize(std_args->input_bufsize);
#endif /* TEST_LOGGING */

    ff_destroy_bufsize(newform_log);

 main_exit:

    err_disp(std_args);

    if (std_args)
	ff_destroy_std_args(std_args);
  
    return bufsz->bytes_used;
}

#ifdef TEST 

#define OUTPUT_FMT_STR "binary_output_data \"output\"\n\
year 1 4 int32 0\n\
day 5 8 int32 0\n\
time 9 9 text 0\n\
DODS_URL 10 135 text 0"

int main(int argc, char *argv[])
{
    char *datafile;
    char *in_format;
    char *out_format;
    char *data;
    unsigned long data_size;
    unsigned long bytes_in;

    /* name and directory for data file */
    datafile = malloc(strlen(argv[1]) + 1);
    strcpy(datafile, argv[1]);

    /* name and directory for the input format file */
    in_format = malloc(strlen(argv[2]) + 1); 
    strcpy(in_format, argv[2]);

    /* output format written in the buffer */
    out_format = malloc(strlen(OUTPUT_FMT_STR) + 1);
    strcpy(out_format, OUTPUT_FMT_STR);

    /* size of data expecting to read (can be oversized) */
    data_size = atoi(argv[3]);
    data = malloc(data_size);

    printf("Converting to the following format:\n%s\n",out_format);
    bytes_in = read_ff(datafile, in_format, out_format, data, data_size);

    printf("Bytes read: %ld\n", bytes_in);
    if (bytes_in > 0)
        fwrite(data, 1, bytes_in, stdout);

    exit(0);
}

#endif 

/*
 * $Log: ff_read.c,v $
 * Revision 1.12  2001/09/28 23:19:43  jimg
 * Merged with 3.2.3.
 *
 * Revision 1.11.14.1  2001/05/21 22:55:00  jimg
 * Moved Log to the end of the file.
 *
 * Revision 1.11  1999/05/04 02:55:38  jimg
 * Merge with no-gnu
 *
 * Revision 1.10  1998/11/13 05:43:41  jimg
 * Fixed up the TEST_LOGGING stuff.
 *
 * Revision 1.9  1998/11/12 05:42:40  jimg
 * Changed defines so that you can build the test code separately from the
 * error logging code. I don't really understand the latter.
 *
 * Revision 1.8  1998/11/10 03:46:19  jimg
 * Various fixes for the fields of std_args.
 *
 * Revision 1.7  1998/10/29 16:58:47  mao
 * turned off error prompting
 *
 * Revision 1.6  1998/09/17 17:41:42  jimg
 * Changed the error messages to aid in bug tracking.
 *
 * Revision 1.5  1998/09/08 19:19:12  jimg
 * Changed the error reporting scheme a bit. This should actually be sending
 * back Error objects, but that will require some doing. Maybe there is a
 * better way to decode FF's error messages?
 *
 * Revision 1.4  1998/08/12 21:21:13  jimg
 * Massive changes from Reza. Compatible with the new FFND library
 *
 * Revision 1.3  1998/04/21 17:14:09  jimg
 * Fixes for warnings, etc
 *
 * Revision 1.2  1998/04/16 18:11:24  jimg
 * Sequence support added by Reza
 */

/* 
   (c) COPYRIGHT URI/MIT 1997-98
   Please read the full copyright statement in the file COPYRIGH.  

   Authors: reza (Reza Nekovei)
*/

/*
 * $Log: ff_read.c,v $
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

#include "config_ff.h"

static char rcsid[] __unused__ ={"$Id: ff_read.c,v 1.7 1998/10/29 16:58:47 mao Exp $"};

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
  if (!std_args)
    {
      error = ERR_MEM_LACK;
      goto main_exit;
    }

  /* set the std_arg structure values **/
  std_args->error_prompt = FALSE;
  std_args->user.is_stdin_redirected = 0;
  std_args->input_file = dataset;
  std_args->input_format_file = if_file;
  std_args->output_file =	NULL;
  std_args->output_format_buffer = o_format;
  std_args->log_file = "/dev/null"; 
  /* Define DBG (as per dap/debug.h) to get a log file from FreeForm. 9/8/98
     jhrg */
  DBG(std_args->log_file = "/tmp/ffdods.log"); 

  bufsz = (FF_BUFSIZE_PTR)memMalloc(sizeof(FF_BUFSIZE), "bufsz");
  bufsz->usage = 1;
  bufsz->buffer = o_buffer;
  bufsz->total_bytes = (FF_BSS_t)bsize;
  bufsz->bytes_used = (FF_BSS_t)0;
  std_args->output_bufsize = bufsz; 
  
  newform_log = ff_create_bufsize(SCRATCH_QUANTA);
  if (!newform_log)
    {
      error = ERR_MEM_LACK;
      goto main_exit;
    }
  
  error = newform(std_args, newform_log, stderr);

#ifdef TEST  
  char log_file_write_mode[4];
  /* Is user asking for both error logging and a log file? */
  if (std_args->error_log && newform_log)
    {
      if (strcmp(std_args->error_log, std_args->log_file))
	strcpy(log_file_write_mode, "w");
      else
	strcpy(log_file_write_mode, "a");
      
    }
  else if (newform_log)
    strcpy(log_file_write_mode, "w");
    
  FILE *fp = NULL;
      
  fp = fopen(std_args->log_file, log_file_write_mode);
  if (fp)
    {
      size_t bytes_written = fwrite(newform_log->buffer, 1, (size_t)newform_log->bytes_used, fp);
      
      if (bytes_written != (size_t)newform_log->bytes_used)
	error = err_push(ERR_WRITE_FILE, "Wrote %d bytes of %d to %s", (int)bytes_written, (int)newform_log->bytes_used, std_args->log_file);
      
      fclose(fp);
    }
  else
    error = err_push(ERR_CREATE_FILE, std_args->log_file);
  
  if (std_args->user.is_stdin_redirected)
    ff_destroy_bufsize(std_args->input_bufsize);
#endif //TEST  

  ff_destroy_bufsize(newform_log);

 main_exit:

  err_disp(std_args);

  if (std_args)
    ff_destroy_std_args(std_args);
  
  /* return size; */
  return bufsz->bytes_used;
  
}


#ifdef TEST 

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

    /* output format writen in the buffer */
    out_format = malloc(250);
    strcpy(out_format, "ascii_output_data \"data st\" \nfvar1 1 4 ARRAY[\"records\"1 to 101] of int32 0");

    /* size of data expecting to read (can be oversized)*/
    data_size = atoi(argv[3]);
    data = malloc(data_size);


    printf("Converting to the following format:\n%s\n",out_format);
    bytes_in = read_ff(datafile, in_format, out_format, data, data_size);

    printf("Bytes read: %d\n", bytes_in);
    if (bytes_in > 0)
        fwrite(data, 1, bytes_in, stdout);
    exit(0);
}

#endif 

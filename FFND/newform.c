/* FILENAME:  newform.c
 *
 * CONTAINS:  newform()
 */

#include <freeform.h>

#ifdef TIMER
#include <time.h>
#endif 

#ifdef FF_MAIN

static int send_to_log(FF_BUFSIZE_PTR log_bufsize, char *format, ...)
{
	va_list va_args;
	int bytes_written;
	enum {MAX_LINE_LENGTH = 80};

	va_start(va_args, format);
  
	if (log_bufsize->bytes_used + MAX_LINE_LENGTH > log_bufsize->total_bytes)
	{
		int error = 0;

		error = ff_resize_bufsize(log_bufsize->total_bytes + MAX_LINE_LENGTH, &log_bufsize);
		if (error)
		{
			err_push(ERR_MEM_LACK, "");
			return(0);
		}
	}
  
	vsprintf(log_bufsize->buffer + log_bufsize->bytes_used,
	         format,
	         va_args
	        );
	bytes_written = strlen(log_bufsize->buffer + log_bufsize->bytes_used);
	log_bufsize->bytes_used += bytes_written;
		
	assert(log_bufsize->bytes_used < log_bufsize->total_bytes);

	return(bytes_written);
}

/*****************************************************************************
 * NAME:  check_for_unused_flags()
 *
 * PURPOSE:  Has user asked for an unimplemented option?
 *
 * USAGE:  check_for_unused_flags(std_args_ptr);
 *
 * RETURNS:  void
 *
 * DESCRIPTION:  All FreeForm utilities do not employ all of the "standard"
 * FreeForm command line options.  Check if the user has unwittingly asked
 * for any options which this utility will ignore.
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

static int check_for_unused_flags(FF_STD_ARGS_PTR std_args)
{
	int error = 0;

	if (std_args->user.set_cv_precision)
	{
		error = err_push(ERR_IGNORED_OPTION, "precision (checkvar only)");
	}
	
	if (std_args->user.set_cv_missing_data)
	{
		error = err_push(ERR_IGNORED_OPTION, "missing data flag (checkvar only)");
	}
	
	if (std_args->cv_maxbins)
	{
		error = err_push(ERR_IGNORED_OPTION, "maximum number of histogram bins (checkvar only)");
	}
	
	if (std_args->cv_maxmin_only)
	{
		error = err_push(ERR_IGNORED_OPTION, "maximum and minimum processing only (checkvar only)");
	}

	if (std_args->user.is_stdin_redirected && std_args->records_to_read)
	{
		error = err_push(ERR_IGNORED_OPTION, "Records to read when redirecting standard input");
	}
	
	return(error);
}
	
static int wfprintf(FILE *stream, const char *format, ...)
/*****************************************************************************
 * NAME: wfprintf()
 *
 * PURPOSE:  FF_MAIN-sensitive wrapper around fprintf()
 *
 * USAGE:  same as fprintf()
 *
 * RETURNS:  same as fprintf()
 *
 * DESCRIPTION:  If FF_MAIN is defined, calls vfprintf().
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
 * ERRORS:  vsprintf returns char * on non-System V Unix environments.
 ****************************************************************************/

{
	va_list va_args;

	va_start(va_args, format);
	
	return(vfprintf(stream, format, va_args));
}

void main(int argc, char *argv[])
{
	char *greeting = 
{
#ifdef FF_ALPHA
"\nWelcome to Newform alpha "FFND_LIB_VER" "__DATE__" -- an NGDC FreeForm ND application\n\n"
#elif defined(FF_BETA)
"\nWelcome to Newform beta "FFND_LIB_VER" "__DATE__" -- an NGDC FreeForm ND application\n\n"
#else
"\nWelcome to Newform release "FFND_LIB_VER" -- an NGDC FreeForm ND application\n\n"
#endif
};

	char *command_line_usage = {
"Several Newform command line elements have default extensions:\n\
data_file: .bin = binary  .dat = ASCII  .dab = dBASE\n\
input/output_format_file: .fmt = format description file\n\
Newform [-d data_file] [-f format_file] [-if input_format_file]\n\
                  [-of output_format_file] [-ft \"format title\"]\n\
                  [-ift \"input format title\"] [-oft \"output format title\"]\n\
                  [-b cache_size] Sets the input data cache size in bytes\n\
                  [-c count] No. records to process at head(+)/tail(-) of file\n\
                  [-q query] Output records matching criteria in query file\n\
                  [-v var] Process variables listed in var file, default = all\n\
                  [-o output_file] default = output to screen\n\
                  [-el error_log_file] error messages go to error_log_file\n\
                  [-ep] passive error messages - non-interactive\n\
                  [-ol log] File to contain log of processing information\n\n\
See the FreeForm User's Guide for detailed information.\n"
};

	FF_BUFSIZE_PTR bufsize = NULL;
	FF_BUFSIZE_PTR newform_log = NULL;

	char log_file_write_mode[4];
  
	DATA_BIN_PTR dbin = NULL;
	int error = 0;
	FF_STD_ARGS_PTR std_args = NULL;

#ifdef TIMER	
	time_t start_time;
	(void)time(&start_time);
#endif

	fprintf(stderr, "%s", greeting);
	
	std_args = ff_create_std_args();
	if (!std_args)
	{
		error = ERR_MEM_LACK;
		goto main_exit;
	}

	error = parse_command_line(argc, argv, std_args);
	if (error)
		goto main_exit;

if (0 && std_args->output_file == NULL && std_args->output_bufsize == NULL)
{
	std_args->output_bufsize = ff_create_bufsize(USHRT_MAX - 1);
	if (!std_args->output_bufsize)
	{
		error = err_push(ERR_MEM_LACK, "");
		goto main_exit;
	}
}

	if (std_args->log_file)
	{
		newform_log = ff_create_bufsize(SCRATCH_QUANTA);
		if (newform_log)
			send_to_log(newform_log, "%s", greeting);
		else
		{
			error = err_push(ERR_MEM_LACK, "");
			goto main_exit;
		}
	}

	/* Check number of command args */
	if (argc == 1 && !std_args->user.is_stdin_redirected)
	{
		fprintf(stderr, "%s", command_line_usage);
		ff_destroy_std_args(std_args);
	
		memExit(EXIT_FAILURE, "main, argc<2");
	}

	error = check_for_unused_flags(std_args);
	if (error)
		goto main_exit;

	error = db_init(std_args, &dbin, NULL);
	if (error && error < ERR_WARNING_ONLY)
		goto main_exit;
	else if (error)
		error = 0;

	if (std_args->user.is_stdin_redirected)
	{
		error = db_set(dbin, DBSET_SETUP_STDIN, std_args);
		if (error)
			goto main_exit;
	}

	if (!isatty(fileno(stdout)))
	{
#if FF_OS == FF_OS_DOS || FF_OS == FF_OS_MACOS
		setmode(fileno(stdout), O_BINARY);
#endif
		error = db_do(dbin, DBDO_CHECK_STDOUT);
		if (error)
			goto main_exit;
	}

	if (std_args->query_file)
	{
		wfprintf(stderr, "Using query file: %s\n", std_args->query_file);
		if (newform_log)
			send_to_log(newform_log, "Using query file: %s\n", std_args->query_file);
	}

	/* Display some information about the data, ignore errors */
	db_ask(dbin, DBASK_FORMAT_SUMMARY, &bufsize);
	wfprintf(stderr, "%s\n", bufsize->buffer);
	if (newform_log)
		send_to_log(newform_log, "%s\n", bufsize->buffer);
	ff_destroy_bufsize(bufsize);

if (0)
{
	int error = 0;
	int i = 0;
	int num_names = 0;
	char **names_vector = NULL;

	error = db_ask(dbin, DBASK_VAR_NAMES, FFF_INPUT, &num_names, &names_vector);
	if (error)
		goto main_exit;

	for (i = 0; i < num_names; i++)
	{
		int num_dim_names = 0;
		char **dim_names_vector = NULL;

		error = db_ask(dbin, DBASK_ARRAY_DIM_NAMES, names_vector[i], &num_dim_names, &dim_names_vector);
		if (!error)
		{
			FF_ARRAY_DIM_INFO_PTR array_dim_info = NULL;
			int j = 0;

			for (j = 0; j < num_dim_names; j++)
			{
				error = db_ask(dbin, DBASK_ARRAY_DIM_INFO, names_vector[i], dim_names_vector[j], &array_dim_info);
				if (!error)
				{
					printf("Array %s, dimension %s:\n", names_vector[i], dim_names_vector[j]);
					printf("Start index is %ld\n", array_dim_info->start_index);
					printf("End index is %ld\n", array_dim_info->end_index);
					printf("Granularity is %ld\n", array_dim_info->granularity);
					printf("Separation is %ld\n", array_dim_info->separation);
					printf("Grouping is %ld\n", array_dim_info->grouping);
					printf("Number of elements in array is %ld\n\n", array_dim_info->num_array_elements);

					memFree(array_dim_info, "");
					array_dim_info = NULL;
				}
			}
			
			memFree(dim_names_vector, "");
			dim_names_vector = NULL;
		}
	}

	memFree(names_vector, "");
}

#ifdef TIMER
	error = newform(start_time, std_args, dbin, newform_log);
#else
	error = newform(std_args, dbin, newform_log);
#endif
	
	wfprintf(stderr,"\n");

main_exit:

	if (dbin)
		db_destroy(dbin);
	
	if (error || err_state())
		err_disp(std_args);

	/* Is user asking for both error logging and a log file? */
	if (std_args->error_log && newform_log)
	{
		if (strcmp(std_args->error_log, std_args->log_file))
		{
#if FF_OS == FF_OS_UNIX
			strcpy(log_file_write_mode, "w");
#else
			strcpy(log_file_write_mode, "wt");
#endif
		}
		else
		{
#if FF_OS == FF_OS_UNIX
			strcpy(log_file_write_mode, "a");
#else
			strcpy(log_file_write_mode, "at");
#endif
		}
	}
	else if (newform_log)
	{
#if FF_OS == FF_OS_UNIX
			strcpy(log_file_write_mode, "w");
#else
			strcpy(log_file_write_mode, "wt");
#endif
	}

	if (newform_log)
	{
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

		ff_destroy_bufsize(newform_log);
	}

if (0 && std_args->output_file == NULL && std_args->output_bufsize)
	ff_destroy_bufsize(std_args->output_bufsize);

	if (std_args->user.is_stdin_redirected)
		ff_destroy_bufsize(std_args->input_bufsize);

	if (std_args)
		ff_destroy_std_args(std_args);
	
	memExit(error ? EXIT_FAILURE : EXIT_SUCCESS, "main");
}

#endif /* FF_MAIN */

#ifdef ROUTINE_NAME
#undef ROUTINE_NAME
#endif
#define ROUTINE_NAME "Newform"

/*****************************************************************************
 * NAME:  newform()
 *
 * PURPOSE:  To change the format of a data set.
 *
 * USAGE:  error = newform(FFF_STD_ARGS_PTR pstd_args);
 *
 * RETURNS:  Zero on success, otherwise an error code defined in err.h or errno.h
 *
 * DESCRIPTION:  newform takes an allocated and filled std_args structure
 * containing at a minimum the data file name to process.  Other fields, if
 * left initialized (zero for integer variables and NULL for char pointers)
 * will result in the following:  Default searches for format files,
 * local_buffer (the data bin scratch buffer) allocated with a size of
 * DEFAULT_BUFFER_SIZE, and no variable thinning on output, no queries, and
 * no initial or trailing records to skip.
 *
 * In any event, local_buffer should never be allocated by the calling routine.
 * Also, while the user has ability to change the local_buffer size, this
 * should probably never actually be done, as newform() changes local_buffer_size
 * to LOCAL_BUFFER_SIZE (32K) but only if local_buffer_size has been set to
 * DEFAULT_BUFFER_SIZE (8K).
 *
 * AUTHOR:  Ted Habermann, NGDC, (303)497-6472, haber@ngdc.noaa.gov
 *
 * SYSTEM DEPENDENT FUNCTIONS:
 *
 * GLOBALS:
 *
 * COMMENTS:  If GeoVu is calling NEWFORM (as determined by FF_MAIN not being defined
 * but TEST_SUITE is not) then the menu information associated with the current
 * DATA SOURCE (as contained in the data bin string data base) gets grafted
 * into the "input" data bin.  I'm assuming that the only way a newform()-
 * generated data bin can get the menu information is by such a graft.  So,
 * before I call db_free(input), I always set input->strdb to NULL, so that
 * db_free() cannot possibly damage the menu information associated with the
 * GeoVu-generated data bin.
 *
 * KEYWORDS:
 *
 * ERRORS:
 ****************************************************************************/

#ifdef FF_MAIN
#ifdef TIMER
int newform
	(
	 time_t start_time,
	 FF_STD_ARGS_PTR std_args,
	 DATA_BIN_PTR dbin,
	 FF_BUFSIZE_PTR newform_log
	)
#else
int newform
	(
	 FF_STD_ARGS_PTR std_args,
	 DATA_BIN_PTR dbin,
	 FF_BUFSIZE_PTR newform_log
	)
#endif
#else
int newform
	(
	 DATA_BIN_PTR dbin,
	 FF_BUFSIZE_PTR newform_log
	)
#endif
{
	int error = 0;
	int percent_done = 0;

	long bytes_remaining = 0;
	long total_bytes = 0L;

	BOOLEAN done_processing = FALSE;

	PROCESS_INFO_LIST pinfo_list = NULL;

#if defined(FF_MAIN) && defined(TIMER)
	long elapsed_time = 0;
	time_t finish_time = 0;
#endif

	FF_VALIDATE(dbin);
	
	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &pinfo_list);
	if (error)
		return(error);
	
	total_bytes = db_ask(dbin, DBASK_BYTES_TO_PROCESS, pinfo_list);

	while (!error && !done_processing)
	{
		error = db_do(dbin, DBDO_PROCESS_FORMATS, pinfo_list);
		if (error == EOF)
		{
			error = 0;
			done_processing = TRUE;
		}

		if (!error)
			error = db_do(dbin, DBDO_WRITE_FORMATS, pinfo_list);
	
		/* Calculate percentage left to process and display */
		bytes_remaining = db_ask(dbin, DBASK_BYTES_TO_PROCESS, pinfo_list);

		percent_done = (int)((1 - ((float)bytes_remaining / total_bytes)) * 100);
#ifdef FF_MAIN
		wfprintf(stderr,"\r%3d%% processed", percent_done);

#ifdef TIMER
		(void)time(&finish_time);
		elapsed_time = (long)difftime(finish_time, start_time);
		wfprintf(stderr, "     Elapsed time - %02d:%02d:%02d",
		        (int)(elapsed_time / (3600)),
		        (int)((elapsed_time / 60) % 60),
		        (int)(elapsed_time % 60)
		);
#endif /* TIMER */
#endif /* FF_MAIN */

#ifdef FF_MAIN
		if (!error && std_args->user.is_stdin_redirected)
			error = db_do(dbin, DBDO_READ_STDIN, std_args, &done_processing);

		if (!error && !done_processing && std_args->user.is_stdin_redirected)
		{
			FF_BUFSIZE_PTR bufsize = NULL;

			/* Display some information about the data, ignore errors */
			db_ask(dbin, DBASK_FORMAT_SUMMARY, &bufsize);
			wfprintf(stderr, "%s\n", bufsize->buffer);
			if (newform_log)
				send_to_log(newform_log, "%s\n", bufsize->buffer);
			ff_destroy_bufsize(bufsize);
		}

#endif
	}
	
	/* End Processing */
	
	bytes_remaining = db_ask(dbin, DBASK_BYTES_TO_PROCESS, pinfo_list);
	if (bytes_remaining)
		error = err_push(ERR_PROCESS_DATA + ERR_WARNING_ONLY, "%ld BYTES OF DATA NOT PROCESSED.", bytes_remaining);

#if defined(TIMER) && defined(FF_MAIN)
	if (newform_log)
	{
		send_to_log(newform_log, "%3d%% processed     Elapsed time - %02d:%02d:%02d\n",
		            percent_done,
		            (int)(elapsed_time / (3600)),
		            (int)((elapsed_time / 60) % 60),
		            (int)(elapsed_time % 60)
		           );
	}
#endif /* TIMER && FF_MAIN */

	ff_destroy_process_info_list(pinfo_list);

	return(error);
} /* newform */


/* FILENAME:  newform.c
 *
 * CONTAINS:  newform()
 */

#include <freeform.h>

#ifdef TIMER
#include <time.h>
#endif 

#ifdef FF_MAIN

static BOOLEAN is_separation(PROCESS_INFO_PTR finfo)
{
	FF_VALIDATE(finfo);

	if (PINFO_ARRAY_MAP(finfo)->subsep)
		return(TRUE);
	else
		return(FALSE);
}

static BOOLEAN is_offset(PROCESS_INFO_PTR finfo)
{
	FF_VALIDATE(finfo);

	if (PINFO_ARRAY_OFFSET(finfo) != 0)
		return(TRUE);
	else
		return(FALSE);
}

/* Check to see if any fseek's will be done on output.  This will happen if an array has
   separation or has an offset other than zero.  Basically, output formats with only a
	single array will not require any fseek's on output.

   Also check for any separate headers
*/
static int check_stdout_contiguity(DATA_BIN_PTR dbin)
{
	PROCESS_INFO_LIST finfo_list = NULL;
	PROCESS_INFO_PTR  finfo = NULL;
	int error = 0;

	FF_VALIDATE(dbin);

	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_OUTPUT, &finfo_list);
	if (!error)
	{
		finfo_list = dll_first(finfo_list);
		finfo      = FF_PI(finfo_list);
		while (finfo)
		{
			FF_VALIDATE(finfo);

			if (is_separation(finfo) || is_offset(finfo) || (IS_HEADER(PINFO_FORMAT(finfo)) && IS_SEPARATE(PINFO_FORMAT(finfo))))
			{
				error = err_push(ERR_GENERAL, "You must use -o with this output format: %s", PINFO_NAME(finfo));
				break;
			}

			finfo_list = dll_next(finfo_list);
			finfo      = FF_PI(finfo_list);
		}

		ff_destroy_process_info_list(finfo_list);
	}
	else
		error = err_push(ERR_GENERAL, "Nothing to redirect as no output formats have been specified");

	return(error);
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
	
static int setup_stdin(DATA_BIN_PTR dbin, FF_STD_ARGS_PTR std_args)
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

			std_args->input_buffer = (char *)memMalloc(bytes_to_alloc, "std_args->input_buffer");
			if (!std_args->input_buffer)
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
			bytes_read = fread(std_args->input_buffer, 1, bytes_to_read, stdin); 
			if (bytes_read != bytes_to_read)
				error = err_push(ERR_READ_FILE, "...from standard input");
			else
			{
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

								PINFO_BUFFER_SIZE(pinfo) = records_in_buffer * PINFO_RECL(pinfo);

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

/* messages for print_info */
#define INFO             1
#define GREET            2

/* FUNCTION: print_info()
 *
 * PURPOSE:     display appropriate message 
 *
 * COMMENTS:
 */
static void print_info(int message)
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
                  [-ep] passive error messages - non-interactive\n\n\
See the FreeForm User's Guide for detailed information.\n"
};

	switch(message)
	{
		case GREET:
			fprintf(stderr, "%s", greeting);
		break;
		
		case INFO:
			fprintf(stderr, "%s", command_line_usage);
		break;
	}/* end switch */
}/* end print_info() */

/* Changed return type from void to int. */
int main(int argc, char *argv[])
{
	FF_BUFSIZE_PTR bufsize = NULL;
  
	DATA_BIN_PTR dbin = NULL;
	int error = 0;
	FF_STD_ARGS_PTR std_args = NULL;

#ifdef TIMER	
	time_t start_time;
	(void)time(&start_time);
#endif

	print_info(GREET);
	
	std_args = ff_create_std_args();
	if (!std_args)
	{
		error = ERR_MEM_LACK;
		goto main_exit;
	}
	
	error = parse_command_line(argc, argv, std_args);
	if (error)
		goto main_exit;

	/* Check number of command args */
	if (argc == 1 && !std_args->user.is_stdin_redirected)
	{
		print_info(INFO);
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
		error = setup_stdin(dbin, std_args);
		if (error)
			goto main_exit;
	}

	if (!isatty(fileno(stdout)))
	{
#if FF_OS == FF_OS_DOS || FF_OS == FF_OS_MACOS
		setmode(fileno(stdout), O_BINARY);
#endif
		error = check_stdout_contiguity(dbin);
		if (error)
			goto main_exit;
	}

	if (std_args->query_file)
		wfprintf(stderr, "Using query file: %s\n", std_args->query_file);

	/* Display some information about the data, ignore errors */
	db_ask(dbin, DBASK_FORMAT_SUMMARY, &bufsize);
	wfprintf(stderr, "%s", bufsize->buffer);
	ff_destroy_bufsize(bufsize);

	wfprintf(stderr, "\n");

#ifdef TIMER
	error = newform(start_time, std_args, dbin);
#else
	error = newform(std_args, dbin);
#endif
	
	if (std_args->user.is_stdin_redirected)
		memFree(std_args->input_buffer, "std_args->input_buffer");

	wfprintf(stderr,"\n");

main_exit:

	if (dbin)
		db_destroy(dbin);
	
	if (error || err_state())
		err_disp(std_args);

	if (std_args)
		ff_destroy_std_args(std_args);
	
	memExit(error ? EXIT_FAILURE : EXIT_SUCCESS, "main");
}

static int check_standard_input
	(
	 FF_STD_ARGS_PTR std_args,
	 DATA_BIN_PTR dbin,
	 BOOLEAN *done_processing
	)
{
	int error = 0;

	FF_VALIDATE(std_args);
	FF_VALIDATE(dbin);

	if (std_args->user.is_stdin_redirected)
	{
		size_t bytes_read = 0;
		size_t bytes_to_read = 0;
		PROCESS_INFO_LIST plist = NULL;

		if (feof(stdin))
		{
			*done_processing = TRUE;
			return(0);
		}

		error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &plist);
		if (!error)
		{
			PROCESS_INFO_PTR pinfo = NULL;

			plist = dll_first(plist);
			pinfo = FF_PI(plist);
			while (pinfo)
			{
				size_t records_to_read = 0;

				records_to_read = PINFO_BUFFER_SIZE(pinfo) / PINFO_RECL(pinfo);
				bytes_to_read = records_to_read * PINFO_RECL(pinfo);

				bytes_read = fread(std_args->input_buffer, 1, bytes_to_read, stdin); 
				if (bytes_read != bytes_to_read)
				{
					if (!feof(stdin))
						error = err_push(ERR_READ_FILE, "...from standard input");
				}
				else
					*done_processing = FALSE;

				if (!error)
				{
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

#endif /* FF_MAIN */

static long bytes_to_process(PROCESS_INFO_LIST finfo_list)
{
	long bytes_to_process = 0;
	PROCESS_INFO_PTR finfo = NULL;

	finfo_list = dll_first(finfo_list);
	finfo      = FF_PI(finfo_list);
	while (finfo)
	{
		if (PINFO_MATE(finfo))
			bytes_to_process += PINFO_MATE_BYTES_LEFT(finfo);

		finfo_list = dll_next(finfo_list);
		finfo      = FF_PI(finfo_list);
	}

	return(bytes_to_process);
}

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
int newform(time_t start_time, FF_STD_ARGS_PTR std_args, DATA_BIN_PTR dbin)
#else
int newform(FF_STD_ARGS_PTR std_args, DATA_BIN_PTR dbin)
#endif
#else
int newform(DATA_BIN_PTR dbin)
#endif
{
	int error = 0;
	int percent_done = 0;

	long bytes_remaining = 0;
	long total_bytes = 0L;

	BOOLEAN done_processing = FALSE;

	PROCESS_INFO_LIST finfo_list = NULL;

#if defined(FF_MAIN) && defined(TIMER)
	long elapsed_time;
	time_t finish_time;
#endif

	FF_VALIDATE(dbin);
	
	error = db_ask(dbin, DBASK_PROCESS_INFO, FFF_INPUT, &finfo_list);
	if (error)
		return(error);
	
	total_bytes = bytes_to_process(finfo_list);

	while (!error && !done_processing)
	{
		error = db_do(dbin, DBDO_PROCESS_FORMATS, finfo_list);
		if (error == EOF)
		{
			error = 0;
			done_processing = TRUE;
		}

		if (!error)
			error = db_do(dbin, DBDO_WRITE_FORMATS, finfo_list);
	
		/* Calculate percentage left to process and display */
		bytes_remaining = bytes_to_process(finfo_list);

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
		if (!error)
			error = check_standard_input(std_args, dbin, &done_processing);
#endif
	}
	
	/* End Processing */
	
	bytes_remaining = bytes_to_process(finfo_list);
	if (bytes_remaining)
		error = err_push(ERR_PROCESS_DATA + ERR_WARNING_ONLY, "%ld BYTES OF DATA NOT PROCESSED.", bytes_remaining);

	ff_destroy_process_info_list(finfo_list);

	return(error);
} /* newform */


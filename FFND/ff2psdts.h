
#ifndef FF2PSDTS_H__
#define FF2PSDTS_H__

#define MODULE_COUNT 20

typedef struct fips_struct_module_stats_struct_t {
    char mnrf[5];		/* name of module */
    char mntf[27];		/* type of module */
    FILE *fp;			/* FILE pointer for modules being appended to */
    int nrec;			/* number of records in module */
    int nsad;			/* number of spatial addresses in module */
} FIPS_MODULE_STATS, *FIPS_MODULE_STATS_PTR;

#define fips_MAX_FILE_PATH 256

typedef struct attr_info_ptr_struct_t {
    char *name;
    enum {
	ATYP_ALPHABET = 1, ATYP_INTEGER = 2, ATYP_ALPHANUM = 3, ATYP_REAL = 4, ATYP_GRCHARS = 5
    } atyp;
} FIPS_ATTR_INFO, *FIPS_ATTR_INFO_PTR;

typedef struct fips_bucket_struct_t {
    char *transfer_name;
    FIPS_ATTR_INFO_PTR attr_info;
    FIPS_MODULE_STATS module_stats[MODULE_COUNT];
} FIPS_BUCKET, *FIPS_BUCKET_PTR;

#endif

/*
 * HISTORY:
 *	r fozzard	7/28/95		-rf03
 *		CodeWarrior doesn't need unix.h
 *	r fozzard	8/3/95		-rf04
 *		Redefine FF_TYPES_t to unsigned int to solve var_args stack problem; apparently
 *		on the Mac, (and maybe Unix), a 4-byte value is pushed, even when you request only
 *		2 bytes.
 *		Also, a fix to the TRUE/FALSE definitions to properly define them if not defined.
*/

#ifndef FREEFORM_H__
#define FREEFORM_H__

#ifndef FREEFORM
#error "You must define FREEFORM as a preprocessor name."
#error "You must do this for the benefit of files in the FreeForm project"
#error "which do not include freeform.h"
#endif

/*

FreeForm ND has been compiled on the following platforms:

	PC running DOS
	PC running LINUX
	Macintosh running MacOS
	Sun Sparcstations running SUNOS
	SGI workstations running IRIX
	and various other Unix workstations, like:
		IBM6000, HP9000, DEC-Alpha, and SGI PowerChallenge

FreeForm ND compiles with these C compilers:

	Microsoft Visual C++ Versions 1.0 and 1.52
	Microsoft Visual C++ Version 4.0 (define MSVC4)
	Metroworks CodeWarrior Version 6 -- for Macintosh
	generic ANSI Unix C compilers

If you are using Microsoft Visual C++ 4.0 then define MSVC4 in addition to
defining PC.

To compile FreeForm on your machine you must define one of the
following preprocessor names:

   
	PC
	LINUX
	SUN
	IRIS4
	MAC
	IBM6000
	HP9000
	DEC_ALPHA
	IRIX

One of the above should be defined as a compiler preprocessor definition, such as on the
cc command line, or within your project settings.
   
*/

#define FF_OS_UNIX 1
#define FF_OS_MAC  2
#define FF_OS_DOS  3

#define FF_CC_MSVC1 1
#define FF_CC_MSVC4 2
#define FF_CC_MACCW 3
#define FF_CC_UNIX  4

#ifdef GOT_MACHINE
#undef GOT_MACHINE
#endif

#ifdef PC

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_32
#define LONGS_ARE_32
#endif

#define FF_OS FF_OS_DOS

#ifdef MSVC1
#define FF_CC FF_CC_MSVC1
#endif

#ifdef MSVC4
#ifdef FF_CC
#error "A PC compiler has already been defined"
#endif
#define FF_CC FF_CC_MSVC4
#endif

#ifndef FF_CC
/* Assume MSVC1 unless MSVC4 is defined */
#define FF_CC FF_CC_MSVC1
#endif

#endif /* PC */

#ifdef LINUX

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_32
#define LONGS_ARE_32
#endif

#define FF_OS FF_OS_UNIX
#define FF_CC FF_CC_UNIX

#endif /* LINUX */

#ifdef SUN

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_32
#define LONGS_ARE_32
#endif

#define FF_OS FF_OS_UNIX
#define FF_CC FF_CC_UNIX

#endif /* SUN */

#ifdef IRIS4

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_32
#define LONGS_ARE_32
#endif

#define FF_OS FF_OS_UNIX
#define FF_CC FF_CC_UNIX

#endif /* IRIS4 */

#ifdef MAC

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_32
#define LONGS_ARE_32
#endif

#define FF_OS FF_OS_MAC
#define FF_CC FF_CC_MACCW

#endif /* MAC */

#ifdef IBM6000

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_32
#define LONGS_ARE_32
#endif

#define FF_OS FF_OS_UNIX
#define FF_CC FF_CC_UNIX

#endif /* IBM6000 */

#ifdef HP9000

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_32
#define LONGS_ARE_32
#endif

#define FF_OS FF_OS_UNIX
#define FF_CC FF_CC_UNIX

#endif /* HP9000 */

#ifdef DEC_ALPHA

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_64
#define LONGS_ARE_64
#endif

#define FF_OS FF_OS_UNIX
#define FF_CC FF_CC_UNIX

#endif /* DEC_ALPHA */

#ifdef IRIX

#ifndef IRIX
#define IRIX
#endif

#ifdef GOT_MACHINE
#define TOO_MANY_MACHINES
#else
#define GOT_MACHINE
#endif

#ifndef LONGS_ARE_64
#define LONGS_ARE_64
#endif

#define FF_OS FF_OS_UNIX
#define FF_CC FF_CC_UNIX

#endif /* IRIX */

#ifndef GOT_MACHINE
#error "Machine type has not been defined."
#error "Please define one in your makefile or project."
#endif

#ifdef TOO_MANY_MACHINES
#error "More than one machine type has been defined"
#ifdef PC
#error "PC is currently defined"
#endif
#ifdef SUN
#error "SUN is currently defined"
#endif
#ifdef IRIS4
#error "IRIS4 is currently defined"
#endif
#ifdef MAC
#error "MAC is currently defined"
#endif
#ifdef IBM6000
#error "IBM6000 is currently defined"
#endif
#ifdef HP9000
#error "HP9000 is currently defined"
#endif
#ifdef DEC_ALPHA
#error "DEC_ALPHA is currently defined"
#endif
#ifdef IRIX
#error "IRIX is currently defined"
#endif
#error "Please check your makefile or project."
#endif /* TOO_MANY_MACHINES */

#if !defined(LONGS_ARE_32) && !defined(LONGS_ARE_64)
#error "longs have not been defined as either 32 or 64 bits"
#error "This should never happen, contact support"
#endif

#ifndef FF_CC
#error "A C compiler has not been defined"
You should define one of the following:

	PC
	LINUX
	SUN
	IRIS4
	MAC
	IBM6000
	HP9000
	DEC_ALPHA
	IRIX

#endif

#ifndef FF_OS
#error "An operating system has not been defined"
You should define one of the following:

	PC
	LINUX
	SUN
	IRIS4
	MAC
	IBM6000
	HP9000
	DEC_ALPHA
	IRIX

#endif

/* Directory Separators: specifically for the three currently supported
   general operating systems, for path names in menu files (same as DOS) -- 
   NATIVE_DIR_SEPARATOR is set to one of the three os-specific separators
   at compile-time.
*/

#define UNIX_DIR_SEPARATOR '/'
#define UNIX_DIR_SEPARATOR_STRING "/"

#define MAC_DIR_SEPARATOR ':'
#define MAC_DIR_SEPARATOR_STRING ":"

#define DOS_DIR_SEPARATOR '\\'
#define DOS_DIR_SEPARATOR_STRING "\\"

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif
		  
#if FF_OS == FF_OS_MACOS

#define NATIVE_DIR_SEPARATOR MAC_DIR_SEPARATOR
#define NATIVE_DIR_SEPARATOR_STRING MAC_DIR_SEPARATOR_STRING

#endif

#if FF_OS == FF_OS_DOS

#define NATIVE_DIR_SEPARATOR DOS_DIR_SEPARATOR
#define NATIVE_DIR_SEPARATOR_STRING DOS_DIR_SEPARATOR_STRING

#endif

#if FF_OS == FF_OS_UNIX

#define NATIVE_DIR_SEPARATOR UNIX_DIR_SEPARATOR
#define NATIVE_DIR_SEPARATOR_STRING UNIX_DIR_SEPARATOR_STRING

#endif

#define FFNT_INT8     0
#define FFNT_UINT8    1
#define FFNT_INT16    2
#define FFNT_UINT16   3
#define FFNT_INT32    4
#define FFNT_UINT32   5
#define FFNT_INT64    6
#define FFNT_UINT64   7
#define FFNT_FLOAT32  8
#define FFNT_FLOAT64  9
#define FFNT_ENOTE   10

#ifdef WANT_NCSA_TYPES

typedef signed char         int8;
typedef unsigned char       uint8;
typedef signed short int    int16;
typedef unsigned short int  uint16;
typedef float               float32;
typedef double              float64;

#endif

#define SIZE_INT8       1
#define SIZE_UINT8      1
#define SIZE_INT16      2
#define SIZE_UINT16     2
#define SIZE_INT32      4
#define SIZE_UINT32     4
#define SIZE_INT64      8
#define SIZE_UINT64     8
#define SIZE_INT128    16   /* No current plans for support */
#define SIZE_UINT128   16   /* No current plans for support */

#define SIZE_FLOAT32    4
#define SIZE_FLOAT64    8
#define SIZE_ENOTE      SIZE_FLOAT64
#define SIZE_FLOAT128  16    /* No current plans for support */

typedef double big_var_type;
typedef big_var_type align_var_type;

#define FFV_INT8_MIN    SCHAR_MIN
#define FFV_INT8_MAX    SCHAR_MAX
#define FFV_UINT8_MIN           0
#define FFV_UINT8_MAX   UCHAR_MAX
#define FFV_INT16_MIN    SHRT_MIN
#define FFV_INT16_MAX    SHRT_MAX
#define FFV_UINT16_MIN          0
#define FFV_UINT16_MAX  USHRT_MAX
#define FFV_FLOAT32_MIN  -FLT_MAX
#define FFV_FLOAT32_MAX   FLT_MAX
#define FFV_FLOAT64_MIN  -DBL_MAX
#define FFV_FLOAT64_MAX   DBL_MAX
#define FFV_ENOTE_MIN     FFV_FLOAT64_MIN
#define FFV_ENOTE_MAX     FFV_FLOAT64_MAX

#define FFV_FLOAT32_EPSILON FLT_EPSILON
#define FFV_FLOAT64_EPSILON DBL_EPSILON
#define FFV_ENOTE_EPSILON   FFV_FLOAT64_EPSILON

extern const char *fft_cnv_flags[FFNT_ENOTE + 1];
extern const char *fft_cnv_flags_width[FFNT_ENOTE + 1];
extern const char *fft_cnv_flags_prec[FFNT_ENOTE + 1];
extern const char *fft_cnv_flags_width_prec[FFNT_ENOTE + 1];

#ifdef LONGS_ARE_32

#ifdef LONGS_ARE_64
#error "longs have been defined as both 32 and 64 bits"
#error "This should never happen, contact support"
#endif

#ifdef WANT_NCSA_TYPES

typedef signed long int     int32;
typedef unsigned long int   uint32;
typedef char                int64;  /* not a real type for the PC */
typedef char                uint64; /* not a real type for the PC */

#endif

#define FFV_INT32_MIN    LONG_MIN
#define FFV_INT32_MAX    LONG_MAX
#define FFV_UINT32_MIN          0
#define FFV_UINT32_MAX  ULONG_MAX
#define FFV_INT64_MIN           0
#define FFV_INT64_MAX           0
#define FFV_UINT64_MIN          0
#define FFV_UINT64_MAX          0

#endif /* LONGS_ARE_32 */

#ifdef LONGS_ARE_64

#ifdef LONGS_ARE_32
#error "longs have been defined as both 32 and 64 bits"
#error "This should never happen, contact support"
#endif

#ifdef WANT_NCSA_TYPES

typedef signed int          int32;
typedef unsigned int        uint32;
typedef signed long int     int64;
typedef unsigned long int   uint64;

#endif

#define FFV_INT32_MIN     INT_MIN
#define FFV_INT32_MAX     INT_MAX
#define FFV_UINT32_MIN          0
#define FFV_UINT32_MAX   UINT_MAX
#define FFV_INT64_MIN    LONG_MIN
#define FFV_INT64_MAX    LONG_MAX
#define FFV_UINT64_MIN          0
#define FFV_UINT64_MAX  ULONG_MAX

#endif /* LONGS_ARE_64 */

typedef double             ff_enote;

/* Variable Types */
/* FFV_NULL MUST BE zero -- NOTHING else can be zero */
#define FFV_NULL            (FF_TYPES_t)0x00000000

#define FFV_SIZE_1          (FF_TYPES_t)0x00000001
#define FFV_SIZE_2          (FF_TYPES_t)0x00000002
#define FFV_UNSIGNED        (FF_TYPES_t)0x00000004
#define FFV_INTEGER         (FF_TYPES_t)0x00000008
#define FFV_REAL            (FF_TYPES_t)0x00000010
#define FFV_TEXT            (FF_TYPES_t)0x00000020
/* FFV_CHAR is used for strings, but I think FFV_TEXT is a better name.
*/
#define FFV_CHAR     FFV_TEXT

/* above are masked by FFV_DATA_TYPES */
#define FFV_DATA_TYPES      (FF_TYPES_t)0x0000003F

#define FFV_BIT_FIELD       (FF_TYPES_t)0x00000040
#define FFV_CONVERT         (FF_TYPES_t)0x00000080
#define FFV_CONSTANT        (FF_TYPES_t)0x00000100 /* if you change this bit pattern, change FFV_EOL too! */
#define FFV_INITIAL         (FF_TYPES_t)0x00000200
#define FFV_EQUATION        (FF_TYPES_t)0x00000400  
#define FFV_EQUIV           (FF_TYPES_t)0x00000800
#define FFV_TRANSLATOR      (FF_TYPES_t)0x00001000
#define FFV_ORPHAN          (FF_TYPES_t)0x00002000
#define FFV_EOL             (FF_TYPES_t)0x00004100 /* |= FFV_CONSTANT */
#define FFV_EQN             (FF_TYPES_t)0x00008000

#define FFV_RECORD          (FF_TYPES_t)0x00010000
#define DONT_USE_THIS_BIT_PATTERN 0x40000000 /* This is used by FF_ARRAY for both formats and variables */

#define FFV_INT8      (               FFV_INTEGER)
#define FFV_UINT8     (FFV_UNSIGNED | FFV_INTEGER)
#define FFV_INT16     (               FFV_INTEGER | FFV_SIZE_1)
#define FFV_UINT16    (FFV_UNSIGNED | FFV_INTEGER | FFV_SIZE_1)
#define FFV_INT32     (               FFV_INTEGER | FFV_SIZE_2)
#define FFV_UINT32    (FFV_UNSIGNED | FFV_INTEGER | FFV_SIZE_2)
#define FFV_INT64     (               FFV_INTEGER | FFV_SIZE_1 | FFV_SIZE_2)
#define FFV_UINT64    (FFV_UNSIGNED | FFV_INTEGER | FFV_SIZE_1 | FFV_SIZE_2)
#define FFV_FLOAT32   (               FFV_REAL    | FFV_SIZE_2)
#define FFV_FLOAT64   (               FFV_REAL    | FFV_SIZE_1 | FFV_SIZE_2)
#define FFV_ENOTE     (               FFV_REAL)

#define FFV_UCHAR    FFV_UINT8
#define FFV_SHORT    FFV_INT16
#define FFV_USHORT   FFV_UINT16

#ifdef LONGS_ARE_32
#define FFV_LONG     FFV_INT32
#define FFV_ULONG    FFV_UINT32
#endif

#ifdef LONGS_ARE_64
#define FFV_LONG     FFV_INT64
#define FFV_ULONG    FFV_UINT64
#endif

#define FFV_FLOAT    FFV_FLOAT32
#define FFV_DOUBLE   FFV_FLOAT64

#define IS_TEXT_TYPE(t)    (FFV_DATA_TYPE_TYPE(t) == FFV_TEXT)
#define IS_INT8_TYPE(t)    (FFV_DATA_TYPE_TYPE(t) == FFV_INT8)
#define IS_UINT8_TYPE(t)   (FFV_DATA_TYPE_TYPE(t) == FFV_UINT8)
#define IS_INT16_TYPE(t)   (FFV_DATA_TYPE_TYPE(t) == FFV_INT16)
#define IS_UINT16_TYPE(t)  (FFV_DATA_TYPE_TYPE(t) == FFV_UINT16)
#define IS_INT32_TYPE(t)   (FFV_DATA_TYPE_TYPE(t) == FFV_INT32)
#define IS_UINT32_TYPE(t)  (FFV_DATA_TYPE_TYPE(t) == FFV_UINT32)
#define IS_INT64_TYPE(t)   (FFV_DATA_TYPE_TYPE(t) == FFV_INT64)
#define IS_UINT64_TYPE(t)  (FFV_DATA_TYPE_TYPE(t) == FFV_UINT64)
#define IS_FLOAT32_TYPE(t) (FFV_DATA_TYPE_TYPE(t) == FFV_FLOAT32)
#define IS_FLOAT64_TYPE(t) (FFV_DATA_TYPE_TYPE(t) == FFV_FLOAT64)
#define IS_ENOTE_TYPE(t)   (FFV_DATA_TYPE_TYPE(t) == FFV_ENOTE)
#define IS_EQN_TYPE(t)     (t & FFV_EQN)

#define IS_RECORD_VAR_TYPE(t) (t == FFV_RECORD)

#define IS_TEXT(v)    IS_TEXT_TYPE(FFV_DATA_TYPE(v))
#define IS_INT8(v)    IS_INT8_TYPE(FFV_DATA_TYPE(v))
#define IS_UINT8(v)   IS_UINT8_TYPE((FFV_DATA_TYPE(v))
#define IS_INT16(v)   IS_INT16_TYPE(FFV_DATA_TYPE(v))
#define IS_UINT16(v)  IS_UINT16_TYPE(FFV_DATA_TYPE(v))
#define IS_INT32(v)   IS_INT32_TYPE(FFV_DATA_TYPE(v))
#define IS_UINT32(v)  IS_UINT32_TYPE(FFV_DATA_TYPE(v))
#define IS_INT64(v)   IS_INT64_TYPE(FFV_DATA_TYPE(v))
#define IS_UINT64(v)  IS_UINT64_TYPE(FFV_DATA_TYPE(v))
#define IS_FLOAT32(v) IS_FLOAT32_TYPE(FFV_DATA_TYPE(v))
#define IS_FLOAT64(v) IS_FLOAT64_TYPE(FFV_DATA_TYPE(v))
#define IS_ENOTE(v)   IS_ENOTE_TYPE(FFV_DATA_TYPE(v))
#define IS_EQN(v)     IS_EQN_TYPE(FFV_TYPE(v))

#define IS_RECORD_VAR(v)  IS_RECORD_VAR_TYPE(FFV_TYPE(v)) 

#define IS_INTEGER(v) (FFV_DATA_TYPE(v) & FFV_INTEGER)
#define IS_REAL(v)    (FFV_DATA_TYPE(v) & FFV_REAL)

#define IS_INTEGER_TYPE(t) (FFV_DATA_TYPE_TYPE(t) & FFV_INTEGER)
#define IS_REAL_TYPE(t)    (FFV_DATA_TYPE_TYPE(t) & FFV_REAL)

#define IS_BIT_FIELD_VAR(v)  ((v) ? (((v)->type & FFV_BIT_FIELD) == FFV_BIT_FIELD) : FALSE)
#define IS_CONVERT_VAR(v)    ((v) ? (((v)->type & FFV_CONVERT) == FFV_CONVERT) : FALSE)
#define IS_CONSTANT_VAR(v)   ((v) ? (((v)->type & FFV_CONSTANT) == FFV_CONSTANT) : FALSE)
#define IS_INITIAL_VAR(v)    ((v) ? (((v)->type & FFV_INITIAL) == FFV_INITIAL) : FALSE)
#define IS_EQUATION_VAR(v)   ((v) ? ((v)->type & FFV_EQUATION) == FFV_EQUATION : FALSE)
#define IS_EQUIV_VAR(v)      ((v) ? ((v)->type & FFV_EQUIV) == FFV_EQUIV : FALSE)
#define IS_TRANSLATOR_VAR(v) ((v) ? ((v)->type & FFV_TRANSLATOR) == FFV_TRANSLATOR : FALSE)
#define IS_ORPHAN_VAR(v)     ((v) ? ((v)->type & FFV_ORPHAN) == FFV_ORPHAN : FALSE)
#define IS_EOL_VAR(v)        ((v) ? ((v)->type & FFV_EOL) == FFV_EOL : FALSE)
#define IS_FLAG_VAR(v)       ((v) ? (v)->type == FF_VAR_TYPE_FLAG : FALSE)

typedef unsigned long FF_TYPES_t;
#define FF_VAR_TYPE_FLAG (FF_TYPES_t)0xffffffff

typedef struct
{
	char       *string;
	FF_TYPES_t  number;
} FFF_LOOKUP, *FFF_LOOKUP_PTR;

#define NUM_VARIABLE_TYPES 25
extern FFF_LOOKUP variable_types[NUM_VARIABLE_TYPES];

/* FreeForm Format bit field masks */
#define FFF_FILE_TYPES (FFF_BINARY | FFF_ASCII | FFF_DBASE)
#define FFF_DATA_TYPES (FFF_DATA | FFF_HEADER | FFF_FILE | FFF_REC)         
#define FFF_GROUP      (FFF_FILE_TYPES | FFF_DATA_TYPES)
/* FD_TYPES == FFF_TABLE | FFF_DATA | FFF_HEADER | FFF_INPUT | FFF_OUTPUT */

/* FreeForm Format bit field constants */
/* FFF_NULL MUST BE zero -- NOTHING else can be zero */
#define FFF_NULL                (FF_TYPES_t)0x00000000
#define FFF_BINARY              (FF_TYPES_t)0x00000001
#define FFF_ASCII               (FF_TYPES_t)0x00000002
#define FFF_DBASE               (FF_TYPES_t)0x00000004

#define FFF_TABLE               (FF_TYPES_t)0x00000008		
#define FFF_DATA                (FF_TYPES_t)0x00000010
#define FFF_HEADER              (FF_TYPES_t)0x00000020
#define FFF_FILE                (FF_TYPES_t)0x00000040
#define FFF_REC                 (FF_TYPES_t)0x00000080

#define FFF_SEPARATE            (FF_TYPES_t)0x00000100
#define FFF_VARIED              (FF_TYPES_t)0x00000200
#define FFF_INPUT               (FF_TYPES_t)0x00000400
#define FFF_OUTPUT              (FF_TYPES_t)0x00000800
#define FFF_IO                  (FF_TYPES_t)0x00000C00

#define FFF_SCALE               (FF_TYPES_t)0x00001000
#define FFF_ADD_HERE            (FF_TYPES_t)0x00002000
#define FFF_DELETE_ME           (FF_TYPES_t)0X00008000

#define FF_ARRAY                (FF_TYPES_t)0x40000000
#define FFF_RECORD              (FF_TYPES_t)0x80000000

/* IS_ARRAY used for either variables or formats */
#define IS_ARRAY_TYPE(t) ((t) & FF_ARRAY)
#define IS_ARRAY(x) IS_ARRAY_TYPE(FFF_TYPE(x))

#define FFF_ALL_TYPES           (FF_TYPES_t)0x0000FFFF
/* above masked by FFF_ALL_TYPES */

#define IS_BINARY_TYPE(t)   ((t) & FFF_BINARY)
#define IS_ASCII_TYPE(t)    ((t) & FFF_ASCII)
#define IS_DBASE_TYPE(t)    ((t) & FFF_DBASE)

#define IS_TABLE_TYPE(t)    ((t) & FFF_TABLE)
#define IS_DATA_TYPE(t)     ((t) & FFF_DATA)
#define IS_HEADER_TYPE(t)   ((t) & FFF_HEADER)
#define IS_FILE_TYPE(t)     ((t) & FFF_FILE)
#define IS_REC_TYPE(t)      ((t) & FFF_REC)
#define IS_SEPARATE_TYPE(t) ((t) & FFF_SEPARATE)
#define IS_EMBEDDED_TYPE(t) (!IS_SEPARATE_TYPE(t))
#define IS_VARIED_TYPE(t)   ((t) & FFF_VARIED)
#define IS_INPUT_TYPE(t)    ((t) & FFF_INPUT)
#define IS_OUTPUT_TYPE(t)   ((t) & FFF_OUTPUT)
#define IS_SCALE_TYPE(t)    ((t) & FFF_SCALE)

#define IS_RECORD_FORMAT_TYPE(t)   ((t) & FFF_RECORD)

#define IS_BINARY(f)   IS_BINARY_TYPE(FFF_TYPE(f))
#define IS_ASCII(f)    IS_ASCII_TYPE(FFF_TYPE(f))
#define IS_DBASE(f)    IS_DBASE_TYPE(FFF_TYPE(f))

#define IS_TABLE(f)    IS_TABLE_TYPE(FFF_TYPE(f))
#define IS_DATA(f)     IS_DATA_TYPE(FFF_TYPE(f))
#define IS_HEADER(f)   IS_HEADER_TYPE(FFF_TYPE(f))
#define IS_FILE(f)     IS_FILE_TYPE(FFF_TYPE(f))
#define IS_REC(f)      IS_REC_TYPE(FFF_TYPE(f))
#define IS_SEPARATE(f) IS_SEPARATE_TYPE(FFF_TYPE(f))
#define IS_EMBEDDED(f) (!IS_SEPARATE(f))
#define IS_VARIED(f)   IS_VARIED_TYPE(FFF_TYPE(f))
#define IS_INPUT(f)    IS_INPUT_TYPE(FFF_TYPE(f))
#define IS_OUTPUT(f)   IS_OUTPUT_TYPE(FFF_TYPE(f))
#define IS_SCALE(f)    IS_SCALE_TYPE(FFF_TYPE(f))

#define IS_RECORD_FORMAT(f)   IS_RECORD_FORMAT_TYPE(FFF_TYPE(f))

#define NUM_FORMAT_TYPES 51
extern FFF_LOOKUP format_types[NUM_FORMAT_TYPES];

#define FFND_LIB_VER "4.0"
#define FF_DBG_LOG "ff_debug.log"
#define FF_DBG_LOG_SIZE 10240

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#if defined(DEBUG) || defined(_DEBUG) && !defined(FF_DBG)
#define FF_DBG
#endif

#define assert(exp) ff_err_assert(exp)

#ifdef NDEBUG
#define ff_err_assert(exp) ((void)0)
#else /* NDEBUG */
#define ff_err_assert(exp) ( (exp) ? (void)0 : _ff_err_assert(#exp, __FILE__, __LINE__) )
void _ff_err_assert(char *, char *, unsigned);
#endif /* (ELSE) NDEBUG */

/* FF_CHK_ADDR is independent of NDEBUG */
#ifdef FF_CHK_ADDR
#define FF_VALIDATE(o) (((o) && ((void *)(o) == (o)->check_address)) ? (void)0 : _ff_err_assert(#o, __FILE__, __LINE__))
#else
#define FF_VALIDATE(o) 0
#endif

#include <errno.h>

/* Following are to be used when converting a floating point type to an
   integer type -- this should avoid the occasional truncation error */

#include <float.h>
#include <math.h>
#include <fcntl.h>

#define DOUBLE_UP DBL_EPSILON
#define FLOAT_UP FLT_EPSILON
#define STR_END '\0' /* NULL-terminator for the C-string data type */

#if FF_CC == FF_CC_UNIX

#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define O_BINARY 0

#endif /* #if FF_CC == FF_CC_UNIX */

#if FF_CC == FF_CC_MSVC1 || FF_CC == FF_CC_MSVC4

#include <malloc.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>

#endif /* FF_CC == FF_CC_MSVC1 || FF_CC == FF_CC_MSVC4 */

#include <dl_lists.h>
#include <os_utils.h>
#include <memtrack.h>

#include <ndarray.h>

#ifdef HUGE
#undef HUGE
#endif /* HUGE */

#if FF_CC == FF_CC_MSVC1
#define HUGE huge
#else
#define HUGE 
#endif /* (ELSE) FF_CC == FF_CC_MSVC1 */

typedef char HUGE *FF_DATA_BUFFER;
typedef void HUGE *FF_DATA_PTR;

typedef char **FF_STRING_HANDLE;

/* Utility macros */
#define FD_FORMAT(fd)           ((fd)->format)
#define FFF_FIRST(fl)           dll_first(fl)
#define FFF_NEXT(fl)            dll_next(fl)
#define FD_FIRST(fdl)           dll_first(fdl)
#define FD_LAST(fdl)            dll_last(fdl)
#define FFV_FIRST_VARIABLE(f)   ((VARIABLE_LIST)dll_first((f)->variables))
#define FFV_NEXT_VARIABLE(vl)   dll_next(vl)
#define FORMAT_LENGTH(f)        ((size_t)(f)->length)
#define FF_VAR_LENGTH(v)        ((size_t)(v)->end_pos - (size_t)(v)->start_pos + (size_t)1)

#define FFV_TYPE(v) ((v)->type)
#define FFV_DATA_TYPE_TYPE(t) (t & FFV_DATA_TYPES)
#define FFV_DATA_TYPE(v) FFV_DATA_TYPE_TYPE(FFV_TYPE(v))
#define FFF_TYPE(f) ((f)->type)
#define FFF_FILE_TYPE(f) (FFF_TYPE(f) & FFF_FILE_TYPES)
#define FD_TYPE(fd) (FFF_TYPE((fd)->format) & FD_TYPES)

#define MAX_PV_LENGTH _MAX_PATH /* Maximum parameter or parameter-value length */
#define MAX_NAME_LENGTH MAX_PV_LENGTH
#define TMP_BUFFER_SIZE 1024
#define SCRATCH_QUANTA 1024U

/*
 * FF_STD_ARGS
 */

typedef struct struct_ff_std_args FF_STD_ARGS, *FF_STD_ARGS_PTR, **FF_STD_ARGS_HANDLE;
struct struct_ff_std_args
{
#ifdef FF_CHK_ADDR
	void *check_address;
#endif
	char           *input_file;           /* string: name of input file */
	char           *input_buffer;         /* buffer containing input data */
	char           *input_format_file;    /* string: name of input format file */
	char           *input_format_title;   /* string: input format title */
	char           *input_format_buffer;  /* string: input format description */
	char           *output_file;          /* string: name of output file */
	char           *output_buffer;        /* buffer to contain output data */
	char           *output_format_file;   /* string: name of output format file */ 
	char           *output_format_title;  /* string: output forma title */
	char           *output_format_buffer; /* string: output format description */
	char           *var_file;             /* string: name of variable file */
	char           *query_file;           /* string: name of query file */
	unsigned long   cache_size;           /* integer: default data cacheing size */
	long            records_to_read;      /* integer: if negative, file tail */

	char           *error_log;            /* string: file name to send error messages */
	BOOLEAN         error_prompt;
	
	/* Checkvar specific option flags */
	int     cv_precision;
	double  cv_missing_data;
	int     cv_maxbins;
	BOOLEAN cv_maxmin_only;
	
	struct struct_std_args_user
	{
		unsigned int set_cv_precision : 1;
		unsigned int set_cv_missing_data : 1;
		unsigned int is_stdin_redirected : 1;
	} user;
};

#include <err.h>

/* ff_copy_variable depends on the contents of the VARIABLE structure, below.
   If you change this structure, you must also change ff_copy_variable
*/


typedef struct struct_ff_dll_node DLL_NODE, *DLL_NODE_PTR, **DLL_NODE_HANDLE;

typedef DLL_NODE_PTR  FORMAT_LIST,
                                    VARIABLE_LIST,
                                    FORMAT_DATA_LIST,
                                    PROCESS_INFO_LIST,
                                    FORMAT_DATA_MAP_LIST,
                                    FF_ARRAY_CONDUIT_LIST,
						                  FF_ERROR_LIST;

typedef DLL_NODE_HANDLE  FORMAT_LIST_HANDLE,
                                     VARIABLE_LIST_HANDLE,
                                     FORMAT_DATA_LIST_HANDLE,
                                     PROCESS_INFO_LIST_HANDLE,
                                     FORMAT_DATA_MAP_LIST_HANDLE,
                                     FF_ARRAY_CONDUIT_LIST_HANDLE,
						                   FF_ERROR_LIST_HANDLE;

typedef struct struct_ff_error FF_ERROR, *FF_ERROR_PTR, **FF_ERROR_HANDLE;
struct struct_ff_error
{
#ifdef FF_CHK_ADDR
	void *check_address;
#endif

	int code;

	char *message;
	char *problem;

	int warning_num;
	int error_num;
};

#define FF_MAX_NAME _MAX_PATH

#define FF_ARRAY_ELEMENT_SIZE_WIDTH 5

#define FFV_MISC_INIT NULL

typedef unsigned long  FF_NDX_t;

/* ff_copy_format depends on the contents of the FORMAT structure, below.
   If you change this structure, you must also change ff_copy_format
*/

/*
 * FORMAT
 */

typedef struct struct_ff_format FORMAT, *FORMAT_PTR, **FORMAT_HANDLE;
struct struct_ff_format
{
#ifdef FF_CHK_ADDR
	void          *check_address;     /* assert(format->check_address == format); */
#endif
	VARIABLE_LIST  variables;         /* the DLL of variable structures */
	char           *name; /* title if given in a format file or menu section */
	char           *locus;  /* path-name or menu section title of format */
	FF_TYPES_t     type;              /* format type, see FFF_LOOKUP format_types */
	unsigned int   num_vars;       /* number of variables in format */
	FF_NDX_t       length;        /* record length in bytes */
};

#define FORMAT_NAME_INIT "Format Was Untitled"
#define FORMAT_LOCUS_INIT "Unknown Origin"
#define FORMAT_LOCUS_GV_EDIT "GeoVu Format Editor"

/* ff_copy_variable depends on the contents of the VARIABLE structure, below.
   If you change this structure, you must also change ff_copy_variable
*/

/* I don't know if I should copy the EQUATION_INFO or not... */

/*
 * VARIABLE
 */

#include <eval_eqn.h>

typedef struct struct_ff_variable VARIABLE, *VARIABLE_PTR, **VARIABLE_HANDLE;
struct struct_ff_variable
{
#ifdef FF_CHK_ADDR
	void       *check_address;      /* assert(variable->check_address == variable); */
#endif
	EQUATION_INFO_PTR eqn_info; /* For derived output variables */
	union
	{
		void *nt_trans;   /* TRANSLATOR_PTR */
		int   cv_var_num; /* conversion function number */
	} misc;
	char        *name;      /* the variable name */
	FF_TYPES_t  type;       /* variable type, see FFF_LOOKUP variable_types */
	FF_NDX_t    start_pos;  /* starting byte of variable in a record, counting from 1 (one) */
	FF_NDX_t    end_pos;    /* ending byte of variable in a record */
	short       precision;  /* implied (if integer) or real (if floating point) precision, i.e., number of decimal places */
	char *array_desc_str;
	char *record_title;     /* if type == FF_VAR_TYPE_FLAG */
};

#define FMT_BUFSIZE_BUFFER_SIZE 4096 /* pick a reasonable size... */

/* even though ff_bss_t is an unsigned (long, in Unix) there will be problems
   if bytes_used or total_bytes ever takes on a value greater than LONG_MAX */

#define FFBS_GRAFT 0

typedef unsigned FF_BSS_t; /* FreeForm bufsize size type */

/*
 * FF_BUFSIZE
 */

typedef struct struct_ff_bufsize FF_BUFSIZE, *FF_BUFSIZE_PTR, **FF_BUFSIZE_HANDLE;
struct struct_ff_bufsize
{
#ifdef FF_CHK_ADDR
	void *check_address;
#endif
	char HUGE *buffer;
	unsigned short usage;
	FF_BSS_t  bytes_used;
	FF_BSS_t  total_bytes;
};

#ifndef INT_MAX
#include <limits.h>
#endif /* INT_MAX */

#define BUFSIZE_TOTAL_BYTES_UNKNOWN INT_MAX

/*
 * FORMAT_DATA
 */

typedef struct struct_ff_format_data FORMAT_DATA, *FORMAT_DATA_PTR, **FORMAT_DATA_HANDLE;
struct struct_ff_format_data
{
/* generic io_element suitable for name tables, headers, and caches --
   anything that contains data in a dynamic buffer described by a format
*/
#ifdef FF_CHK_ADDR
	void *check_address;
#endif
	FORMAT_PTR format;
	FF_BUFSIZE_PTR data;
	struct struct_fd_state
	{
		unsigned int byte_order : 1; /*	1=big endian (Unix), 0=little endian */
		unsigned int new_record : 1; /* used for headers */
		unsigned int locked     : 1;
		unsigned int unused     : 13;
	} state;
};

typedef FORMAT_DATA          NAME_TABLE,
                            *NAME_TABLE_PTR,
                           **NAME_TABLE_HANDLE;

typedef FORMAT_DATA_LIST     NAME_TABLE_LIST,
                            *NAME_TABLE_LIST_HANDLE;

#include <name_tab.h>

/*
 * FORMAT_DATA_MAPPING
 */

typedef struct struct_ff_format_data_mapping FORMAT_DATA_MAPPING, *FORMAT_DATA_MAPPING_PTR, **FORMAT_DATA_MAPPING_HANDLE;
struct struct_ff_format_data_mapping
{
#ifdef FF_CHK_ADDR
	void *check_address;
#endif
	FORMAT_DATA_PTR input;
	FORMAT_DATA_PTR middle;
	FORMAT_DATA_PTR output;
};

typedef enum enum_pp_object
{
	PPO_FORMAT_LIST = 0,
	PPO_NT_LIST     = 1,
	ADD_YOUR_TYPE_HERE = 2
} PP_OBJECT_TYPES;

/*
 * PP_OBJECT
 */

typedef struct struct_ff_pp_object PP_OBJECT, *PP_OBJECT_PTR, **PP_OBJECT_HANDLE;
struct struct_ff_pp_object
{
	union union_u
	{
		FORMAT_LIST_HANDLE hf_list;
		struct struct_nt_list
		{		 
			FF_TYPES_t nt_io_type;
			NAME_TABLE_LIST_HANDLE hnt_list;
		} nt_list;
		void *add_your_object_here;
	} u;
	PP_OBJECT_TYPES ppo_type;
};

/*
 * FF_ARRAY_DIPOLE
 */

typedef struct struct_ff_array_dipole FF_ARRAY_DIPOLE, *FF_ARRAY_DIPOLE_PTR, **FF_ARRAY_DIPOLE_HANDLE;
struct struct_ff_array_dipole
{
#ifdef FF_CHK_ADDR
	void                    *check_address;
#endif
	char                    *name;  /* the array variable name */
	FF_ARRAY_DIPOLE_PTR      mate;
	FORMAT_DATA_MAPPING_PTR  format_data_mapping;
	ARRAY_MAPPING_PTR        array_mapping;
	FORMAT_DATA_PTR          fd;
	struct struct_connect
	{
		NDARR_SOURCE id; /* NDARRS_FILE or NDARRS_BUFFER */
		void *locus; /* file name if NDARRS_FILE, pointer if NDARRS_BUFFER */
		struct struct_file_info
		{
			unsigned long array_offset; /* file header size if NDARRS_FILE */
		} file_info;
		struct struct_buffer_info
		{
			unsigned long size; /* buffer size if NDARRS_BUFFER */
		} buffer_info;
		unsigned long bytes_left;
		int array_done;
	} connect;
};

/*
 * FF_ARRAY_CONDUIT
 */

typedef struct struct_ff_array_conduit FF_ARRAY_CONDUIT, *FF_ARRAY_CONDUIT_PTR, **FF_ARRAY_CONDUIT_HANDLE;
struct struct_ff_array_conduit
{
#ifdef FF_CHK_ADDR
	void *check_address;
#endif
	char name[FF_MAX_NAME]; /* the array variable name */
	FF_ARRAY_DIPOLE_PTR input;
	FF_ARRAY_DIPOLE_PTR output;
};

/*
 * PROCESS_INFO
 */

typedef struct struct_ff_process_info PROCESS_INFO, *PROCESS_INFO_PTR, **PROCESS_INFO_HANDLE;
struct struct_ff_process_info
{
#ifdef FF_CHK_ADDR
	void *check_address;
#endif
	FF_ARRAY_DIPOLE_PTR pole;

	PROCESS_INFO_PTR mate;
};

#ifdef FF_DBG
VARIABLE_PTR           FF_VARIABLE(VARIABLE_LIST);
FORMAT_PTR             FF_FORMAT(FORMAT_LIST);
FORMAT_DATA_PTR		  FD_FORMAT_DATA(FORMAT_DATA_LIST);
FF_ARRAY_CONDUIT_PTR	  FF_AC(FF_ARRAY_CONDUIT_LIST);
PROCESS_INFO_PTR       FF_PI(PROCESS_INFO_LIST);
FF_ERROR_PTR           FF_EP(FF_ERROR_LIST);

DLL_NODE_PTR dll_next(DLL_NODE_PTR node);
DLL_NODE_PTR dll_previous(DLL_NODE_PTR node);

#else
#define FF_VARIABLE(vl)              ((VARIABLE_PTR)(vl)->data.u.var)
#define FF_FORMAT(fl)                  ((FORMAT_PTR)(fl)->data.u.fmt)
#define FD_FORMAT_DATA(fdl)       ((FORMAT_DATA_PTR)(fdl)->data.u.fd)
#define FF_AC(acl)           ((FF_ARRAY_CONDUIT_PTR)(acl)->data.u.ac)
#define FF_PI(pil)               ((PROCESS_INFO_PTR)(pil)->data.u.pi)
#define FF_EP(el)                    ((FF_ERROR_PTR)(el)->data.u.err)

#define dll_next(n)	((n)->next)
#define dll_previous(n)	((n)->previous)

#endif

DLL_NODE_PTR dll_first(DLL_NODE_PTR node);
DLL_NODE_PTR dll_last(DLL_NODE_PTR node);

typedef enum enum_ff_dll_data_types
{
	DLL_VAR = 1,
	DLL_FMT = 2,
	DLL_FD  = 3,
	DLL_AC  = 4,
	DLL_PI  = 5,
	DLL_ERR = 6,
	DLL_HEAD_NODE = SHRT_MAX
} FF_DLL_DATA_TYPES;

typedef struct struct_ff_dll_data DLL_DATA, *DLL_DATA_PTR, **DLL_DATA_HANDLE; 
struct struct_ff_dll_data
{
	union union_struct_ff_dll_data_u
	{
		VARIABLE_PTR var;
		FORMAT_PTR fmt;
		FORMAT_DATA_PTR fd;
		FF_ARRAY_CONDUIT_PTR ac;
		PROCESS_INFO_PTR pi;
		FF_ERROR_PTR err;
	} u;
  FF_DLL_DATA_TYPES type;
};

struct struct_ff_dll_node
{
#ifdef FF_CHK_ADDR
	void *check_address;
#endif
	DLL_DATA data;
	DLL_NODE_PTR previous;
	DLL_NODE_PTR next;
#ifdef DLL_CHK
	unsigned int status;
	size_t count; /* head node: count of non-head nodes */
#endif
};

/* Define BIN structures */

#include <eval_eqn.h>

typedef struct struct_databin
{
#ifdef FF_CHK_ADDR
	void *check_address;
#endif
	char *title;

	EQUATION_INFO_PTR eqn_info;

	FF_ARRAY_CONDUIT_LIST array_conduit_list;
	
	NAME_TABLE_LIST table_list;
} DATA_BIN, *DATA_BIN_PTR, **DATA_BIN_HANDLE;

/* Define Defaults */
#if FF_OS == FF_OS_UNIX
#define DEFAULT_CACHE_SIZE	2097152L
#else
#define DEFAULT_CACHE_SIZE	16384L
#endif

#define FF_MAX_CACHE_SIZE (unsigned long)UINT_MAX

#define DBSET_INPUT_FORMATS             1
#define DBSET_OUTPUT_FORMATS            2
#define DBSET_INPUT_HEADER              5
#define DBSET_READ_EQV                  6
#define DBSET_CACHE_SIZE                8
#define DBSET_HEADER_FILE_NAMES         9
#define DBSET_QUERY_RESTRICTION        13
#define DBSET_VARIABLE_RESTRICTION     15
#define DBSET_BYTE_ORDER		         12

#define DBASK_FORMAT_SUMMARY                3
#define DBASK_PROCESS_INFO        18

#define DBDO_READ_FORMATS                    4
#define DBDO_WRITE_FORMATS                 7
#define DBDO_CONVERT_DATA	          10
#define DBDO_BYTE_SWAP               11
#define DBDO_FILTER_ON_QUERY              14
#define DBDO_CONVERT_FORMATS              16
#define DBDO_PROCESS_FORMATS              17

/* not needed */
#define DBDO_WRITE_OUTPUT_FMT_FILE        19

#define DBSET_CREATE_CONDUITS             20
#define DBSET_USER_UPDATE_FORMATS         21
#define DBSET_INIT_CONDUITS               22
#define DBSET_FORMAT_MAPPINGS             23
#define DBSET_EQUATION_VARIABLES          24

#ifdef NT_ANYWHERE
#error "NT_ANYWHERE is already defined!"
#endif
#define NT_ANYWHERE  FFF_REC | FFF_HEADER | FFF_TABLE
#define NT_HEADERS   FFF_REC | FFF_HEADER
#define NT_TABLE     FFF_TABLE

/* Internal stuff */
#define PINFO_POLE(pi)        ((pi)->pole)
#define PINFO_FD(pi)          (PINFO_POLE(pi)->fd)
#define PINFO_DATA(pi)        (PINFO_FD(pi)->data)
#define PINFO_BUFFER(pi)      (PINFO_DATA(pi)->buffer)
#define PINFO_BYTES_USED(pi)  (PINFO_DATA(pi)->bytes_used)
#define PINFO_TOTAL_BYTES(pi) (PINFO_DATA(pi)->total_bytes)
#define PINFO_STATE(pi)       (PINFO_FD(pi)->state)
#define PINFO_LOCKED(pi)      (PINFO_STATE(pi).locked)
#define PINFO_NEW_RECORD(pi)  (PINFO_STATE(pi).new_record)
#define PINFO_BYTE_ORDER(pi)  (PINFO_STATE(pi).byte_order)
#define PINFO_FORMAT(pi)      (PINFO_FD(pi)->format)

#define PINFO_SUPER_ARRAY_BYTES(pi)  (PINFO_ARRAY_MAP(pi)->super_array->total_size)
#define PINFO_SUB_ARRAY_BYTES(pi)    (PINFO_ARRAY_MAP(pi)->sub_array->total_size)

#define PINFO_SUPER_ARRAY_ELS(pi)  (PINFO_ARRAY_MAP(pi)->super_array->total_elements)
#define PINFO_SUB_ARRAY_ELS(pi)    (PINFO_ARRAY_MAP(pi)->sub_array->total_elements)

#define PINFO_ARRAY_BYTES(pi) (PINFO_ARRAY_MAP(pi)->sub_array->contig_size)

/* External stuff */
#define PINFO_TYPE(pi)          (PINFO_FORMAT(pi)->type)
#define PINFO_NAME(pi)          (PINFO_FORMAT(pi)->name)
#define PINFO_ORIGIN(pi)        (PINFO_FORMAT(pi)->locus)
#define PINFO_RECL(pi)          (FORMAT_LENGTH(PINFO_FORMAT(pi)))
#define PINFO_CACHEL(pi)        (PINFO_TOTAL_BYTES(pi) - 1)
#define PINFO_NUMVARS(pi)       (PINFO_FORMAT(pi)->num_vars)
#define PINFO_IS_FILE(pi)       (PINFO_POLE(pi)->connect.id & NDARRS_FILE)
#define PINFO_FNAME(pi)         (PINFO_IS_FILE(pi) ? PINFO_POLE(pi)->connect.locus : NULL)
#define PINFO_LOCUS(pi)         (PINFO_POLE(pi)->connect.locus)
#define PINFO_ARRAY_OFFSET(pi)  (PINFO_POLE(pi)->connect.file_info.array_offset)
#define PINFO_ARRAY_DONE(pi)    (PINFO_POLE(pi)->connect.array_done)
#define PINFO_BYTES_LEFT(pi)    (PINFO_POLE(pi)->connect.bytes_left)
#define PINFO_BUFFER_SIZE(pi)   (PINFO_POLE(pi)->connect.buffer_info.size)
#define PINFO_ARRAY_MAP(pi)     (PINFO_POLE(pi)->array_mapping)
#define PINFO_ID(pi)            (PINFO_POLE(pi)->connect.id)
#define PINFO_FORMAT_MAP(pi)    (PINFO_POLE(pi)->format_data_mapping)

#define PINFO_MATE(pi) ((pi)->mate)

/* Internal stuff */
#define PINFO_MATE_POLE(pi)        PINFO_POLE(PINFO_MATE(pi))
#define PINFO_MATE_FD(pi)          PINFO_FD(PINFO_MATE(pi))
#define PINFO_MATE_DATA(pi)        PINFO_DATA(PINFO_MATE(pi))
#define PINFO_MATE_BUFFER(pi)      PINFO_BUFFER(PINFO_MATE(pi))
#define PINFO_MATE_BYTES_USED(pi)  PINFO_BYTES_USED(PINFO_MATE(pi))
#define PINFO_MATE_TOTAL_BYTES(pi) PINFO_TOTAL_BYTES(PINFO_MATE(pi))
#define PINFO_MATE_STATE(pi)       PINFO_STATE(PINFO_MATE(pi))
#define PINFO_MATE_LOCKED(pi)      PINFO_LOCKED(PINFO_MATE(pi))
#define PINFO_MATE_NEW_RECORD(pi)  PINFO_NEW_RECORD(PINFO_MATE(pi))
#define PINFO_MATE_BYTE_ORDER(pi)  PINFO_BYTE_ORDER(PINFO_MATE(pi))
#define PINFO_MATE_FORMAT(pi)      PINFO_FORMAT(PINFO_MATE(pi))

#define PINFO_MATE_SUPER_ARRAY_BYTES(pi)  PINFO_SUPER_ARRAY_BYTES(PINFO_MATE(pi))
#define PINFO_MATE_SUB_ARRAY_BYTES(pi)    PINFO_SUB_ARRAY_BYTES(PINFO_MATE(pi))

#define PINFO_MATE_SUPER_ARRAY_ELS(pi)  PINFO_SUPER_ARRAY_EL(PINFO_MATE(pi))
#define PINFO_MATE_SUB_ARRAY_ELS(pi)    PINFO_SUB_ARRAY_EL(PINFO_MATE(pi))

/* External stuff */
#define PINFO_MATE_TYPE(pi)          PINFO_TYPE(PINFO_MATE(pi))
#define PINFO_MATE_NAME(pi)          PINFO_NAME(PINFO_MATE(pi))
#define PINFO_MATE_ORIGIN(pi)        PINFO_ORIGIN(PINFO_MATE(pi))
#define PINFO_MATE_RECL(pi)          PINFO_RECL(PINFO_MATE(pi))
#define PINFO_MATE_CACHEL(pi)        PINFO_CACHEL(PINFO_MATE(pi))
#define PINFO_MATE_NUMVARS(pi)       PINFO_NUMVARS(PINFO_MATE(pi))
#define PINFO_MATE_IS_FILE(pi)       PINFO_IS_FILE(PINFO_MATE(pi))
#define PINFO_MATE_FNAME(pi)         PINFO_FNAME(PINFO_MATE(pi))
#define PINFO_MATE_LOCUS(pi)         PINFO_LOCUS(PINFO_MATE(pi))
#define PINFO_MATE_ARRAY_OFFSET(pi)  PINFO_ARRAY_OFFSET(PINFO_MATE(pi))
#define PINFO_MATE_ARRAY_DONE(pi)    PINFO_ARRAY_DONE(PINFO_MATE(pi))
#define PINFO_MATE_BYTES_LEFT(pi)    PINFO_BYTES_LEFT(PINFO_MATE(pi))
#define PINFO_MATE_BUFFER_SIZE(pi)   PINFO_BUFFER_SIZE(PINFO_MATE(pi))
#define PINFO_MATE_ARRAY_MAP(pi)     PINFO_ARRAY_MAP(PINFO_MATE(pi))
#define PINFO_MATE_ID(pi)            PINFO_ID(PINFO_MATE(pi))

void dll_assign(void *data, FF_DLL_DATA_TYPES type, DLL_NODE_PTR node);

typedef BOOLEAN (*pgenobj_cmp_t)(void *, void *); /* pointer to generic object comparison function */

int list_replace_items(pgenobj_cmp_t lmi_cmp, DLL_NODE_PTR list);

/* Function Prototypes */

size_t ffv_type_size(FF_TYPES_t var_type);

DLL_NODE_PTR dll_init(void);
DLL_NODE_PTR dll_add(DLL_NODE_PTR);
void dll_delete(DLL_NODE_PTR);
void dll_delete_node(DLL_NODE_PTR);
int dll_free_list(DLL_NODE_PTR head);
int dll_free_holdings(DLL_NODE_PTR head);
void dll_rewind(DLL_NODE_HANDLE head_handle);

/* Format Attributes and object types */
#define FFF_NAME_CASE        (FF_TYPES_t)2
extern "C" {
int db_set(DATA_BIN_PTR, int, ...);
int db_ask(DATA_BIN_PTR, int, ...);
int db_do(DATA_BIN_PTR, int, ...);
}
/* Format List Functions */
FORMAT_DATA_PTR fd_get_header(DATA_BIN_PTR dbin, FF_TYPES_t header_type);
FORMAT_DATA_PTR fd_get_data(DATA_BIN_PTR dbin, FF_TYPES_t data_type);

void db_format_list_mark_io(FORMAT_LIST f_list, FF_TYPES_t format_type, char *input_file_name, char *output_file_name);
FORMAT_DATA_PTR fd_find_format_data(FORMAT_DATA_LIST, ...);
FORMAT_PTR db_find_format(FORMAT_LIST, ...);

int nt_ask(DATA_BIN_PTR, FF_TYPES_t origin, char *, FF_TYPES_t data, void *);
BOOLEAN nt_putvalue(DATA_BIN_PTR, char *, FF_TYPES_t, void *, short);
BOOLEAN nt_askexist(DATA_BIN_PTR, FF_TYPES_t, char *);

extern "C"
int db_init(FF_STD_ARGS_PTR, DATA_BIN_HANDLE, int (*pfi)(int));

void db_destroy(DATA_BIN_PTR);
BOOLEAN endian(void);

int create_array_conduit_list
	(
	 FORMAT_DATA_LIST format_data_list,
	 FF_STD_ARGS_PTR std_args,
	 FF_ARRAY_CONDUIT_LIST_HANDLE array_conduit_list_handle
	);

char *ff_lookup_string(FFF_LOOKUP_PTR, FF_TYPES_t);
FF_TYPES_t ff_lookup_number(FFF_LOOKUP_PTR, char *);
int ff_file_to_bufsize(char *fname, FF_BUFSIZE_HANDLE);
int ff_bufsize_to_textfile_overwrite(char *, FF_BUFSIZE_PTR);
int ff_bufsize_to_textfile_append(char *, FF_BUFSIZE_PTR);

int parse_command_line(int, char **, FF_STD_ARGS_PTR);

int ff_get_double(VARIABLE_PTR, void *, double *, FF_TYPES_t);
int ff_binary_to_string(void *source, FF_TYPES_t data_type, char *target);

#ifdef FF_DBG
int ff_format_to_debug_log(FORMAT_PTR format);
#endif /* FF_DBG */

int ff_copy_variable(VARIABLE_PTR source, VARIABLE_PTR target);
BOOLEAN ff_format_comp(FORMAT_PTR, FORMAT_PTR);
int ff_text_pre_parser(char *origin, FF_BUFSIZE_PTR, PP_OBJECT_PTR pp_object);
VARIABLE_PTR ff_make_variable(VARIABLE_PTR);
FORMAT_PTR ff_create_format(char *name, char *origin);
void ff_destroy_format(FORMAT_PTR);
void ff_destroy_variable(VARIABLE_PTR);
VARIABLE_PTR ff_create_variable(char *name);

extern "C" {
FF_STD_ARGS_PTR ff_create_std_args(void);
void ff_destroy_std_args(FF_STD_ARGS_PTR);
}

FORMAT_PTR ff_find_format(FORMAT_LIST, ...);
FORMAT_PTR ff_afm2bfm(FORMAT_PTR);
VARIABLE_PTR ff_find_variable(char *, FORMAT_PTR);
VARIABLE_PTR ff_new_name(FORMAT_PTR, char *, char *);
unsigned char ff_var_length(VARIABLE_PTR);
int btype_to_btype(void *src_value, FF_TYPES_t src_type, void *dest_value, FF_TYPES_t dest_type);
BOOLEAN type_cmp(FF_TYPES_t type, void *value0, void *value1);

/* Other */
FF_BUFSIZE_PTR ff_create_bufsize(long total_bytes);
int ff_resize_bufsize(long new_size, FF_BUFSIZE_HANDLE hbufsize);
void ff_destroy_bufsize(FF_BUFSIZE_PTR bufsize);
void ff_destroy_array_conduit(FF_ARRAY_CONDUIT_PTR conduit);
void ff_destroy_array_pole(FF_ARRAY_DIPOLE_PTR pole);
void ff_destroy_error(FF_ERROR_PTR error);
void fd_destroy_format_data(FORMAT_DATA_PTR fd);
FORMAT_DATA_PTR fd_create_format_data(FORMAT_PTR format, long data_size, char *name);

void ff_destroy_process_info_list(PROCESS_INFO_LIST);
void ff_destroy_process_info(PROCESS_INFO_PTR);

int ff_create_format_data_mapping
	(
	 FORMAT_DATA_PTR input,
	 FORMAT_DATA_PTR output,
	 FORMAT_DATA_MAPPING_HANDLE pd_h
	);

void ff_destroy_format_data_mapping(FORMAT_DATA_MAPPING_PTR pd);

void ff_destroy_array_conduit_list(FF_ARRAY_CONDUIT_LIST conduit_list);

int new_name_string__(const char *new_name, FF_STRING_HANDLE name_h);

int make_tabular_format_array_mappings
	(
	 PROCESS_INFO_PTR finfo,
	 long num_records,
	 long records_to_do
	);

int initialize_middle_data
	(
	 FORMAT_DATA_PTR input,
	 FORMAT_DATA_PTR output,
	 FORMAT_DATA_PTR middle
	);

int ff_process_format_data_mapping(FORMAT_DATA_MAPPING_PTR pd);

/* Conversion Function Prototypes */

typedef int FF_CVF(VARIABLE_PTR, double*, FORMAT_PTR, FF_DATA_BUFFER);

FF_CVF cv_abs;
FF_CVF cv_abs_sign_to_value;
FF_CVF cv_units;
FF_CVF cv_date_string;
FF_CVF cv_time_string;
FF_CVF cv_deg;
FF_CVF cv_deg_nsew;
FF_CVF cv_degabs;
FF_CVF cv_degabs_nsew;
FF_CVF cv_deg_abs;
FF_CVF cv_dms;
FF_CVF cv_feet_to_meters;
FF_CVF cv_geo44tim;
FF_CVF cv_geog_quad;
FF_CVF cv_geog_sign;
FF_CVF cv_ipe2ser;
FF_CVF cv_lon_east;
FF_CVF cv_long2mag;
FF_CVF cv_mag2long;
FF_CVF cv_meters_to_feet;
FF_CVF cv_nsew;
FF_CVF cv_ser2ymd;
FF_CVF cv_ser2ipe;
FF_CVF cv_sea_flags;
FF_CVF cv_slu_flags;
FF_CVF cv_ymd2ser;
FF_CVF cv_ydec2ymd;
FF_CVF cv_ymd2ipe;
FF_CVF cv_noaa_eq;

/* Application function prototypes */

#ifdef FF_MAIN
#ifdef TIMER
int newform(time_t start_time, FF_STD_ARGS_PTR std_args, DATA_BIN_PTR dbin);
#else
int newform(FF_STD_ARGS_PTR std_args, DATA_BIN_PTR dbin);
#endif
#else
int newform(DATA_BIN_PTR dbin);
#endif


#endif  /* (NOT) FREEFORM_H__ */


#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([freeform_handler], [3.6.1], [dods-tech@unidata.ucar.edu])
AC_CONFIG_AUX_DIR([conf])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([DODS_Date_Time.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LEX
AC_PROG_RANLIB
AC_PROG_YACC

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h float.h limits.h malloc.h stddef.h stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([floor isascii memmove memset pow sqrt strcasecmp strchr strcspn strerror strncasecmp strpbrk strrchr strspn strstr strtol strtoul])

dnl Checks for specific libraries
AC_CHECK_LIBDAP([3.7.0],
 [
  LIBS="$LIBS $DAP_SERVER_LIBS"
  CPPFLAGS="$CPPFLAGS $DAP_CFLAGS"
 ],
 [ AC_MSG_ERROR([Cannot find libdap])
])

AC_CANONICAL_HOST

case "$host" in
    *linux*) CPPFLAGS="$CPPFLAGS -DLINUX" ;;
    *hp*)    CPPFLAGS="$CPPFLAGS -DHP9000" ;;
    *alpha*) CPPFLAGS="$CPPFLAGS -DDEC_ALPHA" ;;
    *sun*)   CPPFLAGS="$CPPFLAGS -DSUN" ;;
    *darwin*)   CPPFLAGS="$CPPFLAGS -DLINUX" ;;
    *sgi*)   if test $ac_cv_sizeof_long -eq 4
	     then
	         CPPFLAGS="$CPPFLAGS -DIRIS4"
	     else       
	         CPPFLAGS="$CPPFLAGS -DIRIX"
	     fi ;;
    *)       echo "Either this machine is not supported by FreeForm or"
	     echo "the configure script needs to be fixed" ;;
esac

AC_CONFIG_FILES([FFND/Makefile
                 Makefile])
AC_OUTPUT

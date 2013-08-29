# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT(freeform_handler, 3.8.6, opendap-tech@opendap.org)
AC_CONFIG_AUX_DIR([conf])
AC_CONFIG_MACRO_DIR([conf])

AM_INIT_AUTOMAKE

# The handler is a shared object library, so we don't normally need
# the static lib. Override this default by specifying `--enable-static',
# which can make running unit tests with valgrind far more pleasant. 
AM_DISABLE_STATIC

AC_CONFIG_TESTDIR(bes-testsuite, [.])


PACKAGE_MAJOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
PACKAGE_MINOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
PACKAGE_SUBMINOR_VERSION=`echo $PACKAGE_VERSION | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
AC_MSG_NOTICE(Package Major Version:     $PACKAGE_MAJOR_VERSION)
AC_MSG_NOTICE(Package Minor Version:     $PACKAGE_MINOR_VERSION)
AC_MSG_NOTICE(Package SubMinor Version:  $PACKAGE_SUBMINOR_VERSION)
AC_SUBST(PACKAGE_MAJOR_VERSION)
AC_SUBST(PACKAGE_MINOR_VERSION)
AC_SUBST(PACKAGE_SUBMINOR_VERSION)


AC_CONFIG_SRCDIR([DODS_Date_Time.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LEX
# AC_PROG_RANLIB
AC_PROG_YACC
AC_PROG_LIBTOOL

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
AC_CHECK_LIBDAP([3.11.0],
 [
  LIBS="$LIBS $DAP_SERVER_LIBS"
  CPPFLAGS="$CPPFLAGS $DAP_CFLAGS"
 ],
 [ AC_MSG_ERROR([Cannot find libdap])
])

AC_CHECK_BES([3.9.0],
[
],
[ AC_MSG_ERROR([Cannot find bes])
])

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

AC_CONFIG_FILES([Makefile
		 bes-testsuite/Makefile
                 bes-testsuite/atlocal
                 FFND/Makefile])

# Build this using the Makefile.am as per the automake 1.10 manual, p.79
# AC_CONFIG_FILES([install-in-bes-conf.sh], [chmod +x install-in-bes-conf.sh])

AC_OUTPUT

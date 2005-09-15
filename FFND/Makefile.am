
# Build libdap, libtest-types.a, getdap and deflate

AUTOMAKE_OPTIONS = foreign 

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -DFREEFORM

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
#     export CXXFLAGS='...'; ./configure --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

noinst_LIBRARIES = libfreeform.a libmaxmin.a

noinst_PROGRAMS = readfile newform chkform bufform checkvar

libfreeform_a_SOURCES = $(FF_SRCS) $(FF_HDRS)

libmaxmin_a_SOURCES = mm_make.c

readfile_SOURCES = readfile.c
readfile_CPPFLAGS = -DFREEFORM
readfile_LDADD = libfreeform.a libmaxmin.a

newform_SOURCES = newform.c
newform_CPPFLAGS = -DFREEFORM
newform_LDADD = libfreeform.a libmaxmin.a

chkform_SOURCES = chkform.c
chkform_CPPFLAGS = -DFF_MAIN -DFREEFORM
chkform_LDADD = libfreeform.a libmaxmin.a

bufform_SORUCES = bufform.c
bufform_CPPFLAGS = -DFF_MAIN -DFREEFORM
bufform_LDADD = libfreeform.a libmaxmin.a

checkvar_SOURCES = checkvar.c
checkvar_CPPFLAGS = -DFREEFORM
checkvar_LDADD = libfreeform.a libmaxmin.a

EXTRA_DIST = 

CLEANFILES = *~

FF_SRCS = afm2bfm.c avlins.c avlfree.c cv_units.c dbevents.c dl_lists.c \
	  eqn_util.c error.c eval_eqn.c ff_utils.c file2buf.c formlist.c \
	  freeform.c makedbin.c makeform.c name_tab.c ndarray.c os_utils.c \
	  proclist.c setdbin.c showdbin.c  

FF_HDRS = avl.h err.h ff_types.h memtrack.h ndarray.h		\
	avltree.h eval_eqn.h freeform.h menuindx.h os_utils.h	\
	dl_lists.h ff2psdts.h maxmin.h name_tab.h

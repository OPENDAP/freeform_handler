# Makefile for libff.  We include the "minmax" library within libff under win32.

# Setup some preliminary directories
OBJDIR		= .\objects
SRCDIR		= ..
INSTALLBINDIR	= ..\..\..\..\bin
INSTALLETCDIR	= ..\..\..\..\etc
INSTALLLIBDIR	= ..\..\..\..\lib
INSTALLDLLDIR	= ..\..\..\..\dll
INSTALLINCDIR	= ..\..\..\..\include

INCLUDEDIR	= ..\..\..\..\include

# Compiler, Linker
CC		= cl
LINK		= link

# Setup compiler flags
STCFLAGS	= /ML				# Single-Threaded Model
MTCFLAGS	= /MT 				# Multi-Threaded Model
MDCFLAGS	= /MD				# Multi-Threaded DLL Model
STDCFLAGS	= /MLd				# Single-Threaded Model, debug version
MTDCFLAGS	= /MTd 				# Multi-Threaded Model, debug version
MDDCFLAGS	= /MDd				# Multi-Thread DLL Model, debug version
CFLAGS		= /nologo /W0 /GR /GX /c

# Setup linker flags for libraries
STLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MTLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MDLFLAGS	= -libpath:$(INSTALLLIBDIR) /dll /def:libff.def /map /fixed:NO /base:0x07000000
STDLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MTDLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MDDLFLAGS	= -libpath:$(INSTALLLIBDIR) /dll /def:libff.def /map /debug /fixed:NO /base:0x07000000

# Link libraries for static libff libraries and dll's
STLIBS		=
MTLIBS		=
MDLIBS		= libdapMD.lib libxml2MD.lib libcurlMD.lib libzMD.lib libxdrMD.lib pthreadVCE.lib
STDLIBS		=
MTDLIBS		=
MDDLIBS		= libdapMDD.lib libxml2MDD.lib libcurlMDD.lib libzMDD.lib libxdrMDD.lib pthreadVCE.lib

# Setup linker flags for executables
ESTLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)
EMTLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)
EMDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)
ESTDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)
EMTDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)
EMDDLFLAGS	= /nologo /libpath:$(INSTALLLIBDIR)

# Link libraries for exe's
ESTLLIBS	= libdapST.lib libffST.lib libxml2ST.lib libcurlST.lib libzST.lib libxdrST.lib
EMTLLIBS	= libdapMT.lib libffMT.lib libxml2MT.lib libcurlMT.lib libzMT.lib libxdrMT.lib pthreadVCE.lib
EMDLLIBS	= libdapMD.lib libffMD.lib libxml2MD.lib libcurlMD.lib libzMD.lib libxdrMD.lib pthreadVCE.lib
ESTDLLIBS	= libdapSTD.lib libffSTD.lib libxml2STD.lib libcurlSTD.lib libzSTD.lib libxdrSTD.lib
EMTDLLIBS	= libdapMTD.lib libffMTD.lib libxml2MTD.lib libcurlMTD.lib libzMTD.lib libxdrMTD.lib pthreadVCE.lib
EMDDLLIBS	= libdapMDD.lib libffMDD.lib libxml2MDD.lib libcurlMDD.lib libzMDD.lib libxdrMDD.lib pthreadVCE.lib

# Setup compiler includes and defines
INCS		= /I . /I .. /I $(INCLUDEDIR) /I $(INCLUDEDIR)\GNU /I $(INCLUDEDIR)\pthreads
DEFS		= /D WIN32 /D FREEFORM /D PC /D MSVC4
CPPFLAGS	= $(DEFS) $(INCS)

usage:
	@echo "usage: nmake all; nmake install"
	@echo "or"
	@echo "usage: nmake model<ST|MT|MD|STD|MTD|MDD>; nmake install<ST|MT|MD|STD|MTD|MDD>"

all:	setup
	nmake modelST
	nmake modelMT
	nmake modelMD
	nmake modelSTD
	nmake modelMTD
	nmake modelMDD

modelST: setup
	@-md $(OBJDIR)\ST
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(STLFLAGS)" LIBS="$(STLIBS)" OBJDIR=$(OBJDIR)\ST libffST.lib
	nmake installST

modelMT: setup
	@-md $(OBJDIR)\MT
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(MTLFLAGS)" LIBS="$(MTLIBS)" OBJDIR=$(OBJDIR)\MT libffMT.lib
	nmake installMT

modelMD: setup
	@-md $(OBJDIR)\MD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(MDLFLAGS)" LIBS="$(MDLIBS)" OBJDIR=$(OBJDIR)\MD libffMD.dll
	nmake installMD

modelSTD: setup
	@-md $(OBJDIR)\STD
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(STDLFLAGS)" LIBS="$(STDLIBS)" OBJDIR=$(OBJDIR)\STD libffSTD.lib
	nmake installSTD

modelMTD: setup
	@-md $(OBJDIR)\MTD
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(MTDLFLAGS)" LIBS="$(MTDLIBS)" OBJDIR=$(OBJDIR)\MTD libffMTD.lib
	nmake installMTD

modelMDD: setup
	@-md $(OBJDIR)\MDD
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(MDDLFLAGS)" LIBS="$(MDDLIBS)" OBJDIR=$(OBJDIR)\MDD libffMDD.dll
	nmake installMDD

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .c .cc

{$(SRCDIR)}.c{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ $<

LIBOBJS = $(OBJDIR)\afm2bfm.obj \
	$(OBJDIR)\avlins.obj \
	$(OBJDIR)\avlfree.obj \
	$(OBJDIR)\cv_units.obj \
	$(OBJDIR)\dbevents.obj \
	$(OBJDIR)\dl_lists.obj \
	$(OBJDIR)\eqn_util.obj \
	$(OBJDIR)\error.obj \
	$(OBJDIR)\eval_eqn.obj \
	$(OBJDIR)\ff_utils.obj \
	$(OBJDIR)\file2buf.obj \
	$(OBJDIR)\formlist.obj \
	$(OBJDIR)\freeform.obj \
	$(OBJDIR)\makedbin.obj \
	$(OBJDIR)\makeform.obj \
	$(OBJDIR)\name_tab.obj \
	$(OBJDIR)\ndarray.obj \
	$(OBJDIR)\os_utils.obj \
	$(OBJDIR)\proclist.obj \
	$(OBJDIR)\setdbin.obj \
	$(OBJDIR)\showdbin.obj \
	$(OBJDIR)\mm_make.obj

# Convenience dependancies
libffST.lib: $(OBJDIR)\libffST.lib
libffMT.lib: $(OBJDIR)\libffMT.lib
libffMD.dll: $(OBJDIR)\libffMD.dll
libffSTD.lib: $(OBJDIR)\libffSTD.lib
libffMTD.lib: $(OBJDIR)\libffMTD.lib
libffMDD.dll: $(OBJDIR)\libffMDD.dll

# Link the freeform library
$(OBJDIR)\libffST.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS) 

$(OBJDIR)\libffMT.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS) 

$(OBJDIR)\libffMD.dll: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

$(OBJDIR)\libffSTD.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@  $(LIBOBJS) $(LIBS) 

$(OBJDIR)\libffMTD.lib: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS) 

$(OBJDIR)\libffMDD.dll: $(LIBOBJS)
	@$(LINK) $(LFLAGS) /out:$@ $(LIBOBJS) $(LIBS)

# Freeform library Objects
$(OBJDIR)\afm2bfm.obj		: $(SRCDIR)\afm2bfm.c
$(OBJDIR)\avlins.obj		: $(SRCDIR)\avlins.c
$(OBJDIR)\avlfree.obj		: $(SRCDIR)\avlfree.c
$(OBJDIR)\cv_units.obj		: $(SRCDIR)\cv_units.c
$(OBJDIR)\dbevents.obj		: $(SRCDIR)\dbevents.c
$(OBJDIR)\dl_lists.obj		: $(SRCDIR)\dl_lists.c
$(OBJDIR)\eqn_util.obj		: $(SRCDIR)\eqn_util.c
$(OBJDIR)\error.obj		: $(SRCDIR)\error.c
$(OBJDIR)\eval_eqn.obj		: $(SRCDIR)\eval_eqn.c
$(OBJDIR)\ff_utils.obj		: $(SRCDIR)\ff_utils.c
$(OBJDIR)\file2buf.obj		: $(SRCDIR)\file2buf.c
$(OBJDIR)\formlist.obj		: $(SRCDIR)\formlist.c
$(OBJDIR)\freeform.obj		: $(SRCDIR)\freeform.c
$(OBJDIR)\makedbin.obj		: $(SRCDIR)\makedbin.c
$(OBJDIR)\makeform.obj		: $(SRCDIR)\makeform.c
$(OBJDIR)\name_tab.obj		: $(SRCDIR)\name_tab.c
$(OBJDIR)\ndarray.obj		: $(SRCDIR)\ndarray.c
$(OBJDIR)\os_utils.obj		: $(SRCDIR)\os_utils.c
$(OBJDIR)\proclist.obj		: $(SRCDIR)\proclist.c
$(OBJDIR)\setdbin.obj		: $(SRCDIR)\setdbin.c
$(OBJDIR)\showdbin.obj		: $(SRCDIR)\showdbin.c
$(OBJDIR)\mm_make.obj		: $(SRCDIR)\mm_make.c

# Preliminary setup
setup:
	@-mkdir $(OBJDIR)
	@-mkdir $(INSTALLBINDIR)
	@-mkdir $(INSTALLETCDIR)
	@-mkdir $(INSTALLLIBDIR)
	@-mkdir $(INSTALLDLLDIR)
	@-mkdir $(INSTALLINCDIR)

install: installST installMT installMD installSTD installMTD installMDD

installST:
	@copy $(OBJDIR)\ST\libffST.lib $(INSTALLLIBDIR)

installMT:
	@copy $(OBJDIR)\MT\libffMT.lib $(INSTALLLIBDIR)

installMD:
	@copy $(OBJDIR)\MD\libffMD.lib $(INSTALLLIBDIR)
	@copy $(OBJDIR)\MD\libffMD.dll $(INSTALLDLLDIR)

installSTD:
	@copy $(OBJDIR)\STD\libffSTD.lib $(INSTALLLIBDIR)

installMTD:
	@copy $(OBJDIR)\MTD\libffMTD.lib $(INSTALLLIBDIR)

installMDD:
	@copy $(OBJDIR)\MDD\libffMDD.lib $(INSTALLLIBDIR)
	@copy $(OBJDIR)\MDD\libffMDD.dll $(INSTALLDLLDIR)

clean:
	@-rmdir /Q /S $(OBJDIR)

!INCLUDE "dependancies"




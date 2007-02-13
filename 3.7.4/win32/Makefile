# Makefile for the win32 dods freeform server

# Setup some preliminary directories
OBJDIR		= .\objects
SRCDIR		= ..
INSTALLBINDIR	= ..\..\..\bin
INSTALLETCDIR	= ..\..\..\etc
INSTALLLIBDIR	= ..\..\..\lib
INSTALLDLLDIR	= ..\..\..\dll
INSTALLINCDIR	= ..\..\..\include

INCLUDEDIR	= ..\..\..\include

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
MDLFLAGS	= -libpath:$(INSTALLLIBDIR) /dll /def:libnc-dods.def /map /fixed:NO /base:0x07000000
STDLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MTDLFLAGS	= -lib /nologo -libpath:$(INSTALLLIBDIR)
MDDLFLAGS	= -libpath:$(INSTALLLIBDIR) /dll /def:libnc-dods.def /map /debug /fixed:NO /base:0x07000000

# Link libraries for static libraries and dll's
STLIBS		=
MTLIBS		=
MDLIBS		= libdapMD.lib libffMD.lib libxml2MD.lib libcurlMD.lib libzMD.lib libxdrMD.lib pthreadVCE.lib
STDLIBS		=
MTDLIBS		=
MDDLIBS		= libdapMDD.lib libffMDD.lib libxml2MDD.lib libcurlMDD.lib libzMDD.lib libxdrMDD.lib pthreadVCE.lib

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
INCS		= /I . /I .. /I ../FFND /I $(INCLUDEDIR) /I $(INCLUDEDIR)\GNU /I $(INCLUDEDIR)\pthreads
DEFS		= /D WIN32 /D WIN32_LEAN_AND_MEAN /D LOCAL /D FREEFORM /D PC /D MSVC4
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
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST ff_das.exe
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST ff_dds.exe
	nmake CFLAGS="$(CFLAGS) $(STCFLAGS)" LFLAGS="$(ESTLFLAGS)" LIBS="$(ESTLLIBS)" OBJDIR=$(OBJDIR)\ST ff_dods.exe

modelMT: setup
	@-md $(OBJDIR)\MT
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT ff_das.exe
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT ff_dds.exe
	nmake CFLAGS="$(CFLAGS) $(MTCFLAGS)" LFLAGS="$(EMTLFLAGS)" LIBS="$(EMTLLIBS)" OBJDIR=$(OBJDIR)\MT ff_dods.exe

modelMD: setup
	@-md $(OBJDIR)\MD
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD ff_das.exe
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD ff_dds.exe
	nmake CFLAGS="$(CFLAGS) $(MDCFLAGS)" LFLAGS="$(EMDLFLAGS)" LIBS="$(EMDLLIBS)" OBJDIR=$(OBJDIR)\MD ff_dods.exe

modelSTD: setup
	@-md $(OBJDIR)\STD
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD ff_das.exe
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD ff_dds.exe
	nmake CFLAGS="$(CFLAGS) $(STDCFLAGS)" LFLAGS="$(ESTDLFLAGS)" LIBS="$(ESTDLLIBS)" OBJDIR=$(OBJDIR)\STD ff_dods.exe

modelMTD: setup
	@-md $(OBJDIR)\MTD
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD ff_das.exe
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD ff_dds.exe
	nmake CFLAGS="$(CFLAGS) $(MTDCFLAGS)" LFLAGS="$(EMTDLFLAGS)" LIBS="$(EMTDLLIBS)" OBJDIR=$(OBJDIR)\MTD ff_dods.exe

modelMDD: setup
	@-md $(OBJDIR)\MDD
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD ff_das.exe
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD ff_dds.exe
	nmake CFLAGS="$(CFLAGS) $(MDDCFLAGS)" LFLAGS="$(EMDDLFLAGS)" LIBS="$(EMDDLLIBS)" OBJDIR=$(OBJDIR)\MDD ff_dods.exe

# Clear built-in rules and set new ones
.SUFFIXES :
.SUFFIXES :	.obj .c .cc

{$(SRCDIR)}.cc{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tp $<

{$(SRCDIR)}.c{$(OBJDIR)}.obj:
	@$(CC) $(CFLAGS) $(CPPFLAGS) /Fo$@ /Tc $<

FFOBJ = $(OBJDIR)\FFArray.obj \
	$(OBJDIR)\FFByte.obj \
	$(OBJDIR)\FFFloat64.obj \
	$(OBJDIR)\FFGrid.obj \
	$(OBJDIR)\FFInt32.obj \
	$(OBJDIR)\FFList.obj \
	$(OBJDIR)\FFSequence.obj \
	$(OBJDIR)\FFStr.obj \
	$(OBJDIR)\FFUInt32.obj \
	$(OBJDIR)\FFStructure.obj \
	$(OBJDIR)\FFUrl.obj \
	$(OBJDIR)\util_ff.obj \
	$(OBJDIR)\ff_read.obj \
	$(OBJDIR)\FFInt16.obj \
	$(OBJDIR)\FFUInt16.obj \
	$(OBJDIR)\FFFloat32.obj \
	$(OBJDIR)\ce_functions.obj \
	$(OBJDIR)\date_proc.obj \
	$(OBJDIR)\DODS_Date.obj \
	$(OBJDIR)\DODS_Time.obj \
	$(OBJDIR)\DODS_Time_Factory.obj \
	$(OBJDIR)\DODS_Date_Factory.obj \
	$(OBJDIR)\DODS_Date_Time.obj \
	$(OBJDIR)\DODS_Date_Time_Factory.obj \
	$(OBJDIR)\DODS_StartDate_Factory.obj \
	$(OBJDIR)\DODS_StartTime_Factory.obj \
	$(OBJDIR)\DODS_StartDate_Time_Factory.obj \
	$(OBJDIR)\DODS_EndDate_Factory.obj \
	$(OBJDIR)\DODS_EndTime_Factory.obj \
	$(OBJDIR)\DODS_EndDate_Time_Factory.obj

DASOBJS = $(OBJDIR)\ff_das.obj \
	$(OBJDIR)\ffdas.obj

DDSOBJS = $(OBJDIR)\ff_dds.obj \
	$(OBJDIR)\ffdds.obj

SERVOBJ = $(OBJDIR)\ff_dods.obj \
	$(OBJDIR)\ffdds.obj

# A convenience dependancy
ff_das.exe : $(OBJDIR)\ff_das.exe
ff_dds.exe : $(OBJDIR)\ff_dds.exe
ff_dods.exe : $(OBJDIR)\ff_dods.exe

$(OBJDIR)\ff_das.exe: $(DASOBJS) $(FFOBJ)
	@$(LINK) $(LFLAGS) /out:$@ $(DASOBJS) $(FFOBJ) $(LIBS)

$(OBJDIR)\ff_dds.exe: $(DDSOBJS) $(FFOBJ)
	@$(LINK) $(LFLAGS) /out:$@ $(DDSOBJS) $(FFOBJ) $(LIBS)

$(OBJDIR)\ff_dods.exe: $(SERVOBJ) $(FFOBJ)
	@$(LINK) $(LFLAGS) /out:$@ $(SERVOBJ) $(FFOBJ) $(LIBS)

# Dods ff server objects
$(OBJDIR)\FFArray.obj				: $(SRCDIR)\FFArray.cc
$(OBJDIR)\FFByte.obj				: $(SRCDIR)\FFByte.cc
$(OBJDIR)\FFFloat64.obj				: $(SRCDIR)\FFFloat64.cc
$(OBJDIR)\FFGrid.obj				: $(SRCDIR)\FFGrid.cc
$(OBJDIR)\FFInt32.obj				: $(SRCDIR)\FFInt32.cc
$(OBJDIR)\FFList.obj				: $(SRCDIR)\FFList.cc
$(OBJDIR)\FFSequence.obj			: $(SRCDIR)\FFSequence.cc
$(OBJDIR)\FFStr.obj				: $(SRCDIR)\FFStr.cc
$(OBJDIR)\FFUInt32.obj				: $(SRCDIR)\FFUInt32.cc
$(OBJDIR)\FFStructure.obj			: $(SRCDIR)\FFStructure.cc
$(OBJDIR)\FFUrl.obj				: $(SRCDIR)\FFUrl.cc
$(OBJDIR)\util_ff.obj				: $(SRCDIR)\util_ff.cc
$(OBJDIR)\ff_read.obj				: $(SRCDIR)\ff_read.c
$(OBJDIR)\FFInt16.obj				: $(SRCDIR)\FFInt16.cc
$(OBJDIR)\FFUInt16.obj				: $(SRCDIR)\FFUInt16.cc
$(OBJDIR)\FFFloat32.obj				: $(SRCDIR)\FFFloat32.cc
$(OBJDIR)\ce_functions.obj			: $(SRCDIR)\ce_functions.cc
$(OBJDIR)\date_proc.obj				: $(SRCDIR)\date_proc.cc
$(OBJDIR)\DODS_Date.obj				: $(SRCDIR)\DODS_Date.cc
$(OBJDIR)\DODS_Time.obj				: $(SRCDIR)\DODS_Time.cc
$(OBJDIR)\DODS_Time_Factory.obj			: $(SRCDIR)\DODS_Time_Factory.cc
$(OBJDIR)\DODS_Date_Factory.obj			: $(SRCDIR)\DODS_Date_Factory.cc
$(OBJDIR)\DODS_Date_Time.obj			: $(SRCDIR)\DODS_Date_Time.cc
$(OBJDIR)\DODS_Date_Time_Factory.obj		: $(SRCDIR)\DODS_Date_Time_Factory.cc
$(OBJDIR)\DODS_StartDate_Factory.obj		: $(SRCDIR)\DODS_StartDate_Factory.cc
$(OBJDIR)\DODS_StartTime_Factory.obj		: $(SRCDIR)\DODS_StartTime_Factory.cc
$(OBJDIR)\DODS_StartDate_Time_Factory.obj	: $(SRCDIR)\DODS_StartDate_Time_Factory.cc
$(OBJDIR)\DODS_EndDate_Factory.obj		: $(SRCDIR)\DODS_EndDate_Factory.cc
$(OBJDIR)\DODS_EndTime_Factory.obj		: $(SRCDIR)\DODS_EndTime_Factory.cc
$(OBJDIR)\DODS_EndDate_Time_Factory.obj		: $(SRCDIR)\DODS_EndDate_Time_Factory.cc
$(OBJDIR)\ff_das.obj				: $(SRCDIR)\ff_das.cc
$(OBJDIR)\ffdas.obj				: $(SRCDIR)\ffdas.cc
$(OBJDIR)\ff_dds.obj				: $(SRCDIR)\ff_dds.cc
$(OBJDIR)\ffdds.obj				: $(SRCDIR)\ffdds.cc
$(OBJDIR)\ff_dds.obj				: $(SRCDIR)\ff_dds.cc
$(OBJDIR)\ff_dods.obj				: $(SRCDIR)\ff_dods.cc


# Preliminary setup
setup:
	@-mkdir $(OBJDIR)
	@-mkdir $(INSTALLBINDIR)
	@-mkdir $(INSTALLETCDIR)
	@-mkdir $(INSTALLLIBDIR)
	@-mkdir $(INSTALLDLLDIR)
	@-mkdir $(INSTALLINCDIR)

# Install.  We use the Multi-threaded versions of executables by default
install:
	copy $(OBJDIR)\MD\ff_das.exe $(INSTALLETCDIR)
	copy $(OBJDIR)\MD\ff_dds.exe $(INSTALLETCDIR)
	copy $(OBJDIR)\MD\ff_dods.exe $(INSTALLETCDIR)

installST:

installMT:

installMD:

installSTD:

installMTD:

installMDD:

clean:
	@-rmdir /Q /S $(OBJDIR)

!INCLUDE "dependancies"




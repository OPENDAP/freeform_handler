
# Automake file for freeform-handler
#
# 9/14/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign check-news dejagnu

ACLOCAL_AMFLAGS = -I conf

SUBDIRS = FFND

# There are some defines which must be set with you build code which uses
# FreeForm.h. Add them here. 4/17/98 jhrg
AM_CXXFLAGS = -DFREEFORM -I$(top_srcdir)/FFND
AM_CFLAGS = -DFREEFORM -I$(top_srcdir)/FFND

bin_PROGRAMS = dap_ff_handler

lib_LTLIBRARIES = libff_handler.la
if OPENDAPSERVER
    lib_besdir=$(libdir)/bes
    lib_bes_LTLIBRARIES = libff_module.la
endif

libff_handler_la_SOURCES = $(FFTYPE_SRC) $(HANDLER_SRC) $(FFTYPE_HDR) \
	$(HANDLER_HDR)
libff_handler_la_LIBADD = FFND/libfreeform.la

libff_module_la_SOURCES = $(FFTYPE_SRC) $(HANDLER_SRC) $(FFTYPE_HDR) \
	$(HANDLER_HDR) $(SERVER_SRC) $(SERVER_HDR)
libff_module_la_CPPFLAGS = $(BES_CPPFLAGS)
libff_module_la_LDFLAGS = -avoid-version -module 
libff_module_la_LIBADD = $(BES_LIBS) FFND/libfreeform.la

dap_ff_handler_SOURCES = ff_handler.cc
dap_ff_handler_LDADD = libff_handler.la FFND/libfreeform.la 

FFTYPE_SRC = FFArray.cc FFFloat64.cc FFInt32.cc FFStructure.cc	\
	FFUrl.cc FFByte.cc FFGrid.cc FFSequence.cc FFUInt16.cc	\
	FFFloat32.cc FFInt16.cc FFStr.cc FFUInt32.cc


FFTYPE_HDR = FFArray.h FFFloat32.h FFInt16.h FFStr.h FFUInt32.h	\
	FFByte.h FFFloat64.h FFInt32.h FFStructure.h FFUrl.h	\
	FFGrid.h FFSequence.h FFUInt16.h

HANDLER_HDR = ff_ce_functions.h date_proc.h DODS_Date_Factory.h DODS_Date.h   \
	DODS_Date_Time_Factory.h DODS_Date_Time.h DODS_Decimal_Year_Factory.h \
	DODS_Decimal_Year.h DODS_EndDate_Factory.h			      \
	DODS_EndDate_Time_Factory.h					      \
	DODS_EndDecimal_Year_Factory.h DODS_EndTime_Factory.h		      \
	DODS_StartDate_Factory.h DODS_StartDate_Time_Factory.h		      \
	DODS_StartDecimal_Year_Factory.h DODS_StartTime_Factory.h	      \
	DODS_Time_Factory.h DODS_Time.h FreeForm.h read_ff.h util_ff.h	      \
	FFTypeFactory.h config_ff.h


HANDLER_SRC = util_ff.cc ff_read.c ffdds.cc ffdas.cc		\
	ff_ce_functions.cc date_proc.cc DODS_Date.cc		\
	DODS_Time.cc DODS_Time_Factory.cc DODS_Date_Factory.cc	\
	DODS_Date_Time.cc DODS_Date_Time_Factory.cc		\
	DODS_StartDate_Factory.cc DODS_StartTime_Factory.cc	\
	DODS_StartDate_Time_Factory.cc				\
	DODS_EndDate_Factory.cc DODS_EndTime_Factory.cc		\
	DODS_EndDate_Time_Factory.cc FFTypeFactory.cc

SERVER_SRC = FFRequestHandler.cc FFModule.cc

SERVER_HDR = FFRequestHandler.h FFModule.h

DEJATOOL = dap_ff_handler

RUNTESTDEFAULTFLAGS = --tool $$tool --srcdir ff-test

EXTRA_DIST = ff-test COPYRIGHT COPYING freeform_handler.spec

CLEANFILES = *~ *.sum *.log

###########################################################################

# Build linux RPMs

srpm:
	rpmbuild -ts freeform_handler-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb freeform_handler-@PACKAGE_VERSION@.tar.gz

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
dist_bin_SCRIPTS = bes-ff-data.sh
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

EXTRA_DIST = ff-test data COPYRIGHT COPYING freeform_handler.spec

CLEANFILES = *~ *.sum *.log

if OPENDAPSERVER

EXTRA_DIST += bes-ff-data.sh.in
CLEANFILES += bes-ff-data.sh

sample_datadir = $(datadir)/hyrax/data/ff
sample_data_DATA = data/1998-6-avhrr.dat data/avhrr.dat data/gsodock.dat \
	data/1998-6-avhrr.fmt data/avhrr.fmt data/gsodock.fmt \
	data/1998-6-avhrr.dat.das data/avhrr.dat.das

do_subst = sed -e 's,[@]sysconfdir[@],$(sysconfdir),g' \
	-e 's,[@]libdir[@],$(libdir),g' \
	-e 's,[@]datadir[@],$(datadir),g'

bes-ff-data.sh: bes-ff-data.sh.in Makefile
	$(do_subst) < $(srcdir)/bes-ff-data.sh.in > bes-ff-data.sh
	chmod +x bes-ff-data.sh

# This makes sure that the handler modifies the bes
# even with distcheck is run.
bes-conf: bes-ff-data.sh
	(bes_prefix=`bes-config --prefix` ; \
	if test "$$bes_prefix" = "/usr" ; \
	then \
	    ./bes-ff-data.sh /etc/bes/bes.conf /usr/lib/bes ;\
	else \
	    ./bes-ff-data.sh $$bes_prefix/etc/bes/bes.conf $$bes_prefix/lib/bes ; \
	fi)

endif

###########################################################################

# Build linux RPMs

srpm: dist
	rpmbuild -ts freeform_handler-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb freeform_handler-@PACKAGE_VERSION@.tar.gz

###########################################################################
#
# Build OS/X Packages.

PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker*

pkg:
	rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg
	./configure --prefix=/usr $(PKG_CONFIGURE_FLAGS)
	make bes-ff-data.sh
	cp bes-ff-data.sh bes-ff-data.sh.macosx
	./configure --prefix=`pwd`/mac_osx $(PKG_CONFIGURE_FLAGS)
	make clean install
	mv bes-ff-data.sh.macosx mac_osx/bin/bes-ff-data.sh
	${PACKAGEMAKER} -build -p @PACKAGE@-@PACKAGE_VERSION@.pkg -f mac_osx -ds -r OSX_Resources -i OSX_Resources/Info.plist -d OSX_Resources/Description.plist


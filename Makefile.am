# Automake file for freeform-handler
#
# 9/14/2005 jhrg
#
# $Id$

AUTOMAKE_OPTIONS = foreign check-news

ACLOCAL_AMFLAGS = -I conf

SUBDIRS = FFND bes-testsuite

# There are some defines which must be set with you build code which uses
# FreeForm.h. Add them here. 4/17/98 jhrg
AM_CXXFLAGS = -DFREEFORM -I$(top_srcdir)/FFND
AM_CFLAGS = -DFREEFORM -I$(top_srcdir)/FFND

lib_besdir=$(libdir)/bes
lib_bes_LTLIBRARIES = libff_module.la

libff_module_la_SOURCES = $(FFTYPE_SRC) $(HANDLER_SRC) $(FFTYPE_HDR) \
	$(HANDLER_HDR) $(SERVER_SRC) $(SERVER_HDR)
libff_module_la_CPPFLAGS = $(BES_CPPFLAGS)
libff_module_la_LDFLAGS = -avoid-version -module 
libff_module_la_LIBADD = $(BES_DAP_LIBS) FFND/libfreeform.la

FFTYPE_SRC = FFArray.cc FFFloat64.cc FFInt32.cc FFStructure.cc	\
	FFUrl.cc FFByte.cc FFGrid.cc FFSequence.cc FFUInt16.cc	\
	FFFloat32.cc FFInt16.cc FFStr.cc FFUInt32.cc


FFTYPE_HDR = FFArray.h FFFloat32.h FFInt16.h FFStr.h FFUInt32.h	\
	FFByte.h FFFloat64.h FFInt32.h FFStructure.h FFUrl.h	\
	FFGrid.h FFSequence.h FFUInt16.h

HANDLER_HDR = ff_ce_functions.h date_proc.h DODS_Date_Factory.h DODS_Date.h   \
	DODS_Date_Time_Factory.h DODS_Date_Time.h DODS_Decimal_Year_Factory.h \
	DODS_Decimal_Year.h DODS_EndDate_Time_Factory.h			      \
	DODS_EndDecimal_Year_Factory.h DODS_StartDate_Time_Factory.h	      \
	DODS_StartDecimal_Year_Factory.h DODS_Time_Factory.h DODS_Time.h      \
	FreeFormCPP.h read_ff.h util_ff.h config_ff.h DODS_EndDate_Factory.h  \
	DODS_EndTime_Factory.h DODS_StartDate_Factory.h			      \
	DODS_StartTime_Factory.h

HANDLER_SRC = util_ff.cc ff_read.c ffdds.cc ffdas.cc		\
	ff_ce_functions.cc date_proc.cc DODS_Date.cc		\
	DODS_Time.cc DODS_Time_Factory.cc DODS_Date_Factory.cc	\
	DODS_Date_Time.cc DODS_Date_Time_Factory.cc		\
	DODS_StartDate_Time_Factory.cc				\
	DODS_EndDate_Time_Factory.cc


SERVER_SRC = FFRequestHandler.cc FFModule.cc

SERVER_HDR = FFRequestHandler.h FFModule.h

EXTRA_DIST = data COPYRIGHT COPYING freeform_handler.spec \
	OSX_Resources ff.conf.in

CLEANFILES = ff.conf

sample_datadir = $(datadir)/hyrax/data/ff
sample_data_DATA = data/1998-6-avhrr.dat data/avhrr.dat data/gsodock.dat \
	data/1998-6-avhrr.fmt data/avhrr.fmt data/gsodock.fmt \
	data/1998-6-avhrr.dat.das data/avhrr.dat.das

install-data-local: ff.conf
	@currdate=`date +"%y%m%d%H%M"`; \
	test -d $(DESTDIR)$(sysconfdir)/bes || $(MKDIR_P) $(DESTDIR)$(sysconfdir)/bes; \
	test -d $(DESTDIR)$(sysconfdir)/bes/modules || $(MKDIR_P) $(DESTDIR)$(sysconfdir)/bes/modules; \
	test -f $(DESTDIR)$(sysconfdir)/bes/modules/ff.conf && mv -f $(DESTDIR)$(sysconfdir)/bes/modules/ff.conf $(DESTDIR)$(sysconfdir)/bes/modules/ff.conf.$$currdate; \
	$(INSTALL_DATA) ff.conf $(DESTDIR)$(sysconfdir)/bes/modules/ff.conf

uninstall-local:
	rm -f $(DESTDIR)$(sysconfdir)/bes/modules/ff.conf

ff.conf: ff.conf.in config.status
	sed -e "s%[@]bes_modules_dir[@]%${lib_besdir}%" $< > ff.conf

# Not nearly as clean as it could be, but this removes .svn directories
# in subdirs.
dist-hook:
	rm -rf `find $(distdir) -name .svn`

C4_DIR=./cccc
.PHONY: cccc
cccc:	
	cccc --outdir=$(C4_DIR) $(libff_module_la_SOURCES)

###########################################################################

# Fortify targets.

.PHONY: fortify
fortify:
	(cd FFND && $(MAKE) fortify)
	sourceanalyzer -b @PACKAGE@ $(MAKE) libff_handler.la
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr

.PHONY: fortify-lite
fortify-lite:
	(cd FFND && $(MAKE))
	sourceanalyzer -b @PACKAGE@ $(MAKE) libff_handler.la
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr

# Use this to clean the fortify project.
.PHONY: fortifyclean
fortifyclean:
	(cd FFND && $(MAKE) fortifyclean)
	sourceanalyzer -b @PACKAGE@ -clean

###########################################################################

# Build linux RPMs

srpm: dist
	rpmbuild -ts --clean $(RPM_OPTIONS) @PACKAGE@-@PACKAGE_VERSION@.tar.gz

rpm: dist
	rpmbuild -tb --clean $(RPM_OPTIONS) @PACKAGE@-@PACKAGE_VERSION@.tar.gz

###########################################################################

# Build OS/X Packages.

PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker*

UNIV_ARCH=-arch ppc -arch i386
MAC_SYSROOT=Developer/SDKs/MacOSX10.4u.sdk

PKG_CONF_FLAGS=

clean-pkg:
	-rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@

pkg-build:
	./configure --prefix=/usr --disable-dependency-tracking $(PKG_CONF_FLAGS)
	make clean all
	DESTDIR=${PWD}/mac_osx make install

pkg-main:
	./OSX_Resources/update_mac_package_contents.pl README "configure.ac" "mac_osx/usr"
	${PACKAGEMAKER} -build -p @PACKAGE@-@PACKAGE_VERSION@.pkg \
		-f mac_osx/usr \
		-ds -r OSX_Resources \
		-i OSX_Resources/Info.plist \
		-d OSX_Resources/Description.plist
	-rm -rf mac_osx

pkg-dmg:
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@.dmg
	mkdir @PACKAGE@-@PACKAGE_VERSION@
	mv @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@
	cp README NEWS @PACKAGE@-@PACKAGE_VERSION@
	dropdmg -i --sanitize-for-servers --format zlib @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@

pkg: clean-pkg pkg-build pkg-main pkg-dmg


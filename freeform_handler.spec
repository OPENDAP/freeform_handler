Summary:         FreeForm data handler for the OPeNDAP Data server
Name:            freeform_handler
Version:         3.7.8
Release:         2
License:         LGPL
Group:           System Environment/Daemons 
Source0:         http://www.opendap.org/pub/source/%{name}-%{version}.tar.gz
URL:             http://www.opendap.org/

BuildRoot:       %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:   libdap-devel >= 3.8.0
BuildRequires:   bes-devel >= 3.6.0

%description 
This is the freeform data handler for our data server. It reads ASCII,
binary and DB4 files which have been described using FreeForm and returns DAP
responses that are compatible with DAP2 and the dap-server software. This
package contains both the OPeNDAP CGI server 'handlers' and the new OPeNDAP 4
Data Server (aka Hyrax) modules.

%prep 
%setup -q

%build
%configure --disable-static --disable-dependency-tracking
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install INSTALL="install -p"
rm $RPM_BUILD_ROOT%{_libdir}/*.la
rm $RPM_BUILD_ROOT%{_libdir}/*.so
rm $RPM_BUILD_ROOT%{_libdir}/bes/libff_module.la

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_bindir}/dap_ff_handler
%{_bindir}/bes-ff-data.sh
%{_libdir}/libff_handler.so.*
%{_libdir}/libfreeform.so.*
%{_libdir}/bes/libff_module.so
%{_datadir}/hyrax/
%doc COPYING COPYRIGHT NEWS README

%changelog
* Mon Mar 03 2008 Patrick West <patrick@ucar.edu> 3.7.8-1
- Update to 3.7.8

* Mon Mar 26 2006 James Gallagher <jgallagher@opendap.org> 3.6.1-1
- Update to 3.6.1: Mac OS/X fixes.

* Mon Feb 27 2006 James Gallagher <jgallagher@opendap.org> 3.6.0-1
- Update to 3.6.0

* Thu Sep 21 2005 James Gallagher <jgallagher@opendap.org> 3.5.0-1
- initial release

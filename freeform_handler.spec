Summary:         FreeForm data handler for the OPeNDAP Data server
Name:            freeform_handler
Version:         3.7.4
Release:         1
License:         LGPL
Group:           System Environment/Daemons 
Source0:         ftp://ftp.unidata.ucar.edu/pub/opendap/source/%{name}-%{version}.tar.gz
URL:             http://www.opendap.org/

BuildRoot:       %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:   libdap-devel >= 3.7.4
#Requires:        bes

%description 
This is the freeform data handler for our data server. It reads ASCII,
binary and DB4 files which have been described using FreeForm and returns DAP
responses that are compatible with DAP2 and the dap-server software. This
package contains both the OPeNDAP CGI server 'handlers' and the new OPeNDAP 4
Data Server (aka Hyrax) modules.

%prep 
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

# pre: commands to run before install; post: commnds run after install;
# preun; postun for commands before and after uninstall

%post -p /sbin/ldconfig

# Only try to configure the bes.conf file if the bes can be found.
if bes-config --version >/dev/null 2>&1
then
	bes_prefix=`bes-config --prefix`
	configure-ff-data.sh $bes_prefix/etc/bes/bes.conf $bes_prefix/lib/bes
fi

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_bindir}/dap_ff_handler
%{_bindir}/configure-ff-data.sh
%{_libdir}/
%{_libdir}/bes/
%{_datadir}/hyrax/data/ff
%doc COPYING COPYRIGHT NEWS README

%changelog
* Mon Mar 26 2006 James Gallagher <jgallagher@opendap.org> 3.6.1-1
- Update to 3.6.1: Mac OS/X fixes.

* Mon Feb 27 2006 James Gallagher <jgallagher@opendap.org> 3.6.0-1
- Update to 3.6.0

* Thu Sep 21 2005 James Gallagher <jgallagher@opendap.org> 3.5.0-1
- initial release

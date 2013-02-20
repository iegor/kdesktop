%define distversion %( perl -e '$_=\<\>;/(\\d+)\\.(\\d)\\.?(\\d)?/; print "$1$2".($3||0)' /etc/*-release)
Name: ksmssend
Summary: %{name} -- A frontend to smssend for KDE
Version: 3.0.8
Release: %{_vendor}_%{distversion}
Copyright: GPL
Group: X11/KDE/Utilities
Source: ftp://ftp.kde.org/pub/kde/unstable/apps/utils/%{name}-%{version}.tar.gz
Packager: ian reinhart geiser <geiseri@kde.org>
BuildRoot: /tmp/%{name}-%{version}
Prefix: `kde-config --prefix`

%description
A signature management utility.  This version is compiled for %{_vendor}  %{distversion}.

%prep
rm -rf $RPM_BUILD_ROOT
%setup -n %{name}-%{version}
WANT_AUTOCONF_2_5=1 make -f admin/Makefile.common
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
        --disable-debug --enable-final --prefix=%{prefix} \
        -with-install-root=$RPM_BUILD_ROOT

%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make prefix=$RPM_BUILD_ROOT%{prefix} install
cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/%{name}-master.list
find . -type f -o -type l | sed 's|^\.||' >> $RPM_BUILD_DIR/%{name}-master.list

%clean
#rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/%{name}-%{version}
rm -rf $RPM_BUILD_DIR/%{name}-master.list

%files -f ../%{name}-master.list


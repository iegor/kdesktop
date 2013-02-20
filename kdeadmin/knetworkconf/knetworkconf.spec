# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
%define name knetworkconf
%define version 0.5
%define release 1mdk

Summary:   A KDE application to configure TCP/IP settings.
Name:      %{name}
Version:   %{version}
Release:   %{release}
License: GPL
Url:       http://www.merlinux.org/knetworkconf/
Group:     Networking/Other
Source:    %{name}-%{version}.tar.bz2
BuildRoot:  %{_tmppath}/%{name}-buildroot
Requires: %_bindir/kdesu

%description
KNetworkConf is a KDE application to configure  TCP/IP settings
in a Linux machine. I developed it because I couldn't find any
application to configure TCP/IP settings from within KDE, and I
think this is a "must have" app for a serious Desktop Enviroment
like KDE.
KNetworkConf can configure installed network devices (you can't
add new ones for now), the default gateway,host and domain names,
and add/remove DNS servers. This first version is a standalone
application, but the idea is to make it a KDE Control Center
module and a KApplet to have a fast access to it from the KDE
panel.

%prep
rm -rf $RPM_BUILD_ROOT

%setup 

./configure --prefix=%_prefix --mandir=%_mandir --datadir=%_datadir --libdir=%_libdir

%build

%make

%install
#icons
mkdir -p %buildroot/{%_miconsdir,%{_prefix}/share/pixmaps/}
#cd $RPM_BUILD_DIR/%{name}-%{version}
install -m 644 %{name}/lo16-app-%{name}.png %buildroot/%{_miconsdir}/%{name}.png
install -m 644 %{name}/lo32-app-%{name}.png %buildroot/%{_iconsdir}/%{name}.png
install -m 644 %{name}/network_card.png %buildroot/%{_prefix}/share/pixmaps/network_card.png

# Menu
mkdir -p %buildroot/%_menudir
cat <<EOF > %buildroot/%_menudir/%name  
?package(%name): command="kdesu %_bindir/%name" needs="X11" \
icon="%name.png" section="Configuration/Networking" \
title="KNetworkConf" longtitle="Configure TCP/IP settings under KDE"
EOF

make DESTDIR=$RPM_BUILD_ROOT bitsdata=$RPM_BUILD_ROOT/%{_datadir} bitssysconf=$RPM_BUILD_ROOT/%{_sysconfdir} install

%find_lang %name

%post
%{update_menus}

%postun
%{clean_menus}

%clean
rm -rf $RPM_BUILD_ROOT/*


%files -f %name.lang
%defattr(-,root,root,0755)
%doc README COPYING AUTHORS LEAME
%{_libdir}/menu/*
%{_bindir}/knetworkconf
%{_iconsdir}/*.png
%{_miconsdir}/%{name}.png
%_datadir/pixmaps/network_card.png
%_datadir/applnk-mdk/Configuration/Networking/*
%_datadir/applnk/System/*
#%_datadir/applnk/Internet/*
%_datadir/icons/locolor/16x16/apps/*
%_datadir/icons/locolor/32x32/apps/*
%_docdir/HTML/en/knetworkconf
%_docdir/HTML/de/knetworkconf
%_docdir/HTML/es/knetworkconf
%_datadir/apps/knetworkconf/backends/*
%_datadir/apps/knetworkconf/pixmaps/*
%{_libdir}/pkgconfig/system-tools-backends.pc


%changelog
* Sun Feb 22 2004 Juan Luis Baptiste <juan.baptiste@kdemail.net> 0.5-1mdk
- 0.5

* Thu Oct 02 2003 Lenny Cartier <lenny@mandrakesoft.com> 0.4.2-1mdk
- 0.4.2

* Fri Jul 18 2003 Laurent MONTEL <lmontel@mandrakesoft.com> 0.4.1-2mdk
- Rebuild

* Thu Apr 10 2003 Lenny Cartier <lenny@mandrakesoft.com> 0.4.1-1mdk
- 0.4.1
- find lang macro

* Fri Mar 28 2003 Lenny Cartier <lenny@mandrakesoft.com> 0.4-1mdk
- from Juan Luis Baptiste <juancho@linuxmail.org> :
	- Initial release.

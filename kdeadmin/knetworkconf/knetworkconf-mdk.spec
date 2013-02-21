%define name knetworkconf
%define version 0.6.1
%define release 1mdk
#%define _unpackaged_files_terminate_build 0

Summary:   	A KDE Control Center Module to configure Network settings.
Name:      	%{name}
Version:   	%{version}
Release:   	%{release}
License: 	GPL
Url:       	http://www.merlinux.org/knetworkconf/
Group:     	Networking/Other
Source:    	%{name}-%{version}.tar.bz2
BuildRoot:  %{_tmppath}/%{name}-buildroot
Requires: 	%_bindir/kdesu
Patch1:		knetworkconf-0.6-fix-menu.patch.bz2

%description
KNetworkConf is a KDE Control Center module to configure  the Network 
settings in a Linux machine. I developed it because I couldn't find any
application to do this from within KDE, and I think this is a "must have" 
app for a serious Desktop Enviroment like KDE.
KNetworkConf can configure installed network devices (you can't
add new ones for now), the default gateway,host and domain names,
add/remove DNS servers and add/remove static hosts. 

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q 
%patch1 -p0 -b .fix_menu

%build
make -f Makefile.cvs
QTDIR=%qtdir
export LD_LIBRARY_PATH=$QTDIR/%_lib:$LD_LIBRARY_PATH

# Search for qt/kde libraries in the right directories (avoid patch)
# NOTE: please don't regenerate configure scripts below
perl -pi -e "s@/lib(\"|\b[^/])@/%_lib\1@g if /(kde|qt)_(libdirs|libraries)=/" configure


%{?__cputoolize: %{__cputoolize} }


%ifarch %ix86
CFLAGS="%optflags" CXXFLAGS="`echo %optflags |sed -e 's/-fomit-frame-pointer//'`" \
%else
CFLAGS="%optflags" CXXFLAGS="%optflags" \
%endif
	%configure --disable-debug

%make

%install

%makeinstall_std

%find_lang %name

mkdir -p %buildroot/{%_miconsdir,%{_prefix}/share/pixmaps/}
install -m 644 %{name}/lo16-app-%{name}.png %buildroot/%{_miconsdir}/%{name}.png
install -m 644 %{name}/lo32-app-%{name}.png %buildroot/%{_iconsdir}/%{name}.png
install -m 644 %{name}/network_card.png %buildroot/%{_prefix}/share/pixmaps/network_card.png

mkdir -p %{buildroot}/%{_menudir}
kdedesktop2mdkmenu.pl %{name} System/Configuration/KDE/Network  $RPM_BUILD_ROOT%{_datadir}/applications/kde/kcm_knetworkconfmodule.desktop $RPM_BUILD_ROOT%{_menudir}/%{name}


%post
/sbin/ldconfig
%{update_menus}

%postun
/sbin/ldconfig
%{clean_menus}

%clean
rm -rf $RPM_BUILD_ROOT/*

%files -f %name.lang

#Quick Hack: for some reason the %find_lang command doesn't find 
#the translations files, it outputs a knetworkconf.lang file full
#of empty lines instead of the following ones:
%lang(de) /usr/share/locale/de/LC_MESSAGES/knetworkconfmodule.mo
%lang(es) /usr/share/locale/es/LC_MESSAGES/knetworkconfmodule.mo
%lang(eu) /usr/share/locale/eu/LC_MESSAGES/knetworkconfmodule.mo
%lang(fr) /usr/share/locale/fr/LC_MESSAGES/knetworkconfmodule.mo
%lang(hu) /usr/share/locale/hu/LC_MESSAGES/knetworkconfmodule.mo
%lang(no) /usr/share/locale/no/LC_MESSAGES/knetworkconfmodule.mo
%lang(pl) /usr/share/locale/pl/LC_MESSAGES/knetworkconfmodule.mo
%lang(pt_BR) /usr/share/locale/pt_BR/LC_MESSAGES/knetworkconfmodule.mo

%defattr(-,root,root,0755)
%doc README COPYING AUTHORS LEAME

%{_libdir}/menu/*

%{_libdir}/kde3/*.la
%{_libdir}/kde3/*.so
%{_libdir}/pkgconfig/*.pc
%{_iconsdir}/*.png
%{_miconsdir}/%{name}.png
%_datadir/applications/kde/*.desktop
%_datadir/pixmaps/network_card.png

%_datadir/icons/locolor/16x16/apps/*.png
%_datadir/icons/locolor/22x22/apps/*.png
%_datadir/icons/locolor/32x32/apps/*.png
%_datadir/icons/hicolor/22x22/actions/*.png

#%dir %_docdir/HTML/en/%{name}/
%_docdir/HTML/en/%{name}/
#%dir %_docdir/HTML/de/%{name}/
%_docdir/HTML/de/%{name}/
#%dir %_docdir/HTML/es/%{name}/
%_docdir/HTML/es/%{name}/

%dir %_datadir/apps/%{name}/backends/
%_datadir/apps/%{name}/backends/*

%dir %_datadir/apps/%{name}/pixmaps/
%_datadir/apps/%{name}/pixmaps/*.png


%changelog
* Sun Feb 20 2005 Juan Luis Baptiste <juan.baptiste@kdemail.net> 0.6.1-1mdk
- 0.6.1

* Tue Feb 08 2005 Laurent MONTEL <lmontel@mandrakesoft.com> 0.6-3mdk
- Fix section

* Thu Sep 30 2004 Laurent MONTEL <lmontel@mandrakesoft.com> 0.6-2mdk
- Fix menu

* Tue Aug 10 2004 Lenny Cartier <lenny@mandrakesoft.com> 0.6-1mdk
- from Juan Luis Baptiste <juan.baptiste@kdemail.net> :
	- 0.6

* Tue Jun 15 2004 Lenny Cartier <lenny@mandrakesoft.com> 0.5-2mdk
- rebuild

* Thu Feb 26 2004 Lenny Cartier <lenny@mandrakesoft.com> 0.5-1mdk
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

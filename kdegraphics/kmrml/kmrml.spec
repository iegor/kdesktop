%define version 0.3
%define release 1
%define serial  1
%define prefix  /opt/kde3

Name:      kmrml
Summary:   MRML for KDE -- Content based image retrieval
Version:   %{version}
Release:   %{release}
Serial:    %{serial}
Source:    http://devel-home.kde.org/~pfeiffer/kmrml/kmrml-%{version}.tgz
URL:       http://devel-home.kde.org/~pfeiffer/kmrml/
Copyright: GPL
Packager:  Carsten Pfeiffer <pfeiffer@kde.org>
Group:     X11/KDE/Utilities
BuildRoot: /tmp/kmrml-%{version}-root
Prefix:    %{prefix}

%description
MRML is short for Multimedia Retrieval Markup Language,
which defines a protocol for querying a server for images
based on their content. See http://www.mrml.net about MRML
and the GNU Image Finding Tool (GIFT), an MRML server.
 
This package consists of an mrml kio-slave that handles
the communication with the MRML server and a KPart to
be embedded e.g. into Konqueror.

With those, you can search for images by giving an example
image and let the server look up similar images. The query
result can be refined by giving positive/negative feedback.
 
Install with '--prefix $KDEDIR' unless you have KDE in /opt/kde3

%prep
rm -rf $RPM_BUILD_ROOT

%setup -n kmrml-%{version}

%build
export KDEDIR=%{prefix}
CXXFLAGS="$RPM_OPT_FLAGS -fno-exceptions -malign-functions=2 -malign-jumps=2 -malign-loops=2 -pipe" LDFLAGS=-s ./configure --prefix=%{prefix} --enable-final --disable-debug
mkdir -p $RPM_BUILD_ROOT
make

%install
make install DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
 
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.%{name}
 
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.%{name}
 
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.%{name}

%clean
rm -rf $RPM_BUILD_ROOT
rm -f $RPM_BUILD_DIR/file.list.%{name}

%files -f ../file.list.%{name}


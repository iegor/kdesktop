SUBDIRS = pics
# set the include path for X, qt, KDE, mico and the synthesizer class defs
INCLUDES= -I$(top_srcdir)/arts/runtime -I$(top_builddir)/arts/runtime  -I$(kde_includes)/arts  $(all_includes)

xdg_apps_DATA = artsbuilder.desktop

kdemime_DATA = x-artsbuilder.desktop
kdemimedir = $(kde_mimedir)/application

bin_PROGRAMS = 	artsbuilder

artsbuilder_SOURCES = autorouter.cpp main.cpp propertypanelbase.ui \
  module.cpp propertypanel.cpp structure.cpp \
  retrievedlg.cpp createtool.cpp structureport.cpp \
  drawutils.cpp scomponent.cpp portposdlg.cpp \
  menumaker.cpp session.cpp dirmanager.cpp mwidget.cpp \
  execdlg.cpp qttableview.cpp interfacedlg.cpp
artsbuilder_COMPILE_FIRST = ../runtime/artsbuilder.h

noinst_HEADERS = qttableview.h

METASOURCES = AUTO

artsbuilder_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
artsbuilder_LDADD   = $(LIB_KFILE) -lqtmcop $(LIB_X11) $(LIBPTHREAD) $(top_builddir)/arts/runtime/libartsbuilder.la -lsoundserver_idl -lartskde

rcdir = $(kde_datadir)/artsbuilder
rc_DATA = artsbuilderui.rc

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages: rc.cpp
	$(XGETTEXT)  *.cpp -o $(podir)/artsbuilder.pot

propertypanel.h: propertypanelbase.h


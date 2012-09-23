INCLUDES = -I$(kde_includes)/kio -I$(top_srcdir)/noatun/library -I$(kde_includes)/arts $(all_includes)

SUBDIRS = pics library app . modules

KDE_ICON = AUTO

xdg_apps_DATA = noatun.desktop

presetsdir = $(kde_datadir)/noatun/eq.preset
presets_DATA = preset.dance preset.jazz preset.metal preset.trance preset.zero

updatedir = $(kde_datadir)/kconf_update
update_DATA = noatun.upd
update_PROGRAMS = noatun20update

noatun20update_SOURCES  = noatun20update.cpp
noatun20update_LDFLAGS = $(all_libraries) $(KDE_RPATH)
noatun20update_LDADD = $(LIB_QT)

messages:
	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui"` > rc.cpp
	$(XGETTEXT) rc.cpp `find . -name "*.cc" -o -name "*.cpp" -o -name "*.h"` -o $(podir)/noatun.pot

api:
	$(mkinstalldirs) $(top_builddir)/noatun/apidocs/libnoatun
	if test ! -x $(top_builddir)/noatun/apidocs/common; then \
		$(LN_S) $(kde_libs_htmldir)/en/common $(top_builddir)/noatun/apidocs/common; \
	fi
	doxygen noatun.api

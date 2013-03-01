AUTOMAKE_OPTIONS = foreign
KDE_CXXFLAGS = -DQT_PLUGIN
METASOURCES = AUTO
INCLUDES = -I$(top_srcdir)/kdefx -I$(kde_includes)/kwin $(all_includes)
noinst_HEADERS = qindustrial.h common.h windecoration.h

# Qt/KDE widget theme

kde_style_LTLIBRARIES = qindustrial.la
qindustrial_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
qindustrial_la_LIBADD = -lkdefx
qindustrial_la_SOURCES = qindustrial.cpp plugin.cpp common.c

qtc_themerc_files = qindustrial.themerc
stylercdir = $(kde_datadir)/kstyle/themes
stylerc_DATA = $(qtc_themerc_files)

# Kdisplay Qt/KDE colour theme

colorrcdir=$(kde_datadir)/kdisplay/color-schemes
colorrc_DATA = Industrial.kcsrc

# KWin decoration theme

# kwindir = $(kde_datadir)/kwin/
# kwin_DATA = qindustrial.desktop
# kde_module_LTLIBRARIES = kwin3_qindustrial.la
# kwin3_qindustrial_la_SOURCES = windecoration.cpp
# kwin3_qindustrial_la_LIBADD = $(kde_libraries)/libkdecorations.la
# kwin3_qindustrial_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -lkdecore -module
# kwin3_qindustrial_la_METASOURCES = AUTO

EXTRA_DIST = \
	$(stylerc_DATA)		\
	$(colorrc_DATA)		\
	qindustrial.themerc	\
	checkbox-on.xpm		\
	checkbox-off.xpm	\
	checkbox-mixed.xpm	\
	radio-on.xpm		\
	radio-off.xpm		\
	radio-mixed.xpm		\
	table-checkbox-on.xpm	\
	table-checkbox-off.xpm	\
	table-checkbox-mixed.xpm\
	menu-checkbox-on.xpm	\
	menu-checkbox-off.xpm	\
	menu-checkbox-mixed.xpm	\
	menu-radio-on.xpm	\
	menu-radio-off.xpm	\
	menu-radio-mixed.xpm	\
	menu-check-8.xpm	\
	menu-check-11.xpm	\
	menu-check-13.xpm

DISTCLEANFILES = $(METASOURCES)

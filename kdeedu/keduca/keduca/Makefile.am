# set the include path for X, qt and KDE
INCLUDES= -I../libkeduca $(all_includes)

# The binary program keduca
bin_PROGRAMS   = keduca
keduca_LDFLAGS = $(all_libraries)
keduca_LDADD   = $(LIB_KFILE) $(LIB_KPARTS) ../libkeduca/libkeduca.la
keduca_SOURCES = main.cpp keduca.cpp 

# Convenience library
#noinst_LTLIBRARIES = libkeduca_common.la

#libkeduca_common_la_SOURCES = kradioeduca.cpp kquestion.cpp kgroupeduca.cpp keducaview.cpp \
#                              keducaprefs.cpp kcheckeduca.cpp
#libkeduca_common_la_LDFLAGS = $(all_libraries) -no-undefined
#libkeduca_common_la_LIBADD  = ../libkeduca/libkeduca.la

# The KParts Component
kde_module_LTLIBRARIES = libkeducapart.la
libkeducapart_la_LDFLAGS = $(KDE_PLUGIN) $(all_libraries)
libkeducapart_la_LIBADD  = ../libkeduca/libkeduca.la $(LIB_KPARTS)
libkeducapart_la_SOURCES = keduca_part.cpp kradioeduca.cpp kquestion.cpp \
                           kgroupeduca.cpp keducaview.cpp keducaprefs.cpp \
                           kcheckeduca.cpp settings.kcfgc configdialogbase.ui

EXTRA_DIST = main.cpp keduca.cpp keduca.h kcheckeduca.cpp kcheckeduca.h keducaprefs.cpp \
keducaprefs.h keducaui.rc keducaview.cpp keducaview.h kgroupeduca.cpp kgroupeduca.h kquestion.cpp \
kquestion.h kradioeduca.cpp kradioeduca.h

METASOURCES = AUTO

rcdir = $(kde_datadir)/keduca
rc_DATA = keducaui.rc keduca_partui.rc

# Install the Part's desktop file
kde_services_DATA = keduca_part.desktop

kde_kcfg_DATA = keduca.kcfg

include ../../admin/Doxyfile.am

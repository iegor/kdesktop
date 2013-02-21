INCLUDES= -I$(srcdir)/../common -I$(srcdir)/../kde-qt-common $(all_includes)

kde_module_LTLIBRARIES = kcm_lilo.la

kcm_lilo_la_SOURCES = kcontrol.cpp Details.cpp InputBox.cpp
kcm_lilo_la_LDFLAGS  = $(all_libraries) -module -avoid-version -no-undefined
kcm_lilo_la_LIBADD = ../kde-qt-common/libwidgets.la $(LIB_KIO)
kcm_lilo_la_METASOURCES = AUTO
AM_CXXFLAGS = -DUSE_KDE

noinst_HEADERS = Details.h InputBox.h

xdg_apps_DATA = lilo.desktop

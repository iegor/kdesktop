INCLUDES= $(all_includes)

kde_module_LTLIBRARIES = noatuncharlatan.la

noatuncharlatan_la_SOURCES = \
  charlatanui.cpp \
  seeker.cpp \
  userinterface.cpp \
  configmodule.cpp

noatuncharlatan_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module

noatuncharlatan_la_LIBADD = -lnoatuncontrols -lnoatun

noatuncharlatan_la_METASOURCES = AUTO

noinst_HEADERS = userinterface.h configmodule.h seeker.h

noatun_DATA = charlatanui.plugin
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/charlatanui.pot

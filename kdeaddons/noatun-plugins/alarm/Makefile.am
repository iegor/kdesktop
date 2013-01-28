INCLUDES= -I$(kde_includes)/arts $(all_includes)
kde_module_LTLIBRARIES = noatunwakeup.la

noatunwakeup_la_SOURCES = wakeup.cpp \
	kminutespinbox.cpp \
	kpercentspinbox.cpp

noatunwakeup_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
noatunwakeup_la_LIBADD = $(LIB_KFILE) -lnoatun -lm

noatunwakeup_la_METASOURCES = AUTO

noinst_HEADERS = wakeup.h \
	kminutespinbox.h \
	kpercentspinbox.h

noatun_DATA = wakeup.plugin
noatundir = $(kde_datadir)/noatun

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/wakeup.pot

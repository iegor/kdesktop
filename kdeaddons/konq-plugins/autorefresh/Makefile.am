kde_module_LTLIBRARIES = libautorefresh.la

INCLUDES = $(all_includes)

libautorefresh_la_METASOURCES = AUTO
libautorefresh_la_SOURCES = autorefresh.cpp
libautorefresh_la_LIBADD =  $(LIB_KHTML)

libautorefresh_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

KDE_ICON = autorefresh

autorefreshdir = $(kde_datadir)/khtml/kpartplugins
autorefresh_DATA = autorefresh.rc autorefresh.desktop

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
		 $(XGETTEXT) $$LIST -o $(podir)/autorefresh.pot; \
	fi


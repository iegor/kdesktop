
lib_LTLIBRARIES = libkspy.la
libkspy_la_SOURCES = navviewitem.cpp propsview.cpp navview.cpp spy.cpp sigslotview.cpp \
		receiversview.cpp classinfoview.cpp

include_HEADERS = kspy.h

noinst_HEADERS = spy.h navview.h propsview.h navviewitem.h receiversview.h classinfoview.h

libkspy_la_LIBADD   =  $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI)  $(LIBSOCKET)

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

libkspy_la_LDFLAGS = $(all_libraries) -version-info 3:0:2 -no-undefined $(USER_LDFLAGS)

EXTRA_DIST = 	main.cpp spy.cpp spy.h navview.cpp navview.h propsview.cpp \
		propsview.h navviewitem.cpp navviewitem.h sigslotview.h receiversview.h classinfoview.h

METASOURCES = AUTO

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/spy.pot; \
	fi

check_PROGRAMS = testkspy
testkspy_SOURCES = main.cpp
testkspy_LDADD = libkspy.la

bin_PROGRAMS = keducabuilder
keducabuilder_SOURCES = kcontroladdeditbase.ui keducaeditorstartdialogbase.ui keducaeditorstartdialog.cpp ktagcombobox.cpp klangcombo.cpp kcontrolheader.cpp keducabuilder.cpp kcontroladdedit.cpp  main.cpp
keducabuilder_LDADD   =  $(LIB_KFILE) ../libkeduca/libkeduca.la 

EXTRA_DIST = main.cpp kcontroladdedit.cpp kcontroladdedit.h keducabuilder.cpp keducabuilder.h kcontrolheader.cpp kcontrolheader.h keducabuilderui.rc klangcombo.cpp klangcombo.h ktagcombobox.cpp ktagcombobox.h keducaeditorstartdialog.cpp keducaeditorstartdialog.h 

# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= -I../libkeduca $(all_includes)

METASOURCES = AUTO

# the library search path. 
keducabuilder_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDE´s XML GUI builing
rcdir = $(kde_datadir)/keduca
rc_DATA = keducabuilderui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/keduca.pot; \
	fi

include ../../admin/Doxyfile.am

bin_PROGRAMS = kpercentage
kpercentage_SOURCES =  kanimation.cpp kanswer.cpp kpercentmain.cpp  kpercentage.cpp ksplashscreen.cpp main.cpp
kpercentage_LDADD   =  $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = icons pics

EXTRA_DIST = main.cpp kpercentage.cpp kpercentage.h kpercentmain.cpp kanswer.h kanswer.cpp right.txt wrong.txt kpercentage.desktop ksplashscreen.cpp ksplashscreen.h kanimation.cpp kanimation.h

text_DATA = right.txt wrong.txt
textdir = $(kde_datadir)/kpercentage

# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to.
xdg_apps_DATA = kpercentage.desktop
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
INCLUDES= $(all_includes)

METASOURCES = AUTO


# the library search path.
kpercentage_LDFLAGS = $(all_libraries) $(KDE_RPATH)


stringstocpp:
	rm -f feedback_i18n.cpp; \
	echo "# right.txt" >> feedback_i18n.cpp ;\
	cat right.txt | sed -e 's#^\(.*\)#i18n(\"\1\")#' >> feedback_i18n.cpp ;\
	echo "# wrong.txt" >> feedback_i18n.cpp ;\
	cat wrong.txt | sed -e 's#^\(.*\)#i18n(\"\1\")#' >> feedback_i18n.cpp


messages: stringstocpp rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kpercentage.pot; \
	fi


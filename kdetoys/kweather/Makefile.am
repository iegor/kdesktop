SUBDIRS = graphics

METASOURCES = AUTO
INCLUDES = $(all_includes)

check_PROGRAMS = sun_test stationdatabase_test metar_parser_test
bin_PROGRAMS = kweatherservice
lib_LTLIBRARIES =
kdeinit_LTLIBRARIES = kweatherreport.la
kde_module_LTLIBRARIES = weather_panelapplet.la kcm_weather.la kcm_weatherservice.la

sun_test_SOURCES = sun_test.cpp sun.cpp
sun_test_LDFLAGS = $(all_libraries)
sun_test_LDADD = -lqt-mt

stationdatabase_test_SOURCES = stationdatabase_test.cpp stationdatabase.cpp
stationdatabase_test_LDFLAGS = $(all_libraries)
stationdatabase_test_LDADD = -lqt-mt -lkdecore

metar_parser_test_SOURCES = metar_parser_test.cpp metar_parser.cpp\
                            stationdatabase.cpp sun.cpp
metar_parser_test_LDFLAGS = $(all_libraries)
metar_parser_test_LDADD = -lqt-mt -lkdecore

weather_panelapplet_la_SOURCES = reportview.cpp kweather.cpp dockwidget.cpp \
                                 weatherIface.skel weatherservice.stub \
                                 weatherbutton.cpp
weather_panelapplet_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
weather_panelapplet_la_LIBADD = $(LIB_KHTML)

appletdir = $(kde_datadir)/kicker/applets
applet_DATA = kweather.desktop

stationdir = $(kde_datadir)/kweatherservice
station_DATA = stations.dat

kcm_weather_la_SOURCES = kcmweather.cpp prefdialogdata.ui weatherservice.stub
kcm_weather_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) \
                         -avoid-version -no-undefined
kcm_weather_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO)

kcm_weatherservice_la_SOURCES = kcmweatherservice.cpp serviceconfigwidget.cpp \
                                serviceconfigdata.ui weatherservice.stub
kcm_weatherservice_la_LDFLAGS = -module $(KDE_PLUGIN) $(KDE_RPATH) $(all_libraries) \
                                -avoid-version -no-undefined
kcm_weatherservice_la_LIBADD = $(LIB_KDEUI)

#weather_sidebar_la_SOURCES = sidebarwidgetbase.ui sidebarwidget.cpp dockwidget.cpp weatherbar.cpp weatherbar.skel
#weather_sidebar_la_LDFLAGS = $(all_libraries) -module -no-undefined
#weather_sidebar_la_LIBADD = $(LIB_KPARTS) $(LIB_KFILE) -lkonqsidebarplugin

#globalentrydir   = $(kde_datadir)/konqsidebartng/entries
#globalentry_DATA = weatherbar.desktop

#globaladddir   = $(kde_datadir)/konqsidebartng/add
#globaladd_DATA = weatherbar_add.desktop

kweatherservice_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kweatherservice_LDADD = $(LIB_KIO)
kweatherservice_SOURCES = main.cpp metar_parser.cpp weatherlib.cpp \
                          weatherservice.cpp weatherservice.skel \
                          stationdatabase.cpp sun.cpp weatherservice.stub

service_DATA = kweatherservice.desktop
servicedir = $(kde_servicesdir)

kde_services_DATA = kcmweather.desktop kcmweatherservice.desktop

kweatherreport_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version
kweatherreport_la_LIBADD = $(LIB_KHTML) reportview.lo
kweatherreport_la_SOURCES = reportmain.cpp weatherservice.stub

KDE_ICON = kweather

kweatherdatadir = $(kde_datadir)/kweatherservice
kweatherdata_DATA = weather_stations.desktop

messages: rc.cpp
	$(EXTRACTRC) *.ui > rc.cpp
	$(XGETTEXT) rc.cpp *.cpp *.h -o $(podir)/kweather.pot
	rm -f rc.cpp

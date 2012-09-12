# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = status.h katapultdisplay.h katapultcatalog.h katapultaction.h \
	katapultitem.h cachedcatalog.h actionregistry.h match.h imagedisplay.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

lib_LTLIBRARIES = libkatapult.la

# common library
libkatapult_la_SOURCES = katapultitem.cpp katapultaction.cpp actionregistry.cpp katapultcatalog.cpp cachedcatalog.cpp match.cpp katapultdisplay.cpp imagedisplay.cpp imagedisplaysettings.ui
libkatapult_la_LDFLAGS = $(all_libraries) -version-info 2:0:0

# this is where the service type files will go
kde_servicetypes_DATA = katapultdisplay.desktop katapultcatalog.desktop

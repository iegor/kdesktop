INCLUDES = -I$(top_srcdir) $(all_includes)
METASOURCES = AUTO

check_PROGRAMS = sampletests sampletestsgui

sampletests_SOURCES = main.cpp sampletest.cpp
sampletests_LDFLAGS = $(KDE_RPATH) $(all_libraries)
sampletests_LDADD = -lkunittest

sampletestsgui_SOURCES = maingui.cpp sampletest.cpp
sampletestsgui_LDFLAGS = $(KDE_RPATH) $(all_libraries)
# Normally you would write -lkunittestgui here, but since the examples
# are bundled with the library source code itself we don't want to
# have you install the libraries before you can compile the examples.
sampletestsgui_LDADD = ../../libkunittestgui.la

noinst_HEADERS = sampletest.h

TESTS = sampletests

guicheck: sampletestsgui
	kunittest ./sampletestsgui SampleTests

INCLUDES = -I$(top_srcdir)/kview $(all_includes)

METASOURCES = AUTO

check_PROGRAMS = test

test_SOURCES = main.cpp test.cpp
test_LDFLAGS = $(KDE_RPATH) $(all_libraries)
test_LDADD   = $(LIB_KPARTS)

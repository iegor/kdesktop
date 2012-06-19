# this might be a naive way of seeing it, but
# automake does not support C++ flex files.
#LEX_OUTPUT_ROOT = lex.GettextBase
#AM_LFLAGS = -+

CLEANFILES = pofiles.cc

noinst_LTLIBRARIES = libgettext.la

libgettext_la_SOURCES = pofiles.cc

pofiles.cc: $(srcdir)/pofiles.ll
	$(LEX) -+ -opofiles.cc $(srcdir)/pofiles.ll

noinst_HEADERS = tokens.h pofiles.h

## kdeartwork/Makefile.am
## (C) 1997-2001 Stephan Kulow

AUTOMAKE_OPTIONS = foreign 1.6.1

EXTRA_DIST = admin
dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/admin/am_edit -padmin

include admin/deps.am

SUBDIRS=$(TOPSUBDIRS)

## kdegraphics/Makefile.am $Id: Makefile.am.in 571342 2006-08-09 10:30:55Z mueller $
## (C) 1997 Stephan Kulow

AUTOMAKE_OPTIONS = foreign 1.6.1

# COMPILE_BEFORE_kooka = libkscan
# COMPILE_BEFORE_kfaxview = kfax
# COMPILE_BEFORE_kfile-plugins = kghostview
# COMPILE_AFTER_kviewshell = kdvi kfaxview

DISTCLEANFILES = inst-apps

EXTRA_DIST = admin debian kdebase.spec.in README.pam kde.pamd 

dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/admin/am_edit -padmin

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 SUBDIRS

include admin/Doxyfile.am
include admin/deps.am

SUBDIRS=$(TOPSUBDIRS)

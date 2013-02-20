if include_XSCREENSAVER
KXSCONFIG_SUBDIR = kxsconfig
endif

if COMPILE_X11_KSAVERS
XSAVERS_SUBDIR = xsavers
endif

SUBDIRS = kdesavers kpartsaver $(KXSCONFIG_SUBDIR) $(XSAVERS_SUBDIR)

messages:
	$(EXTRACTRC) kdesavers/*.ui > rc.cpp
	$(XGETTEXT) kdesavers/*.cpp kdesavers/*.h xsavers/*.cpp xsavers/*.h rc.cpp -o $(podir)/klock.pot
	rm -f rc.cpp

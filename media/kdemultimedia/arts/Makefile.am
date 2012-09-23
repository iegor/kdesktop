if arts_within_KDE
ARTS_BUILD_KDE=builder tools
endif

SUBDIRS = runtime midi gui modules examples $(ARTS_BUILD_KDE)
DIST_SUBDIRS = runtime midi gui modules examples builder tools

messages:
	$(XGETTEXT) modules/*/*.cpp modules/*/*.cc gui/*/*.cpp gui/*/*.cc -o $(podir)/artsmodules.pot

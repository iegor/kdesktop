### Makefile for the popmail conduit
###

INCLUDES= $(PISOCK_INCLUDE) -I$(top_srcdir)/kpilot/lib $(all_includes)
METASOURCES = AUTO

servicedir  = $(kde_servicesdir)
service_DATA = popmail-conduit.desktop
kde_kcfg_DATA = popmail.kcfg

####### This part is very kpilot specific
# you can add here more. This one gets installed 
kde_module_LTLIBRARIES = conduit_popmail.la

# Which sources should be compiled for popmail_conduit
conduit_popmail_la_SOURCES = popmailSettings.kcfgc setup-dialog.ui \
	popmail-factory.cc setupDialog.cc \
	popmail-conduit.cc
conduit_popmail_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
conduit_popmail_la_LIBADD  =  ../../lib/libkpilot.la $(LIB_KIO)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = popmail-conduit.h setupDialog.h


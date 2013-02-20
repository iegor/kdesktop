AM_CPPFLAGS = -UQT_NO_ASCII_CAST -DQT_CLEAN_NAMESPACE

INCLUDES = $(all_includes) $(GLINC)
AM_LDFLAGS = $(all_libraries) $(KDE_RPATH)

if COMPILE_GL_XSAVERS
GL_XSAVERS=kspace.kss
endif

bin_PROGRAMS = kswarm.kss $(GL_XSAVERS) 

BASE_SRCS = main.cpp demowin.cpp saver.cpp helpers.cpp

XS_SRCS = xs_colors.c xs_hsv.c xs_visual.c
XL_SRCS = xlock.cpp

kswarm_kss_SOURCES = swarm.cpp $(BASE_SRCS) $(XL_SRCS) 
kswarm_kss_LDADD = $(LIB_KDEUI) -lm

kspace_kss_SOURCES = space.cpp $(BASE_SRCS) $(XL_SRCS)
kspace_kss_LDADD = $(LIB_KDEUI) $(GLLIB) -lm

METASOURCES = AUTO

noinst_HEADERS = saver.h demowin.h xlock.h swarm.h space.h \
		xlockmore.h mode.h helpers.h \
		xs_colors.h xs_hsv.h xs_yarandom.h xs_utils.h \
		xs_visual.h

########## Meta objects ##########

desktop_DATA = KSpace.desktop KSwarm.desktop
desktopdir = $(kde_appsdir)/System/ScreenSavers

EXTRA_DIST = $(desktop_DATA)


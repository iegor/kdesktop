# $Id: Makefile.am 468702 2005-10-09 03:57:34Z jriddell $
# Makefile.am for kscreensaver.
#

SUBDIRS = . data

AM_CPPFLAGS = -UQT_NO_ASCII_CAST

INCLUDES = $(all_includes) $(GLINC) $(LIBART_CFLAGS)
AM_LDFLAGS = $(all_libraries) $(KDE_RPATH)
AM_LDADD = $(LIB_KDEUI) -lkscreensaver

if COMPILE_X11_KSAVERS
X11_KSAVERS = kscience.kss
X11_DESKTOP_FILES = KScience.desktop
endif

if COMPILE_LIBART_KSAVERS
LIBART_KSAVERS = kclock.kss
LIBART_DESKTOP_FILES = KClock.desktop
endif
LIBART_KSAVERS_LDADD = $(AM_LDADD) $(LIBART_LIBS)

if COMPILE_ARTS_GL_KSAVERS
ARTS_GL_KSAVERS = kfiresaver.kss
ARTS_GL_DESKTOP_FILES = KFiresaver.desktop
endif

if COMPILE_GL_KSAVERS
GL_KSAVERS=kfountain.kss kwave.kss kgravity.kss kflux.kss keuphoria.kss ksolarwinds.kss krotation.kss kpendulum.kss
GL_DESKTOP_FILES=KGravity.desktop KWave.desktop KFountain.desktop KFlux.desktop KEuphoria.desktop KSolarWinds.desktop KRotation.desktop KPendulum.desktop
endif
GL_KSAVERS_LDADD = $(AM_LDADD) $(GLLIB)

bin_PROGRAMS = kbanner.kss kpolygon.kss kslideshow.kss \
                klines.kss klorenz.kss kblob.kss kvm.kss \
                $(X11_KSAVERS) $(LIBART_KSAVERS) $(ARTS_GL_KSAVERS) $(GL_KSAVERS)

# The "normal" screensavers
kbanner_kss_SOURCES = banner.cpp
kbanner_kss_LDADD = $(AM_LDADD)
kpolygon_kss_SOURCES = polygon.cpp
kpolygon_kss_LDADD = $(AM_LDADD)
kslideshow_kss_SOURCES = slideshow.cpp slideshowcfg.ui
kslideshow_kss_LDADD = $(LIB_KFILE) $(AM_LDADD) $(LIB_KSYCOCA) -lm
klines_kss_SOURCES = lines.cpp
klines_kss_LDADD = $(AM_LDADD)
klorenz_kss_SOURCES = lorenz.cpp
klorenz_kss_LDADD = $(AM_LDADD) -lm
kblob_kss_SOURCES = blob.cpp
kblob_kss_LDADD = $(AM_LDADD) -lm
kvm_kss_SOURCES = kvm.cpp vm.c vm_random.c
kvm_kss_LDADD = $(AM_LDADD)
kscience_kss_SOURCES = science.cpp
kscience_kss_LDADD = $(AM_LDADD) -lm

# The GL screensavers
kfountain_kss_SOURCES = fountain.cpp fountaincfg.ui
kfountain_kss_LDADD = $(GL_KSAVERS_LDADD) -lm
kwave_kss_SOURCES = wave.cpp wavecfg.ui
kwave_kss_LDADD = $(GL_KSAVERS_LDADD) -lm
kgravity_kss_SOURCES = gravity.cpp gravitycfg.ui
kgravity_kss_LDADD = $(GL_KSAVERS_LDADD) -lm
kflux_kss_SOURCES = Flux.cpp
kflux_kss_LDADD = $(GL_KSAVERS_LDADD) -lm
keuphoria_kss_SOURCES = Euphoria.cpp
keuphoria_kss_LDADD = $(GL_KSAVERS_LDADD) -lm
ksolarwinds_kss_SOURCES = SolarWinds.cpp
ksolarwinds_kss_LDADD = $(GL_KSAVERS_LDADD) -lm
krotation_kss_SOURCES = rotation.cpp rotationcfg.ui sspreviewarea.cpp rkodesolver.cpp vec3.cpp
krotation_kss_LDADD = $(GL_KSAVERS_LDADD) -lm
kpendulum_kss_SOURCES = pendulum.cpp pendulumcfg.ui sspreviewarea.cpp rkodesolver.cpp
kpendulum_kss_LDADD = $(GL_KSAVERS_LDADD) -lm
kfiresaver_kss_SOURCES = firesaversetup.ui firesaverparticle.cpp firesaverwriter.cpp firesaver.cpp
kfiresaver_kss_LDADD = $(GL_KSAVERS_LDADD) -lartskde -lm

# The LIBART screensavers
kclock_kss_SOURCES = kclock.cpp
kclock_kss_LDADD = $(LIBART_KSAVERS_LDADD)

METASOURCES = AUTO

noinst_HEADERS = banner.h polygon.h slideshow.h lines.h\
                 lorenz.h blob.h kvm.h vm.h vm_random.h science.h \
                 fountain.h wave.h gravity.h kclock.h\
                 rotation.h pendulum.h sspreviewarea.h rkodesolver.h vec3.h\
                 firesaver.h firesaverparticle.h firesaversetup.ui.h\
                 firesaverwriter.h

########## Meta objects ##########

pics_DATA = kscience.png particle.png image.png
picsdir = $(kde_datadir)/kscreensaver

desktop_DATA = KBanner.desktop KPolygon.desktop \
               KSlideshow.desktop KLines-saver.desktop KLorenz.desktop \
               KBlob.desktop KVm.desktop \
               KClock.desktop $(X11_DESKTOP_FILES) $(GL_DESKTOP_FILES) $(LIBART_DESKTOP_FILES) $(ARTS_GL_DESKTOP_FILES)

desktopdir = $(kde_appsdir)/System/ScreenSavers

EXTRA_DIST = $(desktop_DATA) vm.xpm vm.xbm kscience.png  particle.png image.png

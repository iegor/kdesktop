INCLUDES = -I$(top_builddir)/libksirtet -I$(top_srcdir)/libksirtet -I$(srcdir)/../base -I$(top_srcdir)/libkdegames/highscore -I$(top_srcdir)/libkdegames $(all_includes)

# Don't compile with hidden symbols since we are a library.
if disable_VISIBILITY
KDE_CXXFLAGS = -fvisibility=default
endif

noinst_LTLIBRARIES             = libksirtetcommon.la
libksirtetcommon_la_LDFLAGS = $(all_libraries) -no-undefined
libksirtetcommon_la_DEPENDENCIES = $(LIB_KDEGAMES_DEP) $(top_builddir)/libksirtet/lib/libksirtetmultiplayers.la $(top_builddir)/libksirtet/base/libksirtetbase.la
libksirtetcommon_la_LIBADD  = $(LIB_KDEGAMES) $(top_builddir)/libksirtet/lib/libksirtetmultiplayers.la $(top_builddir)/libksirtet/base/libksirtetbase.la

noinst_HEADERS              = types.h factory.h misc_ui.h highscores.h \
                              board.h ai.h field.h settings.h inter.h main.h
libksirtetcommon_la_SOURCES = types.cpp factory.cpp misc_ui.cpp \
                              highscores.cpp \
                              board.cpp ai.cpp field.cpp settings.cpp \
                              inter.cpp main.cpp commonprefs.kcfgc
METASOURCES                 = misc_ui.moc board.moc ai.moc field.moc \
                              settings.moc inter.moc main.moc

ai.lo: ../base/baseprefs.h
board.lo: ../base/baseprefs.h
commonprefs.lo: ../base/baseprefs.h
field.lo: ../base/baseprefs.h
inter.lo: ../base/baseprefs.h
misc_ui.lo: ../base/baseprefs.h


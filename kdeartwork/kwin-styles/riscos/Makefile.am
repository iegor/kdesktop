AUTOMAKE_OPTIONS = foreign

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes) 

kwindir = $(kde_datadir)/kwin/
kwin_DATA = riscos.desktop

noinst_HEADERS = AboveButton.h \
                 Button.h \
                 CloseButton.h \
                 HelpButton.h \
                 IconifyButton.h \
                 LowerButton.h \
                 Manager.h \
                 MaximiseButton.h \
                 Palette.h \
                 Static.h \
                 StickyButton.h

kde_module_LTLIBRARIES = kwin3_riscos.la
kwin3_riscos_la_SOURCES = AboveButton.cpp \
                          Button.cpp \
                          CloseButton.cpp \
                          HelpButton.cpp \
                          IconifyButton.cpp \
                          LowerButton.cpp \
                          Manager.cpp \
                          MaximiseButton.cpp \
                          Static.cpp \
                          StickyButton.cpp

kwin3_riscos_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin3_riscos_la_LIBADD = $(LIB_KDEUI) -lkdecorations
kwin3_riscos_la_METASOURCES = AUTO

EXTRA_DIST = $(kwin_DATA)


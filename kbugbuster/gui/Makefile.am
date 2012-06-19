INCLUDES= -I$(top_srcdir)/kbugbuster/backend -I$(top_srcdir)/kbugbuster/ $(all_includes)

noinst_LTLIBRARIES = libkbbmainwindow.la

libkbbmainwindow_la_SOURCES = packagelvi.cpp buglvi.cpp cwloadingwidget.cpp \
                              cwsearchwidget_base.ui cwsearchwidget.cpp \
                              cwbugdetailscontainer_base.ui \
                              cwbugdetailscontainer.cpp \
                              cwbuglistcontainer.cpp \
                              cwbugdetails.cpp \
                              centralwidget_base.ui centralwidget.cpp \
                              kbbmainwindow.cpp msginputdialog.cpp \
                              packageselectdialog.cpp messageeditor.cpp \
                              severityselectdialog.cpp \
                              preferencesdialog.cpp loadallbugsdlg.cpp \
                              serverconfigdialog.cpp

METASOURCES = AUTO

EXTRA_DIST = kbugbusterui.rc

rcdir = $(kde_datadir)/kbugbuster
rc_DATA = kbugbusterui.rc


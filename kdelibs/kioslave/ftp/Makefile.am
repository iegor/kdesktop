INCLUDES= $(all_includes)

####### Files

kde_module_LTLIBRARIES = kio_ftp.la

kio_ftp_la_SOURCES = ftp.cc
kio_ftp_la_LIBADD  = $(LIB_KIO)
kio_ftp_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) 

noinst_HEADERS = ftp.h

kdelnk_DATA = ftp.protocol
kdelnkdir = $(kde_servicesdir)



backends = \
	network-conf

perl_libs = \
	debug.pl     \
	file.pl      \
	general.pl   \
	network.pl   \
	parse.pl     \
	platform.pl  \
	process.pl   \
	replace.pl   \
	report.pl    \
	service.pl   \
	service-list.pl	\
	util.pl      \
	xml.pl

scripts_SCRIPTS = $(backends) $(perl_libs)

EXTRA_DIST = \
	system-tools-backends.pc.in \
	AUTHORS

CLEANFILES = $(backends) $(perl_libs) system-tools-backends.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = system-tools-backends.pc


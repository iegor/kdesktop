libwidgets.a: ../kde-qt-common/*.cpp ../kde-qt-common/*.h
	rm -f ../kde-qt-common/.COMPILED_KDE
	if ! test -e ../kde-qt-common/.COMPILED_QT; then \
	perl -pi -e "s,^AM_CXXFLAGS.*,AM_CXXFLAGS = -UUSE_KDE," ../kde-qt-common/Makefile.am; \
	perl -pi -e "s,libwidgets_la_LIBADD.*,libwidgets_la_LIBADD = -lqt ../common/libcommon.la," ../kde-qt-common/Makefile.am; \
	make -C ../kde-qt-common clean; fi
	make -C ../kde-qt-common
	install -c -m755 ../kde-qt-common/.libs/libwidgets.a .
	touch ../kde-qt-common/.COMPILED_QT

INCLUDES= -I../common -I../kde-qt-common $(all_includes)

bin_PROGRAMS = lilo-config

lilo_config_SOURCES = standalone.cpp Details.cpp InputBox.cpp
lilo_config_LDFLAGS = $(all_libraries)
lilo_config_LDADD = $(LIB_QT) libwidgets.a
lilo_config_METASOURCES = AUTO
AM_CXXFLAGS = -UUSE_KDE

KDE_OPTIONS = qtonly

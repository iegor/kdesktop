
SUBDIRS = libkeduca keduca keducabuilder resources

EXTRA_DIST = AUTHORS COPYING ChangeLog INSTALL README TODO keduca.lsm 

messages:
	$(EXTRACTRC) `find . -name *.rc` > rc.cpp
	$(EXTRACTRC) `find . -name *.ui` >> rc.cpp

	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
		$(XGETTEXT) $$LIST -o $(podir)/keduca.pot; \
	fi
	rm -f rc.cpp

DOXYGEN_REFERENCES = kdeui
include ../admin/Doxyfile.am

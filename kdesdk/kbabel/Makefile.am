AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = common filters datatools kbabeldict commonui catalogmanager kbabel addons

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.rc -o -name \*.ui` >> rc.cpp
	$(XGETTEXT) `find . -name \*.cpp -o -name \*.cc` -o $(podir)/kbabel.pot	


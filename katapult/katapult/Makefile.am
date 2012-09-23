SUBDIRS = common plugins katapult 

messages: rc.cpp
	$(EXTRACTRC) `find . -name "*.ui"` >> ./rc.cpp
	$(XGETTEXT) `find . -name "*.cpp"` -o $(podir)/katapult.pot

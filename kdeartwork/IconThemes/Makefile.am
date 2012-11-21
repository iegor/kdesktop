## kdeartwork/Makefile.am
## (C) 1997-2001 Stephan Kulow

install-exec-am: install-data-local

ICONTHEMES=Locolor ikons kdeclassic slick kids

# For Themes
themesdir =  $(kde_datadir)/kthememgr/Themes
install-data-local:
	@for theme in $(ICONTHEMES) ; do \
	    $(mkinstalldirs) $(DESTDIR)$(kde_icondir)/$$theme ; \
	    $(INSTALL_DATA) $(top_srcdir)/IconThemes/$$theme/index.theme $(DESTDIR)$(kde_icondir)/$$theme ; \
	    sizes=`cd $(top_srcdir)/IconThemes/$$theme && ls -1`; \
	    for size in $$sizes ; do \
	      if [ -d $(top_srcdir)/IconThemes/$$theme/$$size -a $$size != "CVS" -a $$size != ".svn" ]; then \
	        $(mkinstalldirs) $(DESTDIR)$(kde_icondir)/$$theme/$$size ; \
	        for group in actions apps devices filesystems mimetypes; do \
	          if [ -d $(top_srcdir)/IconThemes/$$theme/$$size/$$group ]; then \
	              echo "installing $$theme/$$size/$$group" ; \
	            $(mkinstalldirs) $(DESTDIR)$(kde_icondir)/$$theme/$$size/$$group ; \
		    pngs=`cd $(top_srcdir)/IconThemes/$$theme/$$size/$$group && ls -1 *.png 2> /dev/null`; \
		    for png in $$pngs; do $(INSTALL_DATA) $(top_srcdir)/IconThemes/$$theme/$$size/$$group/$$png \
	                 $(DESTDIR)$(kde_icondir)/$$theme/$$size/$$group/$$png ; done; \
		    mngs=`cd $(top_srcdir)/IconThemes/$$theme/$$size/$$group && ls -1 *.mng 2> /dev/null`; \
		    for mng in $$mngs; do \
		         $(INSTALL_DATA) $(top_srcdir)/IconThemes/$$theme/$$size/$$group/$$mng \
			 $(DESTDIR)$(kde_icondir)/$$theme/$$size/$$group/$$mng ; done; \
		  fi ; \
	        done ; \
	      fi ; \
	    done ; \
	  done


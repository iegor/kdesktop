edu_folder = $(kde_appsdir)/Edutainment

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(edu_folder)/Languages
	$(INSTALL_DATA) $(srcdir)/languages.desktop $(DESTDIR)$(edu_folder)/Languages/.directory
	
	$(mkinstalldirs) $(DESTDIR)$(edu_folder)/Mathematics
	$(INSTALL_DATA) $(srcdir)/mathematics.desktop $(DESTDIR)$(edu_folder)/Mathematics/.directory
	
	$(mkinstalldirs) $(DESTDIR)$(edu_folder)/Science
	$(INSTALL_DATA) $(srcdir)/science.desktop $(DESTDIR)$(edu_folder)/Science/.directory
	
	$(mkinstalldirs) $(DESTDIR)$(edu_folder)/Tools
	$(INSTALL_DATA) $(srcdir)/tools.desktop $(DESTDIR)$(edu_folder)/Tools/.directory

	$(mkinstalldirs) $(DESTDIR)$(edu_folder)/Miscellaneous
	$(INSTALL_DATA) $(srcdir)/miscellaneous.desktop $(DESTDIR)$(edu_folder)/Miscellaneous/.directory

uninstall-local:
	-rm -f $(DESTDIR)$(kde_appsdir)/Settings/Databases/.directory
	-rm -f $(DESTDIR)$(edu_folder)/Languages/.directory
	-rm -f $(DESTDIR)$(edu_folder)/Mathematics/.directory
	-rm -f $(DESTDIR)$(edu_folder)/Sience/.directory
	-rm -f $(DESTDIR)$(edu_folder)/Tools/.directory
	-rm -f $(DESTDIR)$(edu_folder)/Miscellaneous/.directory

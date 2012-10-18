

EXTRA_DIST = kaffeine_open_directory.desktop kaffeine_append_file.desktop kaffeine_play_audiocd.desktop kaffeine_play_dvd.desktop kaffeine_play_vcd.desktop

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/konqueror/servicemenus
	$(INSTALL_DATA) $(srcdir)/kaffeine_open_directory.desktop $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_open_directory.desktop
	$(INSTALL_DATA) $(srcdir)/kaffeine_append_file.desktop $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_append_file.desktop
	$(INSTALL_DATA) $(srcdir)/kaffeine_play_audiocd.desktop $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_play_audiocd.desktop
	$(INSTALL_DATA) $(srcdir)/kaffeine_play_dvd.desktop $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_play_dvd.desktop
	$(INSTALL_DATA) $(srcdir)/kaffeine_play_vcd.desktop $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_play_vcd.desktop

uninstall-local:
	-rm -f $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_open_directory.desktop
	-rm -f $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_append_file.desktop
	-rm -f $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_play_audiocd.desktop
	-rm -f $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_play_dvd.desktop
	-rm -f $(DESTDIR)$(kde_datadir)/konqueror/servicemenus/kaffeine_play_vcd.desktop


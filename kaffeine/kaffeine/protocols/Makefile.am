

EXTRA_DIST = mms.protocol rtsp.protocol

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kaffeine
	$(INSTALL_DATA) $(srcdir)/mms.protocol $(DESTDIR)$(kde_datadir)/kaffeine/mms.protocol
	$(INSTALL_DATA) $(srcdir)/rtsp.protocol $(DESTDIR)$(kde_datadir)/kaffeine/rtsp.protocol

uninstall-local:
	-rm -f $(DESTDIR)$(kde_datadir)/kaffeine/mms.protocol
	-rm -f $(DESTDIR)$(kde_datadir)/kaffeine/rtsp.protocol



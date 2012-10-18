EXTRA_DIST = kaffeine16x16.png kaffeine22x22.png kaffeine32x32.png \
	kaffeine48x48.png kaffeine64x64.png logo.mpg logo.avi kaffeine128x128.png \
	kaffeine-play.png kaffeine-pause.png kaffeine-record.png player_record16x16.png \
	player_record22x22.png player_record16x16.png mime_kaffeine32x32.png \
	mime_kaffeine32x32.png mime_kaffeine48x48.png player_mute16x16.png \
	player_mute22x22.png player_mute32x32.png kdvbtv.png kdvbtvc.png \
	kdvbra.png kdvbrac.png dvbdata.tar.gz nocover.png kilogram16x16.png kilogram22x22.png kilogram32x32.png

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/kaffeine16x16.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kaffeine.png
	$(INSTALL_DATA) $(srcdir)/kdvbtv.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kdvbtv.png
	$(INSTALL_DATA) $(srcdir)/kdvbtvc.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kdvbtvc.png
	$(INSTALL_DATA) $(srcdir)/kdvbra.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kdvbra.png
	$(INSTALL_DATA) $(srcdir)/kdvbrac.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kdvbrac.png
	$(INSTALL_DATA) $(srcdir)/kilogram16x16.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kilogram.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/16x16/mimetypes/
	$(INSTALL_DATA) $(srcdir)/mime_kaffeine16x16.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/mimetypes/mime_kaffeine.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/16x16/actions/
	$(INSTALL_DATA) $(srcdir)/player_record16x16.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/actions/player_record.png
	$(INSTALL_DATA) $(srcdir)/player_mute16x16.png $(DESTDIR)$(kde_icondir)/hicolor/16x16/actions/player_mute.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/22x22/apps/
	$(INSTALL_DATA) $(srcdir)/kaffeine22x22.png $(DESTDIR)$(kde_icondir)/hicolor/22x22/apps/kaffeine.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/22x22/actions/
	$(INSTALL_DATA) $(srcdir)/player_record22x22.png $(DESTDIR)$(kde_icondir)/hicolor/22x22/actions/player_record.png
	$(INSTALL_DATA) $(srcdir)/player_mute22x22.png $(DESTDIR)$(kde_icondir)/hicolor/22x22/actions/player_mute.png
	$(INSTALL_DATA) $(srcdir)/kilogram22x22.png $(DESTDIR)$(kde_icondir)/hicolor/22x22/apps/kilogram.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/kaffeine-play.png $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/kaffeine-play.png
	$(INSTALL_DATA) $(srcdir)/kaffeine-pause.png $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/kaffeine-pause.png
	$(INSTALL_DATA) $(srcdir)/kaffeine-record.png $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/kaffeine-record.png
	$(INSTALL_DATA) $(srcdir)/kaffeine32x32.png $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/kaffeine.png
	$(INSTALL_DATA) $(srcdir)/kilogram32x32.png $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/kilogram.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/32x32/mimetypes/
	$(INSTALL_DATA) $(srcdir)/mime_kaffeine32x32.png $(DESTDIR)$(kde_icondir)/hicolor/32x32/mimetypes/mime_kaffeine.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/32x32/actions/
	$(INSTALL_DATA) $(srcdir)/player_record32x32.png $(DESTDIR)$(kde_icondir)/hicolor/32x32/actions/player_record.png
	$(INSTALL_DATA) $(srcdir)/player_mute32x32.png $(DESTDIR)$(kde_icondir)/hicolor/32x32/actions/player_mute.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/48x48/apps/
	$(INSTALL_DATA) $(srcdir)/kaffeine48x48.png $(DESTDIR)$(kde_icondir)/hicolor/48x48/apps/kaffeine.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/48x48/mimetypes/
	$(INSTALL_DATA) $(srcdir)/mime_kaffeine48x48.png $(DESTDIR)$(kde_icondir)/hicolor/48x48/mimetypes/mime_kaffeine.png
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/64x64/apps/
	$(INSTALL_DATA) $(srcdir)/kaffeine64x64.png $(DESTDIR)$(kde_icondir)/hicolor/64x64/apps/kaffeine.png
	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kaffeine/
	$(INSTALL_DATA) $(srcdir)/logo.mpg $(DESTDIR)$(kde_datadir)/kaffeine/logo.mpg
	$(INSTALL_DATA) $(srcdir)/logo.avi $(DESTDIR)$(kde_datadir)/kaffeine/logo.avi
	$(INSTALL_DATA) $(srcdir)/logo $(DESTDIR)$(kde_datadir)/kaffeine/logo
	$(mkinstalldirs) $(DESTDIR)$(kde_icondir)/hicolor/128x128/apps/
	$(INSTALL_DATA) $(srcdir)/kaffeine128x128.png $(DESTDIR)$(kde_icondir)/hicolor/128x128/apps/kaffeine.png
	$(INSTALL_DATA) $(srcdir)/dvbdata.tar.gz $(DESTDIR)$(kde_datadir)/kaffeine/dvbdata.tar.gz
	$(INSTALL_DATA) $(srcdir)/nocover.png $(DESTDIR)$(kde_datadir)/kaffeine/nocover.png

uninstall-local:
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kilogram.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/22x22/apps/kilogram.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/kilogram.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kaffeine.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kdvbtv.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kdvbtvc.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kdvbra.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/kdvbrac.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/player_record.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/16x16/apps/player_mute.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/22x22/apps/kaffeine.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/22x22/apps/kaffeine-play.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/22x22/apps/kaffeine-pause.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/22x2/apps/player_record.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/22x22/apps/player_mute.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/kaffeine.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/player_record.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/32x32/apps/player_mute.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/48x48/apps/kaffeine.png
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/64x64/apps/kaffeine.png
	-rm -f $(DESTDIR)$(kde_datadir)/kaffeine/logo.mpg
	-rm -f $(DESTDIR)$(kde_datadir)/kaffeine/logo.avi
	-rm -f $(DESTDIR)$(kde_datadir)/kaffeine/logo
	-rm -f $(DESTDIR)$(kde_icondir)/hicolor/128x128/apps/kaffeine.png
	-rm -f $(DESTDIR)$(kde_datadir)/kaffeine/dvbdata.tar.gz
	-rm -f $(DESTDIR)$(kde_datadir)/kaffeine/nocover.png

pics_DATA = playing.png
picsdir = $(kde_datadir)/kaffeine/pics

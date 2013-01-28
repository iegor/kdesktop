
# Install the .rc file in the Part's directory (in this case, the part
# is KHTMLPart)
#pluginsdir=$(kde_datadir)/kate/plugins/katekjswrapper
#plugins_DATA=ui.rc

plugins1dir=$(kde_datadir)/kate/plugins/katekjstest1
plugins1_DATA=katekjstest1.js

pluginsconsolewindowdir=$(kde_datadir)/kate/plugins/katekjsconsolewindow
pluginsconsolewindow_DATA=katekjsconsolewindow.js katekjsconsolewindow.rc

pluginsselectdir=$(kde_datadir)/kate/plugins/katekjsselect
pluginsselect_DATA=katekjsselect.js katekjsselect.rc

#plugins2dir = $(kde_datadir)/kate/plugins/katekjstest2
#plugins2_DATA = katekjstest2.js

kde_services_DATA = katekjstest1.desktop katekjsconsolewindow.desktop katekjsselect.desktop
#kde_services_DATA = katekjstest2.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kateinsertcommand.pot

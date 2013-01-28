#ifndef LYRICS_H
#define LYRICS_H

#include <noatun/plugin.h>
#include <noatun/player.h>
#include <noatun/app.h>
#include <kmainwindow.h>
#include <qvaluevector.h>
#include <khtml_part.h>
#include <kaction.h>
#include "cmodule.h"

class HistoryManager;

class Lyrics : public KMainWindow, public Plugin {
Q_OBJECT

public:
  Lyrics();
  ~Lyrics();
  void go(const KURL &);
  void setProviders( QValueVector<SearchProvider> &sites );

public slots:
  void viewLyrics(int index = -1);
  void back();
  void forward();

protected:
	bool queryClose();

protected slots:
  void changeUI(int, bool);
  void openURLRequest( const KURL &, const KParts::URLArgs & );
  void loadingURL(KIO::Job *);
  void loadedURL();
  void attach(bool);
  void newSong();
  void goTo();

private:
	int menuID;
  KAction *back_act, *forward_act;
  KToggleAction *follow_act;
  KToggleAction *attach_act;
  KSelectAction *site_act;
	KHTMLPart *htmlpart;
  QValueVector<SearchProvider> mSites;
  HistoryManager *history;
  bool active;

};


#endif

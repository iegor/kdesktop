/* Slide Show Screen Saver
 * (C) 1999 Stefan Taferner <taferner@kde.org>
 * (C) 2003 Sven Leiber <s.leiber@web.de>
 */


#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <qtimer.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>

#include <kscreensaver.h>
#include <kdialogbase.h>

#include "slideshowcfg.h"

class SlideShowCfg;

//-----------------------------------------------------------------------------
class kSlideShowSaver: public KScreenSaver
{
  Q_OBJECT
public:
  kSlideShowSaver( WId id );
  virtual ~kSlideShowSaver();

  typedef int (kSlideShowSaver::*EffectMethod)(bool);

  void readConfig();

  void restart();

protected slots:
  void slotTimeout();

protected:
  void blank();

  /** Load list of images from directory */
  virtual void loadDirectory();

  /** Helper for loadDirectory() */
  virtual void traverseDirectory(const QString &dirName);

  /** Load next image from list. If the file cannot be read
	   it is automatically removed from the file list.
		mImage contains the image after loading. */
  virtual void loadNextImage();

  /** Show next screen, completely, without transition. */
  virtual void showNextScreen();

  /** Set loaded image to next-screen buffer. */
  virtual void createNextScreen();

  /** Initialize next-screen buffer. */
  virtual void initNextScreen();

  /** Register effect methods in effect list. */
  virtual void registerEffects();

  /** Various effects. If adding one, do not forget to manually
	add the effect to the list in the registerEffects() method. */
  int effectHorizLines(bool doInit);
  int effectVertLines(bool doInit);
  int effectRandom(bool doInit);
  int effectGrowing(bool doInit);
  int effectChessboard(bool doInit);
  int effectIncomingEdges(bool doInit);
  int effectBlobs(bool doInit);
  int effectCircleOut(bool doInit);
  int effectSweep(bool doInit);
  int effectMeltdown(bool doInit);
  int effectSpiralIn(bool doInit);
  int effectMultiCircleOut(bool doInit);

protected:
  /** Init mPainter with next-screen's pixmap and call
      mPainter.begin(&mWidget) */
  void startPainter(Qt::PenStyle penStyle=NoPen);

protected:
  struct mScreenGeo {
    mScreenGeo(int w, int h, int x, int y) : mW(w), mH(h), mXorg(x), mYorg(y) {};
    int mW;
    int mH;
    int mXorg;
    int mYorg;
  };
  QPtrList<mScreenGeo> mGeoList;
  bool mEffectRunning;
  QTimer mTimer;
  int mColorContext;
  QStringList mFileList;
  QStringList mRandomList;
  int mFileIdx;
  QImage mImage;
  QPixmap mNextScreen;
  EffectMethod* mEffectList;
  EffectMethod mEffect;
  int mNumEffects;
  QPainter mPainter;
  QString mImageName;

  // config settings:
  bool mShowRandom;
  bool mZoomImages;
  bool mPrintName;
  bool mSubdirectory;
  bool mRandomPosition;
  int mDelay;
  QString mDirectory;

  // values for state of various effects:
  int mx, my, mw, mh, mdx, mdy, mix, miy, mi, mj, mSubType;
  int mx0, my0, mx1, my1, mwait;
  double mfx, mfy, mAlpha, mfd;
  int* mIntArray;
};


//-----------------------------------------------------------------------------
class kSlideShowSetup : public KDialogBase
{
  Q_OBJECT
public:
  kSlideShowSetup(QWidget *parent=NULL, const char *name=NULL);
  ~kSlideShowSetup();

protected:
  void readSettings();

protected slots:
  void slotOk();
  void slotHelp();
  void writeSettings();
  void slotDirSelected(const QString &where);

private:
  kSlideShowSaver *mSaver;
  SlideShowCfg *cfg;
};

#endif /*SLIDESHOW_H*/


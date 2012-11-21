/* Slide Show Screen Saver
 *  (C) 1999 Stefan Taferner <taferner@kde.org>
 *  (C) 2001 Martin R. Jones <mjones@kde.org>
 *  (C) 2003 Chris Howells <howells@kde.org>
 *  (C) 2003 Sven Leiber <s.leiber@web.de>
 *
 * This code is under GPL
 *
 * 2001/03/04 Converted to libkscreensaver by Martin R. Jones.
 */


#include <qdir.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfile.h>
#include <qpaintdevicemetrics.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qframe.h>
#include <qdesktopwidget.h>

#include <kconfig.h>
#include <kglobal.h>
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kimageio.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <kaboutdata.h>
#include <kaboutapplication.h>
#include <kdebug.h>

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "slideshow.h"
#include "slideshow.moc"
#include "slideshowcfg.h"


#define SLIDESHOW_VERSION "2.3.0"
static const char version[] = SLIDESHOW_VERSION;
static const char description[] = I18N_NOOP("KSlideshow");


// libkscreensaver interface
extern "C"
{
    KDE_EXPORT const char *kss_applicationName = "kslideshow.kss";
    KDE_EXPORT const char *kss_description = I18N_NOOP( "KSlideshow" );
    KDE_EXPORT const char *kss_version = SLIDESHOW_VERSION;

    KDE_EXPORT KScreenSaver *kss_create( WId id )
    {
        return new kSlideShowSaver( id );
    }

    KDE_EXPORT QDialog *kss_setup()
    {
        return new kSlideShowSetup();
    }
}


//=============================================================================
//  Class kSlideShowSaver
//=============================================================================
kSlideShowSaver::kSlideShowSaver( WId id ): KScreenSaver(id)
{
  KImageIO::registerFormats();

  blank();

  mEffect = NULL;
  mNumEffects = 0;
  mIntArray = NULL;
  registerEffects();

  readConfig();
  initNextScreen();

  mFileIdx = 0;
  mColorContext = QColor::enterAllocContext();

  mEffectRunning = false;

  mTimer.start(10, true);
  connect(&mTimer, SIGNAL(timeout()), SLOT(slotTimeout()));

  QDesktopWidget *d = QApplication::desktop();
  if( geometry() == d->geometry() && d->numScreens() > 1)
  {
	for(int i = 0; i < d->numScreens(); ++i)
	{
		QRect s = d->screenGeometry(i);
		mGeoList.append(new mScreenGeo(s.width(), s.height(), s.topLeft().x(), s.topLeft().y()));
	}
   }
   else
   {
	mGeoList.append(new mScreenGeo(width(), height(), 0, 0 ));
   }
   createNextScreen();
}


//----------------------------------------------------------------------------
kSlideShowSaver::~kSlideShowSaver()
{
  delete [] mIntArray;
  delete [] mEffectList;

  mTimer.stop();
  if (mPainter.isActive()) mPainter.end();
  QColor::leaveAllocContext();
  QColor::destroyAllocContext(mColorContext);
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::initNextScreen()
{
  QPaintDeviceMetrics metric(this);
  int w, h;

  w = width();
  h = height();
  mNextScreen = QPixmap(w, h, metric.depth());
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::readConfig()
{
  KConfig *config = KGlobal::config();
  config->setGroup("Settings");
  mShowRandom = config->readBoolEntry("ShowRandom", true);
  mZoomImages = config->readBoolEntry("ZoomImages", false);
  mPrintName = config->readBoolEntry("PrintName", true);
  mDirectory = config->readPathEntry("Directory", KGlobal::dirs()->findDirs("wallpaper", "").last());
  mDelay = config->readNumEntry("Delay", 10) * 1000;
  mSubdirectory = config->readBoolEntry("SubDirectory", false);
  mRandomPosition = config->readBoolEntry("RandomPosition", false);

  loadDirectory();
}


//----------------------------------------------------------------------------
void kSlideShowSaver::registerEffects()
{
  int i = 0;

  mEffectList = new EffectMethod[64];
  mEffectList[i++] = &kSlideShowSaver::effectChessboard;
  mEffectList[i++] = &kSlideShowSaver::effectMultiCircleOut;
  mEffectList[i++] = &kSlideShowSaver::effectSpiralIn;
  mEffectList[i++] = &kSlideShowSaver::effectSweep;
  mEffectList[i++] = &kSlideShowSaver::effectMeltdown;
  mEffectList[i++] = &kSlideShowSaver::effectCircleOut;
  mEffectList[i++] = &kSlideShowSaver::effectBlobs;
  mEffectList[i++] = &kSlideShowSaver::effectHorizLines;
  mEffectList[i++] = &kSlideShowSaver::effectVertLines;
  mEffectList[i++] = &kSlideShowSaver::effectRandom;
  mEffectList[i++] = &kSlideShowSaver::effectGrowing;
  mEffectList[i++] = &kSlideShowSaver::effectIncomingEdges;

  mNumEffects = i;
  // mNumEffects = 1;  //...for testing
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectMultiCircleOut(bool aInit)
{
  int x, y, i;
  double alpha;
  static QPointArray pa(4);

  if (aInit)
  {
    startPainter();
    mw = width();
    mh = height();
    mx = mw;
    my = mh>>1;
    pa.setPoint(0, mw>>1, mh>>1);
    pa.setPoint(3, mw>>1, mh>>1);
    mfy = sqrt((double)mw*mw + mh*mh) / 2;
    mi  = KApplication::random()%15 + 2;
    mfd = M_PI*2/mi;
    mAlpha = mfd;
    mwait = 10 * mi;
    mfx = M_PI/32;  // divisor must be powers of 8
  }

  if (mAlpha < 0)
  {
    mPainter.end();
    showNextScreen();
    return -1;
  }

  for (alpha=mAlpha, i=mi; i>=0; i--, alpha+=mfd)
  {
    x = (mw>>1) + (int)(mfy * cos(-alpha));
    y = (mh>>1) + (int)(mfy * sin(-alpha));

    mx = (mw>>1) + (int)(mfy * cos(-alpha + mfx));
    my = (mh>>1) + (int)(mfy * sin(-alpha + mfx));

    pa.setPoint(1, x, y);
    pa.setPoint(2, mx, my);

    mPainter.drawPolygon(pa);
  }
  mAlpha -= mfx;

  return mwait;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectSpiralIn(bool aInit)
{
  if (aInit)
  {
    startPainter();
    mw = width();
    mh = height();
    mix = mw / 8;
    miy = mh / 8;
    mx0 = 0;
    mx1 = mw - mix;
    my0 = miy;
    my1 = mh - miy;
    mdx = mix;
    mdy = 0;
    mi = 0;
    mj = 16 * 16;
    mx = 0;
    my = 0;
  }

  if (mi==0 && mx0>=mx1)
  {
    mPainter.end();
    showNextScreen();
    return -1;
  }

  if (mi==0 && mx>=mx1) // switch to: down on right side
  {
    mi = 1;
    mdx = 0;
    mdy = miy;
    mx1 -= mix;
  }
  else if (mi==1 && my>=my1) // switch to: right to left on bottom side
  {
    mi = 2;
    mdx = -mix;
    mdy = 0;
    my1 -= miy;
  }
  else if (mi==2 && mx<=mx0) // switch to: up on left side
  {
    mi = 3;
    mdx = 0;
    mdy = -miy;
    mx0 += mix;
  }
  else if (mi==3 && my<=my0) // switch to: left to right on top side
  {
    mi = 0;
    mdx = mix;
    mdy = 0;
    my0 += miy;
  }

  bitBlt(this, mx, my, &mNextScreen, mx, my, mix, miy, CopyROP, true);

  mx += mdx;
  my += mdy;
  mj--;

  return 8;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectMeltdown(bool aInit)
{
  int i, x, y;
  bool done;

  if (aInit)
  {
    delete [] mIntArray;
    mw = width();
    mh = height();
    mdx = 4;
    mdy = 16;
    mix = mw / mdx;
    mIntArray = new int[mix];
    for (i=mix-1; i>=0; i--)
      mIntArray[i] = 0;
  }

  done = true;
  for (i=0,x=0; i<mix; i++,x+=mdx)
  {
    y = mIntArray[i];
    if (y >= mh) continue;
    done = false;
    if ((KApplication::random()&15) < 6) continue;
    bitBlt(this, x, y+mdy, this, x, y, mdx, mh-y-mdy, CopyROP, true);
    bitBlt(this, x, y, &mNextScreen, x, y, mdx, mdy, CopyROP, true);
    mIntArray[i] += mdy;
  }

  if (done)
  {
    delete [] mIntArray;
    mIntArray = NULL;
    return -1;
  }

  return 15;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectCircleOut(bool aInit)
{
  int x, y;
  static QPointArray pa(4);

  if (aInit)
  {
    startPainter();
    mw = width();
    mh = height();
    mx = mw;
    my = mh>>1;
    mAlpha = 2*M_PI;
    pa.setPoint(0, mw>>1, mh>>1);
    pa.setPoint(3, mw>>1, mh>>1);
    mfx = M_PI/16;  // divisor must be powers of 8
    mfy = sqrt((double)mw*mw + mh*mh) / 2;
  }

  if (mAlpha < 0)
  {
    mPainter.end();
    showNextScreen();
    return -1;
  }

  x = mx;
  y = my;
  mx = (mw>>1) + (int)(mfy * cos(mAlpha));
  my = (mh>>1) + (int)(mfy * sin(mAlpha));
  mAlpha -= mfx;

  pa.setPoint(1, x, y);
  pa.setPoint(2, mx, my);

  mPainter.drawPolygon(pa);

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectSweep(bool aInit)
{
  int w, h, x, y, i;

  if (aInit)
  {
    // subtype: 0=sweep right to left, 1=sweep left to right
    //          2=sweep bottom to top, 3=sweep top to bottom
    mSubType = KApplication::random() % 4;
    mw  = width();
    mh  = height();
    mdx = (mSubType==1 ? 16 : -16);
    mdy = (mSubType==3 ? 16 : -16);
    mx  = (mSubType==1 ? 0 : mw);
    my  = (mSubType==3 ? 0 : mh);
  }

  if (mSubType==0 || mSubType==1)
  {
    // horizontal sweep
    if ((mSubType==0 && mx < -64) ||
	(mSubType==1 && mx > mw+64))
    {
       return -1;
    }
    for (w=2,i=4,x=mx; i>0; i--, w<<=1, x-=mdx)
    {
      bitBlt(this, x, 0, &mNextScreen, x, 0, w, mh, CopyROP, true);
    }
    mx += mdx;
  }
  else
  {
    // vertical sweep
    if ((mSubType==2 && my < -64) ||
	(mSubType==3 && my > mh+64))
    {
      return -1;
    }
    for (h=2,i=4,y=my; i>0; i--, h<<=1, y-=mdy)
    {
      bitBlt(this, 0, y, &mNextScreen, 0, y, mw, h, CopyROP, true);
    }
    my += mdy;
  }

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectBlobs(bool aInit)
{
  int r;

  if (aInit)
  {
    startPainter();
    mAlpha = M_PI * 2;
    mw = width();
    mh = height();
    mi = 150;
  }

  if (mi <= 0)
  {
    mPainter.end();
    showNextScreen();
    return -1;
  }

  mx = KApplication::random() % mw;
  my = KApplication::random() % mh;
  r = (KApplication::random() % 200) + 50;

  mPainter.drawEllipse(mx-r, my-r, r, r);
  mi--;

  return 10;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectRandom(bool /*aInit*/)
{
  int x, y, i, w, h, fact, sz;

  fact = (KApplication::random() % 3) + 1;

  w = width() >> fact;
  h = height() >> fact;
  sz = 1 << fact;

  for (i = (w*h)<<1; i > 0; i--)
  {
    x = (KApplication::random() % w) << fact;
    y = (KApplication::random() % h) << fact;
    bitBlt(this, x, y, &mNextScreen, x, y, sz, sz, CopyROP, true);
  }
  showNextScreen();

  return -1;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectGrowing(bool aInit)
{
  if (aInit)
  {
    mw = width();
    mh = height();
    mx = mw >> 1;
    my = mh >> 1;
    mi = 0;
    mfx = mx / 100.0;
    mfy = my / 100.0;
  }

  mx = (mw>>1) - (int)(mi * mfx);
  my = (mh>>1) - (int)(mi * mfy);
  mi++;

  if (mx<0 || my<0)
  {
    showNextScreen();
    return -1;
  }

  bitBlt(this, mx, my, &mNextScreen, mx, my,
	 mw - (mx<<1), mh - (my<<1), CopyROP, true);

  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectChessboard(bool aInit)
{
  int y;

  if (aInit)
  {
    mw  = width();
    mh  = height();
    mdx = 8;         // width of one tile
    mdy = 8;         // height of one tile
    mj  = (mw+mdx-1)/mdx; // number of tiles
    mx  = mj*mdx;    // shrinking x-offset from screen border
    mix = 0;         // growing x-offset from screen border
    miy = 0;         // 0 or mdy for growing tiling effect
    my  = mj&1 ? 0 : mdy; // 0 or mdy for shrinking tiling effect
    mwait = 800 / mj; // timeout between effects
  }

  if (mix >= mw)
  {
    showNextScreen();
    return -1;
  }

  mix += mdx;
  mx  -= mdx;
  miy = miy ? 0 : mdy;
  my  = my ? 0 : mdy;

  for (y=0; y<mw; y+=(mdy<<1))
  {
    bitBlt(this, mix, y+miy, &mNextScreen, mix, y+miy,
	   mdx, mdy, CopyROP, true);
    bitBlt(this, mx, y+my, &mNextScreen, mx, y+my,
	   mdx, mdy, CopyROP, true);
  }

  return mwait;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectIncomingEdges(bool aInit)
{
  int x1, y1;

  if (aInit)
  {
    mw = width();
    mh = height();
    mix = mw >> 1;
    miy = mh >> 1;
    mfx = mix / 100.0;
    mfy = miy / 100.0;
    mi = 0;
    mSubType = KApplication::random() & 1;
  }

  mx = (int)(mfx * mi);
  my = (int)(mfy * mi);

  if (mx>mix || my>miy)
  {
    showNextScreen();
    return -1;
  }

  x1 = mw - mx;
  y1 = mh - my;
  mi++;

  if (mSubType)
  {
    // moving image edges
    bitBlt(this,  0,  0, &mNextScreen, mix-mx, miy-my, mx, my, CopyROP, true);
    bitBlt(this, x1,  0, &mNextScreen, mix, miy-my, mx, my, CopyROP, true);
    bitBlt(this,  0, y1, &mNextScreen, mix-mx, miy, mx, my, CopyROP, true);
    bitBlt(this, x1, y1, &mNextScreen, mix, miy, mx, my, CopyROP, true);
  }
  else
  {
    // fixed image edges
    bitBlt(this,  0,  0, &mNextScreen,  0,  0, mx, my, CopyROP, true);
    bitBlt(this, x1,  0, &mNextScreen, x1,  0, mx, my, CopyROP, true);
    bitBlt(this,  0, y1, &mNextScreen,  0, y1, mx, my, CopyROP, true);
    bitBlt(this, x1, y1, &mNextScreen, x1, y1, mx, my, CopyROP, true);
  }
  return 20;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectHorizLines(bool aInit)
{
  static int iyPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
  int y;

  if (aInit)
  {
    mw = width();
    mh = height();
    mi = 0;
  }

  if (iyPos[mi] < 0) return -1;

  for (y=iyPos[mi]; y<mh; y+=8)
  {
    bitBlt(this, 0, y, &mNextScreen, 0, y, mw, 1, CopyROP, true);
  }

  mi++;
  if (iyPos[mi] >= 0) return 160;
  return -1;
}


//----------------------------------------------------------------------------
int kSlideShowSaver::effectVertLines(bool aInit)
{
  static int ixPos[] = { 0, 4, 2, 6, 1, 5, 3, 7, -1 };
  int x;

  if (aInit)
  {
    mw = width();
    mh = height();
    mi = 0;
  }

  if (ixPos[mi] < 0) return -1;

  for (x=ixPos[mi]; x<mw; x+=8)
  {
    bitBlt(this, x, 0, &mNextScreen, x, 0, 1, mh, CopyROP, true);
  }

  mi++;
  if (ixPos[mi] >= 0) return 160;
  return -1;
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::startPainter(Qt::PenStyle aPen)
{
  QBrush brush;
  brush.setPixmap(mNextScreen);
  if (mPainter.isActive()) mPainter.end();
  mPainter.begin(this);
  mPainter.setBrush(brush);
  mPainter.setPen(aPen);
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::restart()
{
  mEffectRunning = false;
  mEffect = NULL;
  blank();
  slotTimeout();
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::slotTimeout()
{
  int tmout = -1;
  int i;

  if (mEffectRunning)
  {
    tmout = (this->*mEffect)(false);
  }
  else
  {
    loadNextImage();
    createNextScreen();

    if (mNumEffects > 1) i = KApplication::random() % mNumEffects;
    else i = 0;

    mEffect = mEffectList[i];
    mEffectRunning = true;
    tmout = (this->*mEffect)(true);
  }
  if (tmout <= 0)
  {
    tmout = mDelay;
    mEffectRunning = false;
  }
  mTimer.start(tmout, true);
}


//----------------------------------------------------------------------------
void kSlideShowSaver::showNextScreen()
{
  bitBlt(this, 0, 0, &mNextScreen, 0, 0,
	 mNextScreen.width(), mNextScreen.height(), CopyROP, true);
}


//----------------------------------------------------------------------------
void kSlideShowSaver::createNextScreen()
{
  QPainter p;
  int ww, wh, iw, ih, x, y;
  double fx, fy;

  if (mNextScreen.size() != size())
    mNextScreen.resize(size());

  mNextScreen.fill(black);

  p.begin(&mNextScreen);
  
  mScreenGeo *geoptr = 0;
  for(geoptr = mGeoList.first(); geoptr; geoptr = mGeoList.next())
  {
    loadNextImage();
    
  iw = mImage.width();
  ih = mImage.height();
    ww = geoptr->mW;
    wh = geoptr->mH;

  if (mFileList.isEmpty())
  {
    p.setPen(QColor("white"));
    p.drawText(20 + (KApplication::random() % (ww>>1)), 20 + (KApplication::random() % (wh>>1)),
	       i18n("No images found"));
  }
  else
  {
    if (mZoomImages)
    {
      fx = (double)ww / iw;
      fy = (double)wh / ih;
      if (fx > fy) fx = fy;
      if (fx > 2) fx = 2;
      iw = (int)(iw * fx);
      ih = (int)(ih * fx);
      QImage scaledImg = mImage.smoothScale(iw, ih);

        x = ((ww - iw) >> 1) + geoptr->mXorg;
        y = ((wh - ih) >> 1) + geoptr->mYorg;

      p.drawImage(x, y, scaledImg);
    }
    else
    {
      if(iw >= ww || ih >= wh)
      {
        fx = (double)ww / iw;
	fy = (double)wh / ih;
	if (fx > fy) fx = fy;
	if (fx > 2) fx = 2;
	iw = (int)(iw * fx);
	ih = (int)(ih * fx);
	QImage scaledImg = mImage.smoothScale(iw, ih);

          x = ((ww - iw) >> 1) + geoptr->mXorg;
          y = ((wh - ih) >> 1) + geoptr->mYorg;

	p.drawImage(x, y, scaledImg);
      }
      else
      {
        if(mRandomPosition)
        {
            x = (KApplication::random() % (ww - iw)) + geoptr->mXorg;
            y = (KApplication::random() % (wh - ih)) + geoptr->mYorg;
        }
        else
        {
            x = ((ww - iw) >> 1) + geoptr->mXorg;
            y = ((wh - ih) >> 1) + geoptr->mYorg;
        }

        // bitBlt(&mNextScreen, x, y, &mImage, 0, 0, iw, ih, CopyROP, false);
        p.drawImage(x, y, mImage);
      }
    }

    if (mPrintName)
    {
      p.setPen(QColor("black"));
      for (x=9; x<=11; x++)
	for (y=21; y>=19; y--)
            p.drawText(x + geoptr->mXorg, wh-y+geoptr->mYorg, mImageName);
      p.setPen(QColor("white"));
        p.drawText(10 + geoptr->mXorg, wh-20 + geoptr->mYorg, mImageName);
      }
    }
  }
  p.end();
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadNextImage()
{
  QString fname;
  int num;

  num = mFileList.count();
  if (num <= 0) //no files in the directory
  {
    return;
  }

  if (mShowRandom)
  {
    mFileIdx = KApplication::random() % num;
    fname = mFileList[mFileIdx];
    mFileList.remove(fname);
    if (num == 1) //we're about to run out of images
    {
      mFileList = mRandomList;
    }
  }
  else
  {
    if (mFileIdx >= num) mFileIdx = 0;
    fname = mFileList[mFileIdx];
  }

  if (!mImage.load(fname))
  {
    kdDebug() << "Failed to load image " << fname << endl;
    mFileList.remove(fname);
    mRandomList.remove(fname);
    if (!mFileList.isEmpty())
        loadNextImage();
    return;
  }
  mFileIdx++;

  int i, j;
  i = fname.findRev('.');
  if (i < 0) i = 32767;
  j = fname.findRev('/') + 1;
  if (j < 0) j = 0;
  mImageName = fname.mid(j, i-j);
}


//----------------------------------------------------------------------------
void kSlideShowSaver::loadDirectory()
{
  mFileIdx = 0;
  mFileList.clear();
  traverseDirectory(mDirectory);
  mRandomList = mFileList;
}

void kSlideShowSaver::traverseDirectory(const QString &dirName)
{
  QDir dir(dirName);
  if (!dir.exists())
  {
     return ;
  }
  dir.setFilter(QDir::Dirs | QDir::Files);

  const QFileInfoList *fileinfolist = dir.entryInfoList();
  QFileInfoListIterator it(*fileinfolist);
  QFileInfo *fi;
  while ((fi = it.current()))
  {
     if (fi->fileName() == "." || fi->fileName() == "..")
     {
        ++it;
        continue;
     }
     if (fi->isDir() && fi->isReadable() && mSubdirectory)
     {
        traverseDirectory(fi->filePath());
     }
     else
     {
        if (!fi->isDir())
        {
           mFileList.append(fi->filePath());
        }
     }
  ++it;
   }
}


//-----------------------------------------------------------------------------
void kSlideShowSaver::blank()
{
   erase();
}


//=============================================================================
//  Class kSlideShowSetup
//=============================================================================
kSlideShowSetup::kSlideShowSetup(QWidget *aParent, const char *aName)
  : KDialogBase(aParent, aName, true, i18n( "Setup Slide Show Screen Saver" ),
    Ok|Cancel|Help, Ok, true )
{
  setButtonText( Help, i18n( "A&bout" ) );

  QWidget *main = makeMainWidget();
  QVBoxLayout *top = new QVBoxLayout( main, 0, spacingHint() );

  cfg = new SlideShowCfg( main, "SlideShowCfg" );
  top->addWidget( cfg );
  top->addStretch();

  cfg->mPreview->setFixedSize(220, 170);
  cfg->mPreview->setBackgroundColor(black);
  cfg->mPreview->show();    // otherwise saver does not get correct size
  mSaver = new kSlideShowSaver(cfg->mPreview->winId());

  cfg->mDirChooser->setMode(KFile::Directory | KFile::ExistingOnly);
  connect(cfg->mDirChooser, SIGNAL(returnPressed(const QString &)),
      SLOT(slotDirSelected(const QString &)));
  connect(cfg->mDirChooser, SIGNAL(urlSelected(const QString &)),
      SLOT(slotDirSelected(const QString &)));

  readSettings();
}

kSlideShowSetup::~kSlideShowSetup()
{
    delete mSaver;
}

//-----------------------------------------------------------------------------
void kSlideShowSetup::readSettings()
{
  KConfig *config = KGlobal::config();

  config->setGroup("Settings");
  cfg->mCbxRandom->setChecked(config->readBoolEntry("ShowRandom", true));
  cfg->mCbxZoom->setChecked(config->readBoolEntry("ZoomImages", false));
  cfg->mCbxShowName->setChecked(config->readBoolEntry("PrintName", true));
  cfg->mDelay->setValue(config->readNumEntry("Delay", 20));
  cfg->mDirChooser->setURL(config->readPathEntry("Directory"));
  cfg->mCbxSubdirectory->setChecked(config->readBoolEntry("SubDirectory", false));
  cfg->mCbxRandomPosition->setChecked(config->readBoolEntry("RandomPosition", false));
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::writeSettings()
{
  KConfig *config = KGlobal::config();

  config->setGroup("Settings");
  config->writeEntry("ShowRandom", cfg->mCbxRandom->isChecked());
  config->writeEntry("ZoomImages", cfg->mCbxZoom->isChecked());
  config->writeEntry("PrintName",  cfg->mCbxShowName->isChecked());
  config->writeEntry("Delay", cfg->mDelay->value());
  config->writePathEntry("Directory", cfg->mDirChooser->url());
  config->writeEntry("SubDirectory", cfg->mCbxSubdirectory->isChecked());
  config->writeEntry("RandomPosition", cfg->mCbxRandomPosition->isChecked());

  config->sync();

  if (mSaver)
  {
    mSaver->readConfig();
    mSaver->restart();
  }
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotDirSelected(const QString &)
{
  writeSettings();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotOk()
{
  writeSettings();
  accept();
}


//-----------------------------------------------------------------------------
void kSlideShowSetup::slotHelp()
{
  KAboutData aboutData( "kslideshow.kss", I18N_NOOP("SlideShow"),
                       version, description, KAboutData::License_GPL,
                       "(c) 1999-2003, The KDE Team" );
  aboutData.addAuthor("Stefan Taferner", 0, "taferner@kde.org");
  aboutData.addAuthor("Chris Howells", 0, "howells@kde.org");
  aboutData.addAuthor("Sven Leiber", 0, "s.leiber@web.de");

  KAboutApplication mAbout(&aboutData, this, 0);
  mAbout.exec();
}

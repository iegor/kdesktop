//////////////////////////////////////////////////////////////////////////////
// polyester.cc
// -------------------
// Polyester window decoration for KDE
// Copyright (c) 2006 Marco Martin
// -------------------
// derived from Smooth Blend
// Copyright (c) 2005 Ryan Nickell
// -------------------
// Shadow engine from Plastik decoration
// Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>
// -------------------
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////


#include <kconfig.h>
#include <kdeversion.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kimageeffect.h>
#include <kpixmapeffect.h>
#include <kpixmap.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qsettings.h>
#include <math.h>
#include <stdlib.h>

#include "polyester.h"
#include "buttons.h"
#include "shadow.h"
#include "../style/misc.h"

using namespace polyester;

//////////////////////////////////////////////////////////////////////////////
// polyesterFactory Class
//////////////////////////////////////////////////////////////////////////////
polyesterFactory* factory=NULL;

bool polyesterFactory::initialized_              = false;
Qt::AlignmentFlags polyesterFactory::titlealign_ = Qt::AlignHCenter;
int polyesterFactory::contrast_                  = 6;
bool polyesterFactory::cornerflags_              = true;
int polyesterFactory::titlesize_                 = 22;
int polyesterFactory::buttonsize_                = 18;
bool polyesterFactory::squareButton_             = false;
int polyesterFactory::framesize_                 = 2;
int polyesterFactory::roundsize_                 = 50;
int polyesterFactory::titleBarStyle_             = 0;
int polyesterFactory::buttonStyle_               = 1;
bool polyesterFactory::titleshadow_              = true;
bool polyesterFactory::nomodalbuttons            = false;
bool polyesterFactory::lightBorder_              = true;
bool polyesterFactory::animatebuttons            = true;
int polyesterFactory::btnComboBox                = 0;
bool polyesterFactory::menuClose                 = false;

// global constants
static const int TOPMARGIN       = 4; // do not change
static const int DECOHEIGHT      = 4; // do not change
static const int SIDETITLEMARGIN = 2;
// Default button layout
const char default_left[]  = "M";
const char default_right[] = "HIAX";

static const uint TIMERINTERVAL = 25; // msec
static const uint ANIMATIONSTEPS = 20;

extern "C" KDecorationFactory* create_factory() {
    return new polyester::polyesterFactory();
}

//////////////////////////////////////////////////////////////////////////////
// polyesterFactory()
// ----------------
// Constructor

polyesterFactory::polyesterFactory() {
    readConfig();
    initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// ~polyesterFactory()
// -----------------
// Destructor

polyesterFactory::~polyesterFactory() {
    initialized_ = false;
}

//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// -----------------
// Create the decoration

KDecoration* polyesterFactory::createDecoration(KDecorationBridge* b) {
    return new polyesterClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a repaint is necessary

bool polyesterFactory::reset(unsigned long changed) {
    // read in the configuration
    initialized_ = false;
    bool confchange = readConfig();
    initialized_ = true;

    if (confchange ||
            (changed & (SettingDecoration | SettingButtons | SettingBorder))) {
        return true;
    } else {
        resetDecorations(changed);
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////
// readConfig()
// ------------
// Read in the configuration file

bool polyesterFactory::readConfig() {
    // create a config object
    KConfig config("kwinpolyesterrc");
    config.setGroup("General");

    // grab settings
    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft")
        titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter")
        titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight")
        titlealign_ = Qt::AlignRight;

    QSettings globalSettings;
    contrast_ = globalSettings.readNumEntry("/Qt/KDE/contrast", 6);
    cornerflags_ = config.readBoolEntry("RoundCorners", true);
    titlesize_ = config.readNumEntry("TitleSize",20);
    titleBarStyle_ = config.readNumEntry("TitleBarStyle", 0);
    buttonStyle_ = config.readNumEntry("ButtonStyle", 1);

    buttonsize_ = config.readNumEntry("ButtonSize",18);
    squareButton_ = config.readBoolEntry("SquareButton", false);
    framesize_ = config.readNumEntry("FrameSize",2);
    roundsize_ = config.readNumEntry("RoundPercent",50);


    titleshadow_ = config.readBoolEntry("TitleShadow", true);
    lightBorder_ = config.readBoolEntry("LightBorder", true);
    animatebuttons = config.readBoolEntry("AnimateButtons", true);
    nomodalbuttons = config.readBoolEntry("NoModalButtons", false);
    btnComboBox = config.readNumEntry("ButtonComboBox", 0);
    menuClose = config.readBoolEntry("CloseOnMenuDoubleClick");

    if(buttonsize_ > titlesize_ - framesize_)
    {
        buttonsize_ = titlesize_-framesize_;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// polyesterButton Class 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// polyesterButton()
// ---------------
// Constructor
polyesterButton::polyesterButton(polyesterClient *parent, const char *name, const QString& tip, ButtonType type, int button_size, bool squareButton, bool toggle): QButton(parent->widget(), name), 
     client_(parent), 
     type_(type), 
     size_(button_size),
     deco_(0), 
     lastmouse_(NoButton),
     buttonImgActive_created(false),
     buttonImgInactive_created(false),
     hover_(false)
{
    setBackgroundMode(NoBackground);

    int buttonWidth = button_size;
    if( squareButton )
      buttonProportions_ = 1;
    else
    {
      buttonProportions_ = sqrt(2);
      buttonWidth = (int)round(button_size*buttonProportions_);
      //makes always centered icons
      if( buttonWidth % 2 != 0 )
        buttonWidth++;
    }

    setFixedSize( buttonWidth, button_size);
    setCursor(arrowCursor);
    QToolTip::add(this, tip);
    setToggleButton(toggle);
    //button animation setup
    animTmr = new QTimer(this);
    connect(animTmr, SIGNAL(timeout() ), this, SLOT(animate() ) );
    connect(this, SIGNAL(pressed() ), this, SLOT(buttonClicked() ) );
    connect(this, SIGNAL(released() ), this, SLOT(buttonReleased() ) );
    animProgress = 0;
    m_clicked=false;
}

polyesterButton::~polyesterButton() {
    if ( deco_ )
        delete deco_;
}

//////////////////////////////////////////////////////////////////////////////
// sizeHint()
// ----------
// Return size hint

QSize polyesterButton::sizeHint() const {
    return QSize(size_, size_);
}

//////////////////////////////////////////////////////////////////////////////
// buttonClicked()
// ----------
// Button animation progress reset so we don't get any leave event animation
// when the mouse is still pressed
void polyesterButton::buttonClicked() {
    m_clicked=true;
    animProgress=0;
}
void polyesterButton::buttonReleased() {
    //This doesn't work b/c a released() signal is thrown when a leaveEvent occurs
    //m_clicked=false;
}

//////////////////////////////////////////////////////////////////////////////
// animate()
// ----------
// Button animation timing
void polyesterButton::animate() {
    animTmr->stop();
    if (!::factory->animateButtons() )
      return;

    if(hover_) {
        if(animProgress < ANIMATIONSTEPS) {
            animProgress++;
            animTmr->start(TIMERINTERVAL, true); // single-shot
        }
    } else {
        if(animProgress > 0) {
            animProgress--;
            animTmr->start(TIMERINTERVAL, true); // single-shot
        }
    }
    repaint(false);
}
//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void polyesterButton::enterEvent(QEvent *e) {
    // we wanted to pass on the event
    QButton::enterEvent(e);
    // we want to do mouseovers, so keep track of it here
    hover_=true;
    if(!m_clicked)
    {
        animate();
    }
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void polyesterButton::leaveEvent(QEvent *e) {
    // we wanted to pass on the event
    QButton::leaveEvent(e);
    // we want to do mouseovers, so keep track of it here
    hover_=false; 
    if(!m_clicked)
    {
        animate();
    }
}

//////////////////////////////////////////////////////////////////////////////
// mousePressEvent()
// -----------------
// Button has been pressed

void polyesterButton::mousePressEvent(QMouseEvent* e) {
    lastmouse_ = e->button();

    // translate and pass on mouse event
    int button = LeftButton;
    if ((type_ != ButtonMax) && (e->button() != LeftButton)) {
        button = NoButton; // middle & right buttons inappropriate
    }
    QMouseEvent me(e->type(), e->pos(), e->globalPos(),
                   button, e->state());
    QButton::mousePressEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// mouseReleaseEvent()
// -----------------
// Button has been released

void polyesterButton::mouseReleaseEvent(QMouseEvent* e) {
    lastmouse_ = e->button();

    // translate and pass on mouse event
    int button = LeftButton;
    if ((type_ != ButtonMax) && (e->button() != LeftButton)) {
        button = NoButton; // middle & right buttons inappropriate
    }
    QMouseEvent me(e->type(), e->pos(), e->globalPos(), button, e->state());
    QButton::mouseReleaseEvent(&me);
    if(m_clicked)
    {
        m_clicked=false;
    }
}

void polyesterButton::setOn(bool on)
{
    QButton::setOn(on);
}

void polyesterButton::setDown(bool on)
{
    QButton::setDown(on);
}

//////////////////////////////////////////////////////////
// getButtonImage()
// ----------------
// get the button QImage based on type and window mode
QImage polyesterButton::getButtonImage(ButtonType type)
{
    QImage finalImage;
    switch(type) {
        case ButtonClose:
            finalImage = uic_findImage( "close.png" );
            break;
        case ButtonHelp:
            finalImage = uic_findImage( "help.png" );
            break;
        case ButtonMin:
            finalImage = uic_findImage( "minimize.png" );
            break;
        case ButtonMax:
            if(client_->maximizeMode() == KDecorationDefines::MaximizeFull)
            {
                finalImage = uic_findImage( "restore.png" );
            }
            else
            {
                finalImage = uic_findImage( "maximize.png" );
            }
            break;
        case ButtonSticky:
            if(client_->isOnAllDesktops())
            {
                finalImage = uic_findImage( "circle2.png" );
            }
            else
            {
                finalImage = uic_findImage( "circle.png" );
            }
            break;
        case ButtonShade:
            if(client_->isShade())
            {
                finalImage = uic_findImage( "unsplat.png" );
            }
            else
            {
                finalImage = uic_findImage( "splat.png" );
            }
            break;
        case ButtonAbove:
            if(client_->keepAbove())
            {
                finalImage = uic_findImage( "keep_above_lit.png" );
            }
            else
            {
                finalImage = uic_findImage( "keep_above.png" );
            }
            break;
        case ButtonBelow:
            if(client_->keepBelow())
            {
                finalImage = uic_findImage( "keep_below_lit.png" );
            }
            else
            {
                finalImage = uic_findImage( "keep_below.png" );
            }
            break;
        default:
            finalImage = uic_findImage( "splat.png" );
            break;
    }
    if(qGray(KDecoration::options()->color(KDecoration::ColorButtonBg, client_->isActive()).rgb()) < 150)
        finalImage.invertPixels();

    return finalImage;
}

//////////////////////////////////////////////////////////
// drawButton()
// -------------------------
// draw the pixmap button

void polyesterButton::drawButton( QPainter *painter ) {
    if ( !polyesterFactory::initialized() )
        return ;

    int newWidth = width() - 2;
    int newHeight = height() - 2;
    int dx = (width() - newWidth) / 2;
    int dy = (height() - newHeight) / 2;
    QImage tmpResult;
    QColorGroup group;
    QColor redColor(red);
    bool active = client_->isActive();

    genButtonPix(active);

    QPixmap backgroundTile = client_->getTitleBarTile(active);
    group = KDecoration::options()->colorGroup(KDecoration::ColorButtonBg, active);

    //draw the titlebar behind the buttons and app icons
    if ((client_->maximizeMode()==client_->MaximizeFull) && !KDecoration::options()->moveResizeMaximizedWindows())
    {
        painter->drawTiledPixmap(0, 0, width(), height(), backgroundTile, 0, 1);
    }
    else
    {
        painter->drawTiledPixmap(0, 0, width(), height(), backgroundTile,
                                 0, y()-::factory->frameSize());
    }

    //the menu button doesn't have a button appearance
    if (type_ == ButtonMenu)
    {
      //slight movement to show the menu button is depressed
      if (isDown()) {
          dx++;
          dy++;
      }
      QPixmap menuButtonPixmap(client_->icon().pixmap(QIconSet::Small, QIconSet::Normal));
      QImage menuButtonImage(menuButtonPixmap.convertToImage());
      //draw the menu button the same size as the other buttons
      //using QIconSet::Large gives us a 32x32 icon to resize, resizing larger than
      //that may produce pixilation of the image
      //painter->setPen(group.background());

      painter->drawImage( dx+(int)round(abs(width()-size_)/2), dy, menuButtonImage.smoothScale(size_-2, newHeight) );
      return;
    }

    //build the button image with the icon superimposed to the button
    QImage buttonImage = (active?(buttonImgActive):(buttonImgInactive))->copy();
    KImageEffect::blendOnLower( (int)round(abs(width()-size_)/2), 0,
                                getButtonImage(type_).smoothScale( size_,size_),
                                buttonImage );

    //highlight on a mouse over repaint
    double factor = animProgress * 0.05;//0.13;

    //do the animation thing
    if( !isDown() && factor != 0.0 )
    {
      tmpResult = buttonImage.copy();
      //always colorize with a threatening red color the close button
      if( type_ == ButtonClose)
      {
        //tmpResult = KImageEffect::blend( QColor(180, 50, 50), buttonImage, factor );
        KImageEffect::desaturate( tmpResult, factor );
        KImageEffect::channelIntensity( tmpResult, factor/3, KImageEffect::Red );
        KImageEffect::channelIntensity( tmpResult, -factor/2, KImageEffect::Green );
        KImageEffect::channelIntensity( tmpResult, -factor/2, KImageEffect::Blue );
      }
      else
          switch(::factory->getBtnComboBox())
          {
              //colorize
              case 0:
                  if( type_ == ButtonMax)
                  {
                    KImageEffect::desaturate( tmpResult, factor );
                    KImageEffect::channelIntensity( tmpResult, -factor/3, KImageEffect::Red );
                    KImageEffect::channelIntensity( tmpResult, factor/2, KImageEffect::Green );
                    KImageEffect::channelIntensity( tmpResult, -factor/3, KImageEffect::Blue );
                  }
                  else if( type_ == ButtonMin)
                  {
                    KImageEffect::desaturate( tmpResult, factor );
                    KImageEffect::channelIntensity( tmpResult, factor/4, KImageEffect::Red );
                    KImageEffect::channelIntensity( tmpResult, factor/4, KImageEffect::Green );
                    KImageEffect::channelIntensity( tmpResult, -factor/2, KImageEffect::Blue );
                  }
                  else if( type_ == ButtonSticky)
                  {
                    KImageEffect::desaturate( tmpResult, factor );
                    KImageEffect::channelIntensity( tmpResult, -factor/3, KImageEffect::Red );
                    KImageEffect::channelIntensity( tmpResult, -factor/3, KImageEffect::Green );
                    KImageEffect::channelIntensity( tmpResult, factor/2, KImageEffect::Blue );
                  }
                  else
                  {
                    KImageEffect::desaturate( tmpResult, factor );
                    KImageEffect::channelIntensity( tmpResult, -factor/2, KImageEffect::Red );
                    KImageEffect::channelIntensity( tmpResult, factor/3, KImageEffect::Green );
                    KImageEffect::channelIntensity( tmpResult, factor/3, KImageEffect::Blue );
                  }
                  break;
              //intensify
              case 1:
                  KImageEffect::intensity( tmpResult, factor);
                  break;
              //fade
              case 2:
                  KImageEffect::intensity( tmpResult, -factor/2);

                  break;
          }
    }

    KImageEffect::blendOnLower(tmpResult, QPoint(1,1), buttonImage, QRect(1,1,width()-2, height()-2) );

    QPixmap finalButton = QPixmap(buttonImage);

    painter->drawPixmap( 0, 0, finalButton );
}

void polyesterButton::genButtonPix( bool active )
{
 if(active && buttonImgActive_created) return;
 else if(!active && buttonImgInactive_created) return;

 KPixmap tempPixmap;
 QPixmap *tempButton = new QPixmap(width(), height());
 QPainter painter(tempButton);

 QColorGroup group;
 group = KDecoration::options()->colorGroup(KDecoration::ColorButtonBg, client_->isActive());

 if( ::factory->buttonStyle() == BUTTON_GLASS )
    tempPixmap.resize(width()-2, (height()/2)-1);
 else
    tempPixmap.resize(width()-2, height()-2);

 /*
 if(  ::factory->buttonStyle() != BUTTON_FLAT  )
 {
  tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                       group.background().light(100+(::factory->buttonStyle()!=BUTTON_REVGRADIENT?60:-20)),
                                       group.background().light(100+(::factory->buttonStyle()!=BUTTON_GLASS?
                                                                       (::factory->buttonStyle()==BUTTON_REVGRADIENT?
                                                                          60:-20)
                                                                       :0)),
                                       KPixmapEffect::VerticalGradient
                                      );
 }
 else
 {
  tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                       group.background(),
                                       group.background(),
                                       KPixmapEffect::VerticalGradient
                                      );
 }
*/

 int contrast = ::factory->contrast();

 switch( ::factory->buttonStyle() )
 {
   case BUTTON_GRADIENT:
       tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                       group.background().light(105+contrast*8),
                                       group.background().dark(100+contrast*2),
                                       KPixmapEffect::VerticalGradient
                                      );
     break;
   case BUTTON_GLASS:
       tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                       group.background().light(105+contrast*8),
                                       group.background(),
                                       KPixmapEffect::VerticalGradient
                                      );
     break;
   case BUTTON_REVGRADIENT:
       tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                       group.background().dark(100+contrast*2),
                                       group.background().light(105+contrast*8),
                                       KPixmapEffect::VerticalGradient
                                      );
     break;

   //case BUTTON_FLAT:
   default:
       tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                       group.background(),
                                       group.background(),
                                       KPixmapEffect::VerticalGradient
                                      );
     break;
 }

 //draw the main area of the buttons
 //painter.begin(tempButton);
 painter.drawPixmap(1, 1, tempPixmap);

 //if the button is glass create the second gradient
 if( ::factory->buttonStyle() == BUTTON_GLASS )
 {
    tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                        group.background().dark(105+contrast),
                                        group.background(),
                                        KPixmapEffect::VerticalGradient
                                       );

    painter.drawPixmap(1, (height()/2), tempPixmap);
  }

 //border
 QColor borderColor = group.background().dark(160);
 painter.setPen( borderColor );
 painter.drawLine(0, 2, 0, height()-3);
 painter.drawLine(width()-1, 2, width()-1, height()-3);
 painter.drawLine(2, 0, width()-3, 0);
 painter.drawLine(2, height()-1, width()-3, height()-1);
 painter.drawPoint(1, 1);
 painter.drawPoint(width()-2, 1);
 painter.drawPoint(1, height()-2);
 painter.drawPoint(width()-2, height()-2);

 //little 3d effect
 painter.setPen( group.background().light(140) );
 painter.drawLine(1, 2, 1, height()-3);
 painter.drawLine(2, 1, width()-3, 1);
 if(!::factory->lightBorder())
   painter.setPen( group.background().dark(110) );
 painter.drawLine(width()-2, 2, width()-2, height()-3);
 painter.drawLine(2, height()-2, width()-3, height()-2);
 painter.end();

  //border antialiasing
 QImage *tempAlphaImg = new QImage( tempButton->convertToImage() );
 //tempAlphaImg = tempPixmap.convertToImage();
 tempAlphaImg->setAlphaBuffer(true);
 int borderRed = borderColor.red();
 int borderGreen = borderColor.green();
 int borderBlue = borderColor.blue();

 tempAlphaImg->setPixel(0,0, qRgba(borderRed, borderGreen , borderBlue, 40));
 tempAlphaImg->setPixel(width()-1, 0, qRgba(borderRed, borderGreen , borderBlue, 40));
 tempAlphaImg->setPixel(0,height()-1, qRgba(borderRed, borderGreen , borderBlue, 40));
 tempAlphaImg->setPixel(width()-1,height()-1, qRgba(borderRed, borderGreen , borderBlue, 40));

 tempAlphaImg->setPixel(0,1, qRgba(borderRed, borderGreen , borderBlue, 127));
 tempAlphaImg->setPixel(1,0, qRgba(borderRed, borderGreen , borderBlue, 127));

 tempAlphaImg->setPixel(width()-2,0, qRgba(borderRed, borderGreen , borderBlue, 127));
 tempAlphaImg->setPixel(width()-1,1, qRgba(borderRed, borderGreen , borderBlue, 127));

 tempAlphaImg->setPixel(0,height()-2, qRgba(borderRed, borderGreen , borderBlue, 127));
 tempAlphaImg->setPixel(1,height()-1, qRgba(borderRed, borderGreen , borderBlue, 127));

 tempAlphaImg->setPixel(width()-1,height()-2, qRgba(borderRed, borderGreen , borderBlue, 127));
 tempAlphaImg->setPixel(width()-2,height()-1, qRgba(borderRed, borderGreen , borderBlue, 127));


 if( client_->isActive() )
 {
   buttonImgActive = tempAlphaImg;
   buttonImgActive_created = true;
 }
 else
 {
   buttonImgInactive = tempAlphaImg;
   buttonImgInactive_created = true;
 }

}

//////////////////////////////////////////////////////////////////////////////
// polyesterClient Class
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// polyesterClient()
// ---------------
// Constructor

polyesterClient::polyesterClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f),
    mainLayout_(0),
    titleLayout_(0),
    topSpacer_(0),
    titleSpacer_(0),
    leftTitleSpacer_(0), rightTitleSpacer_(0),
    decoSpacer_(0),
    leftSpacer_(0), rightSpacer_(0),
    bottomSpacer_(0), windowSpacer_(0),
    aCaptionBuffer(0), iCaptionBuffer(0),
    aTitleBarTile(0), iTitleBarTile(0), aTitleBarTopTile(0), iTitleBarTopTile(0),
    pixmaps_created(false),
    //captionBufferDirty(true),
    closing(false),
    s_titleHeight(0),
    s_titleFont(QFont()),
    maskDirty(true),
    aDoubleBufferDirty(true),
    iDoubleBufferDirty(true)
    {
        aCaptionBuffer = new QPixmap();
        iCaptionBuffer = new QPixmap();

        //copying the most used configuration options
        frameSize = ::factory->frameSize();

        NET::WindowType type = windowType(NET::NormalMask | NET::DesktopMask |
                                          NET::DockMask | NET::ToolbarMask |
                                          NET::MenuMask | NET::DialogMask|
                                          NET::OverrideMask | NET::TopMenuMask |
                                          NET::UtilityMask | NET::SplashMask);
        if( type == NET::Utility || type == NET::Menu || type == NET::Toolbar )
        {
           titleSize = (int)((double)::factory->titleSize()/1.2);
           buttonSize = titleSize - 2;
           roundedCorners = false;
        }
        else
        {
           titleSize = ::factory->titleSize();
           //don't allow buttonSize > titleSize
           buttonSize = titleSize - 2 < ::factory->buttonSize()?
                                                 titleSize - 2 :
                                                 ::factory->buttonSize();
           roundedCorners = ::factory->roundedCorners();
        }

        //s_titleFont = isTool()?polyesterFactory::titleFontTool():polyesterFactory::titleFont();
        s_titleFont = options()->font();
        s_titleHeight = titleSize;
    }

//////////////////////////////////////////////////////////////////////////////////
// ~polyesterClient()
// --------------------
// Destructor
polyesterClient::~polyesterClient() {
    delete aCaptionBuffer;
    delete iCaptionBuffer;
}

void polyesterClient::create_pixmaps() {
    if(pixmaps_created)
        return;
    KPixmap tempPixmap;
    QPainter painter;


    // active top title bar tile
    tempPixmap.resize(1, s_titleHeight+frameSize);
    tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                         KDecoration::options()->color(ColorTitleBar, true).light(160),
                                         KDecoration::options()->color(ColorTitleBlend, true).light(104),
                                         KPixmapEffect::VerticalGradient
                                        );
    // tempPixmap.fill(KDecoration::options()->color(ColorTitleBar, true).light(150));
    aTitleBarTopTile = new QPixmap(1, s_titleHeight+frameSize);
    painter.begin(aTitleBarTopTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();
    
    // inactive top title bar tile
    tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                         KDecoration::options()->color(ColorTitleBar, false).light(160),
                                         KDecoration::options()->color(ColorTitleBlend, false),
                                         KPixmapEffect::VerticalGradient
                                        );
    iTitleBarTopTile = new QPixmap(1, s_titleHeight+frameSize);
    painter.begin(iTitleBarTopTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();

    // active title bar tile
    if( ::factory->titleBarStyle() == TITLEBAR_GLASS)
       tempPixmap.resize(1, ((s_titleHeight+frameSize)/2) - 1 );
    else
       tempPixmap.resize(1, s_titleHeight+frameSize);

    QColor color1;
    QColor color2;
    QColor glassColor;
    if( ::factory->titleBarStyle() == TITLEBAR_GLASS )
    {
      glassColor = alphaBlendColors( KDecoration::options()->color(ColorTitleBar, true),
                                     KDecoration::options()->color(ColorTitleBlend, true),
                                     127 );
      color1 = glassColor.light(105+::factory->contrast()*4);
      color2 = glassColor;
    }
    else
    {
      color1 = KDecoration::options()->color(ColorTitleBar, true);
      color2 = KDecoration::options()->color(ColorTitleBlend, true);
    }

    tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                         color1,
                                         color2,
                                         KPixmapEffect::VerticalGradient
                                        );

    aTitleBarTile = new QPixmap(1, s_titleHeight+frameSize);
    painter.begin(aTitleBarTile);
    painter.drawPixmap(0, 0, tempPixmap);

    if( ::factory->titleBarStyle() == TITLEBAR_GLASS )
    {
       int y = tempPixmap.height();
       tempPixmap.resize(1, (s_titleHeight+frameSize)-y );
       tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                            glassColor.dark(105+::factory->contrast()),
                                            glassColor,
                                            KPixmapEffect::VerticalGradient
                                           );
       painter.drawPixmap(0, y, tempPixmap);
    }
    //  painter.drawPixmap(0, 0, *aTitleBarTopTile);
    painter.end();

    // inactive title bar tile
    //de-glassify inactive titlebar
    if( ::factory->titleBarStyle() == TITLEBAR_GLASS )
       tempPixmap.resize(1, s_titleHeight+frameSize);
    tempPixmap = KPixmapEffect::gradient(tempPixmap,
                                         KDecoration::options()->color(ColorTitleBar, false),
                                         KDecoration::options()->color(ColorTitleBlend, false),
                                         KPixmapEffect::VerticalGradient
                                        );
    iTitleBarTile = new QPixmap(1, s_titleHeight+frameSize);
    painter.begin(iTitleBarTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();


    pixmaps_created = true;
}

void polyesterClient::delete_pixmaps() {
    delete aTitleBarTopTile;
    aTitleBarTopTile = 0;

    delete iTitleBarTopTile;
    iTitleBarTopTile = 0;

    delete aTitleBarTile;
    aTitleBarTile = 0;

    delete iTitleBarTile;
    iTitleBarTile = 0;

    pixmaps_created = false;
}
//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Actual initializer for class

void polyesterClient::init() {
    createMainWidget( WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);
    handlebar = frameSize < 4 ? 4 - frameSize : 0;

    // for flicker-free redraws
    widget()->setBackgroundMode(NoBackground);

    create_pixmaps();
    _resetLayout();

}
void polyesterClient::_resetLayout()
{
    // basic layout:
    //  _______________________________________________________________
    // |                         topSpacer                             |
    // |_______________________________________________________________|
    // | leftTitleSpacer | btns |  titlebar   | bts | rightTitleSpacer |
    // |_________________|______|_____________|_____|__________________|
    // |                         decoSpacer                            |
    // |_______________________________________________________________|
    // | |                                                           | |
    // | |                      windowWrapper                        | |
    // | |                                                           | |
    // |leftSpacer                                          rightSpacer|
    // |_|___________________________________________________________|_|
    // |                           bottomSpacer                        |
    // |_______________________________________________________________|
    //
    if (!::factory->initialized()) return;
    
    delete mainLayout_;
    delete titleLayout_;
    delete topSpacer_;
    delete titleSpacer_;
    delete leftTitleSpacer_;
    delete rightTitleSpacer_;
    delete decoSpacer_;
    delete leftSpacer_;
    delete rightSpacer_;
    delete bottomSpacer_;
    delete windowSpacer_;

    mainLayout_ = new QVBoxLayout(widget());
    // title
    titleLayout_ = new QHBoxLayout();
    QHBoxLayout *windowLayout_ = new QHBoxLayout();


    topSpacer_        = new QSpacerItem(1, frameSize, QSizePolicy::Expanding, QSizePolicy::Fixed);
    titlebar_         = new QSpacerItem(1, s_titleHeight,
                                        QSizePolicy::Expanding, QSizePolicy::Fixed);
    leftTitleSpacer_  = new QSpacerItem(frameSize, s_titleHeight,
                                        QSizePolicy::Fixed, QSizePolicy::Fixed);
    rightTitleSpacer_ = new QSpacerItem(frameSize, s_titleHeight,
                                        QSizePolicy::Fixed, QSizePolicy::Fixed);
    decoSpacer_       = new QSpacerItem(1, frameSize, QSizePolicy::Expanding, QSizePolicy::Fixed);
    leftSpacer_       = new QSpacerItem(frameSize, 1,
                                        QSizePolicy::Fixed, QSizePolicy::Expanding);
    rightSpacer_      = new QSpacerItem(frameSize, 1,
                                        QSizePolicy::Fixed, QSizePolicy::Expanding);
    bottomSpacer_     = new QSpacerItem(1, frameSize,
                                        QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    // sizeof(...) is calculated at compile time
    memset(button, 0, sizeof(polyesterButton *) * ButtonTypeCount);

    // message in preview widget
    if (isPreview()) {
        windowLayout_->addWidget(
            new QLabel( i18n("<b><center>Polyester Preview</center></b>"), widget() ), 1 ); 
    } else {
        windowLayout_->addItem(new QSpacerItem(0, 0));
    }

    // setup titlebar buttons
    for (int n=0; n<ButtonTypeCount; n++)
        button[n] = 0;
    //Deal with the title and buttons
   titleLayout_->addItem(leftTitleSpacer_);

    if( !::factory->noModalButtons() || !isModal() || isResizable())
        addButtons(titleLayout_,
                   options()->customButtonPositions() ? options()->titleButtonsLeft() : QString(default_left),
                   buttonSize);
    titleLayout_->addItem(titlebar_);
    if( !::factory->noModalButtons() ||  !isModal() || isResizable())
        addButtons(titleLayout_,
                   options()->customButtonPositions() ? options()->titleButtonsRight() : QString(default_right),
                   buttonSize);
        titleLayout_->addItem(rightTitleSpacer_);

    //Mid - left side, middle contents and right side
    QHBoxLayout * midLayout_   = new QHBoxLayout();
    midLayout_->addItem(leftSpacer_);
    midLayout_->addLayout(windowLayout_);
    midLayout_->addItem(rightSpacer_);

    //Layout order
    mainLayout_->addItem( topSpacer_ );
    mainLayout_->addLayout( titleLayout_ );
    mainLayout_->addItem( decoSpacer_ );
    mainLayout_->addLayout( midLayout_ );
    mainLayout_->addItem( bottomSpacer_ );
    
    // connections
  //TODO: probably these two connections could be removed
  //  connect(this, SIGNAL(keepAboveChanged(bool)), SLOT(keepAboveChange(bool)));
  //  connect(this, SIGNAL(keepBelowChanged(bool)), SLOT(keepBelowChange(bool)));
}

//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title layout

void polyesterClient::addButtons(QBoxLayout *layout, const QString& s, int button_size) {
    QString tip;
    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
            case 'M': // Menu button
                if (!button[ButtonMenu]) {
                    button[ButtonMenu] =
                        new polyesterButton(this, "splat.png", i18n("Menu"),ButtonMenu,button_size,::factory->squareButton());
                    connect(button[ButtonMenu], SIGNAL(pressed()), this, SLOT(menuButtonPressed()));
                    connect(button[ButtonMenu], SIGNAL(released()), this, SLOT(menuButtonReleased()));
                    layout->addWidget(button[ButtonMenu]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'S': // Sticky button
                if (!button[ButtonSticky]) {
                    if (isOnAllDesktops()) {
                        tip = i18n("Un-Sticky");
                    } else {
                        tip = i18n("Sticky");
                    }
                    button[ButtonSticky] =
                        new polyesterButton(this, "circle.png", tip, ButtonSticky, button_size,::factory->squareButton(), true);
                    connect(button[ButtonSticky], SIGNAL(clicked()),
                            this, SLOT(toggleOnAllDesktops()));
                    layout->addWidget(button[ButtonSticky]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'H': // Help button
                if ((!button[ButtonHelp]) && providesContextHelp()) {
                    button[ButtonHelp] =
                        new polyesterButton(this, "help.png", i18n("Help"), ButtonHelp, button_size, ::factory->squareButton());
                    connect(button[ButtonHelp], SIGNAL(clicked()),
                            this, SLOT(showContextHelp()));
                    layout->addWidget(button[ButtonHelp]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'I': // Minimize button
                if ((!button[ButtonMin]) && isMinimizable())  {
                    button[ButtonMin] =
                        new polyesterButton(this, "minimize.png", i18n("Minimize"), ButtonMin, button_size,::factory->squareButton());
                    connect(button[ButtonMin], SIGNAL(clicked()),
                            this, SLOT(minimize()));
                    layout->addWidget(button[ButtonMin]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'A': // Maximize button
                if ((!button[ButtonMax]) && isMaximizable()) {
                    if (maximizeMode() == MaximizeFull) {
                        tip = i18n("Restore");
                    } else {
                        tip = i18n("Maximize");
                    }
                    button[ButtonMax]  =
                        new polyesterButton(this, "maximize.png", tip, ButtonMax, button_size,::factory->squareButton(), true);
                    connect(button[ButtonMax], SIGNAL(clicked()),
                            this, SLOT(maxButtonPressed()));
                    layout->addWidget(button[ButtonMax]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'X': // Close button
                if ((!button[ButtonClose]) && isCloseable()) {
                    button[ButtonClose] =
                        new polyesterButton(this, "close.png", i18n("Close"), ButtonClose, button_size, ::factory->squareButton());
                    connect(button[ButtonClose], SIGNAL(clicked()),
                            this, SLOT(closeWindow()));
                    layout->addWidget(button[ButtonClose]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'F': // Above button
                if ((!button[ButtonAbove])) {
                    button[ButtonAbove] =
                        new polyesterButton(this, "keep_above.png",
                                            i18n("Keep Above Others"), ButtonAbove, button_size, ::factory->squareButton(), true);
                    connect(button[ButtonAbove], SIGNAL(clicked()),
                            this, SLOT(aboveButtonPressed()));
                    layout->addWidget(button[ButtonAbove]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'B': // Below button
                if ((!button[ButtonBelow])) {
                    button[ButtonBelow] =
                        new polyesterButton(this, "keep_below.png",
                                            i18n("Keep Below Others"), ButtonBelow, button_size, ::factory->squareButton(), true);
                    connect(button[ButtonBelow], SIGNAL(clicked()),
                            this, SLOT(belowButtonPressed()));
                    layout->addWidget(button[ButtonBelow]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'L': // Shade button
                if ((!button[ButtonShade && isShadeable()])) {
                    if ( isSetShade()) {
                        tip = i18n("Unshade");
                    } else {
                        tip = i18n("Shade");
                    }
                    button[ButtonShade] =
                        new polyesterButton(this, "splat.png", tip, ButtonShade, button_size, ::factory->squareButton(), true);
                    connect(button[ButtonShade], SIGNAL(clicked()),
                            this, SLOT(shadeButtonPressed()));
                    layout->addWidget(button[ButtonShade]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case '_': // Spacer item
                layout->addSpacing(frameSize);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void polyesterClient::activeChange() {
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n])
            button[n]->reset();
    maskDirty = true;
    widget()->repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// captionChange()
// ---------------
// The title has changed

void polyesterClient::captionChange() {
    aDoubleBufferDirty = iDoubleBufferDirty = true;
    widget()->repaint(titlebar_->geometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void polyesterClient::desktopChange() {
    bool d = isOnAllDesktops();
    if (button[ButtonSticky]) {
        QToolTip::remove(button[ButtonSticky]);
        QToolTip::add(button[ButtonSticky], d ? i18n("Un-Sticky") : i18n("Sticky"));
        button[ButtonSticky]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// iconChange()
// ------------
// The title has changed

void polyesterClient::iconChange() {
    if (button[ButtonMenu]) {
        button[ButtonMenu]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void polyesterClient::maximizeChange() {
    maskDirty = aDoubleBufferDirty = iDoubleBufferDirty = true;
    bool m = (maximizeMode() == MaximizeFull);
    if (button[ButtonMax]) {
        QToolTip::remove(button[ButtonMax]);
        QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
        button[ButtonMax]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeChange()
// -------------
// Called when window shading changes

void polyesterClient::shadeChange() {
    bool s = isSetShade();
    if (button[ButtonShade]) {
        QToolTip::remove(button[ButtonShade]);
        QToolTip::add(button[ButtonShade], s ? i18n("Unshade") : i18n("Shade"));
        button[ButtonShade]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// TODO: probably these two functions can be removed
// keepAboveChange()
// ------------
// The above state has changed

void polyesterClient::keepAboveChange(bool a) {
    if (button[ButtonAbove]) {
        button[ButtonAbove]->setOn(a);
        button[ButtonAbove]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// keepBelowChange()
// ------------
// The below state has changed

void polyesterClient::keepBelowChange(bool b) {
    if (button[ButtonBelow]) {
        button[ButtonBelow]->setOn(b);
        button[ButtonBelow]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void polyesterClient::borders(int &left, int &right, int &top, int &bottom) const {

    if ((maximizeMode()==MaximizeFull) && !options()->moveResizeMaximizedWindows()) {
        left = right = bottom = 0;
        top = s_titleHeight;

        // update layout etc.
        topSpacer_->changeSize(1, -1, QSizePolicy::Expanding, QSizePolicy::Fixed);
        decoSpacer_->changeSize(1, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
        leftSpacer_->changeSize(left, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
        leftTitleSpacer_->changeSize(left, s_titleHeight, QSizePolicy::Fixed, QSizePolicy::Fixed);
        rightSpacer_->changeSize(right, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
        rightTitleSpacer_->changeSize(right, s_titleHeight, QSizePolicy::Fixed, QSizePolicy::Fixed);
        bottomSpacer_->changeSize(1, bottom, QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        /*if the borders are rounded add more left and right borders*/
        if( roundedCorners )
          left = right = bottom = frameSize*2;
        else
          left = right = bottom = frameSize+1;
        top = titleSize + (frameSize*2);

        // update layout etc.
        topSpacer_->changeSize(1, frameSize, QSizePolicy::Expanding, QSizePolicy::Fixed);
        decoSpacer_->changeSize(1, frameSize, QSizePolicy::Expanding, QSizePolicy::Fixed);
        leftSpacer_->changeSize(left, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
        leftTitleSpacer_->changeSize(left, s_titleHeight, QSizePolicy::Fixed, QSizePolicy::Fixed);
        rightSpacer_->changeSize(right, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
        rightTitleSpacer_->changeSize(right,s_titleHeight,QSizePolicy::Fixed, QSizePolicy::Fixed);
        bottomSpacer_->changeSize(1, bottom, QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    widget()->layout()->activate();
}

//////////////////////////////////////////////////////////////////////////////
// resize()
// --------
// Called to resize the window

void polyesterClient::resize(const QSize &size) {
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// minimumSize()
// -------------
// Return the minimum allowable size for this window

QSize polyesterClient::minimumSize() const {
    return widget()->minimumSize();
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return logical mouse position

KDecoration::Position polyesterClient::mousePosition(const QPoint &point) const {
    const int corner = 24;
    Position pos;
    int fs = frameSize + handlebar;
    //int fs = ::factory->frameSize();

    if (point.y() <= fs) {
        // inside top frame
        if (point.x() <= corner)
            pos = PositionTopLeft;
        else if (point.x() >= (width()-corner))
            pos = PositionTopRight;
        else
            pos = PositionTop;
    } else if (point.y() >= (height()-fs*2)) {
        // inside handle
        if (point.x() <= corner)
            pos = PositionBottomLeft;
        else if (point.x() >= (width()-corner))
            pos = PositionBottomRight;
        else
            pos = PositionBottom;
    } else if (point.x() <= fs ) {
        // on left frame
        if (point.y() <= corner)
            pos = PositionTopLeft;
        else if (point.y() >= (height()-corner))
            pos = PositionBottomLeft;
        else
            pos = PositionLeft;
    } else if (point.x() >= width()-fs) {
        // on right frame
        if (point.y() <= corner)
            pos = PositionTopRight;
        else if (point.y() >= (height()-corner))
            pos = PositionBottomRight;
        else
            pos = PositionRight;
    } else {
        // inside the frame
        pos = PositionCenter;
    }
    return pos;
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Event filter

bool polyesterClient::eventFilter(QObject *obj, QEvent *e) {
    if (obj != widget())
        return false;

    switch (e->type()) {
    case QEvent::MouseButtonDblClick: {
            mouseDoubleClickEvent(static_cast<QMouseEvent *>(e));
            return true;
        }
    case QEvent::MouseButtonPress: {
            processMousePressEvent(static_cast<QMouseEvent *>(e));
            return true;
        }
    case QEvent::Wheel: {
           wheelEvent(static_cast< QWheelEvent* >(e));
           return true;
        }
    case QEvent::Paint: {
            paintEvent(static_cast<QPaintEvent *>(e));
            return true;
        }
    case QEvent::Resize: {
            resizeEvent(static_cast<QResizeEvent *>(e));
            return true;
        }
    case QEvent::Show: {
            showEvent(static_cast<QShowEvent *>(e));
            return true;
        }
    default: {
            return false;
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
// mouseDoubleClickEvent()
// -----------------------
// Doubleclick on title

void polyesterClient::mouseDoubleClickEvent(QMouseEvent *e) {
    if (titlebar_->geometry().contains(e->pos()))
        titlebarDblClickOperation();
}

//////////////////////////////////////////////////////////////////////////////
// wheelEvent()
// -----------------------
// Mousewheel over titlebar

void polyesterClient::wheelEvent(QWheelEvent * e) {
#if KDE_VERSION > KDE_MAKE_VERSION(3,4,3)
if (titleLayout_->geometry().contains(e->pos()))
titlebarMouseWheelOperation(e->delta());
#endif
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void polyesterClient::paintEvent(QPaintEvent* e) {
    if (!::factory->initialized())
    {
        return;
    }

    //int frameSize = ::factory->frameSize();
    const uint maxCaptionLength = 200; // truncate captions longer than this!
                                       // FIXME: truncate related to window size
//titlebar_->geometry().width()/(widget()->font().pointSize());
    QString captionText(caption());
    if (captionText.length() > maxCaptionLength) {
        captionText.truncate(maxCaptionLength);
        captionText.append(" [...]");
    }


    QColor blackColor(black);
    QColor redColor(red);
    QColorGroup group,widgetGroup;

    bool active = isActive();

    //doublebuffering everything to avoid the flicker
    QPainter finalPainter(widget());

    //an ugly debug red border used to thest the speed...
    //finalPainter.fillRect(e->rect(),red);return;

    // mask off which corners we dont want
    // otherwise recycle the old buffer
    /*if( doubleBuff.width()>0 )
    {
       finalPainter.drawPixmap(0,0, doubleBuff);
       finalPainter.end();
       return;
    }*/
    if( maskDirty )
        updateMask();
    if( !(active?aDoubleBufferDirty:iDoubleBufferDirty) && ((active?activeBuff.width():inactiveBuff.width()) == widget()->width() ))
    {
       finalPainter.drawPixmap(0,0, active?activeBuff:inactiveBuff );
       finalPainter.end();
       return;
    }


    if( active )
       activeBuff  =  QPixmap(widget()->width(), widget()->height());
    else
       inactiveBuff  =  QPixmap(widget()->width(), widget()->height());

    QPainter painter(active?&activeBuff:&inactiveBuff);
    //get group information first
    group = options()->colorGroup(KDecoration::ColorTitleBlend, active);
    widgetGroup = widget()->colorGroup();

    QRect topRect( topSpacer_->geometry() );
    QRect titleRect( titleLayout_->geometry() );
    QRect textRect( titlebar_->geometry() );
    QRect Rltitle( leftTitleSpacer_->geometry() );
    QRect Rrtitle( rightTitleSpacer_->geometry() );
    QRect Rdeco( decoSpacer_->geometry() );
    QRect Rleft( leftSpacer_->geometry() );
    QRect Rright( rightSpacer_->geometry() );
    QRect Rbottom( bottomSpacer_->geometry() );
    QRect tempRect;

    
    /*
    if(active)
    {
        qDebug("topRect.y()   = %i\tbottom() = %i",topRect.top(),topRect.bottom());
        qDebug("titleRect.y() = %i\tbottom() = %i",titleRect.top(),titleRect.bottom());
        qDebug("textRect.y()  = %i\tbottom() = %i",textRect.top(),textRect.bottom());
        qDebug("Rltitle.y()   = %i\tbottom() = %i",Rltitle.top(),Rltitle.bottom());
        qDebug("Rrtitle.y()   = %i\tbottom() = %i",Rrtitle.top(),Rrtitle.bottom());
        qDebug("Rdeco.y()     = %i\tbottom() = %i",Rdeco.top(),Rdeco.bottom());
        qDebug("Rleft.y()     = %i\tbottom() = %i",Rleft.top(),Rleft.bottom());
        qDebug("Rright.y()    = %i\tbottom() = %i",Rright.top(),Rright.bottom());
        qDebug("Rbottom.y()   = %i\tbottom() = %i",Rbottom.top(),Rbottom.bottom());
    }
    */

    // top
    painter.drawTiledPixmap(topRect, active ? *aTitleBarTopTile:*iTitleBarTopTile);
    painter.drawTiledPixmap(titleRect.x(),
                            titleRect.y(),
                            titleRect.width(),
                            titleRect.height() + Rdeco.height(),
                            active ? *aTitleBarTile:*iTitleBarTile);

    textRect.setRect(textRect.x()+SIDETITLEMARGIN,
                     textRect.y(),
                     textRect.width()-SIDETITLEMARGIN*2,
                     textRect.height());


    //is the title text too long?
    bool titleTooLong = false;
    Qt::AlignmentFlags titleAlign = ::factory->titleAlign();
    const int gradientWidth = 60;
    if( QFontMetrics(widget()->font()).width(captionText) > (textRect.width() - gradientWidth) )
    {
        titleTooLong = true;
        titleAlign = Qt::AlignLeft;
    }

    //we are shadowing title bar text only if there is some text
    if(::factory->titleShadow() && textRect.width()>0 && textRect.height()>0)
    {
        QPixmap textPixmap;
        QPainter tempPainter;
        QColor shadowColor;

        textPixmap = QPixmap(textRect.width(), textRect.height());
        textPixmap.fill(QColor(0,0,0));
        textPixmap.setMask( textPixmap.createHeuristicMask(TRUE) );
        tempPainter.begin(&textPixmap);


        //shadow text
        tempPainter.setFont(options()->font(isActive(), false));

        tempPainter.setPen(white);

        tempPainter.drawText(1, 1, textRect.width(), textRect.height(),
                         titleAlign | AlignVCenter | Qt::SingleLine,
                         captionText);
        tempPainter.end();

        // make the cute blurred text shadow from Plastik engine
        QImage shadow;
        ShadowEngine se;

        //deciding if the shadow will be black or white
        if(qGray(options()->color(KDecoration::ColorFont, isActive()).rgb()) > 150)
            shadowColor = blackColor;
        else
            shadowColor = white;

        shadow = se.makeShadow(textPixmap, shadowColor);

        painter.drawImage(textRect, shadow);

    }

    // draw title text
    painter.setFont(options()->font(isActive(), false));
    painter.setPen(options()->color(KDecoration::ColorFont, isActive()));
    painter.drawText(textRect,
                     titleAlign | AlignVCenter | Qt::SingleLine,
                     captionText);

    // make title fade out when the caption is too long, veery slow but veery cool :-)
    if( titleTooLong )
    {
        int xGradient = textRect.right()-(gradientWidth-1);

        QPixmap backLayerP = QPixmap(gradientWidth, textRect.height());
        QPainter layerPainter(&backLayerP);
        layerPainter.drawTiledPixmap(backLayerP.rect(), active ? *aTitleBarTile:*iTitleBarTile);
        layerPainter.end();
        QImage backLayer = backLayerP.convertToImage();

        QImage textLayer = ( active ?
                             activeBuff.convertToImage():
                             inactiveBuff.convertToImage()
                            ).copy( xGradient, textRect.y(),
                                    gradientWidth, textRect.height()
                                  );
        KImageEffect::blend(backLayer,  textLayer, KImageEffect::HorizontalGradient);
        painter.drawImage(xGradient, textRect.y(), backLayer);

    }

    //left of buttons and title
    painter.drawTiledPixmap(Rltitle.x(),
                            Rltitle.y(),
                            Rltitle.width(),
                            Rltitle.height()+Rdeco.height(),
                            active ? *aTitleBarTile:*iTitleBarTile);

    //left 3d effect
    painter.drawTiledPixmap(Rltitle.x(),
                            Rltitle.y(),
                            2,
                            Rltitle.height()+Rdeco.height(),
                            active ? *aTitleBarTopTile:*iTitleBarTopTile);

    // left mid layout
    painter.fillRect(Rleft,widgetGroup.background());

    // right of buttons and title
    painter.drawTiledPixmap(Rrtitle.x(),
                            Rrtitle.y(),
                            Rrtitle.width(),
                            Rrtitle.height()+Rdeco.height(),
                            active ? *aTitleBarTile:*iTitleBarTile);
    //right 3d effect
    painter.drawTiledPixmap(width()-2,
                            Rltitle.y(),
                            2,
                            Rltitle.height()+Rdeco.height(),
                            active ? *aTitleBarTopTile:*iTitleBarTopTile);

    // right mid layout
    painter.fillRect(Rright,widgetGroup.background());
    
    // bottom
    /*
    if(isShade())
    {
        frame.setRect(0,::factory->titleSize()+FRAMESIZE, width(), FRAMESIZE);
    }
    else
    {
        frame.setRect(0, height() - (FRAMESIZE*2), width(),  (FRAMESIZE*2));
    }
    */
    painter.fillRect(Rbottom, KDecoration::options()->color(ColorTitleBlend, active));//widgetGroup.background());

    //if the window is maximized the border is invisible
    if( (maximizeMode()==MaximizeFull) && !options()->moveResizeMaximizedWindows() )
      painter.setPen(KDecoration::options()->color(ColorTitleBar, active));
    else
      painter.setPen(group.background().dark(150));

    // outline outside the frame but not the corners if they are rounded
    tempRect = widget()->rect();
    painter.drawRect(tempRect);

    if(roundedCorners && !maximizeMode()) {
        // local temp right and bottom
        int r(width());
        painter.setPen(group.background().dark(150));
        painter.drawPoint(4, 1);
        painter.drawPoint(3, 1);
        painter.drawPoint(2, 2);
        painter.drawPoint(1, 3);
        painter.drawPoint(1, 4);
        painter.drawPoint(r - 5, 1);
        painter.drawPoint(r - 4, 1);
        painter.drawPoint(r - 3, 2);
        painter.drawPoint(r - 2, 3);
        painter.drawPoint(r - 2, 4);

        painter.setPen(KDecoration::options()->color(ColorTitleBar, active).light(160));
        painter.drawPoint(4, 2);
        painter.drawPoint(3, 2);
        painter.drawPoint(2, 3);
        painter.drawPoint(2, 4);

        //painter.setPen(KDecoration::options()->color(ColorTitleBlend, active));
        painter.drawPoint(r - 5, 2);
        painter.drawPoint(r - 4, 2);
        painter.drawPoint(r - 3, 3);
        painter.drawPoint(r - 3, 4);
    } 

    // finally copy the buffer into the widget
    //finalPainter.begin(doubleBuff);
    finalPainter.drawPixmap(0,0, active?activeBuff:inactiveBuff);
    finalPainter.end();

    if( active )
        aDoubleBufferDirty = false;
    else
        iDoubleBufferDirty = false;
}

//////////////////////////////////////////////////////////////////////////////
// updateMask()
// ------------
// update the frame mask
void polyesterClient::updateMask() {

    if ( (!options()->moveResizeMaximizedWindows() && maximizeMode() & MaximizeFull ) ) 
    {
        setMask(QRegion(widget()->rect()));
        return;
    }

    int r(width());
    int b(height());
    QRegion mask;

    mask=QRegion(widget()->rect());
    mask=QRegion( 0, 0, r, b );

   // Remove top-left corner.
    if( roundedCorners &&
        maximizeMode() != KDecorationDefines::MaximizeFull )
    {
        mask -= QRegion(0, 0, 5, 1);
        mask -= QRegion(0, 1, 3, 1);
        mask -= QRegion(0, 2, 2, 1);
        mask -= QRegion(0, 3, 1, 2);
        mask -= QRegion(r - 5, 0, 5, 1);
        mask -= QRegion(r - 3, 1, 3, 1);
        mask -= QRegion(r - 2, 2, 2, 1);
        mask -= QRegion(r - 1, 3, 1, 2);
    }
    //always remove one corner pixel so it simulates a soft corner like plastik
    mask -= QRegion(0,0,1,1);
    mask -= QRegion(r-1,0,1,1);
    mask -= QRegion(0, b-1, 1,1);
    mask -= QRegion(r-1,b-1,1,1);

    setMask(mask, 1);

    maskDirty =  false;
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void polyesterClient::resizeEvent(QResizeEvent *) {
    maskDirty = aDoubleBufferDirty = iDoubleBufferDirty = true;
    if (widget()->isShown()) {
        QRegion region = widget()->rect();
        region = region.subtract(titlebar_->geometry());
        widget()->erase(region);
    }
    updateMask();
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void polyesterClient::showEvent(QShowEvent *) {
    widget()->repaint();
    updateMask();
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void polyesterClient::maxButtonPressed() {
    if (button[ButtonMax]) {
#if KDE_IS_VERSION(3, 3, 0)
        maximize(button[ButtonMax]->lastMousePress());
#else

        switch (button[ButtonMax]->lastMousePress()) {
        case MidButton:
            maximize(maximizeMode() ^ MaximizeVertical);
            break;
        case RightButton:
            maximize(maximizeMode() ^ MaximizeHorizontal);
            break;
        default:
            (maximizeMode() == MaximizeFull) ? maximize(MaximizeRestore)
            : maximize(MaximizeFull);
        }
#endif

    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeButtonPressed()
// -----------------
// Shade button was pressed

void polyesterClient::shadeButtonPressed() {
    if (button[ButtonShade]) {
        setShade( !isSetShade());
    }
}

//////////////////////////////////////////////////////////////////////////////
// aboveButtonPressed()
// -----------------
// Above button was pressed

void polyesterClient::aboveButtonPressed() {
    if (button[ButtonAbove]) {
        setKeepAbove( !keepAbove());
    }
}

//////////////////////////////////////////////////////////////////////////////
// belowButtonPressed()
// -----------------
// Below button was pressed

void polyesterClient::belowButtonPressed() {
    if (button[ButtonBelow]) {
        setKeepBelow( !keepBelow());
    }
}

//////////////////////////////////////////////////////////////////////////////
// menuButtonPressed()
// -------------------
// Menu button was pressed (popup the menu)

void polyesterClient::menuButtonPressed() {
    static QTime* t = NULL;
    static polyesterClient* lastClient = NULL;
    if (t == NULL)
        t = new QTime;
    bool dbl = (lastClient==this && t->elapsed() <= QApplication::doubleClickInterval());
    lastClient = this;
    t->start();
    //if (button[ButtonMenu] && !dbl && !::factory->menuClosed()) {
    if ( !dbl || !::factory->menuClosed()) {
        QPoint p(button[ButtonMenu]->rect().bottomLeft().x(),
                 button[ButtonMenu]->rect().bottomLeft().y());
        KDecorationFactory* f = factory();
        showWindowMenu(button[ButtonMenu]->mapToGlobal(p));
        if (!f->exists(this))
            return; // decoration was destroyed
        button[ButtonMenu]->setDown(false);
    }
    else
    {
        closing = true;
    }
}

void polyesterClient::menuButtonReleased()
{
    if(closing)
    {
        closeWindow();
    }
}

#include "polyester.moc"

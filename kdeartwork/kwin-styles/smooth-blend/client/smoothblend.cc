//////////////////////////////////////////////////////////////////////////////
// smoothblend.cc
// -------------------
// Smooth Blend window decoration for KDE
// -------------------
// Copyright (c) 2005 Ryan Nickell
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

#include "smoothblend.h"
#include "buttons.h"

using namespace smoothblend;

//////////////////////////////////////////////////////////////////////////////
// smoothblendFactory Class
//////////////////////////////////////////////////////////////////////////////
smoothblendFactory* factory=NULL;

bool smoothblendFactory::initialized_              = false;
Qt::AlignmentFlags smoothblendFactory::titlealign_ = Qt::AlignHCenter;
bool smoothblendFactory::cornerflags_               = true;
int smoothblendFactory::titlesize_                 = 30;
int smoothblendFactory::buttonsize_                = 26;
int smoothblendFactory::framesize_                 = 4;
int smoothblendFactory::roundsize_                 = 50;
bool smoothblendFactory::titleshadow_              = true;
bool smoothblendFactory::animatebuttons            = true;
int smoothblendFactory::btnComboBox                = 0;
bool smoothblendFactory::menuClose                 = false;

// global constants
static const int TOPMARGIN       = 4; // do not change
static const int DECOHEIGHT      = 4; // do not change
static const int SIDETITLEMARGIN = 2;
// Default button layout
const char default_left[]  = "M";
const char default_right[] = "HIAX";

static const uint TIMERINTERVAL = 50; // msec
static const uint ANIMATIONSTEPS = 4;

extern "C" KDecorationFactory* create_factory() {
    return new smoothblend::smoothblendFactory();
}

//////////////////////////////////////////////////////////////////////////////
// smoothblendFactory()
// ----------------
// Constructor

smoothblendFactory::smoothblendFactory() {
    readConfig();
    initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// ~smoothblendFactory()
// -----------------
// Destructor

smoothblendFactory::~smoothblendFactory() {
    initialized_ = false;
}

//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// -----------------
// Create the decoration

KDecoration* smoothblendFactory::createDecoration(KDecorationBridge* b) {
    return new smoothblendClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a repaint is necessary

bool smoothblendFactory::reset(unsigned long changed) {
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

bool smoothblendFactory::readConfig() {
    // create a config object
    KConfig config("kwinsmoothblendrc");
    config.setGroup("General");

    // grab settings
    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft")
        titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter")
        titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight")
        titlealign_ = Qt::AlignRight;

    cornerflags_ = config.readBoolEntry("RoundCorners", true);
    titlesize_ = config.readNumEntry("TitleSize",30);
    buttonsize_ = config.readNumEntry("ButtonSize",26);
    framesize_ = config.readNumEntry("FrameSize",4);
    roundsize_ = config.readNumEntry("RoundPercent",50);
    titleshadow_ = config.readBoolEntry("TitleShadow", true);
    animatebuttons = config.readBoolEntry("AnimateButtons", true);
    btnComboBox = config.readNumEntry("ButtonComboBox", 0);
    menuClose = config.readBoolEntry("CloseOnMenuDoubleClick");

    if(buttonsize_ > titlesize_ - framesize_)
    {
        buttonsize_ = titlesize_-framesize_;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// smoothblendButton Class 
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// smoothblendButton()
// ---------------
// Constructor
smoothblendButton::smoothblendButton(smoothblendClient *parent, const char *name, const QString& tip, ButtonType type, int button_size, bool toggle): QButton(parent->widget(), name), 
     client_(parent), 
     type_(type), 
     size_(button_size), 
     deco_(0), 
     lastmouse_(NoButton), 
     hover_(false)
{
    setBackgroundMode(NoBackground);
    setFixedSize( ::factory->buttonSize(), ::factory->buttonSize());
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

smoothblendButton::~smoothblendButton() {
    if ( deco_ )
        delete deco_;
}

//////////////////////////////////////////////////////////////////////////////
// sizeHint()
// ----------
// Return size hint

QSize smoothblendButton::sizeHint() const {
    return QSize(::factory->buttonSize(), ::factory->buttonSize());
}

//////////////////////////////////////////////////////////////////////////////
// buttonClicked()
// ----------
// Button animation progress reset so we don't get any leave event animation
// when the mouse is still pressed
void smoothblendButton::buttonClicked() {
    m_clicked=true;
    animProgress=0;
}
void smoothblendButton::buttonReleased() {
    //This doesn't work b/c a released() signal is thrown when a leaveEvent occurs
    //m_clicked=false;
}

//////////////////////////////////////////////////////////////////////////////
// animate()
// ----------
// Button animation timing
void smoothblendButton::animate() {
    animTmr->stop();

    if(hover_) {
        if(animProgress < ANIMATIONSTEPS) {
            if (::factory->animateButtons() ) {
                animProgress++;
            } else {
                animProgress = ANIMATIONSTEPS;
            }
            animTmr->start(TIMERINTERVAL, true); // single-shot
        }
    } else {
        if(animProgress > 0) {
            if (::factory->animateButtons() ) {
                animProgress--;
            } else {
                animProgress = 0;
            }
            animTmr->start(TIMERINTERVAL, true); // single-shot
        }
    }
    repaint(false);
}
//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void smoothblendButton::enterEvent(QEvent *e) {
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

void smoothblendButton::leaveEvent(QEvent *e) {
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

void smoothblendButton::mousePressEvent(QMouseEvent* e) {
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

void smoothblendButton::mouseReleaseEvent(QMouseEvent* e) {
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

void smoothblendButton::setOn(bool on)
{
    QButton::setOn(on);
}

void smoothblendButton::setDown(bool on)
{
    QButton::setDown(on);
}

//////////////////////////////////////////////////////////
// getButtonImage()
// ----------------
// get the button QImage based on type and window mode
QImage smoothblendButton::getButtonImage(ButtonType type)
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
                finalImage = uic_findImage( "splat.png" );
            }
            else
            {
                finalImage = uic_findImage( "circle.png" );
            }
            break;
        case ButtonShade:
            if(client_->isShade())
            {
                finalImage = uic_findImage( "shade.png" );
            }
            else
            {
                finalImage = uic_findImage( "shade.png" );
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
    return finalImage;
}

//////////////////////////////////////////////////////////
// drawButton()
// -------------------------
// draw the pixmap button

void smoothblendButton::drawButton( QPainter *painter ) {
    if ( !smoothblendFactory::initialized() )
        return ;
    
    int newWidth = width() - 2;
    int newHeight = height() - 2;
    int dx = (width() - newWidth) / 2;
    int dy = (height() - newHeight) / 2;
    QImage tmpResult;
    QColorGroup group;
    QColor redColor(red);
    bool active = client_->isActive();
    QPixmap backgroundTile = client_->getTitleBarTile(active);
    group = KDecoration::options()->colorGroup(KDecoration::ColorTitleBar, active);

    //draw the titlebar behind the buttons and app icons
    if ((client_->maximizeMode()==client_->MaximizeFull) && !KDecoration::options()->moveResizeMaximizedWindows())
    {
        painter->drawTiledPixmap(0, 0, width(), height(), backgroundTile);
    }
    else
    {
        painter->drawTiledPixmap(0, 0, width(), height(), backgroundTile, 0, y()-::factory->frameSize());
    }
    
    QImage buttonImage = getButtonImage(type_).smoothScale( width(),height());
    buttonImage = KImageEffect::blend( group.background(), buttonImage, .50);
    if (type_ == ButtonMenu) {
        //slight movement to show the menu button is depressed
        if (isDown()) {
            dx++;
            dy++;
        }
        QPixmap menuButtonPixmap(client_->icon().pixmap(QIconSet::Large, QIconSet::Normal));
        QImage menuButtonImage(menuButtonPixmap.convertToImage());
        //draw the menu button the same size as the other buttons
        //using QIconSet::Large gives us a 32x32 icon to resize, resizing larger than
        //that may produce pixilation of the image
        painter->drawImage( dx, dy, menuButtonImage.smoothScale(newWidth, newHeight) );
    } else {
        //highlight on a mouse over repaint
        double factor = animProgress * 0.13;

        if(!isDown())
        {
            switch(::factory->getBtnComboBox())
            {
                case 0:
                    tmpResult = KImageEffect::intensity( buttonImage, factor);
                    break;
                case 1:
                    tmpResult = KImageEffect::fade( buttonImage, factor, group.background());
                    break;
            }
        }
        else
        {
            tmpResult = buttonImage;
        }
        painter->drawPixmap( 0, 0, QPixmap( tmpResult ) );
    }
}


//////////////////////////////////////////////////////////////////////////////
// smoothblendClient Class
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// smoothblendClient()
// ---------------
// Constructor

smoothblendClient::smoothblendClient(KDecorationBridge *b, KDecorationFactory *f)
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
    s_titleHeight(0),
    s_titleFont(QFont()),
    closing(false)
    {
        aCaptionBuffer = new QPixmap();
        iCaptionBuffer = new QPixmap();
        //s_titleFont = isTool()?smoothblendFactory::titleFontTool():smoothblendFactory::titleFont();
        s_titleFont = options()->font();
        s_titleHeight = smoothblendFactory::titleSize();
    }
//////////////////////////////////////////////////////////////////////////////////
// ~smoothblendClient()
// --------------------
// Destructor
smoothblendClient::~smoothblendClient() {
    delete aCaptionBuffer;
    delete iCaptionBuffer;
}

void smoothblendClient::create_pixmaps() {
    if(pixmaps_created)
        return;
    KPixmap tempPixmap;
    QPainter painter;
    QColorGroup group,widgetGroup;
    int FRAMESIZE = ::factory->frameSize();
    // Get the color groups we need for the gradients
    group = options()->colorGroup(KDecoration::ColorTitleBar, true);
    widgetGroup = widget()->colorGroup();
    
    // active top title bar tile
    tempPixmap.resize(1, TOPMARGIN);
    tempPixmap = KPixmapEffect::unbalancedGradient(tempPixmap,
                                                   group.background(),
                                                   widgetGroup.background(),
                                                   KPixmapEffect::VerticalGradient,
                                                   100,
                                                   -100);
    aTitleBarTopTile = new QPixmap(1, TOPMARGIN);
    painter.begin(aTitleBarTopTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();
    
    // inactive top title bar tile
    group = options()->colorGroup(KDecoration::ColorTitleBar, false);
    tempPixmap = KPixmapEffect::unbalancedGradient(tempPixmap,
                                                   group.background(),
                                                   widgetGroup.background(),
                                                   KPixmapEffect::VerticalGradient,
                                                   100,
                                                   -100);
    iTitleBarTopTile = new QPixmap(1, TOPMARGIN);
    painter.begin(iTitleBarTopTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();
    
    // active title bar tile
    tempPixmap.resize(1, s_titleHeight+FRAMESIZE);
    group = options()->colorGroup(KDecoration::ColorTitleBar, true);
    tempPixmap = KPixmapEffect::unbalancedGradient(tempPixmap,
                                                   group.background(),
                                                   widgetGroup.background(),
                                                   KPixmapEffect::VerticalGradient,
                                                   100,
                                                   200);
    aTitleBarTile = new QPixmap(1, s_titleHeight+FRAMESIZE);
    painter.begin(aTitleBarTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();
    
    // inactive title bar tile
    group = options()->colorGroup(KDecoration::ColorTitleBar, false);
    tempPixmap = KPixmapEffect::unbalancedGradient(tempPixmap,
                                                   group.background(),
                                                   widgetGroup.background(),
                                                   KPixmapEffect::VerticalGradient,
                                                   100,
                                                   200);
    iTitleBarTile = new QPixmap(1, s_titleHeight+FRAMESIZE);
    painter.begin(iTitleBarTile);
    painter.drawPixmap(0, 0, tempPixmap);
    painter.end();

    pixmaps_created = true;
}

void smoothblendClient::delete_pixmaps() {
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

void smoothblendClient::init() {
    createMainWidget(WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);
    handlebar = ::factory->frameSize() < 4 ? 4 - ::factory->frameSize() : 0;
    // for flicker-free redraws
    widget()->setBackgroundMode(NoBackground);

    _resetLayout();

    create_pixmaps();
}
void smoothblendClient::_resetLayout()
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
    int FRAMESIZE = ::factory->frameSize();

    topSpacer_        = new QSpacerItem(1, FRAMESIZE, QSizePolicy::Expanding, QSizePolicy::Fixed);
    titlebar_         = new QSpacerItem(1, ::factory->buttonSize(),
                                        QSizePolicy::Expanding, QSizePolicy::Fixed);
    leftTitleSpacer_  = new QSpacerItem(FRAMESIZE, s_titleHeight,
                                        QSizePolicy::Fixed, QSizePolicy::Fixed);
    rightTitleSpacer_ = new QSpacerItem(FRAMESIZE, s_titleHeight,
                                        QSizePolicy::Fixed, QSizePolicy::Fixed);
    decoSpacer_       = new QSpacerItem(1, FRAMESIZE, QSizePolicy::Expanding, QSizePolicy::Fixed);
    leftSpacer_       = new QSpacerItem(::factory->frameSize(), 1,
                                        QSizePolicy::Fixed, QSizePolicy::Expanding);
    rightSpacer_      = new QSpacerItem(::factory->frameSize(), 1,
                                        QSizePolicy::Fixed, QSizePolicy::Expanding);
    bottomSpacer_     = new QSpacerItem(1, ::factory->frameSize(),
                                        QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    // sizeof(...) is calculated at compile time
    memset(button, 0, sizeof(smoothblendButton *) * ButtonTypeCount);

    // message in preview widget
    if (isPreview()) {
        windowLayout_->addWidget(
            new QLabel( i18n("<b><center>Smooth Blend</center></b>"), widget() ), 1 ); 
    } else {
        windowLayout_->addItem(new QSpacerItem(0, 0));
    }

    // setup titlebar buttons
    for (int n=0; n<ButtonTypeCount; n++)
        button[n] = 0;
    //Deal with the title and buttons
    titleLayout_->addItem(leftTitleSpacer_);
    addButtons(titleLayout_,
               options()->customButtonPositions() ? options()->titleButtonsLeft() : QString(default_left),
               ::factory->titleSize()-1);
    titleLayout_->addItem(titlebar_);
    addButtons(titleLayout_,
               options()->customButtonPositions() ? options()->titleButtonsRight() : QString(default_right),
               ::factory->titleSize()-1);
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
    connect(this, SIGNAL(keepAboveChanged(bool)), SLOT(keepAboveChange(bool)));
    connect(this, SIGNAL(keepBelowChanged(bool)), SLOT(keepBelowChange(bool)));
}

//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title layout

void smoothblendClient::addButtons(QBoxLayout *layout, const QString& s, int button_size) {
    QString tip;
    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
            case 'M': // Menu button
                if (!button[ButtonMenu]) {
                    button[ButtonMenu] =
                        new smoothblendButton(this, "splat.png", i18n("Menu"),ButtonMenu,button_size);
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
                        new smoothblendButton(this, "circle.png", tip, ButtonSticky, button_size, true);
                    connect(button[ButtonSticky], SIGNAL(clicked()),
                            this, SLOT(toggleOnAllDesktops()));
                    layout->addWidget(button[ButtonSticky]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'H': // Help button
                if ((!button[ButtonHelp]) && providesContextHelp()) {
                    button[ButtonHelp] =
                        new smoothblendButton(this, "help.png", i18n("Help"), ButtonHelp, button_size);
                    connect(button[ButtonHelp], SIGNAL(clicked()),
                            this, SLOT(showContextHelp()));
                    layout->addWidget(button[ButtonHelp]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'I': // Minimize button
                if ((!button[ButtonMin]) && isMinimizable())  {
                    button[ButtonMin] =
                        new smoothblendButton(this, "minimize.png", i18n("Minimize"), ButtonMin, button_size);
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
                        new smoothblendButton(this, "maximize.png", tip, ButtonMax, button_size, true);
                    connect(button[ButtonMax], SIGNAL(clicked()),
                            this, SLOT(maxButtonPressed()));
                    layout->addWidget(button[ButtonMax]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'X': // Close button
                if ((!button[ButtonClose]) && isCloseable()) {
                    button[ButtonClose] =
                        new smoothblendButton(this, "close.png", i18n("Close"), ButtonClose, button_size);
                    connect(button[ButtonClose], SIGNAL(clicked()),
                            this, SLOT(closeWindow()));
                    layout->addWidget(button[ButtonClose]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'F': // Above button
                if ((!button[ButtonAbove])) {
                    button[ButtonAbove] =
                        new smoothblendButton(this, "keep_above.png",
                                          i18n("Keep Above Others"), ButtonAbove, button_size, true);
                    connect(button[ButtonAbove], SIGNAL(clicked()),
                            this, SLOT(aboveButtonPressed()));
                    layout->addWidget(button[ButtonAbove]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'B': // Below button
                if ((!button[ButtonBelow])) {
                    button[ButtonBelow] =
                        new smoothblendButton(this, "keep_below.png",
                                          i18n("Keep Below Others"), ButtonBelow, button_size, true);
                    connect(button[ButtonBelow], SIGNAL(clicked()),
                            this, SLOT(belowButtonPressed()));
                    layout->addWidget(button[ButtonBelow]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case 'L': // Shade button
                if ((!button[ButtonShade]) && isShadeable()) {
                    if ( isSetShade()) {
                        tip = i18n("Unshade");
                    } else {
                        tip = i18n("Shade");
                    }
                    button[ButtonShade] =
                        new smoothblendButton(this, "shade.png", tip, ButtonShade, button_size, true);
                    connect(button[ButtonShade], SIGNAL(clicked()),
                            this, SLOT(shadeButtonPressed()));
                    layout->addWidget(button[ButtonShade]);
                    if (n < s.length()-1) layout->addSpacing(1);
                }
                break;

            case '_': // Spacer item
                layout->addSpacing(::factory->frameSize());
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void smoothblendClient::activeChange() {
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n])
            button[n]->reset();
    widget()->repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// captionChange()
// ---------------
// The title has changed

void smoothblendClient::captionChange() {
    widget()->repaint(titlebar_->geometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void smoothblendClient::desktopChange() {
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

void smoothblendClient::iconChange() {
    if (button[ButtonMenu]) {
        button[ButtonMenu]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void smoothblendClient::maximizeChange() {
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

void smoothblendClient::shadeChange() {
    bool s = isSetShade();
    if (button[ButtonShade]) {
        QToolTip::remove(button[ButtonShade]);
        QToolTip::add(button[ButtonShade], s ? i18n("Unshade") : i18n("Shade"));
        button[ButtonShade]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// keepAboveChange()
// ------------
// The above state has changed

void smoothblendClient::keepAboveChange(bool a) {
    if (button[ButtonAbove]) {
        button[ButtonAbove]->setOn(a);
        button[ButtonAbove]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// keepBelowChange()
// ------------
// The below state has changed

void smoothblendClient::keepBelowChange(bool b) {
    if (button[ButtonBelow]) {
        button[ButtonBelow]->setOn(b);
        button[ButtonBelow]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void smoothblendClient::borders(int &left, int &right, int &top, int &bottom) const {
    int FRAMESIZE = ::factory->frameSize();

    if ((maximizeMode()==MaximizeFull) && !options()->moveResizeMaximizedWindows()) {
        left = right = bottom = 0;
        top = ::factory->buttonSize();

        // update layout etc.
        topSpacer_->changeSize(1, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
        decoSpacer_->changeSize(1, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
        leftSpacer_->changeSize(left, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
        leftTitleSpacer_->changeSize(left, top, QSizePolicy::Fixed, QSizePolicy::Fixed);
        rightSpacer_->changeSize(right, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
        rightTitleSpacer_->changeSize(right, top, QSizePolicy::Fixed, QSizePolicy::Fixed);
        bottomSpacer_->changeSize(1, bottom, QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        left = right = bottom = ::factory->frameSize();
        top = ::factory->titleSize() + (FRAMESIZE*2);

        // update layout etc.
        topSpacer_->changeSize(1, FRAMESIZE, QSizePolicy::Expanding, QSizePolicy::Fixed);
        decoSpacer_->changeSize(1, FRAMESIZE, QSizePolicy::Expanding, QSizePolicy::Fixed);
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

void smoothblendClient::resize(const QSize &size) {
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// minimumSize()
// -------------
// Return the minimum allowable size for this window

QSize smoothblendClient::minimumSize() const {
    return widget()->minimumSize();
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return logical mouse position

KDecoration::Position smoothblendClient::mousePosition(const QPoint &point) const {
    const int corner = 24;
    Position pos;
    int fs = ::factory->frameSize() + handlebar;
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

bool smoothblendClient::eventFilter(QObject *obj, QEvent *e) {
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
    case QEvent::Wheel: {
            wheelEvent( static_cast< QWheelEvent* >( e ));
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

void smoothblendClient::mouseDoubleClickEvent(QMouseEvent *e) {
    if (titlebar_->geometry().contains(e->pos()))
        titlebarDblClickOperation();
}

//////////////////////////////////////////////////////////////////////////////
// wheelEvent()
// ------------
// Mouse wheel on titlebar

void smoothblendClient::wheelEvent(QWheelEvent *e)
{
    if (titleLayout_->geometry().contains(e->pos()) )
        titlebarMouseWheelOperation( e->delta());
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void smoothblendClient::paintEvent(QPaintEvent*) {
    if (!::factory->initialized())
    {
        return;
    }
    
    //int FRAMESIZE = ::factory->frameSize();
    const uint maxCaptionLength = 300; // truncate captions longer than this!
    QString captionText(caption());
    if (captionText.length() > maxCaptionLength) {
        captionText.truncate(maxCaptionLength);
        captionText.append(" [...]");
    }

    QColor blackColor(black);
    QColor redColor(red);
    QColorGroup group,widgetGroup;
    QPainter painter(widget());
    bool active = isActive();
    //get group information first
    group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
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
        qDebug("paintEvent() topRect.y()   = %i\tbottom() = %i",topRect.top(),topRect.bottom());
        qDebug("paintEvent() titleRect.y() = %i\tbottom() = %i",titleRect.top(),titleRect.bottom());
        qDebug("paintEvent() textRect.y()  = %i\tbottom() = %i",textRect.top(),textRect.bottom());
        qDebug("paintEvent() Rltitle.y()   = %i\tbottom() = %i",Rltitle.top(),Rltitle.bottom());
        qDebug("paintEvent() Rrtitle.y()   = %i\tbottom() = %i",Rrtitle.top(),Rrtitle.bottom());
        qDebug("paintEvent() Rdeco.y()     = %i\tbottom() = %i",Rdeco.top(),Rdeco.bottom());
        qDebug("paintEvent() Rleft.y()     = %i\tbottom() = %i",Rleft.top(),Rleft.bottom());
        qDebug("paintEvent() Rright.y()    = %i\tbottom() = %i",Rright.top(),Rright.bottom());
        qDebug("paintEvent() Rbottom.y()   = %i\tbottom() = %i",Rbottom.top(),Rbottom.bottom());
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
    QRect shadowRect(textRect.x()+1,textRect.y()+1,textRect.width(),textRect.height());
    //if we are shadowing title bar text
    if(::factory->titleShadow())
    {
        //shadow text
        painter.setFont(options()->font(isActive(), false));
        painter.setPen(blackColor);
        painter.drawText(shadowRect,
                         ::factory->titleAlign() | AlignVCenter | Qt::SingleLine,
                         captionText);
    }
    // draw title text
    painter.setFont(options()->font(isActive(), false));
    painter.setPen(options()->color(KDecoration::ColorFont, isActive()));
    painter.drawText(textRect,
                     ::factory->titleAlign() | AlignVCenter | Qt::SingleLine,
                     captionText);
    
    //left of buttons and title
    painter.drawTiledPixmap(Rltitle.x(),
                            Rltitle.y(),
                            Rltitle.width(),
                            Rltitle.height()+Rdeco.height(),
                            active ? *aTitleBarTile:*iTitleBarTile);
    // left mid layout
    painter.fillRect(Rleft,widgetGroup.background());

    // right of buttons and title
    painter.drawTiledPixmap(Rrtitle.x(),
                            Rrtitle.y(),
                            Rrtitle.width(),
                            Rrtitle.height()+Rdeco.height(),
                            active ? *aTitleBarTile:*iTitleBarTile);
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
    painter.fillRect(Rbottom, widgetGroup.background());

    //draw a line between title bar and window contents
    painter.setPen(group.background());

    // outline outside the frame but not the corners if they are rounded
    tempRect = widget()->rect();
    painter.drawRect(tempRect);

    bool cornersFlag = ::factory->roundedCorners();
    if(cornersFlag) {
        // local temp right and bottom
        int r(width());
        painter.setPen(group.background());
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
    } 

}

//////////////////////////////////////////////////////////////////////////////
// updateMask()
// ------------
// update the frame mask
void smoothblendClient::updateMask() {
    bool cornersFlag = ::factory->roundedCorners();
    if ( (!options()->moveResizeMaximizedWindows() && maximizeMode() == MaximizeFull ) ) 
    {
        setMask(QRegion(widget()->rect()));
        return;
    }

    int r(width());
    int b(height());
    QRegion mask;

    mask=QRegion(widget()->rect());

   // Remove top-left corner.
    if(cornersFlag) {
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

    setMask(mask);
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void smoothblendClient::resizeEvent(QResizeEvent *) {
    if (widget()->isShown()) {
        QRegion region = widget()->rect();
        region = region.subtract(titlebar_->geometry());
        widget()->erase(region);
        updateMask();
    }
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void smoothblendClient::showEvent(QShowEvent *) {
    updateMask();
    widget()->repaint();
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void smoothblendClient::maxButtonPressed() {
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

void smoothblendClient::shadeButtonPressed() {
    if (button[ButtonShade]) {
        setShade( !isSetShade());
    }
}

//////////////////////////////////////////////////////////////////////////////
// aboveButtonPressed()
// -----------------
// Above button was pressed

void smoothblendClient::aboveButtonPressed() {
    if (button[ButtonAbove]) {
        setKeepAbove( !keepAbove());
    }
}

//////////////////////////////////////////////////////////////////////////////
// belowButtonPressed()
// -----------------
// Below button was pressed

void smoothblendClient::belowButtonPressed() {
    if (button[ButtonBelow]) {
        setKeepBelow( !keepBelow());
    }
}

//////////////////////////////////////////////////////////////////////////////
// menuButtonPressed()
// -------------------
// Menu button was pressed (popup the menu)

void smoothblendClient::menuButtonPressed() {
    static QTime* t = NULL;
    static smoothblendClient* lastClient = NULL;
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

void smoothblendClient::menuButtonReleased()
{
    if(closing)
    {
        closeWindow();
    }
}

#include "smoothblend.moc"

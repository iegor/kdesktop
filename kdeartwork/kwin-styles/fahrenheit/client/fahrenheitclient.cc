//////////////////////////////////////////////////////////////////////////////
// fahrenheitclient.cc
// -------------------
// Fahrenheit window decoration for KDE
// -------------------
// Copyright (c) 2004 Peter Clark
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qtooltip.h>

#include "fahrenheitclient.h"

using namespace Fahrenheit;

// global constants

static const int borderSize_ = 5;
int titleHeight_ = 24;

//////////////////////////////////////////////////////////////////////////////
// FahrenheitFactory Class                                                  //
//////////////////////////////////////////////////////////////////////////////

bool FahrenheitFactory::initialized_              = false;
Qt::AlignmentFlags FahrenheitFactory::titlealign_ = Qt::AlignLeft;

extern "C" KDecorationFactory* create_factory()
{
    return new Fahrenheit::FahrenheitFactory();
}

//////////////////////////////////////////////////////////////////////////////
// FahrenheitFactory()
// ----------------
// Constructor

FahrenheitFactory::FahrenheitFactory()
{
    readConfig();
    initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// ~FahrenheitFactory()
// -----------------
// Destructor

FahrenheitFactory::~FahrenheitFactory() { initialized_ = false; }

//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// -----------------
// Create the decoration

KDecoration* FahrenheitFactory::createDecoration(KDecorationBridge* b)
{
    return new FahrenheitClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a repaint is necessary

bool FahrenheitFactory::reset(unsigned long changed)
{
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

bool FahrenheitFactory::readConfig()
{
    // create a config object
    KConfig config("kwinfahrenheitrc");
    config.setGroup("General");

    // grab settings
    Qt::AlignmentFlags oldalign = titlealign_;
    QString value = config.readEntry("TitleAlignment", "AlignLeft");
    if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight") titlealign_ = Qt::AlignRight;
    
    if (oldalign == titlealign_)
        return false;
    else
        return true;
}

//////////////////////////////////////////////////////////////////////////////
// FahrenheitButton Class                                                   //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// FahrenheitButton()
// ---------------
// Constructor

FahrenheitButton::FahrenheitButton(FahrenheitClient *parent, const char *name,
                             const QString& tip, ButtonType type,
                             QString pixmap)
    : QButton(parent->widget(), name), client_(parent), type_(type),
      deco_(0), lastmouse_(0)
{
    setBackgroundMode(NoBackground);
    setFixedWidth(16);
    setCursor(arrowCursor);
    if (pixmap) setPixmap(pixmap);
    QToolTip::add(this, tip);
}

FahrenheitButton::~FahrenheitButton()
{
    if (deco_) delete deco_;
}

//////////////////////////////////////////////////////////////////////////////
// setPixmap()
// -----------
// Create pixmap button decorations

void FahrenheitButton::setPixmap(QString pixmap)
{
    if (!pixmap) return; // probably the menu button

    QString buttonLocation_ = KGlobal::dirs()->findResource ("data",
                        QString("kwin/fahrenheit/") + pixmap + ".png");

    if (deco_) delete deco_;
    deco_ = new QPixmap(buttonLocation_);
    repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// sizeHint()
// ----------
// Return size hint

QSize FahrenheitButton::sizeHint() const
{
    return QSize(11, 9);
}

//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void FahrenheitButton::enterEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QButton::enterEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void FahrenheitButton::leaveEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QButton::leaveEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// mousePressEvent()
// -----------------
// Button has been pressed

void FahrenheitButton::mousePressEvent(QMouseEvent* e)
{
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

void FahrenheitButton::mouseReleaseEvent(QMouseEvent* e)
{
    lastmouse_ = e->button();

    // translate and pass on mouse event
    int button = LeftButton;
    if ((type_ != ButtonMax) && (e->button() != LeftButton)) {
        button = NoButton; // middle & right buttons inappropriate
    }
    QMouseEvent me(e->type(), e->pos(), e->globalPos(), button, e->state());
    QButton::mouseReleaseEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// drawButton()
// ------------
// Draw the button

void FahrenheitButton::drawButton(QPainter *painter)
{
    if (!FahrenheitFactory::initialized()) return;

    QColorGroup titleColor;
    QColorGroup borderColor;
    int dx, dy;

    if (type_ == ButtonMenu) {
        // we paint the mini icon (which is 16 pixels high)
        titleColor = KDecoration::options()->colorGroup(
                     KDecoration::ColorTitleBar, client_->isActive());

        painter->fillRect(rect(), titleColor.background());
        painter->setPen(titleColor.dark());
        painter->drawLine(0, 0, width(), 0);
        painter->setPen(titleColor.light());
        painter->drawLine(0, 1, width(), 1);
        painter->drawPixmap(0, 3, client_->icon().pixmap(QIconSet::Small,
                                                         QIconSet::Normal));
    } else if (deco_) {
        // otherwise we paint the deco
        borderColor = KDecoration::options()->colorGroup(
                      KDecoration::ColorFrame, client_->isActive());
        dx = 2;
        dy = 0;
        if (isDown()) { dx++; dy++; }
        painter->fillRect(rect(), borderColor.background());

        if (!client_->isShade()) {
            painter->setPen(borderColor.mid());
            painter->drawLine(0, 9, width(), 9);
            painter->setPen(borderColor.dark());
            painter->drawLine(0, 10, width(), 10);
        }
        painter->drawPixmap(dx, dy, *deco_);
    }
}

//////////////////////////////////////////////////////////////////////////////
// FahrenheitClient Class                                                   //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// FahrenheitClient()
// ---------------
// Constructor

FahrenheitClient::FahrenheitClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f), mainLayout_ (0), titleSpacer_ (0)
{ ; }

FahrenheitClient::~FahrenheitClient()
{
    for (int n=0; n<ButtonTypeCount; n++) {
        if (button[n]) delete button[n];
    }
}

//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Actual initializer for class

void FahrenheitClient::init()
{
    createMainWidget(WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);

    // for flicker-free redraws
    widget()->setBackgroundMode(NoBackground);

    // setup layout
    delete mainLayout_;

    mainLayout_ = new QVBoxLayout(widget(), 0, 0);
    topLayout_ = new QBoxLayout(mainLayout_, QBoxLayout::LeftToRight, 0, 0);

    titleSpacer_ = new QSpacerItem ( 0, 24, 
                                     QSizePolicy::Preferred,
                                     QSizePolicy::Fixed);
    barSpacer_ = new QSpacerItem ( 14, 24, 
                                   QSizePolicy::MinimumExpanding,
                                   QSizePolicy::Fixed);

    menuLayout_ = new QBoxLayout(topLayout_, QBoxLayout::LeftToRight, 0, 0);
    menuLayout_->addSpacing(16);

    recalcTitlebar();

    QBoxLayout * titleLayout_ = new QBoxLayout(topLayout_, 
                                    QBoxLayout::LeftToRight, 0, 0);
    buttonLayout_ = new QBoxLayout(topLayout_, 
                                   QBoxLayout::LeftToRight, 0, 0);
    QBoxLayout * barLayout_ = new QBoxLayout(topLayout_, 
                                  QBoxLayout::LeftToRight, 0, 0);
    titleLayout_->addItem(titleSpacer_);

    for (int n=0; n<ButtonTypeCount; n++) {
        button[n] = 0;
    }

    addButtons(buttonLayout_, options()->titleButtonsLeft());
    addButtons(buttonLayout_, options()->titleButtonsRight());
    barLayout_->addItem(barSpacer_);

    QHBoxLayout * midLayout = new QHBoxLayout(mainLayout_, 0, 0);

    midLayout->addSpacing(borderSize_ + 9);
    if( isPreview()) {
        midLayout->addWidget(
            new QLabel( i18n( "<center><b>Fahrenheit</b></center>" ),
            widget()));
    } else {
        midLayout->addItem( new QSpacerItem( 0, 0 )); 
    }

    midLayout->addSpacing(borderSize_ + (isResizable() ? 2 : 0));
    mainLayout_->addSpacing(borderSize_ + (isResizable() ? 2 : 0));

    // Make sure that topLayout doesn't stretch - midLayout should take
    // all spare space.
    mainLayout_->setStretchFactor(topLayout_, 0);
    mainLayout_->setStretchFactor(midLayout, 1);
}

//////////////////////////////////////////////////////////////////////////////
// recalcTitlebar()
// ------------
// Calculate the width of the caption for the titlebar

void FahrenheitClient::recalcTitlebar()
{
    QFontMetrics fm(options()->font(isActive()));
    titleHeight_ = fm.height();
    QString cap = caption();
    if (cap.length() < 5) // make sure the titlebar has sufficiently wide
        cap = "XXXXX";    // area for dragging the window
    int textLen_ = fm.width( cap );
    titleSpacer_->changeSize(4 + textLen_ + 16, 
                             24, QSizePolicy::Preferred, QSizePolicy::Fixed);

}


//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title layout

void FahrenheitClient::addButtons(QBoxLayout *layout, const QString& s)
{
    QString pixmap;
    QString tip;

    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
              case 'M': // Menu button
                  if (!button[ButtonMenu]) {
                      button[ButtonMenu] =
                          new FahrenheitButton(this, "menu", i18n("Menu"),
                                            ButtonMenu, 0);
                      connect(button[ButtonMenu], SIGNAL(pressed()),
                              this, SLOT(menuButtonPressed()));
                      button[ButtonMenu]->setFixedHeight(19);
                      menuLayout_->insertWidget(-1, button[ButtonMenu], 
                                                0, Qt::AlignTop);
                      menuLayout_->addSpacing(4);
                  }
                  break;

              case 'S': // Sticky button
                  if (!button[ButtonSticky]) {
		      if (isOnAllDesktops()) {
			  pixmap = "unsticky";
			  tip = i18n("Un-Sticky");
		      } else {
			  pixmap = "sticky";
			  tip = i18n("Sticky");
		      }
                      button[ButtonSticky] =
                          new FahrenheitButton(this, "sticky", tip,
                                            ButtonSticky, pixmap);
                      connect(button[ButtonSticky], SIGNAL(clicked()),
                              this, SLOT(toggleOnAllDesktops()));
                      button[ButtonSticky]->setFixedHeight(11);
                      layout->insertWidget(-1, button[ButtonSticky], 
                                           0, Qt::AlignBottom);
                  }
                  break;

              case 'H': // Help button
                  if ((!button[ButtonHelp]) && providesContextHelp()) {
                      button[ButtonHelp] =
                          new FahrenheitButton(this, "help", i18n("Help"),
                                            ButtonHelp, "help");
                      connect(button[ButtonHelp], SIGNAL(clicked()),
                              this, SLOT(showContextHelp()));
                      button[ButtonHelp]->setFixedHeight(11);
                      layout->insertWidget(-1, button[ButtonHelp], 
                                           0, Qt::AlignBottom);
                  }
                  break;

              case 'I': // Minimize button
                  if ((!button[ButtonMin]) && isMinimizable()) 	{
                      button[ButtonMin] =
                          new FahrenheitButton(this, "iconify", 
                                               i18n("Minimize"),
                                               ButtonMin, "minimize");
                      connect(button[ButtonMin], SIGNAL(clicked()),
                              this, SLOT(minimize()));
                      button[ButtonMin]->setFixedHeight(11);
                      layout->insertWidget(-1, button[ButtonMin], 
                                           0, Qt::AlignBottom);
                  }
                  break;

              case 'A': // Maximize button
                  if ((!button[ButtonMax]) && isMaximizable()) {
		      if (maximizeMode() == MaximizeFull) {
			  pixmap = "minmax";
			  tip = i18n("Restore");
		      } else {
			  pixmap = "maximize";
			  tip = i18n("Maximize");
		      }
                      button[ButtonMax]  =
                          new FahrenheitButton(this, "maximize", tip,
                                            ButtonMax, pixmap);
                      connect(button[ButtonMax], SIGNAL(clicked()),
                              this, SLOT(maxButtonPressed()));
                      button[ButtonMax]->setFixedHeight(11);
                      layout->insertWidget(-1, button[ButtonMax], 
                                           0, Qt::AlignBottom);
                  }
                  break;

              case 'X': // Close button
                  if ((!button[ButtonClose]) && isCloseable()) {
                      button[ButtonClose] =
                          new FahrenheitButton(this, "close", i18n("Close"),
                                            ButtonClose, "close");
                      connect(button[ButtonClose], SIGNAL(clicked()),
                              this, SLOT(closeWindow()));
                      button[ButtonClose]->setFixedHeight(11);
                      layout->insertWidget(-1, button[ButtonClose], 
                                           0, Qt::AlignBottom);
                  }
                  break;

              case '_': // Spacer item
                  layout->addSpacing(4);
            }
	}
    }
}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void FahrenheitClient::activeChange()
{
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n]) button[n]->reset();
    widget()->repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// captionChange()
// ---------------
// The title has changed

void FahrenheitClient::captionChange()
{
    recalcTitlebar();
    mainLayout_->activate();
    doShape();
    widget()->repaint(topLayout_->geometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void FahrenheitClient::desktopChange()
{
    bool d = isOnAllDesktops();
    if (button[ButtonSticky]) {
        //button[ButtonSticky]->setBitmap(d ? stickydown_bits : sticky_bits);
        button[ButtonSticky]->setPixmap(d ? "unsticky" : "sticky");
	QToolTip::remove(button[ButtonSticky]);
	QToolTip::add(button[ButtonSticky], d ? i18n("Un-Sticky") : i18n("Sticky"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// iconChange()
// ------------
// The title has changed

void FahrenheitClient::iconChange()
{
    if (button[ButtonMenu]) {
        button[ButtonMenu]->setPixmap(0);
        button[ButtonMenu]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void FahrenheitClient::maximizeChange()
{
    bool m = (maximizeMode() == MaximizeFull);
    if (button[ButtonMax]) {
        //button[ButtonMax]->setBitmap(m ? minmax_bits : max_bits);
        button[ButtonMax]->setPixmap(m ? "minmax" : "maximize");
	QToolTip::remove(button[ButtonMax]);
	QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeChange()
// -------------
// Called when window shading changes

void FahrenheitClient::shadeChange()
{ 
    // Workhere
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n]) button[n]->reset();
}

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void FahrenheitClient::borders(int &l, int &r, int &t, int &b) const
{
    int handle = (isResizable() ? 2 : 0);
    l = 9 + borderSize_;
    r = borderSize_ + handle;
    t = 24;
    b = borderSize_ + handle;
}

//////////////////////////////////////////////////////////////////////////////
// resize()
// --------
// Called to resize the window

void FahrenheitClient::resize(const QSize &size)
{
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// minimumSize()
// -------------
// Return the minimum allowable size for this decoration

QSize FahrenheitClient::minimumSize() const
{
    return QSize(100, 50);
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return logical mouse position

KDecoration::Position FahrenheitClient::mousePosition(const QPoint &point) const
{
    //const int corner = 24;
    Position pos;
    int x = point.x();
    int y = point.y();

    if (y < titleSpacer_->geometry().height()) {
        pos = PositionCenter;
    }
    else if (y < height() - 22)
    {
        if (x < borderSize_ + 9)
            pos = PositionLeft;
    else
        if (x > width() - borderSize_ - 2)
            pos = PositionRight;
        else
            pos = PositionCenter;
    }
    else
    {
        if (x < 29)
            pos = PositionBottomLeft;
        else
            if (x > width() - 22)
                pos = PositionBottomRight;
            else
                pos = PositionBottom;
    }
    return pos;
}

//////////////////////////////////////////////////////////////////////////////
// isTool()
// -------------
//

const int SUPPORTED_WINDOW_TYPES_MASK = NET::NormalMask | NET::DesktopMask | NET::DockMask
    | NET::ToolbarMask | NET::MenuMask | NET::DialogMask | NET::OverrideMask | NET::TopMenuMask
    | NET::UtilityMask | NET::SplashMask;

bool FahrenheitClient::isTool()
{
  NET::WindowType type = windowType( SUPPORTED_WINDOW_TYPES_MASK );
  return ((type==NET::Toolbar)||(type==NET::Utility)||(type==NET::Menu));
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Event filter

bool FahrenheitClient::eventFilter(QObject *obj, QEvent *e)
{
    if (obj != widget()) return false;

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

void FahrenheitClient::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (titleSpacer_->geometry().contains(e->pos())) titlebarDblClickOperation();
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void FahrenheitClient::paintEvent(QPaintEvent*)
{
    QColorGroup titleColor = options()->colorGroup(ColorTitleBar, isActive());
    QColorGroup handleColor = options()->colorGroup(ColorHandle, isActive());
    QColorGroup borderColor = options()->colorGroup(ColorFrame, isActive());

    QRect t = titleSpacer_->geometry();
    QRect buttonRect = buttonLayout_->geometry();
    QRect barRect = barSpacer_->geometry();

    t.setTop(1);
    int handle = (isResizable() ? 2 : 0);
    int barWidth = barRect.width() - handle;
    int tr = t.right();
    int tt = t.top();
    int r = width() - handle;
    int b = height() - handle;
    int w = width();
    int h = height();
    QPainter p(widget());

    p.setPen(Qt::black);
    p.setBrush(borderColor.background());
    p.drawRect(9, 13, width() - 9 - handle, height() - 13 - handle); // Outer edge

    // Draw behind button area
    p.fillRect(buttonRect, borderColor.background());

    p.drawLine(width() - 6, 9, 12, 9); // Top edge to left of button area

    p.setPen(borderColor.dark());
    p.drawLine(width() - 2 - handle, 14, width() - 2 - handle, 
               height() - handle); // Right bevel edge
    p.drawLine(11, height() - 2 - handle, width() - handle,
               height() - 2 - handle); // Bottom bevel edge

    // Draw edge of bottom-left corner inside the area removed by the mask.
    // We draw this now, instead of later with the rest, so that it doesn't 
    // cover up part of the grip bar.
    p.setPen(Qt::black);
    p.drawPoint(10, b - 5);
    p.drawPoint(10, b - 4);
    p.drawPoint(11, b - 3);
    p.drawPoint(12, b - 2);
    p.drawPoint(13, b - 2);
    p.setPen(borderColor.mid());
    p.drawPoint(11, b - 5);
    p.drawPoint(11, b - 4);
    p.drawPoint(12, b - 3);
    p.drawPoint(13, b - 3);

    p.setPen(borderColor.light());
    p.drawLine(t.width() + buttonRect.width(), 14, 
               width() - handle - 6, 14); // Top bevel edge of bar
    //p.drawLine(10, 14, 10, height() - 6 - handle); // Left bevel edge
    p.drawLine(12, 10, width() - handle - 6, 10); // Bevel edge to left of button area

    // Draw to the right of the button area
    p.setPen(Qt::black);
    p.drawLine(r - barWidth, 10, r - barWidth + 5, 12);
    p.setPen(borderColor.mid());
    p.drawLine(r - barWidth, 11, r - barWidth + 5, 13);
    p.setPen(borderColor.background());
    p.drawLine(r - barWidth, 12, r - barWidth + 5, 14);
    p.drawLine(r - barWidth, 13, r - barWidth + 3, 14);
    p.drawLine(r - barWidth, 14, r - barWidth + 1, 14);

    //p.setClipRegion(pe->region());

    // Fill in area behind rounded portion of title bar
    p.fillRect(tr - 11, 11, 12, 9, borderColor.background());

    // Draw inner edge of border
    if (!isShade())
    { 
        p.setPen(borderColor.dark());
        p.drawLine(9 + borderSize_, 23, w - borderSize_ - handle - 1, 23);
        p.drawLine(8 + borderSize_, 24, 8 + borderSize_, 
                   h - borderSize_ - handle - 1);

        p.setPen(borderColor.mid());
        p.drawLine(9 + borderSize_, 22, w - borderSize_ - handle - 2, 22);
        p.drawLine(7 + borderSize_, 24, 7 + borderSize_, 
                   h - borderSize_ - handle - 2);
        p.drawPoint(8 + borderSize_, 23);
        p.drawLine(w - borderSize_ - handle - 1, 22, w - borderSize_ - handle + 1, 24);
        p.drawLine(7 + borderSize_, h - borderSize_ - handle - 1, 9 + borderSize_,
                   h - borderSize_ - handle + 1);

        p.setPen(borderColor.light());
        p.drawLine(w - borderSize_ - handle, 24, w - borderSize_ - handle,
                   h - borderSize_ - handle - 1);
        p.drawLine(9 + borderSize_, h - borderSize_ - handle, 
                   w - borderSize_ - handle - 1,
                   h - borderSize_ - handle);
        p.setPen(borderColor.midlight());
        p.drawLine(w - borderSize_ - handle + 1, 25, w - borderSize_ - handle + 1,
                   h - borderSize_ - handle - 2);
        p.drawLine(10 + borderSize_, h - borderSize_ - handle + 1,
                   w - borderSize_ - handle - 2, h - borderSize_ - handle + 1);
        p.drawLine(w - borderSize_ - handle - 1, h - borderSize_ - handle + 1,
                   w - borderSize_ - handle + 1, h - borderSize_ - handle - 1);
    }

    // Draw title bar
    //p.fillRect(t.left() + 1, tt, tr - 12, t.bottom() - 3, titleColor.background());
    p.fillRect(1, tt, tr - 12, t.bottom() - 3, titleColor.background());

    // Draw lower portion of grip
    if (isShade())
    {
        p.fillRect(0, 20, 11, 4, titleColor.background());
    } else {
        p.fillRect(0, 20, 11, 14, titleColor.background());
    }

    // Draw rounded portion of title bar
    p.fillRect(tr - 11, tt + 1, 3, 17, titleColor.background());
    p.setPen(titleColor.background());
    p.drawLine(tr - 8, tt + 2, tr - 8, tt + 16);
    p.drawLine(tr - 7, tt + 3, tr - 7, tt + 15);
    p.drawLine(tr - 6, tt + 4, tr - 6, tt + 14);
    p.drawLine(tr - 5, tt + 7, tr - 5, tt + 11);

    // Draw top edge and left curve of title bar
    //p.setPen(Qt::black);
    p.setPen(titleColor.dark());
    p.drawPoint(3, 1);
    p.drawPoint(4, 1);
    p.drawPoint(2, 2);
    p.drawPoint(1, 3);
    p.drawPoint(1, 4);
    p.drawLine(5, 0, tr - 12, 0);

    p.setPen(titleColor.light());
    p.drawPoint(3, 2);
    p.drawPoint(4, 2);
    p.drawPoint(2, 3);
    p.drawPoint(2, 4);
    p.drawLine(5, 1, tr - 12, 1);

    // Draw rounded edge of title bar
    p.setPen(titleColor.dark());
    p.drawLine(tr - 11, tt, tr - 9, tt);
    p.drawLine(tr - 8, tt + 1, tr - 5, tt + 4);
    p.drawLine(tr - 5, tt + 5, tr - 5, tt + 6);
    p.drawLine(tr - 4, tt + 7, tr - 4, tt + 11);
    p.drawLine(tr - 5, tt + 12, tr - 5, tt + 13);
    p.drawLine(tr - 5, tt + 14, tr - 8, tt + 17);
    p.drawLine(tr - 9, tt + 18, tr - 11, tt + 18);

    p.setPen(titleColor.midlight());
    p.drawLine(tr - 11, tt+ 1, tr - 9, tt + 1);
    p.drawLine(tr - 8, tt + 2, tr - 7, tt + 3);
    p.setPen(titleColor.mid());
    p.drawLine(tr - 6, tt + 4, tr - 6, tt + 6);
    p.drawLine(tr - 5, tt + 7, tr - 5, tt + 11);
    p.drawLine(tr - 6, tt + 12, tr - 6, tt + 13);
    p.drawLine(tr - 6, tt + 14, tr - 9, tt + 17);
    p.drawLine(tr - 10, tt + 17, tr - 11, tt + 17);

    // Draw bottom of title bar
    p.setPen(titleColor.dark());
    p.drawLine(tr - 12, tt + 19, 12, tt + 19);
    p.setPen(titleColor.mid());
    p.drawLine(tr - 12, tt + 18, 12, tt + 18);

    // Draw grip and grip edges
    p.setPen(titleColor.dark());
    p.drawLine(0, tt + 4, 0, tt + 27);
    p.setPen(titleColor.light());
    p.drawLine(1, tt + 4, 1, tt + 27);

    p.setPen(titleColor.dark());
    if (isShade())
    {
//WORKHERE
        p.drawPoint(1, 20);
        p.drawPoint(1, 21);
        p.drawPoint(2, 22);
        p.drawPoint(3, 23);
        p.drawPoint(4, 23);
        p.drawLine(5, 24, 10, 24);
        p.drawLine(11, 23, 11, 21);
        p.setPen(titleColor.midlight());
        p.drawLine(2, 20, 2, 21);
        p.setPen(titleColor.mid());
        p.drawLine(3, 22, 4, 22);
        p.drawLine(5, 23, 9, 23);
        p.drawLine(10, 23, 10, 21);
        p.drawPoint(11, 20);
        p.setPen(borderColor.mid());
        p.drawPoint(11, 24);

        //p.setPen(titleColor.light());
        p.setPen(handleColor.light());
        p.drawLine(8, 3, 3, 8);
        p.drawLine(12, 3, 3, 12);
        p.drawLine(12, 7, 3, 16);
        p.drawLine(12, 11, 4, 19);
        p.drawLine(12, 15, 6, 21);

        //p.setPen(titleColor.midlight());
        p.setPen(handleColor.midlight());
        p.drawLine(6, 3, 3, 6);
        p.drawLine(7, 3, 3, 7);
        p.drawLine(10, 3, 3, 10);
        p.drawLine(11, 3, 3, 11);
        p.drawLine(12, 5, 3, 14);
        p.drawLine(12, 6, 3, 15);
        p.drawLine(12, 9, 4, 17);
        p.drawLine(12, 10, 4, 18);
        p.drawLine(12, 13, 6, 19);
        p.drawLine(12, 14, 6, 20);
        p.drawLine(12, 17, 8, 21);
    } else {
        p.drawPoint(1, 29);
        p.drawPoint(1, 30);
        p.drawPoint(2, 31);
        p.drawPoint(3, 32);
        p.drawPoint(4, 32);
        p.drawPoint(5, 33);
        p.drawPoint(6, 33);
        p.drawLine(7, 34, 9, 34);
        p.drawLine(10, 33, 10, 22);
        p.drawPoint(11, 21);
        p.setPen(titleColor.midlight());
        p.drawLine(2, 29, 2, 30);
        p.setPen(titleColor.mid());
        p.drawLine(3, 31, 4, 31);
        p.drawLine(5, 32, 6, 32);
        p.drawLine(7, 33, 9, 33);
        p.drawLine(9, 32, 9, 23);
        p.drawLine(9, 22, 11, 20);

        //p.setPen(titleColor.light());
        p.setPen(handleColor.light());
        p.drawLine(8, 3, 3, 8);
        p.drawLine(12, 3, 3, 12);
        p.drawLine(12, 7, 3, 16);
        p.drawLine(12, 11, 3, 20);
        p.drawLine(12, 15, 3, 24);
        p.drawLine(8, 23, 3, 28);
        p.drawLine(8, 27, 5, 30);

        //p.setPen(titleColor.midlight());
        p.setPen(handleColor.midlight());
        p.drawLine(6, 3, 3, 6);
        p.drawLine(7, 3, 3, 7);
        p.drawLine(10, 3, 3, 10);
        p.drawLine(11, 3, 3, 11);
        p.drawLine(12, 5, 3, 14);
        p.drawLine(12, 6, 3, 15);
        p.drawLine(12, 9, 3, 18);
        p.drawLine(12, 10, 3, 19);
        p.drawLine(12, 13, 3, 22);
        p.drawLine(12, 14, 3, 23);
        p.drawLine(8, 21, 3, 26);
        p.drawLine(8, 22, 3, 27);
        p.drawLine(8, 25, 5, 28);
        p.drawLine(8, 26, 5, 29);
    }

    p.setPen(Qt::black);
    //p.setPen(borderColor.dark());

    // Draw edge of top-right corner inside the area removed by the mask.

    p.drawPoint(r - 5, 14);
    p.drawPoint(r - 4, 14);
    p.drawPoint(r - 3, 15);
    p.drawPoint(r - 2, 16);
    p.drawPoint(r - 2, 17);

    p.setPen(borderColor.mid());
    p.drawPoint(r - 5, 15);
    p.drawPoint(r - 4, 15);
    p.drawPoint(r - 3, 16);
    p.drawPoint(r - 3, 17);

    // Draw edge of bottom-right corner inside the area removed by the mask.

    p.setPen(Qt::black);
    p.drawPoint(r - 2, b - 5);
    p.drawPoint(r - 2, b - 4);
    p.drawPoint(r - 3, b - 3);
    p.drawPoint(r - 4, b - 2);
    p.drawPoint(r - 5, b - 2);
    p.setPen(borderColor.dark());
    p.drawPoint(r - 3, b - 5);
    p.drawPoint(r - 3, b - 4);
    p.drawPoint(r - 4, b - 3);
    p.drawPoint(r - 5, b - 3);

    if (isResizable()) {
        if (!isShade()) { // Draw handle bar
            p.setPen(Qt::black);
            p.fillRect(w - 5, h - 20, 3, 17, borderColor.background());
            p.fillRect(w - 20, h - 5, 17, 3, borderColor.background());

            // Border edge, moving from top right to left bottom
            p.drawLine(w - 2, h - 20, w - 1, h - 17);
            p.drawLine(w - 1, h - 16, w - 1, h - 8);
            p.drawLine(w - 1, h - 7, w - 2, h - 4);
            p.drawPoint(w - 3, h - 3);
            p.drawLine(w - 4, h - 2, w - 7, h - 1);
            p.drawLine(w - 8, h - 1, w - 16, h - 1);
            p.drawLine(w - 17, h - 1, w - 20, h - 2);

            // Create handle shadow
            p.setPen(borderColor.dark());

            p.drawLine(w - 3, h - 20, w - 2, h - 17);
            p.drawLine(w - 2, h - 16, w - 2, h - 8);
            p.drawLine(w - 2, h - 7, w - 3, h - 4);
            p.drawLine(w - 4, h - 3, w - 7, h - 2);
            p.drawLine(w - 8, h - 2, w - 16, h - 2);
            p.drawLine(w - 17, h - 2, w - 20, h - 3);
        }
    }

    p.setFont(options()->font(isActive(), isTool()));
    p.setPen(options()->color(ColorFont, isActive()));
/*
    p.drawText(20, 0, t.width() - 36, 20, Qt::AlignVCenter, caption());
*/
    p.drawText(menuLayout_->geometry().width(), 0, t.width() - 16, 20, 
               Qt::AlignVCenter, caption());

}

//////////////////////////////////////////////////////////////////////////////
// doShape()
// ------------
// Shape the window

void FahrenheitClient::doShape()
{
    QRegion mask(0, 0, width(), height());
    QRect t(titleSpacer_->geometry());
    int handle = (isResizable() ? 2 : 0);
    int tr = t.right();
    QRect barRect(barSpacer_->geometry());
    QRect buttonRect(buttonLayout_->geometry());
   
    int barWidth = barRect.width() - handle;
    int buttonWidth = buttonRect.width();
    int w = width();
    int h = height();
    int r = width() - handle;
    int b = height() - handle;

    // Remove top-left corner.
 
    mask -= QRegion(0, 0, 5, 1);
    mask -= QRegion(0, 1, 3, 1);
    mask -= QRegion(0, 2, 2, 1);
    mask -= QRegion(0, 3, 1, 2);

    // Remove area above rounded portion of title bar
    mask -= QRegion(tr - 11, 0, 12, 1);
    mask -= QRegion(tr - 8, 1, 9, 1);
    mask -= QRegion(tr - 7, 2, 8, 1);
    mask -= QRegion(tr - 6, 3, 7, 1);
    mask -= QRegion(tr - 5, 4, 6, 1);
    mask -= QRegion(tr - 4, 5, 5, 3);
    mask -= QRegion(tr - 3, 8, 4, 1);

    // Remove area above button area
    mask -= QRegion(r - buttonWidth - barWidth, 0, buttonWidth + 6, 9);

    // Remove top area above blank bar and top-right corner.
    mask -= QRegion(r - barWidth + 6, 0, barWidth, 13);

    mask -= QRegion(r - barWidth, 9, 6, 1);
    mask -= QRegion(r - barWidth + 2, 10, 4, 1);
    mask -= QRegion(r - barWidth + 4, 11, 2, 1);
    mask -= QRegion(r - 5, 13, 5, 1);
    mask -= QRegion(r - 3, 14, 3, 1);
    mask -= QRegion(r - 2, 15, 2, 1);
    mask -= QRegion(r - 1, 16, 1, 2);

    // Remove left side below grip bar
    if (isShade())
    {
        mask -= QRegion(0, 20, 1, 5);
        mask -= QRegion(1, 22, 1, 3);
        mask -= QRegion(2, 23, 1, 2);
        mask -= QRegion(3, 24, 2, 1);
    } else {
        mask -= QRegion(0, 29, 1, 2);
        mask -= QRegion(0, 31, 2, 1);
        mask -= QRegion(0, 32, 3, 1);
        mask -= QRegion(0, 33, 5, 1);
        mask -= QRegion(0, 34, 7, 1);
    }
    mask -= QRegion(0, 35, 9, b - 35);

    // Remove bottom-left corner
    //mask -= QRegion(0, b - 5, 10, 3);
    mask -= QRegion(0, b - 4, 10, 3);
    mask -= QRegion(0, b - 3, 11, 1);
    mask -= QRegion(0, b - 2, 12, 1);
    mask -= QRegion(0, b - 1, 14, 1);

    // Remove bottom-right corner.

    mask -= QRegion(w - 5, h - 1, 5, 1);
    mask -= QRegion(w - 3, h - 2, 3, 1);
    mask -= QRegion(w - 2, h - 3, 2, 1);
    mask -= QRegion(w - 1, h - 5, 1, 2);

    if (isResizable()) {
        if (isShade()) { // We don't have a handle bar to worry about
            mask -= QRect(w - 2, 0, w - 1, h); // right side
            mask -= QRect(0, h - 2, w, h - 1); // bottom
            mask -= QRegion(r - 5, b - 1, 5, 1);
            mask -= QRegion(r - 3, b - 2, 3, 1);
            mask -= QRegion(r - 2, b - 3, 2, 1);
            mask -= QRegion(r - 1, b - 5, 1, 2);
        } else { // Leave area for handle bar
            mask -= QRect(w - 2, 0, w - 1, h - 20); // right side
            mask -= QRect(0, h - 2, w - 20, h - 1); // bottom
            mask -= QRegion(w - 1, h - 20, 1, 2);
            mask -= QRegion(w - 20, h - 1, 2, 1);
        }
    }
    setMask(mask);
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void FahrenheitClient::resizeEvent(QResizeEvent *)
{
    if (widget()->isShown()) {
        QRegion region = widget()->rect();
        region = region.subtract(titleSpacer_->geometry());
        doShape();
	widget()->erase(region);
    }
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void FahrenheitClient::showEvent(QShowEvent *)
{
    doShape();
    widget()->repaint();
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void FahrenheitClient::maxButtonPressed()
{
    if (button[ButtonMax]) {
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
    }
}

//////////////////////////////////////////////////////////////////////////////
// menuButtonPressed()
// -------------------
// Menu button was pressed (popup the menu)

void FahrenheitClient::menuButtonPressed()
{
    if (button[ButtonMenu]) {
        QPoint p(button[ButtonMenu]->rect().bottomLeft().x(),
                 button[ButtonMenu]->rect().bottomLeft().y());
        KDecorationFactory* f = factory();
        showWindowMenu(button[ButtonMenu]->mapToGlobal(p));
        if (!f->exists(this)) return; // decoration was destroyed
        button[ButtonMenu]->setDown(false);
    }
}

#include "fahrenheitclient.moc"

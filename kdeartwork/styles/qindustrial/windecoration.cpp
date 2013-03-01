//////////////////////////////////////////////////////////////////////////////
// Industrialclient.cc
// -------------------
// Industrial window decoration for KDE
// -------------------
// Copyright (c) 2003, 2004 David Johnson
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>

#include <qbitmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qtooltip.h>

#include "windecoration.h"

using namespace Industrial;

// global constants

static const int BUTTONSIZE      = 18;
static const int DECOSIZE        = 8;
static const int TITLESIZE       = 18;
static const int FRAMESIZE       = 4;

// window button decorations

static const unsigned char close_bits[] = {
    0x00, 0x66, 0x7e, 0x3c, 0x3c, 0x7e, 0x66, 0x00};

static const unsigned char help_bits[] = {
    0x7e, 0x7e, 0x60, 0x78, 0x78, 0x00, 0x18, 0x18};

static const unsigned char max_bits[] = {
    0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00};

static const unsigned char min_bits[] = {
    0x00, 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00};

static const unsigned char minmax_bits[] = {
    0x00, 0x02, 0x06, 0x0e, 0x1e, 0x3e, 0x7e, 0x00};

static const unsigned char stickydown_bits[] = {
    0x00, 0x18, 0x18, 0x7e, 0x7e, 0x18, 0x18, 0x00};

static const unsigned char sticky_bits[] = {
    0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00};

//////////////////////////////////////////////////////////////////////////////
// IndustrialFactory Class                                                     //
//////////////////////////////////////////////////////////////////////////////

bool IndustrialFactory::initialized_              = false;
Qt::AlignmentFlags IndustrialFactory::titlealign_ = Qt::AlignHCenter;

extern "C" KDecorationFactory* create_factory()
{
    return new Industrial::IndustrialFactory();
}

//////////////////////////////////////////////////////////////////////////////
// IndustrialFactory()
// ----------------
// Constructor

IndustrialFactory::IndustrialFactory()
{
    readConfig();
    initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// ~IndustrialFactory()
// -----------------
// Destructor

IndustrialFactory::~IndustrialFactory() { initialized_ = false; }

//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// -----------------
// Create the decoration

KDecoration* IndustrialFactory::createDecoration(KDecorationBridge* b)
{
    return new IndustrialClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler. Returns true if decorations need to be remade, false if
// only a repaint is necessary

bool IndustrialFactory::reset(unsigned long changed)
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

bool IndustrialFactory::readConfig()
{
    // create a config object
    KConfig config("kwinIndustrialrc");
    config.setGroup("General");

    // grab settings
    Qt::AlignmentFlags oldalign = titlealign_;
    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight") titlealign_ = Qt::AlignRight;

    if (oldalign == titlealign_)
        return false;
    else
        return true;
}

//////////////////////////////////////////////////////////////////////////////
// IndustrialButton Class                                                      //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// IndustrialButton()
// ---------------
// Constructor

IndustrialButton::IndustrialButton(IndustrialClient *parent, const char *name,
                             const QString& tip, ButtonType type,
                             const unsigned char *bitmap)
    : QButton(parent->widget(), name), client_(parent), type_(type),
      deco_(0), lastmouse_(0)
{
    setBackgroundMode(NoBackground);
    setFixedSize(BUTTONSIZE, BUTTONSIZE);
    setCursor(arrowCursor);
    if (bitmap) setBitmap(bitmap);
    QToolTip::add(this, tip);
}

IndustrialButton::~IndustrialButton()
{
    if (deco_) delete deco_;
}

//////////////////////////////////////////////////////////////////////////////
// setBitmap()
// -----------
// Set the button decoration

void IndustrialButton::setBitmap(const unsigned char *bitmap)
{
    if (!bitmap) return; // no bitmap, probably the menu button

    if (deco_) delete deco_;
    deco_ = new QBitmap(DECOSIZE, DECOSIZE, bitmap, true);
    deco_->setMask(*deco_);
    repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// sizeHint()
// ----------
// Return size hint

QSize IndustrialButton::sizeHint() const
{
    return QSize(BUTTONSIZE, BUTTONSIZE);
}

//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void IndustrialButton::enterEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QButton::enterEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void IndustrialButton::leaveEvent(QEvent *e)
{
    // if we wanted to do mouseovers, we would keep track of it here
    QButton::leaveEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// mousePressEvent()
// -----------------
// Button has been pressed

void IndustrialButton::mousePressEvent(QMouseEvent* e)
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

void IndustrialButton::mouseReleaseEvent(QMouseEvent* e)
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

void IndustrialButton::drawButton(QPainter *painter)
{
    if (!IndustrialFactory::initialized()) return;

    QColorGroup group;
    int dx, dy;

    // paint a plain box with border
    group = KDecoration::options()->
        colorGroup(KDecoration::ColorButtonBg, client_->isActive());
    painter->fillRect(rect(), group.button());
    painter->setPen(group.dark());
    painter->drawRect(rect());

    if (type_ == ButtonMenu) {
        // we paint the mini icon (which is 16 pixels high)
        dx = (width() - 16) / 2;
        dy = (height() - 16) / 2;
        if (isDown()) { dx++; dy++; }
        painter->drawPixmap(dx, dy, client_->icon().pixmap(QIconSet::Small,
                                                           QIconSet::Normal));
    } else if (deco_) {
        // otherwise we paint the deco
        dx = (width() - DECOSIZE) / 2;
        dy = (height() - DECOSIZE) / 2;
        if (isDown()) { dx++; dy++; }
        painter->setPen(group.dark());
        painter->drawPixmap(dx, dy, *deco_);
    }
}

//////////////////////////////////////////////////////////////////////////////
// IndustrialClient Class                                                      //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// IndustrialClient()
// ---------------
// Constructor

IndustrialClient::IndustrialClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f) { ; }

IndustrialClient::~IndustrialClient()
{
    for (int n=0; n<ButtonTypeCount; n++) {
        if (button[n]) delete button[n];
    }
}

//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Actual initializer for class

void IndustrialClient::init()
{
    createMainWidget(WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);

    // for flicker-free redraws
    widget()->setBackgroundMode(NoBackground);

    // setup layout
    QGridLayout *mainlayout = new QGridLayout(widget(), 4, 3); // 4x3 grid
    QHBoxLayout *titlelayout = new QHBoxLayout();
    titlebar_ = new QSpacerItem(1, TITLESIZE, QSizePolicy::Expanding,
                                QSizePolicy::Fixed);

    mainlayout->setResizeMode(QLayout::FreeResize);
    mainlayout->addRowSpacing(0, FRAMESIZE);
    mainlayout->addRowSpacing(3, FRAMESIZE*2);
    mainlayout->addColSpacing(0, FRAMESIZE);
    mainlayout->addColSpacing(2, FRAMESIZE);

    mainlayout->addLayout(titlelayout, 1, 1);
    if (isPreview()) {
        mainlayout->addWidget(
        new QLabel(i18n("<b><center>Industrial preview</center></b>"),
        widget()), 2, 1);
    } else {
        mainlayout->addItem(new QSpacerItem(0, 0), 2, 1);
    }

    // the window should stretch
    mainlayout->setRowStretch(2, 10);
    mainlayout->setColStretch(1, 10);

    // setup titlebar buttons
    for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;
    addButtons(titlelayout, options()->titleButtonsLeft());
    titlelayout->addItem(titlebar_);
    addButtons(titlelayout, options()->titleButtonsRight());
}

//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title layout

void IndustrialClient::addButtons(QBoxLayout *layout, const QString& s)
{
    const unsigned char *bitmap;
    QString tip;

    if (s.length() > 0) {
        for (unsigned n=0; n < s.length(); n++) {
            switch (s[n]) {
              case 'M': // Menu button
                  if (!button[ButtonMenu]) {
                      button[ButtonMenu] =
                          new IndustrialButton(this, "menu", i18n("Menu"),
                                            ButtonMenu, 0);
                      connect(button[ButtonMenu], SIGNAL(pressed()),
                              this, SLOT(menuButtonPressed()));
                      layout->addWidget(button[ButtonMenu]);
                  }
                  break;

              case 'S': // Sticky button
                  if (!button[ButtonSticky]) {
              if (isOnAllDesktops()) {
              bitmap = stickydown_bits;
              tip = i18n("Un-Sticky");
              } else {
              bitmap = sticky_bits;
              tip = i18n("Sticky");
              }
                      button[ButtonSticky] =
                          new IndustrialButton(this, "sticky", tip,
                                            ButtonSticky, bitmap);
                      connect(button[ButtonSticky], SIGNAL(clicked()),
                              this, SLOT(toggleOnAllDesktops()));
                      layout->addWidget(button[ButtonSticky]);
                  }
                  break;

              case 'H': // Help button
                  if ((!button[ButtonHelp]) && providesContextHelp()) {
                      button[ButtonHelp] =
                          new IndustrialButton(this, "help", i18n("Help"),
                                            ButtonHelp, help_bits);
                      connect(button[ButtonHelp], SIGNAL(clicked()),
                              this, SLOT(showContextHelp()));
                      layout->addWidget(button[ButtonHelp]);
                  }
                  break;

              case 'I': // Minimize button
                  if ((!button[ButtonMin]) && isMinimizable())  {
                      button[ButtonMin] =
                          new IndustrialButton(this, "iconify", i18n("Minimize"),
                                            ButtonMin, min_bits);
                      connect(button[ButtonMin], SIGNAL(clicked()),
                              this, SLOT(minimize()));
                      layout->addWidget(button[ButtonMin]);
                  }
                  break;

              case 'A': // Maximize button
                  if ((!button[ButtonMax]) && isMaximizable()) {
              if (maximizeMode() == MaximizeFull) {
              bitmap = minmax_bits;
              tip = i18n("Restore");
              } else {
              bitmap = max_bits;
              tip = i18n("Maximize");
              }
                      button[ButtonMax]  =
                          new IndustrialButton(this, "maximize", tip,
                                            ButtonMax, bitmap);
                      connect(button[ButtonMax], SIGNAL(clicked()),
                              this, SLOT(maxButtonPressed()));
                      layout->addWidget(button[ButtonMax]);
                  }
                  break;

              case 'X': // Close button
                  if ((!button[ButtonClose]) && isCloseable()) {
                      button[ButtonClose] =
                          new IndustrialButton(this, "close", i18n("Close"),
                                            ButtonClose, close_bits);
                      connect(button[ButtonClose], SIGNAL(clicked()),
                              this, SLOT(closeWindow()));
                      layout->addWidget(button[ButtonClose]);
                  }
                  break;

              case '_': // Spacer item
                  layout->addSpacing(FRAMESIZE);
            }
    }
    }
}

//////////////////////////////////////////////////////////////////////////////
// activeChange()
// --------------
// window active state has changed

void IndustrialClient::activeChange()
{
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n]) button[n]->reset();
    widget()->repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// captionChange()
// ---------------
// The title has changed

void IndustrialClient::captionChange()
{
    widget()->repaint(titlebar_->geometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void IndustrialClient::desktopChange()
{
    bool d = isOnAllDesktops();
    if (button[ButtonSticky]) {
        button[ButtonSticky]->setBitmap(d ? stickydown_bits : sticky_bits);
    QToolTip::remove(button[ButtonSticky]);
    QToolTip::add(button[ButtonSticky], d ? i18n("Un-Sticky") : i18n("Sticky"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// iconChange()
// ------------
// The title has changed

void IndustrialClient::iconChange()
{
    if (button[ButtonMenu]) {
        button[ButtonMenu]->setBitmap(0);
        button[ButtonMenu]->repaint(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void IndustrialClient::maximizeChange()
{
    bool m = (maximizeMode() == MaximizeFull);
    if (button[ButtonMax]) {
        button[ButtonMax]->setBitmap(m ? minmax_bits : max_bits);
    QToolTip::remove(button[ButtonMax]);
    QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeChange()
// -------------
// Called when window shading changes

void IndustrialClient::shadeChange()
{ ; }

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void IndustrialClient::borders(int &l, int &r, int &t, int &b) const
{
    l = r = FRAMESIZE;
    t = TITLESIZE + FRAMESIZE;
    b = FRAMESIZE * 2;
}

//////////////////////////////////////////////////////////////////////////////
// resize()
// --------
// Called to resize the window

void IndustrialClient::resize(const QSize &size)
{
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// minimumSize()
// -------------
// Return the minimum allowable size for this window

QSize IndustrialClient::minimumSize() const
{
    return widget()->minimumSize();
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return logical mouse position

KDecoration::Position IndustrialClient::mousePosition(const QPoint &point) const
{
    const int corner = 24;
    Position pos;

    if (point.y() <= FRAMESIZE) {
        // inside top frame
        if (point.x() <= corner)                 pos = PositionTopLeft;
        else if (point.x() >= (width()-corner))  pos = PositionTopRight;
        else                                     pos = PositionTop;
    } else if (point.y() >= (height()-FRAMESIZE*2)) {
        // inside handle
        if (point.x() <= corner)                 pos = PositionBottomLeft;
        else if (point.x() >= (width()-corner))  pos = PositionBottomRight;
        else                                     pos = PositionBottom;
    } else if (point.x() <= FRAMESIZE) {
        // on left frame
        if (point.y() <= corner)                 pos = PositionTopLeft;
        else if (point.y() >= (height()-corner)) pos = PositionBottomLeft;
        else                                     pos = PositionLeft;
    } else if (point.x() >= width()-FRAMESIZE) {
        // on right frame
        if (point.y() <= corner)                 pos = PositionTopRight;
        else if (point.y() >= (height()-corner)) pos = PositionBottomRight;
        else                                     pos = PositionRight;
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

bool IndustrialClient::eventFilter(QObject *obj, QEvent *e)
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

void IndustrialClient::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (titlebar_->geometry().contains(e->pos())) titlebarDblClickOperation();
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// ------------
// Repaint the window

void IndustrialClient::paintEvent(QPaintEvent*)
{
    if (!IndustrialFactory::initialized()) return;

    QColorGroup group;
    QPainter painter(widget());

    // draw the titlebar
    QRect title(titlebar_->geometry());
    group = options()->colorGroup(KDecoration::ColorTitleBar, isActive());
    painter.fillRect(title, group.background());
    painter.setPen(group.dark());
    painter.drawRect(title);

    // draw title text
    painter.setFont(options()->font(isActive(), false));
    painter.setPen(options()->color(KDecoration::ColorFont, isActive()));
    painter.drawText(title.x() + FRAMESIZE, title.y(),
                     title.width() - FRAMESIZE * 2, title.height(),
                     IndustrialFactory::titleAlign() | AlignVCenter,
                     caption());

    // draw frame
    group = options()->colorGroup(KDecoration::ColorFrame, isActive());

    QRect frame(0, 0, width(), FRAMESIZE);
    painter.fillRect(frame, group.background());
    frame.setRect(0, 0, FRAMESIZE, height());
    painter.fillRect(frame, group.background());
    frame.setRect(0, height() - FRAMESIZE*2, width(), FRAMESIZE*2);
    painter.fillRect(frame, group.background());
    frame.setRect(width()-FRAMESIZE, 0, FRAMESIZE, height());
    painter.fillRect(frame, group.background());

    // outline the frame
    painter.setPen(group.dark());
    frame = widget()->rect();
    painter.drawRect(frame);
    frame.setRect(frame.x() + FRAMESIZE-1, frame.y() + FRAMESIZE-1,
                  frame.width() - FRAMESIZE*2 +2,
                  frame.height() - FRAMESIZE*3 +2);
    painter.drawRect(frame);
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void IndustrialClient::resizeEvent(QResizeEvent *)
{
    if (widget()->isShown()) {
        QRegion region = widget()->rect();
        region = region.subtract(titlebar_->geometry());
    widget()->erase(region);
    }
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void IndustrialClient::showEvent(QShowEvent *)
{
    widget()->repaint();
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void IndustrialClient::maxButtonPressed()
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

void IndustrialClient::menuButtonPressed()
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

#include "windecoration.moc"

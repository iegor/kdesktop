//////////////////////////////////////////////////////////////////////////////
// kwinqinx.h
// -------------------
// Qinx window decoration for KDE
// -------------------
// Copyright (c) 2002-2004 David Johnson
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////
// Inspired by QNiX theme by Derek Greene <del@chek.com>
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <kdeversion.h>
#include <kdrawutil.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpixmapeffect.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qtooltip.h>

#include "kwinqinx.h"
#include "bitmaps.h"

using namespace Qinx;

static const int BUTTONSIZE           = 15;
static const int BUTTONSIZESMALL      = 12;
static const int TITLESIZE            = 22;
static const int TITLESIZESMALL       = 16;
static const int MARGIN               = 6;

//////////////////////////////////////////////////////////////////////////////
// KwinQinxHandler Class                                                    //
//////////////////////////////////////////////////////////////////////////////

bool KwinQinxFactory::initialized_              = 0;
unsigned KwinQinxFactory::contrast_             = 112;
bool KwinQinxFactory::mouseover_                = true;
bool KwinQinxFactory::cornerdetail_             = true;
bool KwinQinxFactory::flipgradient_             = false;
Qt::AlignmentFlags KwinQinxFactory::titlealign_ = Qt::AlignHCenter;
KPixmap KwinQinxFactory::pix_[PixmapTypeCount][2][2];

extern "C" KDecorationFactory* create_factory()
{
    return new Qinx::KwinQinxFactory();
}

//////////////////////////////////////////////////////////////////////////////
// KwinQinxFactory()
// -----------------
// Constructor

KwinQinxFactory::KwinQinxFactory()
{
    readConfig();
    createPixmaps();
    initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////
// ~KwinQinxFactory()
// ------------------
// Destructor

KwinQinxFactory::~KwinQinxFactory()
{
    initialized_ = false;
}

//////////////////////////////////////////////////////////////////////////////
// createDecoration()
// ------------------
// Create the decoration

KDecoration* KwinQinxFactory::createDecoration(KDecorationBridge* b)
{
    return new KwinQinxClient(b, this);
}

//////////////////////////////////////////////////////////////////////////////
// reset()
// -------
// Reset the handler

bool KwinQinxFactory::reset(unsigned long changed)
{
    initialized_ = false;
    changed |= readConfig();
    if (changed & (SettingColors | SettingDecoration)) {
        createPixmaps();
    }
    initialized_ = true;

    if (changed & (SettingColors | SettingDecoration | SettingFont |
		   SettingButtons | SettingBorder)) {
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

unsigned long KwinQinxFactory::readConfig()
{
    KConfig config("kwinqinxrc");
    config.setGroup("General");

    unsigned long changed = 0;
    Qt::AlignmentFlags oldalign = titlealign_;
    bool olddetail = cornerdetail_;
    bool oldflip = flipgradient_;
    unsigned oldcontrast = contrast_;

    // grab specific settings
    QString value = config.readEntry("TitleAlignment", "AlignHCenter");
    if (value == "AlignLeft") titlealign_ = Qt::AlignLeft;
    else if (value == "AlignHCenter") titlealign_ = Qt::AlignHCenter;
    else if (value == "AlignRight") titlealign_ = Qt::AlignRight;

    mouseover_ = config.readBoolEntry("MouseOver", true);
    cornerdetail_ = config.readBoolEntry("CornerDetail", true);
    flipgradient_ = config.readBoolEntry("FlipGradient", false);

    // grab stuff from global settings that I need
    contrast_ = 100 + KGlobalSettings::contrast() * 2;

    if (oldalign != titlealign_)
        changed |= SettingFont;
    if ((oldflip != flipgradient_) || (oldcontrast != contrast_))
        changed |= SettingDecoration;
    if (olddetail != cornerdetail_)
        changed |= SettingBorder;

    return changed;
}

//////////////////////////////////////////////////////////////////////////////
// createPixmaps()
// ---------------
// Create all our pixmaps

void KwinQinxFactory::createPixmaps()
{
    QPainter painter;
    QColorGroup group;

    for (int active=0; active<=1; ++active) {
        for (int small=0; small<=1; ++small) {
            KPixmap &button = pix_[Button][active][small];
            KPixmap &hilitebutton = pix_[HiliteButton][active][small];
            KPixmap &tleft = pix_[TitleLeft][active][small];
            KPixmap &tright = pix_[TitleRight][active][small];

            // resize pixmap
            if (small) {
                button.resize(BUTTONSIZESMALL, BUTTONSIZESMALL);
                hilitebutton.resize(BUTTONSIZESMALL, BUTTONSIZESMALL);
                tleft.resize(TITLESIZESMALL, TITLESIZESMALL);
                tright.resize(TITLESIZESMALL, TITLESIZESMALL);
            } else {
                button.resize(BUTTONSIZE, BUTTONSIZE);
                hilitebutton.resize(BUTTONSIZE, BUTTONSIZE);
                tleft.resize(TITLESIZE, TITLESIZE);
                tright.resize(TITLESIZE, TITLESIZE);
            }

            // create gradients and fills
            QColorGroup group;
            if (QPixmap::defaultDepth() > 8) {
                group = KDecoration::options()->colorGroup(ColorButtonBg, active);
                KPixmapEffect::gradient(button,
                    group.button().light(contrast()),
                    group.button().dark(contrast()),
                    KPixmapEffect::VerticalGradient);
                KPixmapEffect::gradient(hilitebutton,
                    group.light().light(contrast()),
                    group.light().dark(contrast()),
                    KPixmapEffect::VerticalGradient);

                group = KDecoration::options()->colorGroup(ColorFrame, active);
                if (flipgradient_) {
                    KPixmapEffect::gradient(tleft,
                        KDecoration::options()->color(ColorTitleBlend, active),
                        KDecoration::options()->color(ColorTitleBar, active),
                        KPixmapEffect::VerticalGradient);
                } else {
                    KPixmapEffect::gradient(tleft,
                        KDecoration::options()->color(ColorTitleBar, active),
                        KDecoration::options()->color(ColorTitleBlend, active),
                        KPixmapEffect::VerticalGradient);
                }
                KPixmapEffect::gradient(tright,
                    group.button().dark(contrast()),
                    group.button().light(contrast()),
                    KPixmapEffect::VerticalGradient);
            } else {
                button.fill(group.button());
                hilitebutton.fill(group.light());
                tleft.fill(KDecoration::options()->color(ColorTitleBar, active));
                tright.fill(group.button());
            }

            int x, y, w, h, x2, y2;

            // finish off buttons
            group = KDecoration::options()->colorGroup(ColorButtonBg, active);
            x = y = 2;
            h = w = button.width() - 4;
            x2 = x + w - 1;
            y2 = y + h - 1;    
            for (int n=0; n<2; n++) {
                // paint twice, once for each pixmap
                if (n) painter.begin(&button);
                else painter.begin(&hilitebutton);

                painter.setPen(group.dark());
                painter.drawLine(x+1, y2, x2, y2);
                painter.drawLine(x2, y+1, x2, y2);
                painter.setPen(group.mid());
                painter.drawPoint(x, y2);
                painter.drawPoint(x2, y);
                painter.setPen(group.light());
                painter.drawLine(x, y, x, y2-1);
                painter.drawLine(x, y, x2-1, y);

                painter.end();
            }

            // finish off TitleLeft
            group = KDecoration::options()->colorGroup(ColorFrame, active);
            painter.begin(&tleft);
            tleft.rect().rect(&x, &y, &w, &h);
            x2 = w-1; y2 = h-1;

            if (flipgradient_) {
                painter.setPen(KDecoration::options()->color(ColorTitleBar, active)
                               .dark(contrast()));
                painter.drawLine(x, y+3, x2, y+3);
                painter.drawLine(x, y2-1, x2, y2-1);
                if (small) painter.drawLine(x, y2-4, x2, y2-4);

                painter.setPen(KDecoration::options()->color(ColorTitleBlend, active)
                               .light(contrast()));
                painter.drawLine(x, y+1, x2, y+1);
                if (small) painter.drawLine(x, y2-3, x2, y2-3);
            } else {
                painter.setPen(KDecoration::options()->color(ColorTitleBlend, active)
                               .dark(contrast()));
                painter.drawLine(x, y+3, x2, y+3);
                painter.drawLine(x, y2-1, x2, y2-1);
                if (small) painter.drawLine(x, y2-4, x2, y2-4);

                painter.setPen(KDecoration::options()->color(ColorTitleBar, active)
                               .light(contrast()));
                painter.drawLine(x, y+1, x2, y+1);
                if (small) painter.drawLine(x, y2-3, x2, y2-3);
            }

            painter.setPen(group.shadow());
            painter.drawLine(x, y, x2, y);
            painter.drawLine(x, y2, x2, y2);

            painter.end();

            // finish off TitleRight
            painter.begin(&tright);
            tright.rect().rect(&x, &y, &w, &h);
            x2 = w-1; y2 = h-1;

            painter.setPen(group.light());
            painter.drawLine(x, y+1, x2, y+1);
            painter.drawLine(x, y2-2, x2, y2-2);
            painter.setPen(group.mid());
            painter.drawLine(x, y2-1, x2, y2-1);

            painter.setPen(group.shadow());
            painter.drawLine(x, y, x2, y);
            painter.drawLine(x, y2, x2, y2);
            painter.end();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// QinxButton Class                                                         //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// QinxButton()
// ------------
// Constructor

QinxButton::QinxButton(KwinQinxClient *parent, const char *name,
                       const QString& tip, bool small, bool left,
                       ButtonType type, const ButtonDeco *deco)
    : QButton(parent->widget(), name), client_(parent), type_(type),
      deco_(0), lastmouse_(NoButton), mouseover_(false),
      small_(small), left_(left)
{
    setBackgroundMode(QWidget::NoBackground);
    setCursor(arrowCursor);
    QToolTip::add(this, tip);

    if (small_) {
        setFixedSize(BUTTONSIZESMALL, BUTTONSIZESMALL);
    } else {
        setFixedSize(BUTTONSIZE, BUTTONSIZE);
    }

    if (deco) setDeco(deco);
}

QinxButton::~QinxButton()
{
}

//////////////////////////////////////////////////////////////////////////////
// setDeco()
// -----------
// Set the button decoration

void QinxButton::setDeco(const ButtonDeco *deco)
{
    deco_ = deco;
    repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// sizeHint()
// ----------
// Return size hint

QSize QinxButton::sizeHint() const
{
    if (small_)
        return QSize(BUTTONSIZESMALL, BUTTONSIZESMALL);
    else
        return QSize(BUTTONSIZE, BUTTONSIZE);
}

//////////////////////////////////////////////////////////////////////////////
// enterEvent()
// ------------
// Mouse has entered the button

void QinxButton::enterEvent(QEvent *e)
{
    if (KwinQinxFactory::mouseOver()) {
        mouseover_ = true;
        repaint(false);
    }
    QButton::enterEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// leaveEvent()
// ------------
// Mouse has left the button

void QinxButton::leaveEvent(QEvent *e)
{
    if (KwinQinxFactory::mouseOver()) {
        mouseover_ = false;
        repaint(false);
    }
    QButton::leaveEvent(e);
}

//////////////////////////////////////////////////////////////////////////////
// mousePressEvent()
// -----------------
// Button has been pressed

void QinxButton::mousePressEvent(QMouseEvent* e)
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

void QinxButton::mouseReleaseEvent(QMouseEvent* e)
{
    lastmouse_ = e->button();

    // translate and pass on mouse event
    int button = LeftButton;
    if ((type_ != ButtonMax) && (e->button() != LeftButton)) {
        button = NoButton; // middle & right buttons inappropriate
    }
    QMouseEvent me(e->type(), e->pos(), e->globalPos(),
                   button, e->state());
    QButton::mouseReleaseEvent(&me);
}

//////////////////////////////////////////////////////////////////////////////
// drawButton()
// ------------
// Draw the button

void QinxButton::drawButton(QPainter *painter)
{
    if (!KwinQinxFactory::initialized()) return;

    int dx=0, dy=0;
    bool active = client_->isActive();
    QColorGroup group = KDecoration::options()->
        colorGroup(KDecoration::ColorButtonBg, active);

    // paint basic button
    if (isDown()) { // shift for pressed appearance
        dx++; dy++;
    }
    painter->drawPixmap(dx, dy, mouseover_ ?
                        KwinQinxFactory::pix(HiliteButton, active, small_)
                        : KwinQinxFactory::pix(Button, active, small_));

    // draw deco
    if (deco_->small) {
        kColorBitmaps(painter, group, dx, dy, 12, 12, true,
                      deco_->light, deco_->mid, 0,
                      deco_->dark, deco_->shadow, 0);
    } else {
        kColorBitmaps(painter, group, dx+2, dy+2, 11, 11, true,
                      deco_->light, deco_->mid, 0,
                      deco_->dark, deco_->shadow, 0);
    }

    // draw outside frame, which will overlap above drawing
    int x, y, x2, y2;
    x = y = 0;
    x2 = y2 = width() - 1; // square buttons

    if (isDown()) { // inner shadow if necessary
        painter->setPen(group.dark());
        painter->drawLine(x+2, y+2, x2-2, y+2);
        painter->drawLine(x+2, y+3, x+2, y2-2);
    }

    if (left_) {
        if (KwinQinxFactory::flipGradient()) {
            painter->setPen(KDecoration::options()->
			    color(KDecoration::ColorTitleBar, active)
                            .dark(KwinQinxFactory::contrast()));
        } else {
            painter->setPen(KDecoration::options()->
			    color(KDecoration::ColorTitleBlend, active)
                            .dark(KwinQinxFactory::contrast()));
        }
    } else {
        painter->setPen(group.mid());
    }
    painter->drawLine(x, y, x2, y);
    painter->drawLine(x, y+1, x, y2-1);

    if (left_) {
        if (KwinQinxFactory::flipGradient()) {
            painter->setPen(KDecoration::options()->
			    color(KDecoration::ColorTitleBlend, active)
                            .light(KwinQinxFactory::contrast()));
        } else {
            painter->setPen(KDecoration::options()->
			    color(KDecoration::ColorTitleBar, active)
                            .light(KwinQinxFactory::contrast()));
        }
    } else {
        painter->setPen(group.light());
    }
    painter->drawLine(x2, y+1, x2, y2);
    painter->drawLine(x, y2, x2-1, y2);

    painter->setPen(group.shadow());
    painter->drawRect(x+1, y+1, width()-2, height()-2);
}

//////////////////////////////////////////////////////////////////////////////
// KwinQinxClient Class                                                     //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// KwinQinxClient()
// ----------------
// Constructor

KwinQinxClient::KwinQinxClient(KDecorationBridge *b, KDecorationFactory *f)
    : KDecoration(b, f) { ; }

KwinQinxClient::~KwinQinxClient()
{
    for (int n=0; n<ButtonTypeCount; n++) {
        if (button[n]) delete button[n];
    }
}

//////////////////////////////////////////////////////////////////////////////
// init()
// ------
// Real initialization

void KwinQinxClient::init()
{
    createMainWidget(WResizeNoErase | WRepaintNoErase);
    widget()->installEventFilter(this);
    widget()->setBackgroundMode(NoBackground);

    if (isTool()) { // tool windows have tiny titlebars
  	titleheight_ = TITLESIZESMALL;
    } else {
  	titleheight_ = TITLESIZE;
    }
    
    // setup layout
    //     rows) titlebar, client, spacing
    //     cols) spacing, client, spacing
    QGridLayout *mainlayout = new QGridLayout(widget(), 0, 0, 0);
    QHBoxLayout *titlelayout = new QHBoxLayout();
    titlebar_ = new QSpacerItem(1, titleheight_, QSizePolicy::Expanding,
                                QSizePolicy::Fixed);
    closespacer_ = 0;

    mainlayout->setResizeMode(QLayout::FreeResize);
    mainlayout->addLayout(titlelayout, 0, 1);

    if (isPreview()) {
        mainlayout->addWidget(
            new QLabel(i18n("<b><center>Qinx preview</center></b>"),
		       widget()), 1, 1);
    } else {
        mainlayout->addItem(new QSpacerItem(0, 0), 1, 1);
    }

    titlelayout->setResizeMode(QLayout::FreeResize);

    // "frame"
    mainlayout->addRowSpacing(2, MARGIN);
    mainlayout->addColSpacing(0, MARGIN);
    mainlayout->addColSpacing(2, MARGIN);
    mainlayout->setRowStretch(1, 10);
    mainlayout->setColStretch(1, 10);
 
    // create buttons
    for (int n=0; n<ButtonTypeCount; n++) button[n] = 0;

    // the default QNX button positions don't match the KDE default
    QString left, right, tool;
    if (options()->customButtonPositions()) {
        left = options()->titleButtonsLeft();
        right = options()->titleButtonsRight();
        tool = left + right;
    } else {
        left = QString("Q"); // traditional QNX menu button
        right = QString("IA_X"); // min, max, spacer, close
        tool = QString("X");
    }

    // titlelayout
    if (isTool()) {
        titlelayout->addItem(titlebar_);
        titlelayout->addSpacing(MARGIN);
        addButtons(titlelayout, tool, false);
    } else {
        addButtons(titlelayout, left, true);
        titlelayout->addItem(titlebar_);
        titlelayout->addSpacing(MARGIN);
        addButtons(titlelayout, right, false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// addButtons()
// ------------
// Add buttons to title layout

void KwinQinxClient::addButtons(QHBoxLayout* layout,
                                const QString &s,
                                bool left)
{
    // we don't use help or sticky buttons
    bool small = isTool();
    const ButtonDeco *deco;
    QString tip;

    if (s.length() > 0) {
        for(unsigned n=0; n < s.length(); n++) {
            switch(s[n].latin1()) {
              case 'A': // Maximize button
                  if (!button[ButtonMax] && isMaximizable()) {
		      if (maximizeMode() == MaximizeFull) {
			  deco = small ? &smallrestoredeco : &restoredeco;
			  tip = i18n("Restore");
		      } else {
			  deco = small ? &smallmaxdeco : &maxdeco;
			  tip = i18n("Maximize");
		      }
                      button[ButtonMax] =
                          new QinxButton(this, "max", tip, small, left,
					 ButtonMax, deco);
                      connect(button[ButtonMax], SIGNAL(clicked()),
                              this, SLOT(maxButtonPressed()));
                      layout->addWidget(button[ButtonMax]);
                      if (n < s.length()-1) layout->addSpacing(1);
                  }
                  break;

              case 'H': // Help button
                  if ((!button[ButtonHelp]) && providesContextHelp()) {
                      button[ButtonHelp] =
                          new QinxButton(this, "help", i18n("Help"),
                                         small, left, ButtonHelp,
                                         small ? &smallhelpdeco : &helpdeco);
                      connect(button[ButtonHelp], SIGNAL(clicked()),
                              this, SLOT(showContextHelp()));
                      layout->addWidget(button[ButtonHelp]);
                      if (n < s.length()-1) layout->addSpacing(1);
                  }
                  break;

              case 'I': // Minimize button
                  if (!button[ButtonMin] && isMinimizable()) {
                      button[ButtonMin] =
                          new QinxButton(this, "min", i18n("Minimize"),
                                         small, left, ButtonMin,
                                         small ? &smallmindeco : &mindeco);
                      connect(button[ButtonMin], SIGNAL(clicked()),
                              this, SLOT(minimize()));
                      layout->addWidget(button[ButtonMin]);
                      if (n < s.length()-1) layout->addSpacing(1);
                  }
                  break;

              case 'M': // Normal menu button
                  if (!button[ButtonMenu]) {
                      button[ButtonMenu] =
                          new QinxButton(this, "menu", i18n("Menu"),
                                         small, left, ButtonMenu,
                                         small ? &smallmenudeco : &menudeco);
                      connect(button[ButtonMenu], SIGNAL(clicked()),
                              this, SLOT(menuButtonPressed()));
                      layout->addWidget(button[ButtonMenu]);
                      if (n < s.length()-1) layout->addSpacing(1);
                  }
                  break;

              case 'Q': // QNX style menu button (custom)
                  if (!button[ButtonMenu]) {
                      button[ButtonMenu] =
                          new QinxButton(this, "menu", i18n("Menu"),
                                         true, left, ButtonMenu,
                                         &smallmenudeco);
                      connect(button[ButtonMenu], SIGNAL(clicked()),
                              this, SLOT(menuButtonPressed()));
                      layout->addWidget(button[ButtonMenu]);
                      if (n < s.length()-1) layout->addSpacing(1);
                  }
                  break;

              case 'S': // Sticky button
                  if (!button[ButtonSticky]) {
                      if (isOnAllDesktops()) {
                          deco = small ? &smallstickydowndeco : &stickydowndeco;
                          tip = i18n("Un-Sticky");
                      } else {
                          deco = small ? &smallstickyupdeco : &stickyupdeco;
                          tip = i18n("Sticky");
                      }
                      button[ButtonSticky] =
                          new QinxButton(this, "sticky", tip,
                                         small, left, ButtonSticky, deco);
                      connect(button[ButtonSticky], SIGNAL(clicked()),
                              this, SLOT(toggleOnAllDesktops()));
                      layout->addWidget(button[ButtonSticky]);
                      if (n < s.length()-1) layout->addSpacing(1);
                  }
                  break;

              case 'X': // Close button
                  if (!button[ButtonClose] && isCloseable()) {
                      button[ButtonClose] =
                          new QinxButton(this, "close", i18n("Close"),
                                         small, left, ButtonClose,
                                         small ? &smallclosedeco : &closedeco);
                      connect(button[ButtonClose], SIGNAL(clicked()),
                              this, SLOT(closeWindow()));
                      layout->addWidget(button[ButtonClose]);
                      if (n < s.length()-1) layout->addSpacing(1);
                  }
                  break;

              case '_': // Spacer item
                  closespacer_ = new QSpacerItem(MARGIN, MARGIN,
                                                 QSizePolicy::Minimum,
                                                 QSizePolicy::Fixed);
                  layout->addItem(closespacer_);
                  // closespacer_ points to the last spacer if more than one
                  break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -------------
// Event filter

bool KwinQinxClient::eventFilter(QObject *obj, QEvent *e)
{
    if (obj != widget()) return false;

    switch (e->type()) {
      case QEvent::MouseButtonDblClick: {
          mouseDoubleClickEvent(static_cast<QMouseEvent*>(e));
          return true;
      }
      case QEvent::MouseButtonPress: {
          processMousePressEvent(static_cast<QMouseEvent*>(e));
          return true;
      }
      case QEvent::Paint: {
          paintEvent(static_cast<QPaintEvent*>(e));
          return true;
      }
      case QEvent::Resize: {
          resizeEvent(static_cast<QResizeEvent*>(e));
          return true;
      }
      case QEvent::Show: {
          showEvent(static_cast<QShowEvent*>(e));
          return true;
      }
      default: {
          return false;
      }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
// mouseCoubleClickEvent()
// -----------------------
// Doubleclick on title

void KwinQinxClient::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (titlebar_->geometry().contains(e->pos())) titlebarDblClickOperation();
}

//////////////////////////////////////////////////////////////////////////////
// paintEvent()
// -------------
// Repaint the window

void KwinQinxClient::paintEvent(QPaintEvent*)
{
    if (!KwinQinxFactory::initialized()) return;

    QColorGroup group;
    QPainter painter(widget());
    int x, y, w, h, x2, y2, offset;
    bool active = isActive();
    bool small = isTool();

    // compute rects
    QRect titlerect, leftrect, rightrect, clientrect, closerect;
    titlerect = titlebar_->geometry();
    offset = titlerect.x() + titlerect.width();
    leftrect.setRect(0, 0, offset, titlerect.height());
    rightrect.setRect(offset, 0, width()-offset, titlerect.height());
    clientrect.setRect(0, titlerect.bottom()+1,
                       width(), height()-titlerect.height());

    // get appropriate colors for left side
    QColor lightcolor, darkcolor;
    if (KwinQinxFactory::flipGradient()) {
        lightcolor = options()->color(ColorTitleBlend, active)
            .light(KwinQinxFactory::contrast());
        darkcolor = options()->color(ColorTitleBar, active)
            .dark(KwinQinxFactory::contrast());
    } else {
        lightcolor = options()->color(ColorTitleBar, active)
            .light(KwinQinxFactory::contrast());
        darkcolor = options()->color(ColorTitleBlend, active)
            .dark(KwinQinxFactory::contrast());
    }
    
    // draw left title innards
    leftrect.rect(&x, &y, &w, &h);
    x2 = leftrect.right(); y2 = leftrect.bottom();

    painter.drawTiledPixmap(x, y, w, h,
                            KwinQinxFactory::pix(TitleLeft, active, small));

    // draw left title frame
    group = options()->colorGroup(ColorFrame, active);

    painter.setPen(group.shadow());
    painter.drawLine(x, y, x, y2);
    painter.drawLine(x2, y+1, x2, y+3);
    painter.drawLine(x2, y2-1, x2, y2-3);
    painter.drawLine(x2-1, y+4, x2-1, y2-4);

    painter.setPen(group.dark());
    painter.drawLine(x2, y+4, x2, y2-4);

    painter.setPen(lightcolor);
    painter.drawLine(x+1, y+4, x+1, y2-1);

    painter.setPen(darkcolor);
    painter.drawLine(x2-1, y+1, x2-1, y+3);
    painter.drawLine(x2-1, y2-1, x2-1, y2-3);
    painter.drawLine(x2-2, y+4, x2-2, y2-4);

    // draw title text
    QFont font;
    if (small) font = options()->font(active, true);
    else font = options()->font(active, false);

    painter.setFont(font);
    painter.setPen(options()->color(ColorFont, active));
    painter.drawText(x+BUTTONSIZE+MARGIN, y,
                     w-BUTTONSIZE-MARGIN*2, h,
                     KwinQinxFactory::titleAlign() | AlignVCenter, caption());

    // draw right title innards
    rightrect.rect(&x, &y, &w, &h);
    x2 = rightrect.right(); y2 = rightrect.bottom();

    painter.drawTiledPixmap(x, y, w, h,
                            KwinQinxFactory::pix(TitleRight, active, small));

    // draw right title frame
    group = options()->colorGroup(ColorFrame, active);
    painter.setPen(group.shadow());
    painter.drawLine(x2, y+1, x2, y2);

    painter.setPen(group.dark());
    painter.drawLine(x2-1, y+1, x2-1, y2);

    painter.setPen(group.mid());
    painter.drawLine(x2-2, y+2, x2-2, y2);

    painter.setPen(group.background());
    painter.drawPoint(x2-3, y2);

    painter.setPen(group.light());
    painter.drawPoint(x2-4, y2);

    painter.setPen(group.dark());
    painter.drawLine(x, y+1, x, y+3);
    painter.drawLine(x, y2-1, x, y2-3);

    // draw close spacer doodad
    if (closespacer_) {
        closerect = closespacer_->geometry();
        // only draw doodad if on the right
        if (closerect.left() > rightrect.left()) {
            closerect.rect(&x, &y, &w, &h);
            y2 = closerect.bottom();
            group = options()->colorGroup(ColorFrame, active);
            painter.setPen(group.dark());
            painter.drawLine(x+3, y+1, x+3, y2-1);
            painter.setPen(group.light());
            painter.drawLine(x+4, y+1, x+4, y2-2);
        }
    }

    // draw client frame
    clientrect.rect(&x, &y, &w, &h);
    x2 = clientrect.right(); y2 = clientrect.bottom();
    group = options()->colorGroup(ColorFrame, active);

    painter.setPen(group.shadow());
    painter.drawLine(x, y, x, y2);
    painter.drawLine(x+1, y2, x2, y2);
    painter.drawLine(x2, y2-1, x2, y);
    painter.drawLine(x+5, y, x+5, y2-5);
    painter.drawLine(x2-5, y2-5, x2-5, y);
    painter.drawLine(x+5, y2-5, x2-5, y2-5);

    painter.setPen(group.dark());
    painter.drawLine(x+1, y2-1, x2-1, y2-1);
    painter.drawLine(x2-1, y2-1, x2-1, y);
    painter.drawLine(x+4, y, x+4, y2-4);

    painter.setPen(group.mid());
    painter.drawLine(x+3, y, x+3, y2-3);
    painter.drawLine(x+2, y2-2, x2-2, y2-2);
    painter.drawLine(x2-2, y2-2, x2-2, y);

    painter.setPen(group.background());
    painter.drawLine(x+2, y, x+2, y2-2);
    painter.drawLine(x+3, y2-3, x2-3, y2-3);
    painter.drawLine(x2-3, y2-3, x2-3, y);

    painter.setPen(group.light());
    painter.drawLine(x+1, y, x+1, y2-1);
    painter.drawLine(x+4, y2-4, x2-4, y2-4);
    painter.drawLine(x2-4, y2-4, x2-4, y);

    if (isResizable() && !isShade() &&
        KwinQinxFactory::cornerDetail() && !small) {
        // draw corner deco
        painter.setPen(group.shadow());
        painter.drawLine(x2-9, y2-5, x2-9, y2);
        painter.drawLine(x2-5, y2-9, x2, y2-9);
        painter.setPen(darkcolor.dark(125));
        painter.drawLine(x2-8, y2-1, x2-1, y2-1);
        painter.drawLine(x2-1, y2-2, x2-1, y2-8);
        painter.setPen(darkcolor);
        painter.drawLine(x2-8, y2-2, x2-2, y2-2);
        painter.drawLine(x2-2, y2-3, x2-2, y2-8);
        painter.setPen(lightcolor);
        painter.drawLine(x2-8, y2-3, x2-3, y2-3);
        painter.drawLine(x2-3, y2-4, x2-3, y2-8);
        painter.setPen(lightcolor.light(125));
        painter.drawLine(x2-8, y2-4, x2-4, y2-4);
        painter.drawLine(x2-4, y2-5, x2-4, y2-8);
    }
}

//////////////////////////////////////////////////////////////////////////////
// resizeEvent()
// -------------
// Window is being resized

void KwinQinxClient::resizeEvent(QResizeEvent*)
{
    if (widget()->isShown()) {
      QRect rect = widget()->rect();
      QRegion region = rect;
        region = region.subtract(titlebar_->geometry());
        region = region.subtract(QRegion(rect.x(), rect.y(),
                                         MARGIN, rect.height()));
        region = region.subtract(QRegion(rect.x(), rect.bottom()-MARGIN,
                                         rect.width(), MARGIN));
        region = region.subtract(QRegion(rect.right()-MARGIN, rect.y(),
                                         MARGIN, rect.height()));
        widget()->erase(region);
    }
}

//////////////////////////////////////////////////////////////////////////////
// showEvent()
// -----------
// Window is being shown

void KwinQinxClient::showEvent(QShowEvent*)
{
    widget()->repaint();
}

//////////////////////////////////////////////////////////////////////////////
// mousePosition()
// ---------------
// Return mouse position (for resizing)

KDecoration::Position KwinQinxClient::mousePosition(const QPoint &point) const
{
    const int corner = 24;
    Position pos;

    if (point.y() <= titleheight_) {
        // inside titlebar
        pos = KDecoration::mousePosition(point);
    } else if (point.y() >= (height()-MARGIN)) {
        // on bottom frame
        if (point.x() <= corner)                 pos = PositionBottomLeft;
        else if (point.x() >= (width()-corner))  pos = PositionBottomRight;
        else                                     pos = PositionBottom;
    } else if (point.x() <= MARGIN) {
        // on left frame
        if (point.y() <= corner)                 pos = PositionTopLeft;
        else if (point.y() >= (height()-corner)) pos = PositionBottomLeft;
        else                                     pos = PositionLeft;
    } else if (point.x() >= width()-MARGIN) {
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
// activeChange()
// --------------
// window active state has changed

void KwinQinxClient::activeChange()
{
    for (int n=0; n<ButtonTypeCount; n++)
        if (button[n]) button[n]->reset();
    widget()->repaint(false);
}

//////////////////////////////////////////////////////////////////////////////
// captionsChange()
// ----------------
// Title caption has changed

void KwinQinxClient::captionChange()
{
    widget()->repaint(titlebar_->geometry(), false);
}

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Sticky state has changed

//////////////////////////////////////////////////////////////////////////////
// desktopChange()
// ---------------
// Called when desktop/sticky changes

void KwinQinxClient::desktopChange()
{
    bool d = isOnAllDesktops();
    if (button[ButtonSticky]) {
        if (isTool()) {
            button[ButtonSticky]->setDeco(d ? &smallstickydowndeco
                                          : &smallstickyupdeco);
        } else {
            button[ButtonSticky]->setDeco(d ? &stickydowndeco
                                          : &stickyupdeco);
        }
        QToolTip::remove(button[ButtonSticky]);
        QToolTip::add(button[ButtonSticky],
                      d ? i18n("Un-Sticky") : i18n("Sticky"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// iconChange()
// ------------
// The title has changed

void KwinQinxClient::iconChange()
{
    // nothing, since menu button doesn't use icon
}

//////////////////////////////////////////////////////////////////////////////
// maximizeChange()
// ----------------
// Maximized state has changed

void KwinQinxClient::maximizeChange()
{
    bool m = (maximizeMode() == MaximizeFull);
    if (button[ButtonMax]) {
        if (isTool()) {
            button[ButtonMax]->setDeco(m ? &smallrestoredeco : &smallmaxdeco);
        } else {
            button[ButtonMax]->setDeco(m ? &restoredeco : &maxdeco);
        }
        button[ButtonMax]->repaint(false);
        QToolTip::remove(button[ButtonMax]);
        QToolTip::add(button[ButtonMax], m ? i18n("Restore") : i18n("Maximize"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// shadeChange()
// -------------
// Called when window shading changes

void KwinQinxClient::shadeChange()
{ ; }

//////////////////////////////////////////////////////////////////////////////
// borders()
// ----------
// Get the size of the borders

void KwinQinxClient::borders(int &l, int &r, int &t, int &b) const
{
    l = r = b = MARGIN;
    t = titleheight_;
}

//////////////////////////////////////////////////////////////////////////////
// resize()
// --------
// Called to resize the window

void KwinQinxClient::resize(const QSize &size)
{
    widget()->resize(size);
}

//////////////////////////////////////////////////////////////////////////////
// minimumSize()
// -------------
// Return the minimum allowable size for this decoration

QSize KwinQinxClient::minimumSize() const
{
    return widget()->minimumSize().expandedTo(QSize(32,16));
}

//////////////////////////////////////////////////////////////////////////////
// maxButtonPressed()
// -----------------
// Max button was pressed

void KwinQinxClient::maxButtonPressed()
{
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
// menuButtonPressed()
// -------------------
// Menu button was pressed

void KwinQinxClient::menuButtonPressed()
{
    if (button[ButtonMenu]) {
        QPoint pt(button[ButtonMenu]->rect().bottomLeft().x(),
                  button[ButtonMenu]->rect().bottomLeft().y()+4);
	KDecorationFactory* f = factory();
        showWindowMenu(button[ButtonMenu]->mapToGlobal(pt));
	if (!f->exists(this)) return; // we have been destroyed
        button[ButtonMenu]->setDown(false);
    }
}

//////////////////////////////////////////////////////////////////////////////
// isTool()
// --------
// Is this a tool window?

bool KwinQinxClient::isTool() const
{
    static const unsigned long winmask = NET::DesktopMask | NET::DialogMask |
                                         NET::DockMask | NET::MenuMask |
                                         NET::NormalMask | NET::OverrideMask |
                                         NET::SplashMask | NET::ToolbarMask |
                                         NET::TopMenuMask | NET::UtilityMask;

    NET::WindowType type = windowType(winmask);
    return (type == NET::Menu || type == NET::Toolbar || type == NET::Utility);
}

#include "kwinqinx.moc"

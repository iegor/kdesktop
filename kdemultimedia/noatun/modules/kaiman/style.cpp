/*
   Copyright (c) 2000 Stefan Schimanski (1Stein@gmx.de)
                 1999-2000 Christian Esken (esken@kde.org)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include <kapplication.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qdropsite.h>
#include <kdebug.h>
#include <klocale.h>
#include <kurldrag.h>
#include <qtimer.h>

#include <stdlib.h>
#include <unistd.h>

#include "style.h"
#include "userinterface.h"
#include <noatun/app.h>
#include <noatun/stdaction.h>

const bool KaimanStyleSlider::optionVertical = 1;
const bool KaimanStyleSlider::optionReversed = 2;
const bool KaimanStyleText::optionExtended = 1;

KaimanStyleElement::KaimanStyleElement(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    // Initialize everything to default values
    filename = "";
    element  = "";
    upperLeft.setX(0);
    upperLeft.setY(0);
    dimension.setWidth(0);
    dimension.setHeight(0);
    optionPrelight = optionStatuslight = false;
    options[0] = options[1] = options[2] = false;
    _currentPixmap = 0;
    digits = -1;
    pixmapLines = 1;
    pixmapColumns = 1;

    setAcceptDrops(true);
    pixmaps.setAutoDelete(true);

    setBackgroundMode( NoBackground );
}


KaimanStyleElement::~KaimanStyleElement()
{
}


void KaimanStyleElement::loadPixmaps(QString &val_s_filename)
{
    QPixmap pixmap;

    bool i_b_ret = pixmap.load(val_s_filename);
    pixmapNum = pixmapLines*pixmapColumns;

    pixmaps.resize(pixmapNum);

    if ( i_b_ret )  {
        if(pixmapNum) {
            int firstWidth, firstHeight, width, height, sourcex = 0, sourcey = 0;

            // first bitmap may be with different size
            if ( dimension.width()!=0 )
            {
                firstWidth = dimension.width();
                if ( pixmapColumns>1 )
                    width = (pixmap.width()-firstWidth) / (pixmapColumns-1);
                else
                    width = 0;
            } else
                firstWidth = width = pixmap.width() / pixmapColumns;

            if ( dimension.height()!=0 )
            {
                firstHeight = dimension.height();
                if ( pixmapLines>1 )
                    height = (pixmap.height()-firstHeight) / (pixmapLines-1);
                else
                    height = 0;
            } else
                firstHeight = height = pixmap.height() / pixmapLines;

            // create single pixmaps
            int i=0;
            sourcey = 0;
            for( int y=0; y<pixmapLines; y++ )
            {
                int h = (y==0) ? firstHeight : height;
                sourcex = 0;

                for( int x=0; x<pixmapColumns; x++ )
                {
                    int w = (x==0) ? firstWidth : width;

                    QPixmap *part = new QPixmap(w,h,pixmap.depth());
                    part->fill(Qt::black);
                    bitBlt(part,0,0,&pixmap,sourcex,sourcey,w,h);
                    pixmaps.insert(i,part);

                    if(pixmap.mask())
                    {
                        QBitmap maskpart(w,h);
                        bitBlt(&maskpart,0,0,pixmap.mask(),sourcex,sourcey,w,h);
                        part->setMask(maskpart);
                    }

                    i++;
                    sourcex += w;
                }

                sourcey += h;
            }
        }
    } else {
        kdDebug() << "Cannot load pixmap " <<  val_s_filename << endl;

        for ( int i=0; i<pixmapNum; i++ )
        {
            QPixmap *pm = new QPixmap(10, 10);
            pm->fill(Qt::black);
            pixmaps.insert( i, pm );
        }
    }

    if ( dimension.width()==0 ) dimension.setWidth( pixmaps[0]->width() );
    if ( dimension.height()==0 ) dimension.setHeight( pixmaps[0]->height() );
    setGeometry( QRect(upperLeft, dimension) );
}

void KaimanStyleElement::setPixmap( int num )
{
    if ( num!=_currentPixmap )
    {
        if ( num>pixmapNum-1 ) num = pixmapNum-1;
        if ( num<0 ) num = 0;

        _currentPixmap = num;
        repaint( FALSE );
    }
}

void KaimanStyleElement::paintEvent ( QPaintEvent */*qpe*/ )
{
    QPixmap *pm = pixmaps[_currentPixmap];
    if ( pm )
        bitBlt(this, 0, 0, pm );
    else
        kdDebug() << "Invalid pixmap" << endl;

/*    QPainter p( this );
    p.setBrush( NoBrush );
    p.setPen( QColor(255,255,255) );
    p.drawRect( 0, 0, width(), height() );
    p.drawText( 2, 16, name() ); */
}

void KaimanStyleElement::dragEnterEvent( QDragEnterEvent *event )
{
    event->accept( KURLDrag::canDecode(event) );
}

void KaimanStyleElement::dropEvent( QDropEvent *event )
{
    ((Kaiman*)(parentWidget()->parentWidget()))->doDropEvent(event);
}

/***************************************************************************/


KaimanStyleButton::KaimanStyleButton(QWidget *parent, const char *name)
    : KaimanStyleMasked(parent, name)
{
    i_b_lit = i_b_prelit = i_b_down = false;
    i_i_currentState = NormalUp;

    I_pmIndex.resize( StateListEND );

    for (int i=0; i<StateListEND; i++) {
        // Set pixmap index of all states to 0 (the default pixmap)
        I_pmIndex.insert( i, new int(0));
    }
}

KaimanStyleButton::~KaimanStyleButton()
{
}

void KaimanStyleButton::mousePressEvent(QMouseEvent *qme)
{
    // We deactivate prelight, because the user presses the button.
    // So it is now down, but there is no PrelitDown icon (BTW: would
    // make no real sense anyhow).
    setPrelight(false);
    setDown(true);
    grabMouse();

    KaimanStyleMasked::mousePressEvent( qme );
}

void KaimanStyleButton::mouseReleaseEvent(QMouseEvent *qme)
{
    releaseMouse ();

    if (down())
    {
        setDown(false);
        emit clicked();
    }

    KaimanStyleMasked::mouseReleaseEvent( qme );
}


/* paint prelight */
void KaimanStyleButton::enterEvent ( QEvent * e )
{
    if ( !down() )
        setPrelight(true);

    KaimanStyleMasked::enterEvent( e );
}

/* unpaint prelight */
void KaimanStyleButton::leaveEvent ( QEvent * e )
{
    if (!down())
        setPrelight(false);

    KaimanStyleMasked::leaveEvent( e );
}

bool KaimanStyleButton::lit()
{
    return i_b_lit;
}

void KaimanStyleButton::setLit(bool val_b_lit)
{
    i_b_lit = val_b_lit;
    updateButtonState();
}

bool KaimanStyleButton::prelit()
{
    return i_b_prelit;
}

void KaimanStyleButton::setPrelight(bool val_b_prelit)
{
    i_b_prelit = val_b_prelit;
    updateButtonState();
}

bool KaimanStyleButton::down()
{
    return i_b_down;
}

void KaimanStyleButton::setDown(bool val_b_down)
{
    i_b_down = val_b_down;
    updateButtonState();
}

void KaimanStyleButton::updateButtonState() {

    if ( i_b_prelit ) {
        if ( i_b_lit ) {
            // Prelit and Lit
            i_i_currentState = PrelightLitUp;
        }
        else {
            // Prelit and not Lit
            i_i_currentState = PrelightUp;
        }
    }

    else if ( i_b_lit ) {
        if ( i_b_down ) {
            // Lit and Down
            i_i_currentState = LitDown;
        } else {
            // Lit and not Down
            i_i_currentState = LitUp;
        }
    }
    else {
        if ( i_b_down ) {
            // Normal and Down
            i_i_currentState = NormalDown;
        }
        else {
            // Normal and not Down
            i_i_currentState = NormalUp;
        }
    }

    setPixmap( *I_pmIndex[i_i_currentState] );
    repaint();
}


/***********************************************************************/

KaimanStyleSlider::KaimanStyleSlider(int min, int max, QWidget *parent, const char *name)
    : KaimanStyleMasked( parent, name )
{
    _min = min;
    _max = max;
    _down = false;
    _lit = false;

    setValue( _min );
}


KaimanStyleSlider::~KaimanStyleSlider()
{
}


void KaimanStyleSlider::setValue( int value )
{
    if (value>_max) value=_max;
    if (value<_min) value=_min;
    _value = value;
    repaint();
}


void KaimanStyleSlider::setValue( int value, int min, int max )
{
    if ( value!=_value || min!=_min || max!=_max ) {
        _min = min;
        _max = max;
        setValue( value );
        repaint();
    }
}

int KaimanStyleSlider::pos2value( int x, int y )
{
    int p;
    int v;
    if ( options[optionVertical] ) {
        p = y;
        v = p*(_max-_min)/height();
    } else {
        p = x;
        v = p*(_max-_min)/width();
    }

    if ( options[optionReversed] ) v = (_max-_min) - v;
    return _min + v;
}


void KaimanStyleSlider::mouseMoveEvent(QMouseEvent *qme)
{
    KaimanStyleMasked::mouseMoveEvent( qme );

    if ( _down )
    {
        setValue( pos2value(qme->x(), qme->y()) );
        emit newValue( value() );
    }
}

void KaimanStyleSlider::mousePressEvent(QMouseEvent *qme)
{
    if ( !_down )
    {
        grabMouse();
        _down = true;

        setValue( pos2value(qme->x(), qme->y()) );
        emit newValueDrag( value() );
        emit newValue( value() );
    }

    KaimanStyleMasked::mousePressEvent( qme );
}


void KaimanStyleSlider::mouseReleaseEvent(QMouseEvent *qme)
{
    if ( _down )
    {
        _down = false;
        releaseMouse();
        repaint();

        setValue( pos2value(qme->x(), qme->y()) );
        emit newValue( value() );
        emit newValueDrop( value() );
    }

    KaimanStyleMasked::mouseReleaseEvent( qme );
}


void KaimanStyleSlider::paintEvent(QPaintEvent */*qpe*/)
{
    // draw background
    bitBlt( this, 0, 0, pixmaps[0] );

    // draw optional handle
    QPixmap *handle;
    if ( _down )
        handle = pixmaps[2];
    else
    {
        if ( _lit && optionPrelight )
            handle = pixmaps[3];
        else
            handle = pixmaps[1];
    }

    if ( handle && handle->width() )
    {
        int x = 0;
        int y = 0;

        if ( _max-_min ) {
            int v = _value-_min;
            if ( options[optionReversed] ) v = (_max-_min) - v;

            if ( options[optionVertical] )
                y = ( height()-handle->height() ) * v / (_max-_min);
            else
                x = ( width()-handle->width() ) * v / (_max-_min);
        }

        bitBlt( this, x, y, handle );
    }
}

void KaimanStyleSlider::enterEvent ( QEvent * e )
{
    if ( !_lit && optionPrelight )
    {
        _lit = true;
        repaint();
    }

    KaimanStyleMasked::enterEvent( e );
}

void KaimanStyleSlider::leaveEvent ( QEvent * e )
{
    if ( _lit )
    {
        _lit = false;
        repaint();
    }

    KaimanStyleMasked::leaveEvent( e );
}


/***********************************************************************/


KaimanStyleBackground::KaimanStyleBackground(QWidget *parent, const char *name)
    : KaimanStyleMasked( parent, name )
{
    i_b_move = false;
}

KaimanStyleBackground::~KaimanStyleBackground()
{
}

void KaimanStyleBackground::mouseReleaseEvent(QMouseEvent *qme)
{
    i_b_move = false;
    KaimanStyleMasked::mouseReleaseEvent( qme );
}

void KaimanStyleBackground::mouseMoveEvent(QMouseEvent *qme)
{
    QPoint diff = qme->globalPos() - i_point_lastPos;
    if ( abs(diff.x()) > 10 || abs(diff.y()) > 10) {
        // Moving starts only, when passing a drag border
        i_b_move = true;
    }

    if ( i_b_move ) {
        QWidget *p = parentWidget()->parentWidget();
        if ( !p ) p = parentWidget();

        p->move( qme->globalPos() - i_point_dragStart );
    }

    KaimanStyleMasked::mouseMoveEvent( qme );
}

void KaimanStyleBackground::mousePressEvent(QMouseEvent *qme)
{
    // On the background we move the shaped toplevel around
    if (!i_b_move) {
        i_point_dragStart = qme->pos();
        i_point_lastPos = qme->globalPos();
    }

    KaimanStyleMasked::mousePressEvent( qme );
}

/***********************************************************************/

KaimanStyleValue::KaimanStyleValue(int min, int max, QWidget *parent, const char *name)
    : KaimanStyleMasked( parent, name )
{
    _min = min;
    _max = max;
    _value = _min;

    setPixmap( 0 );
}

KaimanStyleValue::~KaimanStyleValue()
{
}

void KaimanStyleValue::setValue( int value )
{
    if (value>_max) value=_max;
    if (value<_min) value=_min;
    _value = value;

    int len = _max-_min;
    if ( len )
        setPixmap( (_value-_min)*pixmapNum/len );
    else
        setPixmap( 0 );
}

void KaimanStyleValue::setValue( int value, int min, int max )
{
    _min = min;
    _max = max;

    setValue( value );
}


/***********************************************************************/

KaimanStyleNumber::KaimanStyleNumber(QWidget *parent, const char *name)
    : KaimanStyleElement( parent, name )
{
    //kdDebug(66666) << k_funcinfo << "name = '" << name << "'" << endl;
    _value = 0;
    if (QCString(name) == "In_Rate_Number")
       digits = 3;
    else
       digits = 2;
}

KaimanStyleNumber::~KaimanStyleNumber()
{
}


void KaimanStyleNumber::loadPixmaps(QString &val_s_filename)
{
    KaimanStyleElement::loadPixmaps( val_s_filename );
    resize( digits*pixmaps[0]->width(),pixmaps[0]->height() );
}


void KaimanStyleNumber::setValue( int value )
{
    if ( _value!=value )
    {
        _value = value;
        repaint();
    }
}

void KaimanStyleNumber::paintEvent(QPaintEvent */*qpe*/)
{
    // check for overflow
    int v = _value;
    for ( int i=digits; i>0 && v>0; i-- )
        v /= 10;

    if ( v!=0 )
        v = 999999999; // overflow
    else
        v = _value;

    // draw number
    int x = width();
    do {
        x -= pixmaps[0]->width();
        bitBlt(this, x, 0, pixmaps[v%10] );

        v /= 10;
    } while ( v>0 );

    // draw right free space
    while ( x>0 )
    {
        x -= pixmaps[0]->width();
        bitBlt(this, x, 0, pixmaps[0] );
    }
/*
    QPainter p( this );
    p.setBrush( NoBrush );
    p.setPen( QColor(255,255,255) );
    p.drawRect( 0, 0, width(), height() );
    p.drawText( 2, 16, name() );*/
}

/***********************************************************************/

KaimanStyleText::KaimanStyleText(QWidget *parent, const char *name)
    : KaimanStyleElement( parent, name )
{
    _pos = 0;
    _timer = new QTimer( this );
    _delay = 500;
    connect( _timer, SIGNAL(timeout()), this, SLOT(timeout()) );
}

KaimanStyleText::~KaimanStyleText()
{
}


void KaimanStyleText::loadPixmaps(QString &val_s_filename)
{
    KaimanStyleElement::loadPixmaps( val_s_filename );
    resize( digits*pixmaps[0]->width(), pixmaps[0]->height() );
}


void KaimanStyleText::setValue( QString value )
{
    if ( value!=_value ) {
        _pos = 0;
        _direction = 1;
        _value = value;
        repaint();
    }
}


void KaimanStyleText::startAnimation( int delay )
{
    _pos = 0;
    _direction = 1;
    _delay = delay;
    _timer->start( _delay, TRUE );
}


void KaimanStyleText::stopAnimation()
{
    _pos = 0;
    _timer->stop();
}


void KaimanStyleText::timeout()
{
    // reflect
    if ( _pos+_direction<0 || (int)_value.length()-(_pos+_direction)<digits ) {
        _direction = -_direction;
        _timer->start( _delay*5, TRUE );
    } else {
        // check new position
        if ( _pos+_direction>=0 && (int)_value.length()-(_pos+_direction)>=digits ) {
            _pos += _direction;
            repaint();
        }

        _timer->start( _delay, TRUE );
    }


}


void KaimanStyleText::paintEvent(QPaintEvent */*qpe*/)
{
    // draw number
    int p;
    for (p=0; p<digits && p<(int)_value.length()-_pos; p++ )
    {
        int pmNum = _value[p+_pos].latin1() - ' ' ;
        if ( pmNum>=96 ) pmNum = '?' - ' ';
        if ( pmNum<0 ) pmNum = '?' - ' ';

        QPixmap *pm = pixmaps[pmNum];
        if ( pm ) bitBlt(this, p*pixmaps[0]->width(), 0, pm );
    }

    QPixmap *pm = pixmaps[0];
    for ( ; p<digits; p++ )
        bitBlt(this, p*pixmaps[0]->width(), 0, pm );

/*    QPainter pnt( this );
    pnt.setBrush( NoBrush );
    pnt.setPen( QColor(255,255,255) );
    pnt.drawRect( 0, 0, width(), height() );
    pnt.drawText( 2, 16, name() );*/
}

/***********************************************************************/

KaimanStyleAnimation::KaimanStyleAnimation(int delay, QWidget *parent, const char *name)
    : KaimanStyleMasked( parent, name )
{
    _delay = delay;
    _frame = 0;
    _timer = new QTimer( this );
    connect( _timer, SIGNAL(timeout()), this, SLOT(timeout()) );
}

KaimanStyleAnimation::~KaimanStyleAnimation()
{
}

void KaimanStyleAnimation::start()
{
    _timer->start( _delay, FALSE );
}

void KaimanStyleAnimation::pause()
{
    _timer->stop();
}

void KaimanStyleAnimation::stop()
{
    _timer->stop();
    _frame = 0;
    setPixmap( _frame );
}

void KaimanStyleAnimation::timeout()
{
    _frame++;
    if ( _frame>=pixmapNum ) _frame = 1;
    setPixmap( _frame );
}

/***********************************************************************/

KaimanStyleState::KaimanStyleState(QWidget *parent, const char *name)
    : KaimanStyleMasked( parent, name )
{
    _value = 0;
}

KaimanStyleState::~KaimanStyleState()
{
}

void KaimanStyleState::setValue( int value )
{
    _value = value;
    setPixmap( _value );
}

void KaimanStyleState::mousePressEvent(QMouseEvent *qme)
{
    emit clicked();
    KaimanStyleMasked::mouseReleaseEvent( qme );
}

/***********************************************************************/


KaimanStyle::KaimanStyle( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    i_qw_parent = parent;
    i_eventSemaphore = false;
}


KaimanStyle::~KaimanStyle()
{
}


int KaimanStyle::parseStyleFile(QString &l_s_tmpName)
{
    int l_i_ret = false;

    QStringList l_s_tokens;

    QFile l_fd(l_s_tmpName);
    if ( l_fd.open(IO_ReadOnly) ) {
        // file opened successfully
        QTextStream l_ts_line( &l_fd );
        QString l_s_textLine, l_s_token;
        while ( !l_ts_line.eof() ) {
            // Clear list of tokens (we are going to fill them now)
            l_s_tokens.clear();

            // Read a line
            l_s_textLine = l_ts_line.readLine();
            l_s_textLine = l_s_textLine.simplifyWhiteSpace();

            if ( l_s_textLine.left(1) != "#" ) {
                // OK, this is not a comment line
                if ( l_s_textLine.isNull())
                    l_s_textLine = "";
                while ( !l_s_textLine.isEmpty() ) {
                    l_s_token = getToken(l_s_textLine, ' ');
                    if ( ! l_s_token.isEmpty() ) {
                        // OK. There is a useful token. It is not NULL
                        if ( l_s_token.right(1) == ":" )
                            l_s_tokens.append(l_s_token.left(l_s_token.length() -1 ));
                        else
                            l_s_tokens.append(l_s_token);
                    } // -<- if it is a not-empty token
                } // -<- while there are tokens available

                interpretTokens(l_s_tokens);

            } // -<- if is not comment line
        } // -<- While not EOF on file

        l_i_ret = 0;
    } // -<- if file could be opened

    else {
        l_i_ret = KaimanStyle::FileNotFound;
    }
    return l_i_ret;
}



/*
  This function gets a list of tokens and inserts a new
  KaimanStyleElement in I_styleElem. */
void  KaimanStyle::interpretTokens(QStringList& ref_s_tokens)
{
    if ( ref_s_tokens.count() < 1 ) {
        // A list with less than 1 item is useless to us
        return;
    }

    QString l_s_tokenTypes;
    const QString &l_s_elem = ref_s_tokens.first();
    bool l_vertPixmaps = false;
    int l_i_pmIndex[KaimanStyleButton::StateListEND];
    for (int i=0; i<KaimanStyleButton::StateListEND; i++) l_i_pmIndex[i]=0;

    enum { UnknownElement, BackgroundElement, MaskElement, ButtonElement, SliderElement,
           ValueElement, AnimationElement, StateElement, DigitElement, NumberElement,
           TextElement } l_elementType = UnknownElement;

    // Now determine the meaning of the following tokens
    // l_s_tokenTypes stores the meaning (e.g. x-Position, filename, ...)
    if ( l_s_elem == "Background" ) {
        l_s_tokenTypes = "f";
        l_elementType = BackgroundElement;
    }
    // ---
    else if ( l_s_elem == "Mask" ) {
        l_s_tokenTypes = "f";
        l_elementType = MaskElement;
    } else if ( l_s_elem=="Digit_Small" || l_s_elem=="Digit_Small_Default" ) {
        i_smallFont = ref_s_tokens[1];
        return;
    } else if ( l_s_elem=="Digit_Large" || l_s_elem=="Digit_Large_Default" ) {
        i_largeFont = ref_s_tokens[1];
        return;
    }
    // ---
    else if ( l_s_elem == "Title" ||
              l_s_elem == "Album"  ||
              l_s_elem == "Artist"  ||
              l_s_elem == "Genre" ) {
        // You can have an OPTIONAL argument, so lets see if it is there.
        if (ref_s_tokens.count() == 6 )
            l_s_tokenTypes = "fd1xy";
        else
            l_s_tokenTypes = "fdxy";
        l_elementType = TextElement;
    }
    // ---
    else if ( l_s_elem == "Play_Button" ||
              l_s_elem == "Pause_Button"  ||
              l_s_elem == "Stop_Button"  ||
              l_s_elem == "Shuffle_Button"  ||
              l_s_elem == "Repeat_Button" ) {
        l_s_tokenTypes = "fPSxy";
        l_i_pmIndex[KaimanStyleButton::NormalUp]     = 0;
        l_i_pmIndex[KaimanStyleButton::NormalDown]   = 1;
        l_i_pmIndex[KaimanStyleButton::LitUp]        = 2;
        l_i_pmIndex[KaimanStyleButton::LitDown]      = 3;
        l_i_pmIndex[KaimanStyleButton::PrelightUp]   = 4;
        l_i_pmIndex[KaimanStyleButton::PrelightLitUp] = 5;
        l_elementType = ButtonElement;
    }
    // ---
    else if ( l_s_elem == "Next_Button" ||
              l_s_elem == "Prev_Button"  ||
              l_s_elem == "FF"  ||
              l_s_elem == "RW"  ||
              l_s_elem == "Playlist_Button"  ||
              l_s_elem == "Config_Button"  ||
              l_s_elem == "Iconify_Button"  ||
              l_s_elem == "Mixer_Button"  ||
              l_s_elem == "Exit_Button"  ||
              l_s_elem == "Alt_Skin_Button"  ||
              l_s_elem == "Volume_Up_Button"  ||
              l_s_elem == "Volume_Down_Button"  ||
              l_s_elem == "Balance_Left_Button"  ||
              l_s_elem == "Balance_Right_Button" ) {
        l_i_pmIndex[KaimanStyleButton::NormalUp]     = 0;
        l_i_pmIndex[KaimanStyleButton::NormalDown]  = 1;
        l_i_pmIndex[KaimanStyleButton::PrelightUp]   = 2;
        l_s_tokenTypes = "fPxy";
        l_elementType = ButtonElement;
    }
    // ---
    else if ( l_s_elem == "Stereo_Item" ||
              l_s_elem == "Shuffle_Item"  ||
              l_s_elem == "Repeat_Item"  ||
              l_s_elem == "Mpegversion_Item"  ||
              l_s_elem == "Mpeglayer_Item"  ||
              l_s_elem == "Mpegmode_Item"  ||
              l_s_elem == "Status_Item" ) {
        l_s_tokenTypes = "Vfxy";
        l_elementType = StateElement;
    }
    // ---
    else if ( l_s_elem == "Hour_Number"  ||
              l_s_elem == "Minute_Number"  ||
              l_s_elem == "Second_Number"  ||
              l_s_elem == "Song_Number"  ||
              l_s_elem == "Total_Number"  ||
              l_s_elem == "In_Rate_Number"  ||
              l_s_elem == "In_Hz_Number"  ||
              l_s_elem == "Out_Bits"  ||
              l_s_elem == "Out_Hz"  ||
              l_s_elem == "Song_Minute"  ||
              l_s_elem == "Song_Second"  ||
              l_s_elem == "Frame"  ||
              l_s_elem == "Frame_Total"  ||
              l_s_elem == "CPU_Number"  ||
              l_s_elem == "Hour_Total"  ||
              l_s_elem == "Minute_Total"  ||
              l_s_elem == "Second_Total_Number" ) {
        // You can have an OPTIONAL argument, so lets see if it is there.
        if (ref_s_tokens.count() == 6 )
            l_s_tokenTypes = "fd1xy";
        else if (ref_s_tokens.count() == 5 )
            l_s_tokenTypes = "fdxy";
        else
            l_s_tokenTypes = "fxy";

        l_elementType = NumberElement;
    }
    // ---
    else if ( l_s_elem == "Position_Item" ||
              l_s_elem == "Volume_Item"  ||
              l_s_elem == "Balance_Item" ) {
        l_s_tokenTypes = "Vflxy";
        l_elementType = ValueElement;
    }
    // ---
    else if ( l_s_elem == "Load_Item" ) {
        l_s_tokenTypes = "Vflxy";
        l_elementType = AnimationElement;
    }
    // ---
    else if ( l_s_elem == "Position_Slider" ||
              l_s_elem == "Volume_Slider"  ||
              l_s_elem == "Balance_Slider" ) {

        if ( ref_s_tokens.count()==10 )
            l_s_tokenTypes = "VfP12wxyh";
        else
            l_s_tokenTypes = "VfP12sxy";
        l_elementType = SliderElement;
    }
    else {
        kdDebug() << l_s_elem << " not handled yet." << endl;
        l_s_tokenTypes = "f";
    }

    /* The above lines decode the meanings of the tokens. The rules for
       this are the SKIN-SPECS. So the decoder implements a syntactic
       analyser (parser).

       I now do know the type of each tokens, whether it represents a
       filename, the prelight parameter, the x position or what else. This
       information resides in l_s_tokenTypes, and will help in creating
       the kaiman style elements.

       I will now do two things:
       1) Create a new KaimanStyle*, that is inserted into I_styleElem.
       2) Fill the KaimanStyleElement structure, by interpreting the tokens.
    */
    QStringList::Iterator li_s_tokens = ref_s_tokens.begin();
    ++ li_s_tokens; // Skip the name of the element

    // 1) Create a new KaimanStyleElement, that is inserted into I_styleElem.
    KaimanStyleElement *l_kse_elem = 0;
    KaimanStyleButton *but = 0;

    switch ( l_elementType )
    {
        case BackgroundElement:
            l_kse_elem = new KaimanStyleBackground(this, l_s_elem.latin1());
            l_kse_elem->show();
            break;

        case MaskElement:
            l_kse_elem = new KaimanStyleElement(this, l_s_elem.latin1());
            l_kse_elem->hide();
            break;

        case ButtonElement:
            l_kse_elem = new KaimanStyleButton(this, l_s_elem.latin1());
            but = static_cast<KaimanStyleButton*>(l_kse_elem);
            l_kse_elem->show();
            break;

        case SliderElement:
            l_kse_elem = new KaimanStyleSlider(0, 100, this, l_s_elem.latin1());
            i_sliders.append(l_kse_elem);
            l_kse_elem->show();
            break;

        case ValueElement:
            l_kse_elem = new KaimanStyleValue(0, 100, this, l_s_elem.latin1());
            break;

        case AnimationElement:
            l_kse_elem = new KaimanStyleAnimation(30, this, l_s_elem.latin1());
            break;

        case StateElement:
            l_kse_elem = new KaimanStyleState(this, l_s_elem.latin1());
            break;

        case NumberElement:
            l_kse_elem = new KaimanStyleNumber(this, l_s_elem.latin1());
            break;

        case TextElement:
            l_kse_elem = new KaimanStyleText(this, l_s_elem.latin1());
            break;

        default:
            break;
    }

    if ( !l_kse_elem )
    {
        kdDebug() << "Ignoring style element " << l_s_elem << endl;
        return;
    }

    // insert element into element list
    uint l_i_size = I_styleElem.size();
    I_styleElem.resize(l_i_size + 1);
    I_styleElem.insert(l_i_size, l_kse_elem);
    l_kse_elem->installEventFilter( this );

    // initialize element parameters
    l_kse_elem->element = l_s_elem;

    if (  l_s_tokenTypes.left(1) == "V" ) {
        // Vertical flag
        l_vertPixmaps = true;
        l_s_tokenTypes = l_s_tokenTypes.mid(1);
    }

    // initialize button parameters
    if ( but )
    {
        for (int i=0; i<KaimanStyleButton::StateListEND; i++)
            but->I_pmIndex.insert(i, new int(l_i_pmIndex[i]) );
    }

    // 2) Fill the KaimanStyleElement structure, by interpreting the tokens.
    while ( l_s_tokenTypes.length() != 0 ) {
        /* The skindata format allows omitting arguments if the parser
           can reconstruct without problems what you mean.  This is taken
           into account when writing the l_s_tokenTypes sting.

           Unfortunately, several skins do ship with a broken skindata file.
           Most common problem is that width and height is also given.

           Even worse examples leave out specified parametes and add others.
           For instance, the pause line is specified as "fPSxy". But in k9


           the line looks like

           Pause_Button: pause.jpg FALSE 91 148 116 173

           So the actual parameters are fPxywh. The parser has to be pretty
           smart now. It should "see" that S (status light) is not present,
           since that should be either TRUE or FALSE, and distribute the
           others accordingly. */

        bool skipOne;

        do {
            skipOne = false;

            // Take the first item from the l_s_tokenTypes;
            char l_c_type = (l_s_tokenTypes[0]).latin1();
            l_s_tokenTypes = l_s_tokenTypes.mid(1);
            QString l_s_token = *li_s_tokens;
            switch(l_c_type) {
            case 'f':
                // filename
                l_kse_elem->filename = l_s_token;
                break;
            case 'P':
                // Prelight
                if ( l_s_token.upper() == "TRUE" )
                    l_kse_elem->optionPrelight = true;
                else
                {
                    l_kse_elem->optionPrelight = false;

                    // was that token really there?
                    skipOne = ( l_s_token.upper() != "FALSE" );
                }
                break;
            case 'S':
                // Statuslight
                if ( l_s_token.upper() == "TRUE" )
                    l_kse_elem->optionStatuslight = true;
                else
                {
                    l_kse_elem->optionStatuslight = false;

                    // was that token really there?
                    skipOne = ( l_s_token.upper() != "FALSE" );
                }
                break;
            case '1':
                // parameter 1
                if ( l_s_token.upper() == "TRUE" )
                    l_kse_elem->options[0] = true;
                else
                {
                    l_kse_elem->options[0] = false;

                    // was that token really there?
                    skipOne = ( l_s_token.upper() != "FALSE" );
                }
                break;
            case '2':
                // parameter 2
                if (  l_s_token.upper() == "TRUE" )
                    l_kse_elem->options[1] = true;
                else
                {
                    l_kse_elem->options[1] = false;

                    // was that token really there?
                    skipOne = ( l_s_token.upper() != "FALSE" );
                }
                break;
            case '3':
                // parameter 3
                if (  l_s_token.upper() == "TRUE" )
                    l_kse_elem->options[2] = true;
                else
                {
                    l_kse_elem->options[2] = false;

                    // was that token really there?
                    skipOne = ( l_s_token.upper() != "FALSE" );
                }
                break;
            case 'l':
                // length
                if ( l_vertPixmaps )
                    l_kse_elem->pixmapLines = l_s_token.toInt();
                else
                    l_kse_elem->pixmapColumns = l_s_token.toInt();
                break;
            case 'x':
                // x Position
                l_kse_elem->upperLeft.setX(l_s_token.toInt());
                break;
            case 'y':
                // y Position
                l_kse_elem->upperLeft.setY(l_s_token.toInt());
                break;
            case 's':
                // dimension
                if ( l_kse_elem->options[KaimanStyleSlider::optionVertical] )
                    l_kse_elem->dimension.setHeight(l_s_token.toInt());
                else
                    l_kse_elem->dimension.setWidth(l_s_token.toInt());
                break;
            case 'w':
                // width
                l_kse_elem->dimension.setWidth(l_s_token.toInt());
                break;
            case 'h':
                // height
                l_kse_elem->dimension.setHeight(l_s_token.toInt());
                break;
            case 'd':
                // number of digits
                l_kse_elem->digits = l_s_token.toInt();
                break;

            default:
                kdDebug() << "Element type '" << l_c_type << "' unknown" << endl;
            }

            if(skipOne) {
                kdDebug() << "Skipped one element '" << l_c_type << "'" << endl;
            }
        } while(skipOne && l_s_tokenTypes.length() != 0);

        // Next token.
        ++li_s_tokens;
        if (li_s_tokens == ref_s_tokens.end() ) {
            // End of token list
            break;
        }
    }

    /* Do some post-processing */

    if( l_elementType==ButtonElement ) {
        // <Normal button>
        if(but->optionPrelight) {
            // --- Has Prelight ---
            if(but->optionStatuslight)
                but->pixmapColumns = 6;
            else {
                but->pixmapColumns = 3;
                but->I_pmIndex.insert( KaimanStyleButton::LitUp,
                                       new int( *(but->I_pmIndex[KaimanStyleButton::NormalUp])));
                but->I_pmIndex.insert( KaimanStyleButton::LitDown,
                                       new int( *(but->I_pmIndex[KaimanStyleButton::NormalDown])));
            }
        } else {
            // --- Has No Prelight ---
            but->I_pmIndex.insert( KaimanStyleButton::PrelightUp,
                                   new int( *(but->I_pmIndex[KaimanStyleButton::NormalUp])));
            but->I_pmIndex.insert( KaimanStyleButton::PrelightLitUp,
                                   new int( *(but->I_pmIndex[KaimanStyleButton::LitUp])));
            if(l_kse_elem->optionStatuslight)
                but->pixmapColumns = 4;
            else {
                but->pixmapColumns = 2;
                but->I_pmIndex.insert( KaimanStyleButton::LitUp,
                                       new int( *(but->I_pmIndex[KaimanStyleButton::NormalUp])));
                but->I_pmIndex.insert( KaimanStyleButton::LitDown,
                                       new int( *(but->I_pmIndex[KaimanStyleButton::NormalDown])));
                but->I_pmIndex.insert( KaimanStyleButton::PrelightLitUp,
                                       new int( *(but->I_pmIndex[KaimanStyleButton::NormalUp])));
            }
        }
    } else if( l_elementType==NumberElement ) {
        // number items
        l_kse_elem->pixmapColumns = 11;
        if ( l_kse_elem->filename=="Small" ) l_kse_elem->filename = i_smallFont;
        else if ( l_kse_elem->filename=="Large" ) l_kse_elem->filename = i_largeFont;
    } else if( l_elementType==SliderElement ) {
        // slider items
        if ( l_kse_elem->options[KaimanStyleSlider::optionVertical] )
            l_kse_elem->pixmapLines = l_kse_elem->optionPrelight ? 4 : 3;
        else
            l_kse_elem->pixmapColumns = l_kse_elem->optionPrelight ? 4 : 3;
    } else if( l_elementType==TextElement ) {
        // text items
        l_kse_elem->pixmapColumns = 32;
        l_kse_elem->pixmapLines = l_kse_elem->options[KaimanStyleText::optionExtended] ? 6 : 3;
    } else {
        // <Not standard element>
        if(l_s_elem == "Stereo_Item")
            l_kse_elem->pixmapLines = 3;
        else if(l_s_elem == "Shuffle_Item")
            l_kse_elem->pixmapLines = 2;
        else if(l_s_elem == "Repeat_Item")
            l_kse_elem->pixmapLines = 2;
        else if(l_s_elem == "Mpegversion_Item")
            l_kse_elem->pixmapLines = 3;
        else if(l_s_elem == "Mpegversion_Item")
            l_kse_elem->pixmapLines = 4;
        else if(l_s_elem == "Mpegmode_Item")
            l_kse_elem->pixmapLines = 5;
        else if(l_s_elem == "Status_Item")
            l_kse_elem->pixmapLines = 3;
    } // </Not normal button>
}


QString KaimanStyle::getToken(QString &val_s_string, char val_c_separator)
{
    int l_i_pos;
    QString l_s_token;

    // Find the first occurrence of the separator
    l_i_pos = val_s_string.find(val_c_separator, 0, false);
    if ( l_i_pos == -1 ) {
        // No sparator! Then the whole string is the token
        l_s_token = val_s_string;
        val_s_string = "";
    }
    else {
        // Separator found: Split the string at the separator position
        l_s_token = val_s_string.left(l_i_pos);
        val_s_string.remove(0,l_i_pos);
    }
    val_s_string = val_s_string.simplifyWhiteSpace();

    // Return the first token
    return l_s_token;
}



bool KaimanStyle::loadStyle(const QString &styleName, const QString &descFile)
{
    bool l_b_ret = true;
    int l_i_ret  = 0;
    QString l_s_tmpName;

    i_skinName = styleName;
    this->i_s_styleName = styleName;
    i_s_styleBase = QString("skins/kaiman/") + i_s_styleName + QString("/");

    l_s_tmpName =  locate("appdata",  i_s_styleBase + descFile );
    if ( l_s_tmpName.isNull() ) {
        l_b_ret = false;
    }

    if ( l_b_ret) {
        // Skin description found. Now parse StyleFile.
        l_i_ret = parseStyleFile(l_s_tmpName);
        if (l_i_ret == 0) {
            // If parsiing OK, load the referenced pixmaps
            l_b_ret = loadPixmaps();

            kdDebug(66666) << "Found " << I_styleElem.count() << " elements." << endl;
        }
        else {
            if ( l_i_ret == KaimanStyle::FileNotFound ) {
                // File not found
                KMessageBox::error( 0, i18n("Cannot load style. Style not installed.") );
            }
            else {
                // Parsing not OK: Notify user
                KMessageBox::error( 0, i18n("Cannot load style. Unsupported or faulty style description.") );
            }
        }
    }

    return l_b_ret;
}

bool KaimanStyle::eventFilter( QObject *o, QEvent *e )
{
    /* HACK! HACK! HACK! */
    if ( !i_eventSemaphore )
        if ( e->type()==QEvent::MouseMove || e->type()==QEvent::MouseButtonPress ||
             e->type()==QEvent::MouseButtonRelease )
        {
            QMouseEvent *m = (QMouseEvent*)e;

            // handle noatun context menu
            if (m->button()==RightButton) {
                NoatunStdAction::ContextMenu::showContextMenu();
                return true;
            }

            QPoint mousePos( m->x()+static_cast<QWidget *>(o)->x(),
                             m->y()+static_cast<QWidget *>(o)->y() );
            QWidget *slider = 0;

            /* find slider that is under the mouse position */
            for ( QWidget *s = i_sliders.first(); s!=0; s=i_sliders.next() )
            {
                QRect sliderRect( s->pos(), s->size() );
                if ( sliderRect.contains(mousePos) ) slider = s;
            }

            /* the slider the mouse events instead of the visible widget */
            if ( slider )
            {
                QMouseEvent newMouseEvent( m->type(), mousePos-slider->pos(),
                                           m->globalPos(), m->button(), m->state() );

                i_eventSemaphore = true;
                bool ret = QApplication::sendEvent( slider, &newMouseEvent );
                i_eventSemaphore = false;
                return ret;
            }
        }

    return QWidget::eventFilter( o, e );    // standard event processing
}

bool KaimanStyle::loadPixmaps()
{
    QString l_s_tmpName;
    QPixmap *l_pixmap_Background = 0, *l_pixmap_Mask = 0;
    KaimanStyleElement *l_kse_elem;

    for ( uint l_i_elem = 0; l_i_elem<I_styleElem.count(); l_i_elem++) {
        l_kse_elem = I_styleElem[l_i_elem];

        l_s_tmpName =  locate("appdata", i_s_styleBase + l_kse_elem->filename );
        l_kse_elem->loadPixmaps(l_s_tmpName);
    }

    l_kse_elem = this->find("Background");
    if ( l_kse_elem != 0 ) {
        l_pixmap_Background = l_kse_elem->pixmaps[0];
    }
    l_kse_elem = this->find("Mask");
    if ( l_kse_elem != 0 ) {
        l_pixmap_Mask = l_kse_elem->pixmaps[0];
    }

    if ( (l_pixmap_Background != 0) && (l_pixmap_Mask != 0) ) {

        // OK, background and mask are defined. So now I can calculate the shape
        int l_i_width_Mask  = l_pixmap_Mask->width();
        int l_i_height_Mask = l_pixmap_Mask->height();

        QImage l_image_MaskOrig = l_pixmap_Mask->convertToImage();

        QImage l_image_Mask(l_i_width_Mask,l_i_height_Mask, 1, 2, QImage::LittleEndian);
        l_image_Mask.setColor( 0, 0x00ffffff );
        l_image_Mask.setColor( 1, 0 );
        l_image_Mask.fill( 0xff );

        uchar *l_c_pixel;
        uint l_qcol_white = qRgb(255,255,255);

        for (int l_i_x=0; l_i_x<l_i_width_Mask; l_i_x++) {
            for (int l_i_y=0; l_i_y < l_i_height_Mask; l_i_y++) {
                if ( ((*((QRgb*)  l_image_MaskOrig.scanLine(l_i_y)+l_i_x) & 0x00ffffff)) != (l_qcol_white & 0x00ffffff) ) {
                    l_c_pixel = (uchar *)l_image_Mask.scanLine(l_i_y);
                    *(l_c_pixel + (l_i_x>>3)   )  &= ~(1 << (l_i_x & 7));
                }
            }
        }

        i_bitmap_Mask.convertFromImage(l_image_Mask);
        //  l_pixmap_Background.setMask(i_bitmap_Mask);
    }

    return true;
}


QBitmap* KaimanStyle::Mask()
{
    return &i_bitmap_Mask;
}

KaimanStyleElement* KaimanStyle::find(const char *val_s_elemName)
{
    for (uint i=0; i< I_styleElem.count(); i++) {
        if ( I_styleElem[i]->element == QString(val_s_elemName)) {
            return I_styleElem[i];
        }
    }
    return 0;
}
#include "style.moc"

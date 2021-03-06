/* This file is part of the KDE libraries
    Copyright (C) 1999,2000,2001 Carsten Pfeiffer <pfeiffer@kde.org>

    library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


#include <sys/stat.h>
#include <unistd.h>

#include <qstring.h>
#include <qtooltip.h>
#include <qapplication.h>

#include <kaccel.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kdirselectdialog.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kurlcompletion.h>
#include <kurldrag.h>
#include <kprotocolinfo.h>

#include "kurlrequester.h"


class KURLDragPushButton : public KPushButton
{
public:
    KURLDragPushButton( QWidget *parent, const char *name=0 )
	: KPushButton( parent, name ) {
    	setDragEnabled( true );
    }
    ~KURLDragPushButton() {}

    void setURL( const KURL& url ) {
	m_urls.clear();
	m_urls.append( url );
    }

    /* not needed so far
    void setURLs( const KURL::List& urls ) {
	m_urls = urls;
    }
    const KURL::List& urls() const { return m_urls; }
    */

protected:
    virtual QDragObject *dragObject() {
	if ( m_urls.isEmpty() )
	    return 0L;

	QDragObject *drag = new KURLDrag( m_urls, this, "url drag" );
	return drag;
    }

private:
    KURL::List m_urls;

};


/*
*************************************************************************
*/

class KURLRequester::KURLRequesterPrivate
{
public:
    KURLRequesterPrivate() {
	edit = 0L;
	combo = 0L;
        fileDialogMode = KFile::File | KFile::ExistingOnly | KFile::LocalOnly;
    }

    void setText( const QString& text ) {
	if ( combo )
	{
	    if (combo->editable())
	    {
               combo->setEditText( text );
            }
            else
            {
               combo->insertItem( text );
               combo->setCurrentItem( combo->count()-1 );
            }
        }
	else
	{
	    edit->setText( text );
	}
    }

    void connectSignals( QObject *receiver ) {
	QObject *sender;
	if ( combo )
	    sender = combo;
	else
	    sender = edit;

	connect( sender, SIGNAL( textChanged( const QString& )),
		 receiver, SIGNAL( textChanged( const QString& )));
	connect( sender, SIGNAL( returnPressed() ),
		 receiver, SIGNAL( returnPressed() ));
	connect( sender, SIGNAL( returnPressed( const QString& ) ),
		 receiver, SIGNAL( returnPressed( const QString& ) ));
    }

    void setCompletionObject( KCompletion *comp ) {
	if ( combo )
	    combo->setCompletionObject( comp );
	else
	    edit->setCompletionObject( comp );
    }

    /**
     * replaces ~user or $FOO, if necessary
     */
    QString url() {
        QString txt = combo ? combo->currentText() : edit->text();
        KURLCompletion *comp;
        if ( combo )
            comp = dynamic_cast<KURLCompletion*>(combo->completionObject());
        else
            comp = dynamic_cast<KURLCompletion*>(edit->completionObject());

        if ( comp )
            return comp->replacedPath( txt );
        else
            return txt;
    }

    KLineEdit *edit;
    KComboBox *combo;
    int fileDialogMode;
    QString fileDialogFilter;
};



KURLRequester::KURLRequester( QWidget *editWidget, QWidget *parent,
			      const char *name )
  : QHBox( parent, name )
{
    d = new KURLRequesterPrivate;

    // must have this as parent
    editWidget->reparent( this, 0, QPoint(0,0) );
    d->edit = dynamic_cast<KLineEdit*>( editWidget );
    d->combo = dynamic_cast<KComboBox*>( editWidget );

    init();
}


KURLRequester::KURLRequester( QWidget *parent, const char *name )
  : QHBox( parent, name )
{
    d = new KURLRequesterPrivate;
    init();
}


KURLRequester::KURLRequester( const QString& url, QWidget *parent,
			      const char *name )
  : QHBox( parent, name )
{
    d = new KURLRequesterPrivate;
    init();
    setKURL( KURL::fromPathOrURL( url ) );
}


KURLRequester::~KURLRequester()
{
    delete myCompletion;
    delete myFileDialog;
    delete d;
}


void KURLRequester::init()
{
    myFileDialog    = 0L;
    myShowLocalProt = false;

    if ( !d->combo && !d->edit )
	d->edit = new KLineEdit( this, "line edit" );

    myButton = new KURLDragPushButton( this, "kfile button");
    QIconSet iconSet = SmallIconSet(QString::fromLatin1("fileopen"));
    QPixmap pixMap = iconSet.pixmap( QIconSet::Small, QIconSet::Normal );
    myButton->setIconSet( iconSet );
    myButton->setFixedSize( pixMap.width()+8, pixMap.height()+8 );
    QToolTip::add(myButton, i18n("Open file dialog"));

    connect( myButton, SIGNAL( pressed() ), SLOT( slotUpdateURL() ));

    setSpacing( KDialog::spacingHint() );

    QWidget *widget = d->combo ? (QWidget*) d->combo : (QWidget*) d->edit;
    widget->installEventFilter( this );
    setFocusProxy( widget );

    d->connectSignals( this );
    connect( myButton, SIGNAL( clicked() ), this, SLOT( slotOpenDialog() ));

    myCompletion = new KURLCompletion();
    d->setCompletionObject( myCompletion );

    KAccel *accel = new KAccel( this );
    accel->insert( KStdAccel::Open, this, SLOT( slotOpenDialog() ));
    accel->readSettings();
}


void KURLRequester::setURL( const QString& url )
{
    if ( myShowLocalProt )
    {
        d->setText( url );
    }
    else
    {
        // ### This code is broken (e.g. for paths with '#')
        if ( url.startsWith("file://") )
            d->setText( url.mid( 7 ) );
        else if ( url.startsWith("file:") )
            d->setText( url.mid( 5 ) );
        else
            d->setText( url );
    }
}

void KURLRequester::setKURL( const KURL& url )
{
    if ( myShowLocalProt )
        d->setText( url.url() );
    else
        d->setText( url.pathOrURL() );
}

void KURLRequester::setCaption( const QString& caption )
{
   QWidget::setCaption( caption );
   if (myFileDialog)
      myFileDialog->setCaption( caption );
}

QString KURLRequester::url() const
{
    return d->url();
}

void KURLRequester::slotOpenDialog()
{
    KURL newurl;
    if ( (d->fileDialogMode & KFile::Directory) && !(d->fileDialogMode & KFile::File) ||
         /* catch possible fileDialog()->setMode( KFile::Directory ) changes */
         (myFileDialog && ( (myFileDialog->mode() & KFile::Directory) &&
         (myFileDialog->mode() & (KFile::File | KFile::Files)) == 0 ) ) )
    {
        newurl = KDirSelectDialog::selectDirectory(url(), d->fileDialogMode & KFile::LocalOnly);
        if ( !newurl.isValid() )
        {
            return;
        }
    }
    else
    {
      emit openFileDialog( this );

      KFileDialog *dlg = fileDialog();
      if ( !d->url().isEmpty() ) {
          KURL u( url() );
          // If we won't be able to list it (e.g. http), then don't try :)
          if ( KProtocolInfo::supportsListing( u ) )
              dlg->setSelection( u.url() );
      }

      if ( dlg->exec() != QDialog::Accepted )
      {
          return;
      }

      newurl = dlg->selectedURL();
    }

    setKURL( newurl );
    emit urlSelected( d->url() );
}

void KURLRequester::setMode(uint mode)
{
    Q_ASSERT( (mode & KFile::Files) == 0 );
    d->fileDialogMode = mode;
    if ( (mode & KFile::Directory) && !(mode & KFile::File) )
        myCompletion->setMode( KURLCompletion::DirCompletion );

    if (myFileDialog)
       myFileDialog->setMode( d->fileDialogMode );
}

unsigned int KURLRequester::mode() const
{
    return d->fileDialogMode;
}

void KURLRequester::setFilter(const QString &filter)
{
    d->fileDialogFilter = filter;
    if (myFileDialog)
       myFileDialog->setFilter( d->fileDialogFilter );
}

QString KURLRequester::filter( ) const
{
    return d->fileDialogFilter;
}


KFileDialog * KURLRequester::fileDialog() const
{
    if ( !myFileDialog ) {
        QWidget *p = parentWidget();
        myFileDialog = new KFileDialog( QString::null, d->fileDialogFilter, p,
                                        "file dialog", true );

        myFileDialog->setMode( d->fileDialogMode );
        myFileDialog->setCaption( caption() );
    }

    return myFileDialog;
}


void KURLRequester::setShowLocalProtocol( bool b )
{
    if ( myShowLocalProt == b )
	return;

    myShowLocalProt = b;
    setKURL( url() );
}

void KURLRequester::clear()
{
    d->setText( QString::null );
}

KLineEdit * KURLRequester::lineEdit() const
{
    return d->edit;
}

KComboBox * KURLRequester::comboBox() const
{
    return d->combo;
}

void KURLRequester::slotUpdateURL()
{
    // bin compat, myButton is declared as QPushButton
    KURL u;
    u = KURL( KURL( QDir::currentDirPath() + '/' ), url() );
    (static_cast<KURLDragPushButton *>( myButton ))->setURL( u );
}

bool KURLRequester::eventFilter( QObject *obj, QEvent *ev )
{
    if ( ( d->edit == obj ) || ( d->combo == obj ) )
    {
        if (( ev->type() == QEvent::FocusIn ) || ( ev->type() == QEvent::FocusOut ))
            // Forward focusin/focusout events to the urlrequester; needed by file form element in khtml
            QApplication::sendEvent( this, ev );
    }
    return QWidget::eventFilter( obj, ev );
}

KPushButton * KURLRequester::button() const
{
    return myButton;
}

KEditListBox::CustomEditor KURLRequester::customEditor()
{
    setSizePolicy(QSizePolicy( QSizePolicy::Preferred,
                               QSizePolicy::Fixed));

    KLineEdit *edit = d->edit;
    if ( !edit && d->combo )
        edit = dynamic_cast<KLineEdit*>( d->combo->lineEdit() );

#ifndef NDEBUG
    if ( !edit )
        kdWarning() << "KURLRequester's lineedit is not a KLineEdit!??\n";
#endif

    KEditListBox::CustomEditor editor( this, edit );
    return editor;
}

void KURLRequester::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

KURLComboRequester::KURLComboRequester( QWidget *parent,
			      const char *name )
  : KURLRequester( new KComboBox(false), parent, name)
{
}

#include "kurlrequester.moc"

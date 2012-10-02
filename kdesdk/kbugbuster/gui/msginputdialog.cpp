// $Id: msginputdialog.cpp 302773 2004-04-11 09:05:54Z binner $
// (c) 2001, Cornelius Schumacher

#include <ktextedit.h>
#include <qlayout.h>

#include <klocale.h>
#include <kdebug.h>
#include <qcombobox.h>
#include <qsplitter.h>
#include <qlabel.h>

#include "messageeditor.h"
#include "kbbprefs.h"
#include "bugsystem.h"
#include "bugcommand.h"

#include "msginputdialog.h"
#include "msginputdialog.moc"

MsgInputDialog::MsgInputDialog(MsgInputDialog::MessageType type, const Bug &bug,
                               const Package &package, const QString &quotedMsg,
                               QWidget *parent)
  : KDialogBase(Plain,QString::null,User1|User2|Ok|Cancel,Ok,parent,0,false,
                true,KStdGuiItem::clear(),i18n( "&Edit Presets..." )),
    mBug( bug ),
    mPackage( package ),
    mType( type )
{
  switch ( mType ) {
    case Close:
      setCaption( i18n("Close Bug %1").arg( mBug.number() ) );
      break;
    case Reply:
      setCaption( i18n("Reply to Bug") );
      break;
    case ReplyPrivate:
      setCaption( i18n("Reply Privately to Bug") );
      break;
    default:
      break;
  }

  QFrame *topFrame = plainPage();
  ( new QHBoxLayout( topFrame ) )->setAutoAdd( true );

  mSplitter = new QSplitter( QSplitter::Horizontal, topFrame );

  QWidget *w = new QWidget( mSplitter );
  ( new QVBoxLayout( w, spacingHint(), -1 ) )->setAutoAdd( true );

  if ( mType == Reply ) {
    QWidget *r = new QWidget( w );
    QHBoxLayout* rlayout = new QHBoxLayout( r );

    QLabel *rlabel = new QLabel( i18n("&Recipient:"),r );
    QFont f = r->font();
    f.setBold( true );
    r->setFont( f );
    rlayout->add( rlabel );

    mRecipient = new QComboBox( r );
    mRecipient->insertItem( i18n("Normal (bugs.kde.org & Maintainer & kde-bugs-dist)"), BugCommand::Normal );
    mRecipient->insertItem( i18n("Maintonly (bugs.kde.org & Maintainer)"), BugCommand::Maintonly );
    mRecipient->insertItem( i18n("Quiet (bugs.kde.org only)"), BugCommand::Quiet );
    rlabel->setBuddy( mRecipient );
    rlayout->add( mRecipient );

    QSpacerItem *rspacer= new QSpacerItem( 1,1,QSizePolicy::Expanding );
    rlayout->addItem( rspacer );

    // Reply currently only replies to the bug tracking system
    r->hide();
  }


  QLabel *l = new QLabel( i18n( "&Message" ), w );
  QFont f = l->font();
  f.setBold( true );
  l->setFont( f );

  mMessageEdit = new KTextEdit( w );
  mMessageEdit->setMinimumWidth( mMessageEdit->fontMetrics().width('x') * 72 );
  mMessageEdit->setWordWrap( QTextEdit::FixedColumnWidth );
  mMessageEdit->setWrapColumnOrWidth( 72 );
  l->setBuddy( mMessageEdit );

  w = new QWidget( mSplitter );
  ( new QVBoxLayout( w, spacingHint(), -1 ) )->setAutoAdd( true );
  l = new QLabel( i18n( "&Preset Messages" ), w );
  l->setFont( f );

  mPresets = new KListBox( w );
  updatePresets();
  l->setBuddy( mPresets );

  connect( mPresets, SIGNAL( executed( QListBoxItem* ) ),
           SLOT( slotPresetSelected( QListBoxItem * ) ) );
  connect( this, SIGNAL( user2Clicked() ), SLOT( editPresets() ) );
  connect( this, SIGNAL( user1Clicked() ), SLOT( clearMessage() ) );
  mMessageEdit->setFocus();

  if ( !quotedMsg.isEmpty() )
    insertQuotedMessage( quotedMsg );

  readConfig();
}

MsgInputDialog::~MsgInputDialog()
{
  kdDebug() << "MsgInputDialog::~MsgInputDialog()" << endl;
  writeConfig();
}

void MsgInputDialog::readConfig()
{
  resize( KBBPrefs::instance()->mMsgDlgWidth,
          KBBPrefs::instance()->mMsgDlgHeight );
  QValueList<int> sizes = KBBPrefs::instance()->mMsgDlgSplitter;
  mSplitter->setSizes( sizes );
}

void MsgInputDialog::writeConfig()
{
  KBBPrefs::instance()->mMsgDlgWidth = width();
  KBBPrefs::instance()->mMsgDlgHeight = height();
  KBBPrefs::instance()->mMsgDlgSplitter = mSplitter->sizes();
}

void MsgInputDialog::updatePresets()
{
  mPresets->clear();

  QMap<QString,QString> messageButtons = KBBPrefs::instance()->mMessageButtons;

  int id = 0;
  QMap<QString,QString>::ConstIterator it;
  for( it = messageButtons.begin(); it != messageButtons.end(); ++it )
    mPresets->insertItem( it.key(), id );
}

QString MsgInputDialog::message() const
{
  return mMessageEdit->text();
}

void MsgInputDialog::editPresets()
{
  MessageEditor *dlg = new MessageEditor(this);
  dlg->exec();
  delete dlg;

  updatePresets();
}

void MsgInputDialog::slotPresetSelected( QListBoxItem *lbi )
{
  mMessageEdit->setText( KBBPrefs::instance()->mMessageButtons[ lbi->text() ] );
}

void MsgInputDialog::clearMessage()
{
  mMessageEdit->setText("");
}

void MsgInputDialog::queueCommand()
{
  switch ( mType ) {
    case Close:
      BugSystem::self()->queueCommand(
          new BugCommandClose( mBug, message(), mPackage ) );
      break;
    case Reply:
      BugSystem::self()->queueCommand(
          new BugCommandReply( mBug, message(), mRecipient->currentItem() ) );
      break;
    case ReplyPrivate:
      BugSystem::self()->queueCommand(
          new BugCommandReplyPrivate( mBug, mBug.submitter().email,
	                              message() ) );
      break;
    default:
      break;
  }
}

void MsgInputDialog::slotOk()
{
  queueCommand();
  delete this;
}

void MsgInputDialog::slotCancel()
{
  delete this;
}

void MsgInputDialog::insertQuotedMessage( const QString &msg )
{
	Q_ASSERT( mMessageEdit->wordWrap() == QTextEdit::FixedColumnWidth );

	const QString quotationMarker = "> ";
	const unsigned int wrapColumn = mMessageEdit->wrapColumnOrWidth();

	// ### Needs something more sophisticated than simplifyWhiteSpace to
	// handle quoting multiple paragraphs properly.
	QString line = msg.simplifyWhiteSpace();

	QString quotedMsg;
	while ( line.length() + quotationMarker.length() + 1 > wrapColumn ) {
		int pos = wrapColumn - quotationMarker.length() - 1;
		while ( pos > 0 && !line[ pos ].isSpace() )
			--pos;
		if ( pos == 0 )
			pos = wrapColumn;
		quotedMsg += quotationMarker + line.left( pos ) + "\n";
		line = line.mid( pos + 1 );
	}
	quotedMsg += quotationMarker + line + "\n\n";

	mMessageEdit->setText( quotedMsg );

	const int lastPara = mMessageEdit->paragraphs() - 1;
	const int lastParaLen = mMessageEdit->paragraphLength( lastPara ) - 1;
	mMessageEdit->setCursorPosition( lastPara, lastParaLen );
}

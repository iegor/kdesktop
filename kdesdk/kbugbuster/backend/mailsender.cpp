#ifndef QT_NO_ASCII_CAST
#define QT_NO_ASCII_CAST
#endif

#include <unistd.h>
#include <stdio.h>

#include <qtimer.h>

#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kapplication.h>
#include <dcopclient.h>
#include <kprocess.h>

#include "mailsender.h"
#include "smtp.h"

MailSender::MailSender(MailClient client,const QString &smtpServer) :
  m_client( client ), m_smtpServer( smtpServer )
{
}

MailSender::~MailSender()
{
}

MailSender *MailSender::clone() const
{
  return new MailSender(m_client,m_smtpServer);
}

bool MailSender::send(const QString &fromName,const QString &fromEmail,const QString &to,
                      const QString &subject,const QString &body,bool bcc,
                      const QString &recipient)
{
  QString from( fromName );
  if ( !fromEmail.isEmpty() )
    from += QString::fromLatin1( " <%2>" ).arg( fromEmail );
  kdDebug() << "MailSender::sendMail():\nFrom: " << from << "\nTo: " << to
            << "\nbccflag:" << bcc
            << "\nRecipient:" << recipient
            << "\nSubject: " << subject << "\nBody: \n" << body << endl;

  // ### FIXME: bcc is not supported in direct mode and recipient is not
  // supported in sendmail and kmail mode

  if (m_client == Sendmail) {
    kdDebug() << "Sending per Sendmail" << endl;

    bool needHeaders = true;

    QString command = KStandardDirs::findExe(QString::fromLatin1("sendmail"),
        QString::fromLatin1("/sbin:/usr/sbin:/usr/lib"));
    if (!command.isNull()) command += QString::fromLatin1(" -oi -t");
    else {
      command = KStandardDirs::findExe(QString::fromLatin1("mail"));
      if (command.isNull()) return false; // give up

      command.append(QString::fromLatin1(" -s "));
      command.append(KProcess::quote(subject));

      if (bcc) {
        command.append(QString::fromLatin1(" -b "));
        command.append(KProcess::quote(from));
      }

      command.append(" ");
      command.append(KProcess::quote(to));

      needHeaders = false;
    }

    FILE * fd = popen(command.local8Bit(),"w");
    if (!fd)
    {
      kdError() << "Unable to open a pipe to " << command << endl;
      QTimer::singleShot( 0, this, SLOT( deleteLater() ) );
      return false;
    }

    QString textComplete;
    if (needHeaders)
    {
      textComplete += QString::fromLatin1("From: ") + from + '\n';
      textComplete += QString::fromLatin1("To: ") + to + '\n';
      if (bcc) textComplete += QString::fromLatin1("Bcc: ") + from + '\n';
      textComplete += QString::fromLatin1("Subject: ") + subject + '\n';
      textComplete += QString::fromLatin1("X-Mailer: KBugBuster") + '\n';
    }
    textComplete += '\n'; // end of headers
    textComplete += body;

    emit status( i18n( "Sending through sendmail..." ) );
    fwrite(textComplete.local8Bit(),textComplete.length(),1,fd);

    pclose(fd);
  } else if ( m_client == KMail ) {
    kdDebug() << "Sending per KMail" << endl;

    if (!kapp->dcopClient()->isApplicationRegistered("kmail")) {
      KMessageBox::error(0,i18n("No running instance of KMail found."));
      QTimer::singleShot( 0, this, SLOT( deleteLater() ) );
      return false;
    }

    emit status( i18n( "Passing mail to KDE email program..." ) );
    if (!kMailOpenComposer(to,"", (bcc ? from : ""), subject,body,0,KURL())) {
      QTimer::singleShot( 0, this, SLOT( deleteLater() ) );
      return false;
    }
  } else if ( m_client == Direct ) {
    kdDebug() << "Sending Direct" << endl;

    QStringList recipients;
    if ( !recipient.isEmpty() )
        recipients << recipient;
    else
        recipients << to;

    QString message = QString::fromLatin1( "From: " ) + from +
                      QString::fromLatin1( "\nTo: " ) + to +
                      QString::fromLatin1( "\nSubject: " ) + subject +
                      QString::fromLatin1( "\nX-Mailer: KBugBuster" ) +
                      QString::fromLatin1( "\n\n" ) + body;

    Smtp *smtp = new Smtp( fromEmail, recipients, message, m_smtpServer );
    connect( smtp, SIGNAL( status( const QString & ) ),
             this, SIGNAL( status( const QString & ) ) );
    connect( smtp, SIGNAL( success() ),
             this, SLOT( smtpSuccess() ) );
    connect( smtp, SIGNAL( error( const QString &, const QString & ) ),
             this, SLOT( smtpError( const QString &, const QString & ) ) );

    smtp->insertChild( this ); // die when smtp dies
  } else {
    kdDebug() << "Invalid mail client setting." << endl;
  }

  if (m_client != Direct)
  {
    emit finished();
    QTimer::singleShot( 0, this, SLOT( deleteLater() ) );
  }

  return true;
}

void MailSender::smtpSuccess()
{
  if ( parent() != sender() || !parent()->inherits( "Smtp" ) )
    return;

  static_cast<Smtp *>( parent() )->quit();
  emit finished();
}

void MailSender::smtpError(const QString &_command, const QString &_response)
{
  if ( parent() != sender() || !parent()->inherits( "Smtp" ) )
    return;

  QString command = _command;
  QString response = _response;

  Smtp *smtp = static_cast<Smtp *>( parent() );
  smtp->removeChild( this );
  delete smtp;

  KMessageBox::error( qApp->activeWindow(),
                      i18n( "Error during SMTP transfer.\n"
                            "command: %1\n"
                            "response: %2" ).arg( command ).arg( response ) );

  emit finished();
  QTimer::singleShot( 0, this, SLOT( deleteLater() ) );
}

int MailSender::kMailOpenComposer(const QString& arg0,const QString& arg1,
  const QString& arg2,const QString& arg3,const QString& arg4,int arg5,
  const KURL& arg6)
{
  int result = 0;

  QByteArray data, replyData;
  QCString replyType;
  QDataStream arg( data, IO_WriteOnly );
  arg << arg0;
  arg << arg1;
  arg << arg2;
  arg << arg3;
  arg << arg4;
  arg << arg5;
  arg << arg6;
  if (kapp->dcopClient()->call("kmail","KMailIface","openComposer(QString,QString,QString,QString,QString,int,KURL)", data, replyType, replyData ) ) {
    if ( replyType == "int" ) {
      QDataStream _reply_stream( replyData, IO_ReadOnly );
      _reply_stream >> result;
    } else {
      kdDebug() << "kMailOpenComposer() call failed." << endl;
    }
  } else {
    kdDebug() << "kMailOpenComposer() call failed." << endl;
  }
  return result;
}

#include "mailsender.moc"


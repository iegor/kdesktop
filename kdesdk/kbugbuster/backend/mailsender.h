#ifndef MAILSENDER_H
#define MAILSENDER_H

#include <qstring.h>
#include <qobject.h>

class KURL;
class Smtp;

class MailSender : public QObject
{
  Q_OBJECT
  public:
    enum MailClient { Sendmail = 0, KMail = 1, Direct = 2 };
  
    MailSender(MailClient,const QString &smtpServer=QString::null);
    virtual ~MailSender();

    MailSender *clone() const;

    /**
      Send mail with specified from, to and subject field and body as text. If
      bcc is set, send a blind carbon copy to the sender from.
      If recipient is specified the mail is sent to the specified address
      instead of 'to' . (this currently only works in for direct mail
      sending through SMTP.
    */
    bool send(const QString &fromName, const QString &fromEmail,
              const QString &to,const QString &subject,
              const QString &body,bool bcc=false, 
              const QString &recipient = QString::null);

  signals:
    void status( const QString &message );
    void finished();

  private slots:
    void smtpSuccess();
    void smtpError(const QString &command, const QString &response);

  private:
    int kMailOpenComposer(const QString& arg0,const QString& arg1,
                          const QString& arg2,const QString& arg3,
                          const QString& arg4,int arg5,const KURL& arg6);

    MailClient m_client;
    QString m_smtpServer;
};

#endif

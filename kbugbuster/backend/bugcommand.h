#ifndef BUGCOMMAND_H
#define BUGCOMMAND_H

#include <qstring.h>
#include <qstringlist.h>

#include "bug.h"
#include "package.h"

class KConfig;

class BugCommand {
  public:
    enum Mode  { Normal, Maintonly, Quiet };
    enum State { None, Queued, Sent };

    BugCommand( const Bug &bug ) : m_bug( bug ) {}
    BugCommand( const Bug &bug, const Package &pkg ) : m_bug( bug ), m_package( pkg ) {}
    virtual ~BugCommand() {}

    virtual QString controlString() const { return QString::null; }

    virtual QString mailAddress() const { return QString::null; }
    virtual QString mailText() const { return QString::null; }

    Bug bug() const { return m_bug; }
    Package package() const { return m_package; }

    virtual QString name();
    virtual QString details();

    virtual QString type() const { return QString::null; }

    virtual void save( KConfig * ) = 0;
    static BugCommand *load( KConfig *, const QString &type );

  protected:
    Bug m_bug;
    Package m_package;
};

class BugCommandClose : public BugCommand {
  public:
    BugCommandClose( const Bug &bug, const QString &message, const Package &pkg )  :
        BugCommand( bug, pkg ), m_message( message ) {}

    QString controlString() const;
    QString mailAddress() const;
    QString mailText() const;

    QString name();
    QString details() const;

    QString type() const { return QString::fromLatin1("Close"); }

    void save( KConfig * );

  private:
    QString m_message;
};

class BugCommandCloseSilently : public BugCommand {
  public:
    BugCommandCloseSilently( const Bug &bug, const Package &pkg )  :
        BugCommand( bug, pkg ) {}

    QString controlString() const;

    QString name();

    QString type() const { return QString::fromLatin1("CloseSilently"); }

    void save( KConfig * );
};

class BugCommandReopen : public BugCommand {
  public:
    BugCommandReopen( const Bug &bug, const Package &pkg )  :
        BugCommand( bug, pkg ) {}

    QString controlString() const;

    QString name();

    QString type() const { return QString::fromLatin1("Reopen"); }

    void save( KConfig * );
};

class BugCommandRetitle : public BugCommand {
  public:
    BugCommandRetitle( const Bug &bug, const QString &title, const Package &pkg )  :
        BugCommand( bug, pkg ), m_title( title ) {}

    QString controlString() const;

    QString name();
    QString details() const;

    QString type() const { return QString::fromLatin1("Retitle"); }

    void save( KConfig * );

  private:
    QString m_title;
};

class BugCommandMerge : public BugCommand {
  public:
    BugCommandMerge( const QStringList &bugNumbers, const Package &pkg )  :
        BugCommand( Bug(), pkg ), m_bugNumbers( bugNumbers ) {}

    QString controlString() const;

    QString name();
    QString details() const;

    QString type() const { return QString::fromLatin1("Merge"); }

    void save( KConfig * );

  private:
    QStringList m_bugNumbers;
};

class BugCommandUnmerge : public BugCommand {
  public:
    BugCommandUnmerge( const Bug &bug, const Package &pkg )  :
        BugCommand( bug, pkg ) {}

    QString name();

    QString type() const { return QString::fromLatin1("Unmerge"); }

    void save( KConfig * );

    QString controlString() const;
};

class BugCommandReply : public BugCommand {
  public:
    BugCommandReply( const Bug &bug, const QString &message, const int &recipient) :
        BugCommand( bug ), m_message( message ), m_recipient( recipient ) {}

    QString mailAddress() const;
    QString mailText() const;

    QString name();
    QString details() const;

    QString type() const { return QString::fromLatin1("Reply"); }

    void save( KConfig * );

  private:
    QString m_message;
    int m_recipient;
};

class BugCommandReplyPrivate : public BugCommand {
  public:
    BugCommandReplyPrivate( const Bug &bug, const QString &address,
                            const QString &message ) :
         BugCommand( bug ), m_address( address ), m_message( message ) {}

    QString mailAddress() const;
    QString mailText() const;

    QString name();
    QString details() const;

    QString type() const { return QString::fromLatin1("ReplyPrivate"); }

    void save( KConfig * );

  private:
    QString m_address;
    QString m_message;
};

class BugCommandSeverity : public BugCommand {
  public:
    BugCommandSeverity( const Bug &bug, const QString &severity, const Package &pkg ) :
        BugCommand( bug, pkg ), m_severity( severity ) {}

    QString name();
    QString details() const;

    QString type() const { return QString::fromLatin1("Severity"); }

    QString controlString() const;

    void save( KConfig * );

  private:
    QString m_severity;
};

class BugCommandReassign : public BugCommand {
  public:
    BugCommandReassign( const Bug &bug, const QString &package, const Package &pkg ) :
        BugCommand( bug, pkg ), m_package( package ) {}

    QString name();
    QString details() const;

    QString type() const { return QString::fromLatin1("Reassign"); }

    QString controlString() const;

    void save( KConfig * );

  private:
    QString m_package;
};

#endif

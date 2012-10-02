#ifndef SERVERCONFIGDIALOG_H
#define SERVERCONFIGDIALOG_H

#include <kdialogbase.h>

class BugServerConfig;
class QLineEdit;
class KPasswordEdit;
class QComboBox;

class ServerConfigDialog : public KDialogBase
{
    Q_OBJECT
  public:
    ServerConfigDialog( QWidget *parent = 0 , const char *name = 0 );

    void setServerConfig( const BugServerConfig & );
    BugServerConfig serverConfig();
        
  private:
    QLineEdit *mServerName;
    QLineEdit *mServerUrl;
    QLineEdit *mUser;
    KPasswordEdit *mPassword;
    QComboBox *mVersion;
};

#endif

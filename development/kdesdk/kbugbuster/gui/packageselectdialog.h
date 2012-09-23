#ifndef PACKAGESELECTDIALOG_H
#define PACKAGESELECTDIALOG_H

#include <qdict.h>

#include <kdialogbase.h>

#include "package.h"

class KCompletion;
class KLineEdit;

class PackageListView : public QListView
{
    Q_OBJECT
  public:
    PackageListView( QWidget *parent );

    void resetTyped();

  signals:
    void typed( const QString & );

  protected:
    void keyPressEvent( QKeyEvent *e );
    
  private:
    QString mTyped;
};

class PackageSelectDialog : public KDialogBase
{
    Q_OBJECT
  public:
    PackageSelectDialog(QWidget *parent=0,const char *name=0);
    ~PackageSelectDialog();

    void setRecentPackages( const QStringList & );
    void setPackages( const Package::List &pkgs );

    Package selectedPackage();
    QString selectedComponent();

  protected slots:
    void slotOk();

  private slots:
    void recentSelected( int, QListViewItem * );
    void completeSelected( int, QListViewItem * );
    void completeTyped( const QString & );

  private:
    Package::List mPackages;
    Package mSelectedPackage;
    QString mSelectedComponent;
    
    QListView *mRecentList;
    PackageListView *mCompleteList;
    KLineEdit *mPackageEdit;
    KCompletion *mCompletion;
    QDict<QListViewItem> mCompletionDict;
};

#endif

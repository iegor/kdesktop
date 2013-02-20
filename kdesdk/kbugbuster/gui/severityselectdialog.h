#ifndef SEVERITYSELECTDIALOG_H
#define SEVERITYSELECTDIALOG_H

#include <kdialogbase.h>

#include "bug.h"

class SeveritySelectDialog : public KDialogBase
{
    Q_OBJECT
  public:
    SeveritySelectDialog(QWidget *parent=0,const char *name=0);

    void setSeverity( Bug::Severity );
        
    Bug::Severity selectedSeverity();
    QString selectedSeverityAsString();

  private:
    QButtonGroup *mButtonGroup;
};

#endif

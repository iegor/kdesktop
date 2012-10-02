#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <kdebug.h>

#include "bugsystem.h"
#include "kbbprefs.h"

#include "severityselectdialog.h"
#include "severityselectdialog.moc"

SeveritySelectDialog::SeveritySelectDialog(QWidget *parent,const char *name) :
  KDialogBase( parent, name, true, i18n("Select Severity"), Ok|Cancel )
{
    mButtonGroup = new QButtonGroup( 1, Horizontal, i18n("Severity"), this );
    setMainWidget( mButtonGroup );

    QValueList<Bug::Severity> severities = Bug::severities();
    QValueList<Bug::Severity>::ConstIterator it;
    for( it = severities.begin(); it != severities.end(); ++it ) {
        mButtonGroup->insert(
            new QRadioButton( Bug::severityToString( *it ), mButtonGroup ), int(*it) );
    }
}

void SeveritySelectDialog::setSeverity( Bug::Severity s )
{
    mButtonGroup->setButton( s );
}

Bug::Severity SeveritySelectDialog::selectedSeverity()
{
    return (Bug::Severity)mButtonGroup->id( mButtonGroup->selected() );
}

QString SeveritySelectDialog::selectedSeverityAsString()
{
    return Bug::severityToString( selectedSeverity() );
}

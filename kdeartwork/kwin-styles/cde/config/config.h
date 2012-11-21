#ifndef __KDE_CDECONFIG_H
#define __KDE_CDECONFIG_H

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qhbox.h>
#include <kconfig.h>

class QCheckBox;
class QGroupBox;
class QVBox;
class QLabel;
class QRadioButton;

class CdeConfig: public QObject
{
	Q_OBJECT

	public:
		CdeConfig( KConfig* conf, QWidget* parent );
		~CdeConfig();

	// These public signals/slots work similar to KCM modules
	signals:
		void changed();

	public slots:
		void load( KConfig* conf );	
		void save( KConfig* conf );
		void defaults();

	protected slots:
		void slotSelectionChanged();	// Internal use
		void slotSelectionChanged( int );
		
	private:
		KConfig*   	cdeConfig;
		QCheckBox* 	cbColorBorder;
//		QCheckBox* 	cbTitlebarButton;
		QHBox* 	        groupBox;
		QGroupBox* 	gbSlider;
		QButtonGroup*	bgAlign;
};


#endif

// vim: ts=4

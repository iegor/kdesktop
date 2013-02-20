#ifndef VIEWCONTROL_H
#define VIEWCONTROL_H

#include <qdialog.h>
class QComboBox;
class QPushButton;


class ViewControl : public QDialog
{
	Q_OBJECT
public:
	ViewControl( QWidget *parent, const char *name );
	QComboBox* magComboBox;
	QComboBox* mediaComboBox;
	QComboBox* orientComboBox;
	QPushButton *apply;

	/**
	 * Update the mag combo box.
	 **/
	void updateMag (int mag);

	/**
	 * Enable/disable the apply button.
	 **/
	void applyEnable (bool enable);

protected:
        int prevmag, prevmedia, prevorient;

public slots:
	void slotApplyClicked();
	void slotMagSelection (int i);
	void slotMediaSelection (int i);
	void slotOrientSelection (int i);

signals:
	void applyChanges();
};

#endif

// vim:sw=4:sts=4:ts=8:noet

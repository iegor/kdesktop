#ifndef KJBUTTON_H
#define KJBUTTON_H

#include "kjwidget.h"
#include <qobject.h>
class KJLoader;

class KJButton : public QObject, public KJWidget
{
Q_OBJECT
public:
	KJButton(const QStringList&, KJLoader *);

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &pos, bool);
	virtual void showPressed(bool b=true);

	virtual QString tip();

private slots:
	void slotPlaylistShown(void);
	void slotPlaylistHidden(void);
	void slotEqEnabled(bool on);

private:
	QPixmap mBackground;
	bool mPushedPixmap;
	QPixmap mPressed;
	QString mTitle;
	bool mShowPressed;
};

#endif

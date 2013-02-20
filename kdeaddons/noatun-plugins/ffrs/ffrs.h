#ifndef FFRS_H
#define FFRS_H

#include <noatun/pref.h>
#include <noatun/plugin.h>


class View : public QWidget
{
Q_OBJECT
public:
	View(int width, int height, int block, int unblock, QColor front, QColor back, int channel);
	~View();

	void draw(float intensity);

    virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

private:
	int units;
	QColor fg, bg;
	bool moving;
	QPoint mMousePoint;
	int mChannel;
};

class FFRSPrefs;

class FFRS : public QObject, public Plugin, public StereoScope
{
Q_OBJECT

public:
	FFRS();
	~FFRS();

	virtual void scopeEvent(float *left, float *right, int len);

public slots:
	void changed();

private:
	View *dpyleft, *dpyright;
	FFRSPrefs *prefs;
};


class KIntNumInput;
class KColorButton;

class FFRSPrefs : public CModule
{
Q_OBJECT

public:
	FFRSPrefs( QObject *parent );
	virtual void save();

	int width();
	int height();
	int fgblock();
	int bgblock();
	int rate();
	
	QColor bgcolor();
	QColor fgcolor();

signals:
	void changed();

private:
	KIntNumInput *mWidth, *mHeight, *mFgblock, *mBgblock, *mRate;
	KColorButton *mBgcolor, *mFgcolor;
};


#endif


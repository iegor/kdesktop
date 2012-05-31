#ifndef KJTEXTDISPLAY_H
#define KJTEXTDISPLAY_H

#include "kjwidget.h"
class KJLoader;
class KPixmap;
//#include "kjloader.h"

#include <qobject.h>
#include <qpainter.h>

class KJFilename : public QObject, public KJWidget
{
Q_OBJECT
public:
	KJFilename(const QStringList &, KJLoader *parent);
	~KJFilename();

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &, bool in);
//	virtual void newFile();
	virtual void timeUpdate(int);
	virtual void readConfig();

	void prepareString(const QCString &str);
	virtual QString tip();

	virtual void timerEvent(QTimerEvent *);

private:
	QCString mLastTitle;
	int mDistance;
	int mTimerUpdates;
	int mWidth;
	int mTickerPos;
	QPixmap mView;
	KPixmap *mBack;
};


class KJTime : public KJWidget
{
public:
	KJTime(const QStringList &, KJLoader *parent);
	~KJTime();

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &, bool in);
	virtual void timeUpdate(int);
	virtual void readConfig();

	void prepareString(const QCString &time);
	virtual QString tip();

//	enum countModes { Up=0, Down };

private:
	QCString mLastTime;
	int mWidth;
	bool countDown;
	QPixmap mTime;
	KPixmap *mBack;

private:
	QString lengthString ( void );

};


class KJVolumeText : public KJWidget
{
public:
	KJVolumeText(const QStringList &, KJLoader *parent);
	~KJVolumeText();

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void timeUpdate(int);
	virtual void readConfig();

	void prepareString(const QCString &time);
	virtual QString tip();

private:
	QCString mLastVolume;
	int mWidth;
	QPixmap mVolume;
	KPixmap *mBack;
};


class KJPitchText : public KJWidget
{
public:
	KJPitchText(const QStringList &, KJLoader *parent);
	~KJPitchText();

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &, bool in);
	virtual void timeUpdate(int);
	virtual void readConfig();

	void prepareString(const QCString &time);
	virtual QString tip();

private:
	QCString mLastPitch;
	int mWidth;
	QPixmap mSpeed;
	KPixmap *mBack;
};


class KJFileInfo : public KJWidget
{
public:
	KJFileInfo(const QStringList &, KJLoader *parent);
	~KJFileInfo();

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void timeUpdate(int);
	virtual void readConfig();

	void prepareString(const QCString &time);
	virtual QString tip();

private:
	QCString mLastTime;
	QString mInfoType;
	int mWidth;
	QPixmap mTime;
	KPixmap *mBack;
};

#endif

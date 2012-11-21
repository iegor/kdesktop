// $Id: kclock.h 277841 2004-01-08 14:12:37Z mlaurent $
//
// kclock - Clock screen saver for KDE
// Copyright (c) 2003   Melchior FRANZ
//
// License:		GPL v2
// Author:		Melchior FRANZ  <mfranz@kde.org>
// Dependencies:	libart_lgpl_2   http://www.levien.com/libart/
//
#ifndef __KCLOCK_H__
#define __KCLOCK_H__

#include <qtimer.h>
#include <kdialogbase.h>
#include <kscreensaver.h>


class KClockPainter
{
	int m_width;
	int m_height;
	Q_UINT8 *m_buf;
	double m_matrix[6];
	Q_UINT32 m_color;
	Q_UINT32 m_shadow;

    public:
	KClockPainter(int width, int height);
	~KClockPainter();
	void copy(KClockPainter *p);
	void drawToImage(QImage *q, int x, int y);
	inline int width() { return m_width; }
	inline int height() { return m_height; }
	inline void *image() { return (void *)m_buf; }
	void setColor(const QColor &color);
	void setShadowColor(const QColor &color);
	void fill(const QColor &color);
	void drawRadial(double alpha, double r0, double r1, double width);
	void drawDisc(double radius);
	void drawHand(const QColor &color, double angle, double length,
			double width, bool disc);
};


class KClockSaver : public KScreenSaver
{
    Q_OBJECT
    public:
	KClockSaver(WId id);
	virtual ~KClockSaver();
	inline void setBgndColor(const QColor &c) { m_bgndColor = c; drawScale(); setBackgroundColor(c); };
	inline void setScaleColor(const QColor &c) { m_scaleColor = c; drawScale(); };
	inline void setHourColor(const QColor &c) { m_hourColor = c; forceRedraw(); };
	inline void setMinColor(const QColor &c) { m_minColor = c; forceRedraw(); };
	inline void setSecColor(const QColor &c) { m_secColor = c; forceRedraw(); };
	void setKeepCentered(bool b);
	void restart(int siz);
	inline void forceRedraw() { m_second = -1; }

    private:
	void readSettings();
	void drawScale();
	void drawClock();
	void start(int size);
	void stop();

    protected slots:
	void slotTimeout();

    protected:
	QTimer m_timer;
	QImage *m_image;
	KClockPainter *m_scale;
	KClockPainter *m_clock;

	int m_x;
	int m_y;
	int m_diameter;
	int m_size;
	bool m_showSecond;
	bool m_keepCentered;
	int m_hour;
	int m_minute;
	int m_second;

	QColor m_bgndColor;
	QColor m_scaleColor;
	QColor m_hourColor;
	QColor m_minColor;
	QColor m_secColor;
};


class KClockSetup : public KDialogBase
{
    Q_OBJECT
    public:
	 KClockSetup(QWidget *parent = 0, const char *name = 0);
    ~KClockSetup();
    protected:
	void readSettings();

    private slots:
	void slotOk();
	void slotHelp();

	void slotBgndColor(const QColor &);
	void slotScaleColor(const QColor &);
	void slotHourColor(const QColor &);
	void slotMinColor(const QColor &);
	void slotSecColor(const QColor &);
	void slotSliderMoved(int);
	void slotKeepCenteredChanged(int);

    private:
	KClockSaver *m_saver;

	QColor m_bgndColor;
	QColor m_scaleColor;
	QColor m_hourColor;
	QColor m_minColor;
	QColor m_secColor;

	int m_size;
	bool m_keepCentered;
};

#endif



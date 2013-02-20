/***************************************************************************
                          glowbutton.h  -  description
                             -------------------
    begin                : Thu Sep 14 2001
    copyright            : (C) 2001 by Henning Burchardt
    email                : h_burchardt@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GLOW_BUTTON_H
#define GLOW_BUTTON_H

#include <vector>
#include <qmap.h>
#include <qbutton.h>

class QPixmap;
class QBitmap;
class QTimer;
class QString;

namespace Glow
{

class PixmapCache
{
public:
	static const QPixmap* find(const QString& key);
	static void insert(const QString& key, const QPixmap *pixmap);
	static void erase(const QString& key);
	static void clear();
private:
	static QMap<QString, const QPixmap*> m_pixmapMap;
};


//-----------------------------------------------------------------------------

class GlowButton : public QButton
{
	Q_OBJECT

public:
	GlowButton(QWidget *parent, const char* name, const QString& tip, const int realizeBtns);
	~GlowButton();

	void setTipText( const QString& tip );

	QString getPixmapName() const;
	ButtonState lastButton() const;

	/** Sets the name of the pixmap in the pixmap cache.
	 * If no background pixmap is wanted use QString::null as name. */
	void setPixmapName(const QString& pixmapName);

protected:
	virtual void paintEvent( QPaintEvent * );
	virtual void enterEvent( QEvent * );
	virtual void leaveEvent( QEvent * );
	virtual void mousePressEvent( QMouseEvent * );
	virtual void mouseReleaseEvent( QMouseEvent * );

protected slots:
	void slotTimeout();

private:
	enum TimerStatus { Run, Stop };

	int m_updateTime;
	int _steps;
	QString m_pixmapName;

	QTimer *m_timer;
	int m_pos;
	TimerStatus m_timerStatus;

	int m_realizeButtons;
	ButtonState _last_button;
};

//-----------------------------------------------------------------------------

class GlowButtonFactory
{
public:
	GlowButtonFactory();

	int getSteps();

	/**
	 * Sets the number of pixmaps used to create the glow effect of the
	 * glow buttons.
	 */
	void setSteps(int steps);

	/**
	 * Creates a background pixmap for a glow button.
	 * The pixmap will consist of sub pixmaps of the size of the button which
	 * are placed one below the other. Each sub pixmap is copied on the button
	 * in succession to create the glow effect. The last sub pixmap is used
	 * when the button is pressed.
	 */
	QPixmap * createGlowButtonPixmap(
				const QImage & bg_image,
//				const QImage & bg_alpha_image,
				const QImage & fg_image,
				const QImage & glow_image,
				const QColor & color,
				const QColor & glow_color);

	GlowButton* createGlowButton(
		QWidget *parent, const char* name, const QString& tip, const int realizeBtns = Qt::LeftButton);

private:
	int _steps;
};

} // namespace

#endif

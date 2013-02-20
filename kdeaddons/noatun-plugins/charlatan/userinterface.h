/*
 * Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.	IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <noatun/plugin.h>

class QToolButton;
class ScrollingLabel;
class Seeker;
class QLabel;

/**
 * @short Main window class
 * @author Rik Hemsley <rik@kde.org>
 * @version 1.0
 */
class Charlatan : public QWidget, public UserInterface
{
	Q_OBJECT

	public:

		Charlatan();
		virtual ~Charlatan();

		void load(const QString & url);

	protected slots:

		virtual void slotConfigChanged();

		virtual void slotVolumeChanged(int);

		virtual void slotPlaying();
		virtual void slotStopped();
		virtual void slotPaused();

		virtual void slotPlayListShown();
		virtual void slotPlayListHidden();

		virtual void slotTimeout();

		virtual void slotRestart();

		virtual void slotStopShowingVolume();

	protected:

		virtual void mousePressEvent	(QMouseEvent *);
		virtual void dragEnterEvent		(QDragEnterEvent *);
		virtual void dropEvent				(QDropEvent *);
		virtual void closeEvent				(QCloseEvent *);
		virtual bool eventFilter			(QObject *, QEvent *);
		virtual void wheelEvent				(QWheelEvent *);
		virtual void setTitleText			(const QString & = QString::null);

		void loadConfig();

	private:

		QToolButton 		* previousButton_;
		QToolButton 		* stopButton_;
		QToolButton 		* playButton_;
		QToolButton 		* nextButton_;
		QToolButton 		* playListToggleButton_;
		QToolButton 		* restartButton_;

		Seeker					* seekSlider_;
		ScrollingLabel	* titleLabel_;

		QLabel					* positionLabel_;

		QTimer					* showingVolumeTimer_;
		QString						title_;
};

#endif
// vim:ts=2:sw=2:tw=78:noet

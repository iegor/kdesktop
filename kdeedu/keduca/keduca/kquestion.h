/***************************************************************************
                          kquestion.h  -  description
                             -------------------
    begin                : Tue May 22 2001
    copyright            : (C) 2001 by Javier Campos Morales
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef KQUESTION_H
#define KQUESTION_H

#include <qhbox.h>
#include <qtextedit.h>
#include <qlabel.h>

class QTimer;
class KProgress;

/**Question view
 * The Widget that is used to display the Question.
 *@author Javier Campos Morales
 */
class KQuestion : public QHBox
{
    Q_OBJECT

public:
    KQuestion(QWidget *parent=0, const char *name=0);
    ~KQuestion();
    /** Set pixmap */
    void setPixmap( const QPixmap pixmap);
    /** Set text */
    void setText( const QString &text);

    /** show a countdown, starting at "starttime" and counting
        down to 0, each second one count.

        0 disables the countdown.
    */
    void countdown(int starttime);

    void countdownVisible(bool visible);

    /** Get current time */
    int getCurrentTime();
  
private slots:
    void countDownOne();

private:

    // Private methods

    /** Init graphical interface */
    void initGUI();

    // Private attributes

    /** Main View */
    QTextEdit *_view;
    /** Main Picture */
    QLabel *_picture;
    QTimer *_countdownTimer;
    KProgress *_countdownWidget;
    int _currentCount;
    int _totalCount;
};

#endif

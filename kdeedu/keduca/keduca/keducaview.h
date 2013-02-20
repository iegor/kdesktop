/***************************************************************************
                          keducaview.h  -  description
                             -------------------
    begin                : Thu May 24 2001
    copyright            : (C) 2001 by Javier Campos
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

#ifndef KEDUCAVIEW_H
#define KEDUCAVIEW_H

#include <kurl.h>

#include <qlayout.h>
#include <qsplitter.h>
#include <qwidgetstack.h>

class QTimer;
class QTextEdit;
class QLabel;

class KPushButton;

class FileRead;
class KQuestion;
class KGroupEduca;

/**Main view of keduca file (questions and answers)
   This is the widget for asking questions and allowing selection of answers
   as used by KEduca TestMaster (class KEduca)
 *@author Javier Campos
 */

class KEducaView : public QWidgetStack
{
    Q_OBJECT

public:
    KEducaView(QWidget *parent=0, const char *name=0);
    ~KEducaView();
    /** Open url */
    bool openURL( const KURL &url);
    FileRead const *getKeducaFile() const {return _keducaFile; }

private:

    // Private methods

    /** Init graphical interface */
    void init();
    /** Show record, returns false if there is no record to show. */
    void showRecord();
    /** Show results */
    void showResults( const QString &text );
    /** Set results */
    void setResults();
    /** Set results */
    QString setFinalResult();
    /** Write settings */
    void configWrite();
    /** current results Points */
    QString currentStatusPoints();
    /** Set HTML table view with correct and incorrect answers */
    QString getTableQuestion( bool isCorrect, const QString &correct, const QString &incorrect);
    /** Define the next questions (random, secuencial) */
    bool questionNext();

    QString getInformation();
    /** Init table with title and colspan */
    QString insertTable( const QString &title = "", unsigned int col=0);
    /** Insert row */
    QString insertRow( const QString &text, bool title, unsigned int colSpan=0 );
    QString insertRow( const QString &label, const QString &field, bool formBased );
    QString insertRow( const QString &label1, const QString &field1, const QString &label2, const QString &field2, bool formBased );
    /** Close current table */
    QString insertTableClose();
            
    // Private attributes

    /** Widgets */
    QLabel *_introWidget;
    QWidget *_infoWidget, *_questionWidget, *_resultsWidget;
    /** Push Buttons */
    KPushButton *_buttonNext;
    KPushButton *_buttonSave;
    KPushButton *_buttonResultsNext; // FIXME: this is a hack
    KPushButton *_buttonStartTest;
    /** Group of answers. This is a frame an check or radio buttons. */
    KGroupEduca *_buttonGroup;
    /** KEDUCA file */
    FileRead *_keducaFile;
    /** A question text with pixmaps and movies support */
    KQuestion *_questionText;
    /** A question text with pixmaps and movies support */
    QTextEdit *_viewResults;
    QTextEdit *_viewInfo;
    /** Current record */
    int _keducaFileIndex;
    /** Current results */
    QString _results;
    /** Main vbox */
    QVBoxLayout *_vbox2;
    /** Main splitt */
    QSplitter *_split;
    /** Current results */
    QString _currentResults;
    /** If is init window */
    bool _isInitStatus;
    QTimer *_timeoutTimer;
    /** Num of correct answers */
    int _correctAnswer;
    /** Number of incorrect answers */
    int _incorrectAnswer;
    /** Number of correct answers in points*/
    int _correctPoints;
    /** Number of incorrect answers in points*/
    int _incorrectPoints;
    /** Current time */
    int _currentTime;
    /** Show results when finish? */
    bool _showResultFinish;
    /** Show random questions */
    bool _showRandomQuestions;
    /** Show random answers */
    bool _showRandomAnswers;
    /** Save real order by random questions */
    QValueList<uint> _randomQuestions;
    /** Save real order by random answers */
    QValueList<uint> _randomAnswers;
private slots:
    /** Button Next action */
    void slotButtonNext();
    void slotButtonSave();
    void slotButtonStartTest();
    void questionTimedOut();
};

#endif

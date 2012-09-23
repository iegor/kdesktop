/***************************************************************************
    begin                : Thu Jul 17
    copyright            : (C) 2001-2004 by George Wright
    email                : gwright@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef KLATINVOCAB_H
#define KLATINVOCAB_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdir.h>
#include <qpair.h>

#include "keduvocdata.h"
#include "klatinresultsdialog.h"
#include "klatinvocabwidget.h"

class KLatinVocab : public KLatinVocabWidget
{
    Q_OBJECT
public:
	KLatinVocab(QWidget* parent=0, const char *name=0);
	~KLatinVocab();

	/// Load vocabulary from file @p customFile
	void parseVocab(QString customFile = QString::null);
	/// Generate a new question
	void newQuestion();
	/// Update the scores of the test so far
	void updateScores();
	/// Show the results dialog
	void resultsDialog();
	/// Shuffle the words into a random sequence
	void createRandomSequence();
	/// Check the answer given (forwarded to from checkAnswer*)
	void checkAnswer(int button);
private slots:
	/// Destory the widget
	void goBack();
	/// Check if answer is correct if button 1 is pressed
	void checkAnswerOne();
	/// Check if answer is correct if button 2 is pressed
	void checkAnswerTwo();
	/// Check if answer is correct if button 3 is pressed
	void checkAnswerThree();
	/// Check if answer is correct if button 4 is pressed
	void checkAnswerFour();
public slots:
	/// Load a different vocabulary file in the standard install
	void changeVocab(const QString &filename);
signals:
	/// Emitted when goBack() is called
	void exited();
private:
	/// The list in which the native language and Latin words are assigned
	typedef QPair<QString, QString> QStringPair;
	QValueList<QStringPair> m_texts;
	
	QString m_fileDir;
	/// The filename of the vocabulary file being tested on
	QString m_vocabFile;
	/// A list of the 4 words used for assigning to the 4 multiple choice buttons
	QStringList m_choiceWords;
	/// The previous question's answer
	QStringList m_correctAnswer;
	/// The user's answer to the previous question
	QStringList m_previousAnswer;
	/// The previous question asked
	QStringList m_previousQuestion;
	
	QDir m_vocabDirectory;

	/// The question number in this session
	int m_questionNumber;
	/// The number of questions to ask (defined in Settings)
	int m_numberOfQuestions;
	/// The number of words in the vocabulary list
	int m_numberOfWords;

	/// The button on which the last correct answer was assigned
	int m_lastAnswerNumber;
	
	/// Determines whether the results dialog is currently shown or not
	bool m_dialogShown;
	/// Total number of questions answered correctly
	int m_totalCorrect;
	/// Total number of questions answered
	int m_total;
	
	/// Array to assign random sequence of words to
	int m_sequence[256];

	KLatinResultsDialog *results;

};

#endif // KLATINVOCAB_H

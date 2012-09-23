/***************************************************************************
    begin                : Thu Jul 17 2003
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

#ifndef KLATINGRAMMAR_H
#define KLATINGRAMMAR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "keduvocdata.h"
#include "klatingrammarwidget.h"

/** @short Main class for testing grammar*/
class KLatinGrammar : public KLatinGrammarWidget
{
	Q_OBJECT
public:
	KLatinGrammar(QWidget* parent=0, const char *name=0);
	~KLatinGrammar();

	/// Load next question
	void nextQuestion();
	/// Update score of test so far
	void updateScores();
public slots:
	/// Load grammar definitions database
	void loadDatabase();
	/// Destroys the widget
	void goBack();
	/// Check whether the answer is correct
	void checkAnswer();
signals:
	/// Signal that is emitted when the widget is closed
	void exited();
	void statusMsg(const QString&);
private:
	/// Array that holds the 12 cases of a noun
	QString m_noun[12];
	/// The case of the noun being tested
	int m_grammarNumber;
	/// Total number of questions attempted that are wrong
	int m_numWrong;

	/// Total number of questions attempted that are correct
	int m_totalCorrect;
	/// Total number of questions attempted
	int m_total;
};

#endif // KLATINGRAMMAR_H

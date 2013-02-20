/***************************************************************************
    begin                : Fri Aug 15 2003
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

#ifndef KLATINVERBS_H
#define KLATINVERBS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>

#include "keduvocdata.h"
#include "klatinverbswidget.h"

struct fileparts {
	std::vector<QString> declension;
	std::vector<const char*> mood;
	std::vector<const char*> voice;
};

struct questionparts {
	std::vector<const char*> person;
	std::vector<const char*> number;
	std::vector<const char*> tense;
};

/** @short Main class for testing verb forms */
class KLatinVerbs : public KLatinVerbsWidget
{
	Q_OBJECT
public:
	KLatinVerbs(QWidget* parent=0, const char *name=0);
	~KLatinVerbs();

	/// Load next question
	void nextQuestion();
	/// Update current scores
	void updateScores();
public slots:
	/// Destroy the widget
	void goBack();
	/// Called when the combobox is clicked
	/** @param itemNumber is the index of the item selected */
	void conjugationActivated(int itemNumber);
	/// Load the verb definitions database
	void loadDatabase();
	/// Check the user's input
	void checkAnswer();
signals:
	/// Signal that is emitted when the widget wishes to terminate
	void exited();
	void statusMsg(const QString&);
private:
	/// These are structs containing information required to generate a question
	struct fileparts verbparts;
	struct questionparts question;
	/// String array for the various verbs
	QString m_verb[36];
	QString m_currentVerb;
	/// Number to reference the verb form given
	int m_verbNumber;
	/// Total number of wrong answers given
	int m_numWrong;
	/// Total number of answers given
	int m_total;
	/// Total number of correct answers given
	int m_totalCorrect;
};

#endif // KLATINVERBS_H

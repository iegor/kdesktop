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

#include <qfile.h>
#include <qlabel.h>

#include <kapplication.h>
#include <kcombobox.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include <stdlib.h>
#include <time.h>

#include "klatingrammar.h"

KLatinGrammar::KLatinGrammar(QWidget* parent, const char *name)
	: KLatinGrammarWidget(parent, name)
{
	// Set defaults
	m_total = 0;
	m_totalCorrect = 0;
	m_numWrong = 0;

	OKButton->setIconSet(KGlobal::iconLoader()->loadIconSet("button_ok", KIcon::Small));
	BackButton->setIconSet(KGlobal::iconLoader()->loadIconSet("back", KIcon::Small));

	// Set connections
	connect(BackButton, SIGNAL(clicked()), this, SLOT(goBack()));
	connect(OKButton, SIGNAL(clicked()), this, SLOT(checkAnswer()));
	connect(AnswerBox, SIGNAL(returnPressed()), this, SLOT(checkAnswer()));
	connect(ChooseSection, SIGNAL(activated(int)), this, SLOT(loadDatabase()));

	// Insert the various declensions
	ChooseSection->insertItem(i18n("All Declensions"));
	ChooseSection->insertItem(i18n("1st Feminine"));
	ChooseSection->insertItem(i18n("2nd Masculine"));
	ChooseSection->insertItem(i18n("2nd Neuter"));
	ChooseSection->insertItem(i18n("3rd Masc/Fem"));
	ChooseSection->insertItem(i18n("3rd Neuter"));
	ChooseSection->insertItem(i18n("4th Masc/Fem"));
	ChooseSection->insertItem(i18n("4th Neuter"));
	ChooseSection->insertItem(i18n("5th Feminine"));

	// Load the database of nouns
	loadDatabase();

	AnswerBox->setFocus();
}

KLatinGrammar::~KLatinGrammar()
{
}

void KLatinGrammar::loadDatabase()
{
	int curitem = ChooseSection->currentItem();
	if ( curitem != 0 ) {
		curitem--;
	}
	else 
	{
		// Seed the random number generator
		srandom(time(NULL));

		curitem = (random() % 8);
	}

	// Various files that are used for each declension
	QString file = "klatin/data/grammar/";
	switch (curitem) {
		case 0:
			file += "puella.klat";
			break;
		case 1:
			file += "servus.klat";
			break;
		case 2:
			file += "bellum.klat";
			break;
		case 3:
			file += "rex.klat";
			break;
		case 4:
			file += "opus.klat";
			break;
		case 5:
			file += "gradus.klat";
			break;
		case 6:
			file += "genu.klat";
			break;
		case 7:
			file += "res.klat";
			break;
		default:  // fake case...
			file += "foo.klat";
	}

	// Check whether the file actually exists
	// If not - warn user.
	if (QFile::exists(locate("data", file)) == FALSE) {
		KMessageBox::error(this, i18n("Could not load grammar-definition file, check KLatin installation."), i18n("Error with KLatin"));
		QuestionLabel->setText(i18n("Error."));
		return;
	}

	KEduVocDataItemList grammar = KEduVocData::parse(locate("data", file));

	// Load XML file into memory
	int numberOfWords = 0;
	for(KEduVocDataItemList::Iterator dataIt = grammar.begin(); dataIt != grammar.end(); ++dataIt) {
		m_noun[numberOfWords] = grammar[numberOfWords].originalText();
		numberOfWords++;
	}

	nextQuestion();
}

void KLatinGrammar::goBack()
{
	emit exited();
	close();
}

void KLatinGrammar::nextQuestion()
{
	emit statusMsg(i18n("Grammar"));

	// Seed the random number generator
	srandom(time(NULL));

	m_grammarNumber = (random() % 12) + 1;

	QString questionString;
	
	// Question string...
	switch (m_grammarNumber) {
		case 1: 
			questionString = i18n("What is the nominative singular of %1?").arg(m_noun[0]);
			break;
		case 2: 
			questionString = i18n("What is the vocative singular of %1?").arg(m_noun[0]);
			break;
		case 3: 
			questionString = i18n("What is the accusative singular of %1?").arg(m_noun[0]);
			break;
		case 4: 
			questionString = i18n("What is the genitive singular of %1?").arg(m_noun[0]);
			break;
		case 5: 
			questionString = i18n("What is the dative singular of %1?").arg(m_noun[0]);
			break;
		case 6: 
			questionString = i18n("What is the ablative singular of %1?").arg(m_noun[0]);
			break;
		case 7: 
			questionString = i18n("What is the nominative plural of %1?").arg(m_noun[0]);
			break;
		case 8: 
			questionString = i18n("What is the vocative plural of %1?").arg(m_noun[0]);
			break;
		case 9: 
			questionString = i18n("What is the accusative plural of %1?").arg(m_noun[0]);
			break;
		case 10: 
			questionString = i18n("What is the genitive plural of %1?").arg(m_noun[0]);
			break;
		case 11: 
			questionString = i18n("What is the dative plural of %1?").arg(m_noun[0]);
			break;
		case 12: 
			questionString = i18n("What is the ablative plural of %1?").arg(m_noun[0]);
			break;
	}
	
	// Set question on screen
	QuestionLabel->setText(questionString);
}

void KLatinGrammar::checkAnswer()
{
	if (m_noun[m_grammarNumber - 1] != AnswerBox->text().lower()) {
		m_numWrong++;

		// Allow a tolerance of 1 retry
		if (m_numWrong == 2) {
			m_numWrong = 0;
			KMessageBox::information(this, i18n("The correct answer was %1.").arg(m_noun[m_grammarNumber - 1]), i18n("Wrong"));
			if (ChooseSection->currentItem()==0)
				loadDatabase();
			else
				nextQuestion();
		} else {
			if (AnswerBox->text().isEmpty()) {
				emit statusMsg(i18n("Please enter an answer"));
			}
		}
	} else {
		m_totalCorrect++;

		// Reset the number of questions that were wrong
		m_numWrong = 0;

		if (ChooseSection->currentItem()==0)
			loadDatabase();
		else
			nextQuestion();
	}
	m_total++;
	
	AnswerBox->setText(NULL);
	
	updateScores();
}

void KLatinGrammar::updateScores()
{
	// Update the score labels
	Score1Label->setNum(m_totalCorrect);
	Score2Label->setNum(m_total);

	// Set percentage
	PercentageLabel->setText(QString::number(int(double(m_totalCorrect) / m_total * 100))+QString("%"));
}

#include "klatingrammar.moc"

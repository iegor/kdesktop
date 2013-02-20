/***************************************************************************
    begin                : Fri Aug 15
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

#include "klatinverbs.h"

KLatinVerbs::KLatinVerbs(QWidget* parent, const char *name)
	: KLatinVerbsWidget(parent, name)
{
	OKButton->setIconSet(KGlobal::iconLoader()->loadIconSet("button_ok", KIcon::Small));
	BackButton->setIconSet(KGlobal::iconLoader()->loadIconSet("back", KIcon::Small));
	ChangeButton->setIconSet(KGlobal::iconLoader()->loadIconSet("apply", KIcon::Small));

	ChooseConjugation->insertItem(i18n("1st"));
	ChooseConjugation->insertItem(i18n("2nd"));
	ChooseConjugation->insertItem(i18n("3rd"));
	ChooseConjugation->insertItem(i18n("4th"));
	ChooseConjugation->insertItem("sum"); // Does not need translating as it's in Latin

	ChooseMood->insertItem(i18n("Indicative"));
	ChooseMood->insertItem(i18n("Subjunctive"));

	ChooseVoice->insertItem(i18n("Active"));
	ChooseVoice->insertItem(i18n("Passive"));

	m_total = 0;
	m_totalCorrect = 0;
	m_numWrong = 0;

	// Initializing the data structures
	// These are words in Latin and do not need i18n
	verbparts.declension.push_back("amo");
	verbparts.declension.push_back("moneo");
	verbparts.declension.push_back("rego");
	verbparts.declension.push_back("audio");
	verbparts.declension.push_back("sum");
	
	verbparts.mood.push_back(I18N_NOOP("indicative"));
	verbparts.mood.push_back(I18N_NOOP("subjunctive"));
	
	verbparts.voice.push_back(I18N_NOOP("active"));
	verbparts.voice.push_back(I18N_NOOP("passive"));

	question.person.push_back(I18N_NOOP("first"));
	question.person.push_back(I18N_NOOP("second"));
	question.person.push_back(I18N_NOOP("third"));
	
	question.number.push_back(I18N_NOOP("singular"));
	question.number.push_back(I18N_NOOP("plural"));
	
	question.tense.push_back(I18N_NOOP("present"));
	question.tense.push_back(I18N_NOOP("future"));
	question.tense.push_back(I18N_NOOP("imperfect"));
	question.tense.push_back(I18N_NOOP("perfect"));
	question.tense.push_back(I18N_NOOP("pluperfect"));
	question.tense.push_back(I18N_NOOP("future perfect"));
	
	connect(ChooseConjugation, SIGNAL(activated(int)), this, SLOT(conjugationActivated(int)));
	connect(BackButton, SIGNAL(clicked()), this, SLOT(goBack()));
	connect(ChangeButton, SIGNAL(clicked()), this, SLOT(loadDatabase()));
	connect(AnswerBox, SIGNAL(returnPressed()), this, SLOT(checkAnswer()));
	connect(OKButton, SIGNAL(clicked()), this, SLOT(checkAnswer()));

	loadDatabase();

	AnswerBox->setFocus();
}

KLatinVerbs::~KLatinVerbs()
{
}

void KLatinVerbs::loadDatabase()
{
	QString databaseFile = verbparts.declension[ChooseConjugation->currentItem()];
	m_currentVerb = databaseFile;
	databaseFile += "-" + QString(verbparts.mood[ChooseMood->currentItem()]);
	databaseFile += "-" + QString(verbparts.voice[ChooseVoice->currentItem()]) + ".klat";
	
	QString fileName = "klatin/data/verbs/" + databaseFile;

	if (QFile::exists(locate("data", fileName)) == FALSE) {
		KMessageBox::error(this, i18n("Could not load verb-definition file, check KLatin installation."), i18n("Error with KLatin"));
		goBack();
	}

	int numberOfWords = 0;

	KEduVocDataItemList verbs = KEduVocData::parse(locate("data", fileName));

	for(KEduVocDataItemList::Iterator dataIt = verbs.begin(); dataIt != verbs.end(); ++dataIt) {
			m_verb[numberOfWords] = verbs[numberOfWords].originalText();
			numberOfWords++;
	}

	nextQuestion();
}

void KLatinVerbs::nextQuestion()
{
	emit statusMsg(i18n("Verbs"));

	// Seed the random number generator
	srandom(time(NULL));

	if (ChooseMood->currentText() == i18n("Indicative"))
		m_verbNumber = (random() % 35) + 1;
	if (ChooseMood->currentText() == i18n("Subjunctive"))
		m_verbNumber = (random() % 23) + 1;

	QString QuestionString;

	int person = 0, number = 0, tense = 0;
	
	switch (m_verbNumber % 6) {
		case 1:
			person = 0;
			number = 0;
			break;
		case 2:
			person = 1;
			number = 0;
			break;
		case 3:
			person = 2;
			number = 0;
			break;
		case 4:
			person = 0;
			number = 1;
			break;
		case 5:
			person = 1;
			number = 1;
			break;
		case 0:
			person = 2;
			number = 1;
			break;
	}
	
	switch ((int) (m_verbNumber - 1) / 6) {
		case 0:
			tense = 0;
			break;
		case 1:
			if (ChooseMood->currentText() == i18n("Indicative"))
				tense = 1;
			else
				tense = 2;
			break;
		case 2:
			if (ChooseMood->currentText() == i18n("Indicative"))
				tense = 2;
			else
				tense = 3;
			break;
		case 3:
			if (ChooseMood->currentText() == i18n("Indicative"))
				tense = 3;
			else
				tense = 4;
			break;
		case 4:
			tense = 4;
			break;
		case 5:
			tense = 5;
			break;
		}
		
	QuestionString = i18n("Note: ARGUMENT %1 is an ordinal number (1st, 2nd, 3rd). "
		"ARGUMENT %2 indicates whether the person is singular or plural. "
		"ARGUMENT %3 is the tense of the verb (Present, Imperfect, Perfect). "
		"ARGUMENT %4 is the mood of the verb (Indicative, Subjunctive). "
		"ARGUMENT %5 is the voice of the verb (Active, Passive). "
		"ARGUMENT %6 is the verb name (amo, moneo etc). This is in Latin and should not be translated. "
		"The output string in English is something like: What is the first person singular present indicative active of amo?", 
		"What is the %1 person %2 %3 %4 %5 of %6?").arg(i18n(question.person[person]))
		.arg(i18n(question.number[number]))
		.arg(i18n(question.tense[tense]))
		.arg(i18n(verbparts.mood[ChooseMood->currentItem()]))
		.arg(i18n(verbparts.voice[ChooseVoice->currentItem()]))
		.arg(m_currentVerb);
	
	QuestionLabel->setText(QuestionString);
}

void KLatinVerbs::checkAnswer()
{
	if (AnswerBox->text().isEmpty()) {
		emit statusMsg(i18n("Please enter an answer"));
		return;
	}
	
	if (m_verb[m_verbNumber - 1] != AnswerBox->text().lower()) {
		// Iterate...
		m_numWrong++;

		// Retry tolerance...
		if (m_numWrong == 2) {
			m_numWrong = 0;
			KMessageBox::information(this, i18n("The correct answer was %1.").arg(m_verb[m_verbNumber - 1]), i18n("Wrong"));
			nextQuestion();
		}
	} else {
		// Iterate...
		m_totalCorrect++;

		m_numWrong = 0;

		nextQuestion();
	}
 
	m_total++;
	AnswerBox->setText(NULL);
	
	updateScores();
}

void KLatinVerbs::conjugationActivated(int itemNumber)
{
	if (itemNumber == 4)
		ChooseVoice->removeItem(1);
	else
		if (ChooseVoice->count() == 1)
			ChooseVoice->insertItem(i18n("Passive"));
}

void KLatinVerbs::updateScores()
{
	// Update the score labels
	Score1Label->setNum(m_totalCorrect);
	Score2Label->setNum(m_total);

	// Set percentage
	PercentageLabel->setText(QString::number(int(double(m_totalCorrect) / m_total * 100))+QString("%"));
}

void KLatinVerbs::goBack()
{
	emit exited();
	close();
}

#include "klatinverbs.moc"

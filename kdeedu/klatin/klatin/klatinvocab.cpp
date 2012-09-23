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

#include <qdir.h>
#include <qlabel.h>
#include <qlistview.h>

#include <kapplication.h>
#include <kcombobox.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kstdguiitem.h>

#include <stdlib.h>
#include <time.h>

#include "klatinvocab.h"
#include "settings.h"

KLatinVocab::KLatinVocab(QWidget* parent, const char *name)
	: KLatinVocabWidget(parent, name)
{
	BackButton->setIconSet(KGlobal::iconLoader()->loadIconSet("back", KIcon::Small));

	m_vocabFile = QString::null;
	
	// Goes back to the last dialog
	connect(BackButton, SIGNAL(clicked()), this, SLOT(goBack()));

	// Set default settings
	m_numberOfQuestions = Settings::numOfQuestions();
	m_total = 0;
	m_totalCorrect = 0;
	m_questionNumber = 0;
	m_dialogShown = true;
	
	// Parse the kvtml vocab file
	parseVocab();

	// Connect the multiple choice buttons to the various slots
	connect(Answer1, SIGNAL(clicked()), this, SLOT(checkAnswerOne()));
	connect(Answer2, SIGNAL(clicked()), this, SLOT(checkAnswerTwo()));
	connect(Answer3, SIGNAL(clicked()), this, SLOT(checkAnswerThree()));
	connect(Answer4, SIGNAL(clicked()), this, SLOT(checkAnswerFour()));
}

KLatinVocab::~KLatinVocab()
{
}

void KLatinVocab::parseVocab(QString customFile)
{
	m_vocabFile = customFile;
	
	m_texts.clear();
	
	QString fileName;
	
	// Load all the config variables
	if (Settings::numOfQuestions() != 0) {
		m_numberOfQuestions = Settings::numOfQuestions();
	}

	// Load default file as defined in settings
	fileName = Settings::defaultFile();
	
	kdDebug() << fileName << endl;
	
	if (!customFile.isNull())
		fileName = customFile;
	
	// Check whether file exists or not
	// If not then regenerate working default
	if (QFile::exists(fileName) == FALSE) {
		KConfigBase *globalConf = KGlobal::config();
		globalConf->setGroup("Locale");
		
		QString language = globalConf->readEntry("Language", "en");
		language = language.left(2);
		
		QString fileNameLang = locate("data", "klatin/data/vocabs/");
		fileNameLang += language;
		
		QDir checkLang(fileNameLang);
		
		if (checkLang.exists() == FALSE) {
			language = "en";
		}
		
		QDir vocabDirectory(fileNameLang, "*.kvtml");
		
		QString defaultFile = (fileNameLang + "/" + vocabDirectory[0]);
		
		if (QFile::exists(defaultFile) == FALSE) {
			KMessageBox::error(this, i18n("Could not load vocabulary file; there is a problem with your KLatin installation."), i18n("Error with KLatin"));
			kapp->quit();
		} else {
			fileName = defaultFile;
			Settings::setDefaultFile(defaultFile);
		}
	}

	// Load XML file into memory
	KEduVocDataItemList vocabulary = KEduVocData::parse(fileName);
	
	int it = 0;
	
	// Start parsing file and allocating words to the QValueList<QStringPair>
	for(KEduVocDataItemList::Iterator dataIt = vocabulary.begin(); dataIt != vocabulary.end(); ++dataIt) {
		if (Settings::langToLatin()) {
			m_texts.append(qMakePair(vocabulary[it].originalText(), vocabulary[it].translatedText()));
		} else {
			m_texts.append(qMakePair(vocabulary[it].translatedText(), vocabulary[it].originalText()));
		}
		it++;
	}

	// Number of words in the list
	m_numberOfWords = it - 1;
	
	// Create a random sequence to follow in the test
	createRandomSequence();

	// Ask a question
	newQuestion();
}

void KLatinVocab::createRandomSequence()
{
	// Seed the random number generator
	srandom(time(NULL));

	// This works by assigning an array with 1, 2, 3, 4, 5 etc
	// and then shuffling them into m_sequence.
	// This sequence is then followed for testing purposes
	
	// TODO Change implementation to use KRandomSequence
	
	int i;
	int randomSequence[256];
	m_numberOfQuestions++;

	for (i = 0; i < m_numberOfWords; i++) {
		randomSequence[i] = i;
	}

	// Courtesy of Neil Stevens
	for (i = 0; i < m_numberOfQuestions * 2; i++) {
		const int x = i % m_numberOfWords;
		const int y = x + (rand() % (m_numberOfWords - x));
		const int swap = randomSequence[x];
		randomSequence[x] = randomSequence[y];
		randomSequence[y] = swap;
	}

	if (m_numberOfQuestions <= m_numberOfWords) {
		for (i = 0; i < m_numberOfQuestions; i++) {
			m_sequence[i] = randomSequence[i];
		}
	}
	
	if (m_numberOfQuestions > m_numberOfWords) {
		int j = 0;

		for (i = 0; i < m_numberOfQuestions; i++) {
			if (j < m_numberOfWords) {
				m_sequence[i] = randomSequence[j];
				j++;
			}
			if (j >= m_numberOfWords) {
				j = 0;
			}
		}
	}
}

void KLatinVocab::newQuestion()
{
	int randNum = 0;

	// If we have already asked the number of questions required
	// then show the results dialogue
	if (m_questionNumber == (m_numberOfQuestions - 1)) {
		resultsDialog();
		return;
	}

	// Seed the random number generator
	srandom(time(NULL));

	// Grab number from random sequence
	randNum = m_sequence[m_questionNumber];

	QuestionLabel->setText(m_texts[randNum].first);
	m_previousQuestion  += QuestionLabel->text();

	// Initialise a couple of temporary variables
	int i = 0, j = 0, tempRand[4];
	bool override = FALSE;

	// Assign m_choiceWords with the 4 words
	// to go into the multiple choice. Make sure
	// none of them are the same.
	while (i < 4) {
		tempRand[i] = (random() % (m_numberOfWords - 1)) + 1;
		if (i > 0) {
			j = 0;
			while (j < i) {
				if (tempRand[j] == tempRand[i]) {
					tempRand[i] = (random() % (m_numberOfWords - 1)) + 1;
				} else
					j++;
			}
		}
		i++;
	}

	// Assign the four words to m_choiceWords.
	if (m_choiceWords.count() == 0) {
		for (i = 0; i < 4; i++)
			m_choiceWords += m_texts[tempRand[i]].second;
		m_choiceWords += m_texts[randNum].second;
	} else {
		for (i = 0; i < 4; i++)
			m_choiceWords[i] = m_texts[tempRand[i]].second;
		m_choiceWords[4] = m_texts[randNum].second;
	}

	// Make sure the actual answer isn't already in m_choiceWords
	// If it is, set override to be TRUE
	for (i = 0; i < 4; i++) {
		if (m_choiceWords[4] == m_choiceWords[i]) {
			m_lastAnswerNumber = i;
			override = TRUE;
		}
	}

	bool exitLoop = FALSE;

	// Override a random variable with the correct
	// answer
	if (!override) {
		int answerNumber = (random() % 3);
		while (!exitLoop) {
			if (answerNumber != m_lastAnswerNumber) {
				m_choiceWords[answerNumber] = m_texts[randNum].second;
				m_lastAnswerNumber = answerNumber;
				exitLoop = TRUE;
			} else {
				answerNumber = (random() % 3);
			}
		}
	}

	// Add the correct answer to the list (for use
	// when the results dialogue is called)
	m_correctAnswer += m_choiceWords[4];

	// The four values that will go into the four
	// Option boxes.
	Answer1->setText(m_choiceWords[0]);
	Answer2->setText(m_choiceWords[1]);
	Answer3->setText(m_choiceWords[2]);
	Answer4->setText(m_choiceWords[3]);

	// We have now asked another question so iterate this int.
	m_questionNumber++;
}

// Slots

void KLatinVocab::goBack()
{
	// If the dialogue isn't already shown then show it
	if (!m_dialogShown)
		resultsDialog();

	emit exited();
	close();
}

// Slots for each of the multiple choice buttons

void KLatinVocab::checkAnswer(int button)
{
	m_dialogShown = false;

	if (m_choiceWords[button] == m_choiceWords[4]) {
		// Update the total answered correctly
		m_totalCorrect++;
	}
	// Update the total questions faced
	m_total++;

	m_previousAnswer += m_choiceWords[button];
	updateScores();
	// Setup a new question
	newQuestion();
}

// TODO Use one slot and check the sender() instead
void KLatinVocab::checkAnswerOne()
{
	checkAnswer(0);
}

void KLatinVocab::checkAnswerTwo()
{
	checkAnswer(1);
}

void KLatinVocab::checkAnswerThree()
{
	checkAnswer(2);
}

void KLatinVocab::checkAnswerFour()
{
	checkAnswer(3);
}

void KLatinVocab::updateScores()
{
	// Update the score labels
	Score1Label->setNum(m_totalCorrect);
	Score2Label->setNum(m_total);

	// Set percentage
	PercentageLabel->setText(QString::number(int(double(m_totalCorrect) / m_total * 100))+QString("%"));
}

void KLatinVocab::resultsDialog()
{
	// Show the results dialog

	results = new KLatinResultsDialog(this, 0, TRUE);
	results->CloseButton->setGuiItem(KStdGuiItem::close());

	int temp = Score2Label->text().toInt();

	// Set the lists
	for (int i = 0 ; i < temp; i++) {
		if (m_previousAnswer[i] != 0)
			new QListViewItem(results->ResultsView, m_previousQuestion[i], m_previousAnswer[i], m_correctAnswer[i]);
	}

	// Set the scores
	results->PercentageValue->setText(PercentageLabel->text());
	results->AttemptedValue->setText(Score2Label->text());

	results->show();
	results->resize(QSize(420, 280));

	// Set some variables.
	m_dialogShown = true;

	changeVocab(Settings::defaultFile());
}

void KLatinVocab::changeVocab(const QString &filename)
{
	// Reset to defaults
	m_total = 0;
	m_totalCorrect = 0;
	m_questionNumber = 0;
	m_numberOfQuestions = Settings::numOfQuestions();
	
	// Reset the StringLists as we are now on a new testing session
	m_correctAnswer.clear();
	m_previousAnswer.clear();
	m_previousQuestion.clear();
	
	// Reset score labels
	Score1Label->setNum(m_totalCorrect);
	Score2Label->setNum(m_total);
	PercentageLabel->setText(0);

	// Load the vocabulary file again (and thus create a new random sequence)
	parseVocab(filename);
}


#include "klatinvocab.moc"

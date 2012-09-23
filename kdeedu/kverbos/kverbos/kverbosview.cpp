/***************************************************************************
                          kverbosview.cpp  -  description
                             -------------------
    begin                : Die Mai  1 21:39:04 CEST 2001
    copyright            : (C) 2001 by Arnold Kraschinski
    email                : arnold.k67@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cstdlib>
 
// include files for Qt
#include <qprinter.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qprogressbar.h>


// application specific includes
#include "kverbosview.h"
#include "kverbosdoc.h"

#include "verbspanish.h"


KVerbosView::KVerbosView(QWidget *parent, const char *name) : QLernen/*QWidget*/(parent, name)
{
  // Hier wird gesetzt, wie der Hintergrund des Fensters zu zeichnen ist.
  setBackgroundMode(PaletteBackground);
  // Die gesamten Zeiger auf QLineEdit in ein Feld umwandeln
  formEdit[0][0] = EditYo1;
  formEdit[1][0] = EditTu1;
  formEdit[2][0] = EditEl1;
  formEdit[3][0] = EditNosotros1;
  formEdit[4][0] = EditVosotros1;
  formEdit[5][0] = EditEllos1;
  formEdit[0][1] = EditYo2;
  formEdit[1][1] = EditTu2;
  formEdit[2][1] = EditEl2;
  formEdit[3][1] = EditNosotros2;
  formEdit[4][1] = EditVosotros2;
  formEdit[5][1] = EditEllos2;
  formEdit[0][2] = EditYo3;
  formEdit[1][2] = EditTu3;
  formEdit[2][2] = EditEl3;
  formEdit[3][2] = EditNosotros3;
  formEdit[4][2] = EditVosotros3;
  formEdit[5][2] = EditEllos3;
  // einige Schalterbezeichnungen müssen richtiggestellt werden.
  ButtonN->setText("ñ");
  ButtonA->setText("á");
  ButtonE->setText("é");
  ButtonI->setText("í");
  ButtonO->setText("ó");
  ButtonU->setText("ú");
  TextTu->setText("tú");
  EditAleman->setFocus();
  // Es muss ein Verb aus dem Dokument geholt werden, das dann in dem Fenster dargestellt
  // werden kann, wenn es ein Verb gibt. Ansonsten bleibt das Fenster leer.
  zeigeVerb();
  enteredCorrect = 0;
  verbsTrained = 0;
}

KVerbosView::~KVerbosView()
{
}

KVerbosDoc* KVerbosView::getDocument() const
{
  KVerbosApp *theApp=(KVerbosApp *) parentWidget();

  return theApp->getDocument();
}

void KVerbosView::print(QPrinter *pPrinter)
{
  QPainter printpainter;
  printpainter.begin(pPrinter);

  // TODO: add your printing code here

  printpainter.end();
}

// This funktion erases all accents from the letters of a string
QString KVerbosView::deleteAccents(const QString& s) const
{
  QString s2 = s.simplifyWhiteSpace();
  if (!s2.isEmpty())
  {
    for (unsigned int i=0; i<s2.length(); i++)
    {
      QString t = s2.mid(i, 1);
      if (t == "á") s2.replace(i, 1, "a");
      if (t == "é") s2.replace(i, 1, "e");
      if (t == "í") s2.replace(i, 1, "i");
      if (t == "ó") s2.replace(i, 1, "o");
      if (t == "ú") s2.replace(i, 1, "u");
      if (t == "ñ") s2.replace(i, 1, "n");
    };
  };
  return s2;
}

void KVerbosView::slotN()
{
  QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
  if (pl)
  {
    pl->insert("ñ");
  };
}

void KVerbosView::slotA()
{
  QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
  if (pl)
  {
    pl->insert("á");
  };
}

void KVerbosView::slotE()
{
  QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
  if (pl)
  {
    pl->insert("é");
  };
}

void KVerbosView::slotI()
{
  QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
  if (pl)
  {
    pl->insert("í");
  };
}

void KVerbosView::slotO()
{
  QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
  if (pl)
  {
    pl->insert("ó");
  };
}

void KVerbosView::slotU()
{
  QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
  if (pl)
  {
    pl->insert("ú");
  };
}

// Alle Eingabefelder werden gelöscht, dabei muss auch die Schrift auf normal
// gesetzt werden, damit die korrigierten Felder wieder die normale Schrift erhalten.
void KVerbosView::slotClearAll()
{
	QFont fontNormal = EditVerbo->font();
	QPalette palette = EditVerbo->palette();
	EditAleman->clear();	
	EditAleman->setFont(fontNormal);
	EditAleman->setPalette(palette);
	for (int i=0; i<6; i++)
		for (int j=0; j<3; j++)
		{
			formEdit[i][j]->clear();
			formEdit[i][j]->setFont(fontNormal);
			formEdit[i][j]->setPalette(palette);
		};
}

// Die Ansicht wird aufgefordert das aktuelle Verb vom Dokument zu holen und es in dem
// Fenster zur Ansicht zu bringen. Dabei wird von dem Dokument nur ein Zeiger auf das
// Verb übergeben und die entsprechende Zeit, die gelernt werden soll.
void KVerbosView::zeigeVerb()
{
	// einen Zeiger auf das Dokument besorgen und ein Verb und seine Formen besorgen.
	KVerbosDoc* pDoc = getDocument();
	pDoc->getAuswahl(formen, zeitAuswahl);
	slotClearAll();
	// Die spanische Form der Verbes zur Anzeige bringen
	EditVerbo->setText(formen[6][0]);
	// Die Übungszeit zur Anzeige bringen.
	TextTiempo->setText(verbSpanish::timeNames[zeitAuswahl]);
	// Wenn kein reflexives Verb vorliegt, werden die Felder deaktiviert
	if (formen[0][0].isEmpty())
	{
		for (int i = yo; i<=ellos; i++)
			formEdit[i][0]->setEnabled(false);
	}
	else
	{
		for (int i = yo; i<=ellos; i++)
			formEdit[i][0]->setEnabled(true);
	};
	// Wenn eine einfache, nicht zusammengesetzte Form vorliegt, werden die
	// Felder deaktiviert.
	if (formen[0][1].isEmpty())
	{
		for (int i = yo; i<=ellos; i++)
			formEdit[i][1]->setEnabled(false);
	}
	else
	{
		for (int i = yo; i<=ellos; i++)
			formEdit[i][1]->setEnabled(true);
	};
	EditAleman->setFocus();
	corrected = false;
}

// Es soll das nächste Verb zur Anzeige gebracht werden.
void KVerbosView::slotWeiter()
{
	if (corrected)
	{
		verbsTrained++;
		emit numberTrainedChanged(verbsTrained);
		ProgressBar->setProgress(enteredCorrect*100/verbsTrained);
		zeigeVerb();
	}
	else
	{
		slotCorregir();
	};
}

// Die Eingaben in den Eingabefeldern sollen geprüft werden.
void KVerbosView::slotCorregir()
{
	bool richtig = true;
	QFont fontNormal = EditVerbo->font();
	QFont fontBold = EditVerbo->font();
	fontBold.setUnderline(true);
	QPalette palette = EditVerbo->palette();
	palette.setColor(QColorGroup::Text, red);
	palette.setColor(QColorGroup::Base, white);
	if ( 0 > formen[6][1].find(EditAleman->text().simplifyWhiteSpace(), 0))
	{
		EditAleman->setFont(fontBold);
		EditAleman->setPalette(palette);
		richtig = false;
	};
	// die Formen vergleichen
	KVerbosApp* kApp = dynamic_cast<KVerbosApp*>(parent());
	if (!kApp) {
		return;
	}
	for (int j=0; j<3; j++)
	{
		if (!formen[0][j].isEmpty())
		{
			for (int i=0; i<6; i++)
				if (kApp->doitStrict())
				{
					if (formEdit[i][j]->text().simplifyWhiteSpace() != formen[i][j])
					{
						formEdit[i][j]->setFont(fontBold);
						formEdit[i][j]->setPalette(palette);
						richtig = false;
					};
				}
				else
				{
					QString s1 = formEdit[i][j]->text().simplifyWhiteSpace();
					QString s2 = formen[i][j];
					if (deleteAccents(s1) != deleteAccents(s2))
					{
						formEdit[i][j]->setFont(fontBold);
						formEdit[i][j]->setPalette(palette);
						richtig = false;
					};
				};
		};
	};
	// Die Statistik muss angepasst werden.
	if (!corrected)
	{
		getDocument()->solved(richtig);
		if (richtig)
		{
			enteredCorrect++;
			emit numberCorrectChanged(enteredCorrect);
			emit informKFeeder(1);
			// Show the German expression. So the user can see the other German meanings
			// if there are some.
			EditAleman->setText(formen[6][1]);
		};
	};
	corrected = true;
}

// Die Lösung soll in dem Fenster präsentiert werden.
void KVerbosView::slotSolution()
{
	QFont fontNormal = EditVerbo->font();
	EditAleman->setFont(fontNormal);
	EditAleman->setPalette(EditVerbo->palette());
	EditAleman->setText(formen[6][1]);		
	//
	for (int i=0; i<6; i++)
	  for (int j=0; j<3; j++)
	  {
		formEdit[i][j]->setFont(fontNormal);
		formEdit[i][j]->setPalette(EditVerbo->palette());
		formEdit[i][j]->setText(formen[i][j]);
	  };
	corrected = true;
}

// Wird bei einem bereits korrigiertem Verb eine weitere Eingabe gamacht, muss die
// Markierung, Unterstreichen oder Fettdruck, rückgängig gemacht und wieder der
// normale Font gesetzt werden.
void KVerbosView::slotFont()
{
	if (corrected)
	{
		QFont fontNormal = EditVerbo->font();	
		QLineEdit* pl = dynamic_cast<QLineEdit*>(focusWidget());
		if (0 != pl)
		{
			pl->setFont(fontNormal);
			pl->setPalette(EditVerbo->palette());
		};
	};
}

// Weil bei den Gerund- bzw. Perfektformen das eingeben der für alle Personen
// gleichen Formen langweilig ist und es auch nicht gut ist, wenn man sechs mal
// die falsche Form eingibt, wird die Form aus dem yo-Eingabefeld in die anderen
// kopiert.
void KVerbosView::slotCopyText()
{
	// erst prüfen, ob so eine Zeitform vorliegt.
  if (zeitAuswahl >= MAX_TIEMPOS)
	{
  	QString	s = formEdit[0][2]->text();
		for (int i=1; i<6; i++)
		{
			formEdit[i][2]->setText(s);
			if (corrected)
			{
    		QFont fontNormal = EditVerbo->font();	
		    formEdit[i][2]->setFont(fontNormal);
			};
		};
	};
}

// tell the view that the user has changed. Training results are deleted and set to the
// initial values. A new verb is chosen. The function returns the training result of the
// old user.
int KVerbosView::userChanged(int& r1, int& r2)
{
		r1 = 0;
		r2 = verbsTrained;
		if (verbsTrained !=0)
			r1 = enteredCorrect*100/verbsTrained;
		enteredCorrect = 0;
		verbsTrained = 0;
		ProgressBar->setProgress(0);	
		emit numberTrainedChanged(verbsTrained);
		emit numberCorrectChanged(enteredCorrect);
		return r1;
}

#include "kverbosview.moc"

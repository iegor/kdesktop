/***************************************************************************
                          kverbosuser.cpp  -  description
                             -------------------
    begin                : Tue Dec 18 2001
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

#include "kverbosuser.h"

#include <cstdlib>
#include <qfile.h>
#include <qdatetime.h>

#include <kstandarddirs.h>


bool eintrag::operator== (const eintrag& e) const
{
	return (verb == e.verb);
}

KVerbosUser::KVerbosUser(spanishVerbList* pL, QString n/*=DEFAULTUSER*/)
: name(n), auswahl(""), sessions(0)
{
	// try to read the user-information if there is one.
	int pos;
	while ((pos = n.find(' ')) != -1)
		n.replace(pos, 1, "_");
	QFile file(locateLocal("data", "/kverbos/data/"+name+".kverbos"));
	if ( file.open(IO_ReadOnly) )
	{
		QString s;
		QTextStream t( &file );
		name = t.readLine();
		s = t.readLine();	sessions = s.toInt();
		for (int i=0; i<MAX_RESULTS; i++)
		{
			date[i] = t.readLine();
			s = t.readLine(); result[i][0] = s.toInt();
			s = t.readLine(); result[i][1] = s.toInt();
		};
		while (!file.atEnd())	
		{
			eintrag e;
			e.verb = t.readLine();
			s = t.readLine(); e.status = s.toInt();
			s = t.readLine(); e.right = s.toInt();
			s = t.readLine(); e.wrong = s.toInt();
			if (e.status == 1)
				e.counter = STAY_IN_R_LIST;
			else
				e.counter = 0;
			e.used = false;
			it = liste.find(e);
			if (liste.end() == it)
				liste.append(e);
		};
		file.close();
	}
	else
		for (int i=0; i<MAX_RESULTS; i++)
		{
			date[i] = "";
			result[i][0] = result[i][1] = 0;
		};
	fillList(pL);
	it = liste.begin();
	sessions = (sessions + 1) & 1073741823;
}

KVerbosUser::~KVerbosUser()
{
}

// used zeigt an, ob dieses Verb in der Verbenliste des Hauptprogrammes vorkommt.
// Hier werden die Verben des Hauptprogrammes bertragen, deshalb bekommen alle Elemente
// used = true. Es k�nen aus der Benutzerinformation aber schon Verben in der Liste sein,
// die nicht in der Verbendatei des Hauptprogrammes vorkommen. Diese Verben erhalten alle
// used=false am Anfang. Wird ein Verb also in der Liste gefunden, dann muss zur Sicherheiet
// used=true gesetzt werden.
void KVerbosUser::fillList(spanishVerbList* pL)
{
	deaktivieren();
	for (spanishVerbList::Iterator it2 = pL->begin(); it2 != pL->end(); ++it2)
	{
		eintrag elem;
		elem.verb = (*it2).getSpanish();
		elem.right = elem.wrong = elem.counter = elem.status = 0;
		elem.used = true;
		it = liste.find(elem);
		if (liste.end() == it)
			liste.append(elem);
		else
			(*it).used = true;
	};
}

// alle Verben werden auf nicht genutzt gesetzt.
void KVerbosUser::deaktivieren()
{
	verbenListe::Iterator zeig;
	for (zeig=liste.begin(); zeig!=liste.end(); ++zeig)
		(*zeig).used = false;
}

// tests if verbs that have status RICHTIGGEMACHT have to get the status UNGELERNT.
// And calculates the kumulus of the verbs that have status UNGELERNT and FEFHLERHAFT.
// Der kumulus ist ein gewichteter Z�ler fr die Auswahl des Verbes. Fehlerhafte Verben
// haben ein gr�eres Gewicht und kommen deshalb eher wieder an die Reihe.
void KVerbosUser::updateKumulus()
{
	int k = 0;
	kumulus = 0;
	verbenListe::Iterator zeig;
	for (zeig=liste.begin(); zeig!=liste.end(); ++zeig)
	{
		if ((*zeig).status == RICHTIGGEMACHT)
		{
			if ((*zeig).used && ((*zeig).counter>1))
				(*zeig).counter--;
			else
			(*zeig).status--;
		};
		if ((*zeig).used && ((*zeig).status < RICHTIGGEMACHT))
		{
			kumulus = k;
			if ((*zeig).status == UNGELERNT)
				k = ((*zeig).right < (*zeig).wrong) ? k+2 : k+1;
			else
				k += ((*zeig).wrong) * FALSCHGEWICHT;
			(*zeig).counter = k;
		};
	};
}

// selects a verb out of the verbs that are in the U and F list. That means a
// verb that hasn't been studied yet or one that was wrong.
QString KVerbosUser::suggestVerb()
{
	auswahl = "";
	it = liste.end();
	updateKumulus();
	if ((kumulus == 0) && !liste.isEmpty())
	{
		verbenListe::Iterator zeig;
		for (zeig=liste.begin(); zeig!=liste.end(); ++zeig)
			(*zeig).status = 0;
		// rekursiver Aufruf. Es ist kein Problem, weil die Liste nicht leer ist und alle
		// Elemente den status 0 bekommen haben, also kann eine Auswahl stattfinden.
		auswahl = suggestVerb();
	};
	if (kumulus != 0)
	{
		// RAND_MAX ist auf meinem System 231;
		int i = rand() % kumulus;
		verbenListe::Iterator zeig = liste.begin();
		while ((zeig != liste.end()) && (((*zeig).status==RICHTIGGEMACHT)||((*zeig).counter < i)))
			zeig++;
		auswahl = (*zeig).verb;
		it = zeig;
	};
	return auswahl;
}

// The solution for the suggested verb was right. Now the verb should be deleted from
// the list of unstudied verbs but added to the list with the right verbs.
void KVerbosUser::right()
{
	if (!auswahl.isEmpty())
	{
		(*it).right = ((*it).right+1) & MAX_WERT;
		if ((*it).wrong > 0)
			(*it).wrong--;
		if ((*it).status < RICHTIGGEMACHT)
			(*it).status++;
		if ((*it).status == RICHTIGGEMACHT)
			(*it).counter = STAY_IN_R_LIST;
	};
}

// The opposite function to the above one. The solution was wrong and the verb has to
// be moved to the list with the wrong verbs.
void KVerbosUser::wrong()
{
	if (!auswahl.isEmpty())
	{
		(*it).wrong = ((*it).wrong+1) & MAX_WERT;
		if ((*it).right > 0)
			(*it).right--;
		if ((*it).status > FEHLERHAFT)
			(*it).status--;
	};
}

// Saves all the user information to the $KDEHOME/apps/kverbos/data/username.kverbos file
bool KVerbosUser::saveUser(const int& res, const int& num)
{
	bool re = false;
	if (name != DEFAULTUSER)
	{
		int pos;
		while ((pos = name.find(' ')) != -1)
			name.replace(pos, 1, "_");		
		QFile file(locateLocal("data", "/kverbos/data/"+name+".kverbos"));
		if ( file.open(IO_WriteOnly) )
		{
			QTextStream t( &file );
			t << name << endl;
			sessions = (num == 0) ? sessions - 1 : sessions;
			t << sessions << endl;
			int j = (num == 0) ? 0 : 1;
			for (int i=j; i<MAX_RESULTS; i++)
			{
				t << date[i] << endl;
				t << result[i][0] << endl;
				t << result[i][1] << endl;
			};
			if (j == 1)
			{
				QDate d = QDate::currentDate();
				t << d.toString() << endl;
				t << res << endl;
				t << num << endl;
			};
			for (it=liste.begin(); it!=liste.end(); ++it)	
		{
			if ((*it).status != UNGELERNT)
			{
				t << (*it).verb << endl;
				t << (*it).status << endl << (*it).right << endl << (*it).wrong << endl;
			};
		};
			re = true;
			file.close();	
		};
	};
	return re;
}

bool KVerbosUser::getResults(int& s, QString d[], int r[][2])
{
	s = sessions;
	for (int i=0; i<MAX_RESULTS; i++)
	{
		d[i] = date[i];
		for (int j=0; j<2; j++)
			r[i][j] = result[i][j];
	};
	return true;
}





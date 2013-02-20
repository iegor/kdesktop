/***************************************************************************
                          verbspanish.cpp  -  description
                             -------------------
    begin                : Sun Dec 23 2001
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

#include "verbspanish.h"

// include files for Qt

// application specific includes

endingsArray verbSpanish::verbEndings = { { {"o", "as", "a", "amos", "áis", "an"}, /* presente */
                          {"o", "es", "e", "emos", "éis", "en"},
                          {"o", "es", "e", "imos", "ís", "en"}},
                        { {"aba", "abas", "aba", "ábamos", "abais", "aban"}, /*imperfecto */
                          {"ía", "ías", "ía", "íamos", "íais", "ían"},
                          {"ía", "ías", "ía", "íamos", "íais", "ían"}},
                        { {"é", "aste", "ó", "amos", "asteis", "aron"}, /* indefinido */
                          {"í", "iste", "ió", "imos", "isteis", "ieron"},
                          {"í", "iste", "ió", "imos", "isteis", "ieron"}},
                        { {"é", "ás", "á", "emos", "éis", "án"}, /* futuro */
                          {"é", "ás", "á", "emos", "éis", "án"},
                          {"é", "ás", "á", "emos", "éis", "án"}},
                        { {"ía", "ías", "ía", "íamos", "íais", "ían"}, /* condicional */
                          {"ía", "ías", "ía", "íamos", "íais", "ían"},
                          {"ía", "ías", "ía", "íamos", "íais", "ían"}},
                        { {"e", "es", "e", "emos", "éis", "en"}, /* subjuntivo presente */
                          {"a", "as", "a", "amos", "áis", "an"},
                          {"a", "as", "a", "amos", "áis", "an"}},
                        { {"-", "-", "-", "-", "-", "-"}, /* subjuntivo pasado */
                          {"-", "-", "-", "-", "-", "-"},
                          {"-", "-", "-", "-", "-", "-"}},
                        { {"%", "%", "%", "%", "%", "%"}, /* subjuntivo futuro */
                          {"%", "%", "%", "%", "%", "%"},
                          {"%", "%", "%", "%", "%", "%"}},
                        { {"+", "+", "+", "+", "+", "+"}, /* imperativo */
                          {"+", "+", "+", "+", "+", "+"},
                          {"+", "+", "+", "+", "+", "+"}}
                      };

QString verbSpanish::refPron[PERSONEN] = {"me", "te", "se", "nos", "os", "se"};
QString verbSpanish::estar[MAX_TIEMPOS][PERSONEN] =
                            { {"estoy", "estás", "está", "estamos", "estáis", "están"},
                              {"estaba", "estabas", "estaba", "estábamos", "estabais", "estaban"},
                              {"estuve", "estuviste", "estuvo", "estuvimos", "estuvisteis", "estuvieron"},
                              {"estaré", "estarás", "estará", "estaremos", "estaréis", "estarán"},
                              {"estaría", "estaría", "estaría", "estaríamos", "estarías", "estarían"},
                              {"esté", "estés", "esté", "estemos", "estéis", "estén"},
                              {"estuviera o estuviese", "estuvieras o stuvieses", "estuviera o estuviese",
                               "estuviéramos o estuviésemos", "estuvierais o estuvieseis", "estuvieran o estuviesen"},
                              {"estuviere", "estuvieres", "estuviere", "estuviéremos", "estuviereis", "estuvieren"},
                              {"", "está", "esté", "estemos", "estad", "estén"}
                            };

QString verbSpanish::haber[MAX_TIEMPOS][PERSONEN] =
                              { {"he", "has", "ha", "hemos", "habéis", "han"},
                              {"había", "habías", "había", "habíamos", "habíais", "habían"},
                              {"hube", "hubiste", "hubo", "hubimos", "hubisteis", "hubieron"},
                              {"habré", "habrás", "habrá", "habremos", "habréis", "habrán"},
                              {"habría", "habrías", "habría", "habríamos", "habríais", "habrían"},
                              {"haya", "hayas", "haya", "hayamos", "hayáis", "hayan"},
                              {"hubiera o hubiese", "hubieras o hubieses", "hubiera o hubiese",
                               "hubiéramos o hubiésemos", "hubierais o hubieseis", "hubieran o hubiesen"},
                              {"hubiere", "hubieres", "hubiere", "hubiéremos", "hubiereis", "hubieren"},
                              {"", "he", "haya", "hayamos", "habed", "hayan"}
                            };

QString verbSpanish::timeNames[MAX_TIEMPOS*MAX_SUBTYPES] =
                        {"presente", "imperfecto", "indefinido", "futuro", "condicional",
                         "subjuntivo presente", "subjuntivo pasado", "subjuntivo futuro", "imperativo",
                         "presente progresivo", "imperfecto progresivo", "indefinido progresivo", "futuro progresivo", "condicional progresivo",
                         "subjuntivo presente progresivo", "subjuntivo pasado progresivo", "subjuntivo futuro progresivo", "error:form doesn't exist",
                         "presente perfecto", "imperfecto perfecto", "error:form doesn't exist", "futuro perfecto", "condicional perfecto",
                         "subjuntivo presente perfecto", "subjuntivo pasado perfecto", "subjuntivo futuro perfecto", "error:form doesn't exist",
                        };

QString verbSpanish::groupNames[MAX_GRUPPEN] =
                        {"regular", "irregular", "e > ie", "o > ue", "u > ue", "e > i",
                         "c > qu",  "g > gu", "z > c", "gu > gü", "c > z", "g > j",
                         "gu > g",  "qu > c", "accent like 'enviar'",
                         "accent like 'continuar'", "missing i"};


int verbSpanish::maxTiempos = MAX_TIEMPOS;
int verbSpanish::maxSubtypes = MAX_SUBTYPES;
int verbSpanish::persons = PERSONEN;
int verbSpanish::maxForms = MAX_TIEMPOS * MAX_SUBTYPES;


verbSpanish::verbSpanish()
{
	foreign = "";
	spanish = "";
	gerund = "";
	partizip = "";
	group = REGULAR;
	conjugated = false;
	error = false;
	int z;
	int p;
	for (z = presente; z <= imperativo; z++)
	{
		for (p = yo; p <= ellos; p++)
		{
			forms[z][p] = "";
		};
	};
}

verbSpanish::verbSpanish(const verbSpanish& v)
{
	foreign = v.foreign;
	spanish = v.spanish;
	group = v.group;
	if (IRREGULAR != group)
	{
		gerund = "";
		partizip = "";
		conjugated = false;
		error = v.error;
		int z;
		int p;
		for (z = presente; z <= imperativo; z++)
			for (p = yo; p <= ellos; p++)
				forms[z][p] = "";
	}
	else
	{
		gerund = v.gerund;
		partizip = v.partizip;
		error = v.error;
		conjugated = v.conjugated;
		int z;
		int p;
		for (z = presente; z <= imperativo; z++)
			for (p = yo; p <= ellos; p++)
				forms[z][p] = v.forms[z][p];
	};
}

verbSpanish::~verbSpanish()
{
	// momentan hat die Klasse gar keine dynamisch erstellten
	// Elemente, insofern ist auch der Destruktor recht arbeitslos
}

verbSpanish& verbSpanish::operator= (const verbSpanish& v)
{
	if (this == &v)
		return *this;						// Selbstzuweisung abfangen
	foreign = v.foreign;
	spanish = v.spanish;
	group = v.group;
	error = v.error;
	if (group != IRREGULAR)
	{
		gerund = "";
		partizip = "";
		conjugated = false;
		int z;
		int p;
		for (z = presente; z <= imperativo; z++)
			for (p = yo; p <= ellos; p++)
				forms[z][p] = "";
	}
	else
	{
		gerund = v.gerund;
		partizip = v.partizip;
		conjugated = v.conjugated;
		int z;
		int p;
		for (z = presente; z <= imperativo; z++)
			for (p = yo; p <= ellos; p++)
				forms[z][p] = v.forms[z][p];
	};
	return *this;
}

// Der Vergleichsoperator prüft, ob zwei verb-Objekte das gleiche Verb meinen. Dazu
// werden die spanische und die fremdsprachliche Form geprüft.
bool verbSpanish::operator== (const verbSpanish& v) const
{
	return ((spanish == v.spanish) && vergleich(foreign, v.foreign));
}

// prüft ob der QString mit der spanischen Form übereinstimmt.
bool verbSpanish::operator== (const QString& s) const
{
	return (spanish == s);
}

// prüft die alphabetische Reihenfolge zweier Verben, dabei werden die spanischen Verben als
// Vergleichskriterium verwendet.
bool verbSpanish::operator< (const verbSpanish& v) const
{
	return (spanish < v.spanish) || ((spanish == v.spanish) && (foreign < v.foreign));
}


// Vergleicht die beiden Verbobjekte komplett, mit allen Formen und Feldern.
bool verbSpanish::allEqual(verbSpanish v)
{
	bool result = ((spanish == v.spanish) && (foreign == v.foreign) && (group == v.group));
	if (result && (IRREGULAR == group))
	{
		if (!conjugated)
			conjugateAll();
		if (v.conjugated)
			v.conjugateAll();
		result = ((delAutoForm(gerund) == delAutoForm(v.gerund)) && (delAutoForm(partizip) == delAutoForm(v.partizip)));
		if (result)
		{
			int z;
			int p;
			for (z = presente; z <= imperativo; z++)
				for (p = yo; p <= ellos; p++)
					if (delAutoForm(forms[z][p]) != delAutoForm(v.forms[z][p]))
						result = false;
		};
	};
	return result;
}

// prüft, ob alle Stringfelder des Objektes leere Strings sind.
bool verbSpanish::isEmpty() const
{
	bool result = (spanish.isEmpty() && foreign.isEmpty());
	if (result && (IRREGULAR == group))
	{
		result = (gerund.isEmpty() && partizip.isEmpty());
		if (result)
		{
			int z;
			int p;
			for (z = presente; z <= imperativo; z++)
				for (p = yo; p <= ellos; p++)
					if (!forms[z][p].isEmpty())
						result = false;
		};
	};
	return result;
}

// Gibt den Verbstamm zurück. Dazu werden die letzten beiden Zeichen abgetrennt,
// handelt es sich um ein reflexives Verb, dann sind es vier Zeichen.
// Hier werden auch die Stammveränderungen der Gruppenverben durchgeführt.
QString verbSpanish::getStem() const
{
	int ab = 2;
	if (isReflexiv())
		ab += 2;
	QString s = spanish;
	s.truncate(s.length()-ab);
	return s;
}

// gibt das Verb in der nicht spanischen Sprache zurück
QString verbSpanish::getForeign() const
{
	return foreign;
}

// Setzt die nicht-spanische Bedeutung des Verbes. Dabei werden alle führenden und
// folgenden whitespaces gelöscht und alle im string enthaltenen whitespaces zu einem
// Leerzeichen umgewandelt.
// Die Funktion hat den Nebeneffekt, dass auch der String aus dem aufrufenden Program-
// code umgewandelt wird.
// Es wird nicht geprüft, ob der string keine Leerzeichen mehr enthält, denn das
// dürfte eigentlich nicht vorkommen.
void verbSpanish::setForeign(const QString &s)
{
	foreign = s.simplifyWhiteSpace();
}

// gibt das Verb auf spanisch zurück
QString verbSpanish::getSpanish() const
{
	return spanish;
}

// Setzt die spanische Bedeutung des Verbes.
// Siehe auch die Bemerkungen zur Arbeitsweise der Funktionbei der setForeign Funktion.
void verbSpanish::setSpanish(const QString &s)
{
	spanish = s.simplifyWhiteSpace();
}

// gibt die Gerundform des Verbes zurück. Dabei gilt: Alle Gruppenverben haben einen
// regelmäßigen Gerund. Nur die irregulären Verben haben eventuell auch eine andere Gerundform
QString verbSpanish::getGerund()
{
	if (!conjugated)
		conjugateAll();
	return delAutoForm(gerund);
}

// setzt die Gerundform des Verbes
// Siehe auch die Bemerkungen zur Arbeitsweise der Funktionbei der setForeign Funktion.
void verbSpanish::setGerund(const QString &s)
{
	gerund = s.simplifyWhiteSpace();
}

// gibt die Partizipform der Verbes zurück.Dabei gilt: Alle Gruppenverben haben ein
// regelmäßiges Partizip. Nur die irregulären Verben haben eventuell auch eine andere Partizipform
QString verbSpanish::getPartizip()
{
	if (!conjugated)
		conjugateAll();
	return delAutoForm(partizip);
}

// setzt die Partizipform der Verbes
// Siehe auch die Bemerkungen zur Arbeitsweise der Funktionbei der setForeign Funktion.
void verbSpanish::setPartizip(const QString &s)
{
	partizip = s.simplifyWhiteSpace();
}

// gibt die Endung des Verbes zurück.
endings verbSpanish::getEnding(bool& good)
{
	if (spanish.isEmpty())
		return empty;
	endings en = empty;
	QString endung = "";
	if (isReflexiv())
	{
		endung = spanish.mid(spanish.length()-4, 2);
	}
	else
	{
		endung = spanish.right(2);
	};
	if (endung == "ar")
	{
		en = ar;
	}
	else
		if ((endung == "ir")||(endung == "ír"))
		{
			en = ir;
		}
		else
			if (endung == "er")
			{
				en = er;
			}
			else
			{
				good = false;
				// qWarning("The class verb couldn't find the ending of the verb " + spanish + ".");
			};
	return en;	
}

// gibt zurück, ob das Verb reflexiv ist. Dazu werden die letzten beiden Zeichen
// untersucht. Sind diese beiden 'se', dann ist es ein reflexives Verb.
bool verbSpanish::isReflexiv() const
{
	return (spanish.right(2) == "se");
}

// gibt das Reflexivpronomen zurück, das zu der angegebenen Person gehört.
QString verbSpanish::getRefPron(int i)
{
	return refPron[i];
}

// prüft, ob es ein unregelmäßiges Verb ist
bool verbSpanish::isGroupVerb()
{
	return (IRREGULAR != group);
}

// setzen, ob ein Verb regelmäßig oder unregelmäßig ist
void verbSpanish::setGroup(const int g)
{
	group = g;
}

// Gibt die angeforderte Verbform des Verbes zurück, die durch die Zeit t,
// die Person p und den Untertyp typ angegeben wird. Mit dieser Funktion können auch
// die zusammengesetzten Zeitforms erfragt werden.
// parts[0] = ref. Pron. part[1] = Hilfsverb parts[2] = verb
bool verbSpanish::getForm(int t, int p, int typ, QString parts[])
{
	bool good = !error;
	if (good)
	{
		if (!conjugated)
			good = conjugateAll();
		// sich um das Reflexivpronomen kümmern
		if (isReflexiv())
		{
			parts[0] = getRefPron(p);
		}
		else
			parts[0] = "";
		// die anderen Teile der Form sammeln
		switch (typ)
		{
			// die normale Zeitenbildung: kein Hilfsverb, Zeitform
		case 0:
			parts[1] = "";
			parts[2] = forms[t][p];
			break;
			// die Verlaufsform der Zeitenbildung
		case 1:
			parts[1] = estar[t][p];
			parts[2] = gerund;
			break;
			// die Perfektform der Zeitenbildung
		case 2:
			parts[1] = haber[t][p];
			parts[2] = partizip;
			break;
		default:
			good = false;
		};
		// Das '#' bei den automatisch erstellten Formen löschen.
		parts[2] = delAutoForm(parts[2]);
	};
	return good;
}

// Eine überladene Funktion, die einen String zurückgibt.
QString verbSpanish::getForm(int t, int p)
{
	QString st[3];
	if (getForm(t, p, normal, st))
		return st[2];
	return "";
}

// setzt die angegebene Verbform mit int als Parameter
// Siehe auch die Bemerkungen zur Arbeitsweise der Funktion bei der setForeign Funktion.
void verbSpanish::setForm(int t, int p, QString s)
{
	forms[t][p] = s.simplifyWhiteSpace();
}

// bildet die Formen, d.h. hängt die Endungen an den unveränderlichen
// Teil an.
// Die Form muss bisher ein leerer String sein.
bool verbSpanish::conjugateTime(const int t, const QString &s)
{
	bool good = true;
	for (int i = yo; i <=ellos; i++)
	{
		if (forms[t][i].isEmpty())
		{
			forms[t][i] = s + verbEndings[t][getEnding(good)][i]+'#';
			if (group > IRREGULAR)
				good = groupVerb(forms[t][i], t, i);
		};
	};
	return good;
}

// bildet die Formen des Präsens
// dazu wird an den Verbstamm die entsprechende Endung angehängt
bool verbSpanish::conjugatePresente()
{
	QString s = getStem();
	return conjugateTime(presente, s);
}

// bildet die Formen des Imperfecto
// dazu wird an den Verbstamm die entsprechende Endung angehängt
bool verbSpanish::conjugateImperfecto()
{
	QString s = getStem();
	return conjugateTime(imperfecto, s);
}

// bildet die Formen des Indefinido
// dazu wird an den Verbstamm die entsprechende Endung angehängt
bool verbSpanish::conjugateIndefinido()
{
	QString s = getStem();
	return conjugateTime(indefinido, s);
}

// bildet die Formen des Futuro
// dazu wird die entsprechende Endung an den Infinitiv angehängt. Bei den reflexiven
// Verben muss das 'se' abgetrennt werden.
bool verbSpanish::conjugateFuturo()
{
	QString s = getSpanish();
	if (isReflexiv())
		s = s.left(s.length()-2);
	return conjugateTime(futuro, s);
}

// bildet die Formen des Conditional
// diese Formen werden ebenfalls vom Infinitiv des Verbes abgeleitet
bool verbSpanish::conjugateCondicional()
{
	QString s = getSpanish();
	if (isReflexiv())
		s = s.left(s.length()-2);
	return conjugateTime(condicional, s);
}

// IM ctor der Optionen-Klasse und beim Lesen der Optionen werden die nicht
// fertigen Zeiten abegewählt. NIcht vergessen!
bool verbSpanish::conjugateSubPresente()
{
	// qWarning("Not yet implemented.");
	return true;
}

bool verbSpanish::conjugateSubPasado()
{
	// qWarning("Not yet implemented.");
	return true;
}

bool verbSpanish::conjugateImperativo()
{
	// qWarning("Not yet implemented.");
	return true;
}

// konjugiert alle Zeitenformen
bool verbSpanish::conjugateAll()
{
	bool good = true;
	good = good && conjugatePresente();
	good = good && conjugateImperfecto();
	good = good && conjugateIndefinido();
	good = good && conjugateFuturo();
	good = good && conjugateCondicional();
	good = good && conjugateSubPresente();
	good = good && conjugateSubPasado();
	good = good && conjugateImperativo();
	if (partizip.isEmpty())
	{
		partizip = getStem();
		switch (getEnding(good))
		{
		case ar:	
			partizip += "ado#";
			break;
		case er:
		case ir:
			partizip += "ido#";
			break;
		default:
			good = false;
		};
	};
	if (gerund.isEmpty())
	{
		gerund = getStem();
		switch (getEnding(good))
		{
		case ar:	
			gerund += "ando#";
			break;
		case er:
		case ir:
			// Bei den Verben der Gruppe 16 fällt das i weg.
			if (getGroup() == 16)
				gerund += "endo#";
			else
			{
				// Bei Verben der Gruppe 5 ändert sich der Stamm für den Gerund
				if (getGroup() == 5)
				{          	  	
					int pos = gerund.findRev("e");
					if (pos>=0)
					{
						gerund.replace(pos, 1, "i");	
					};
				}
				gerund += "iendo#";
			};
			break;
		default:
			good = false;
		};
	};
	conjugated = true;
	error = !good;
	return good;
}

// Stringvergleich, bei dem es nur auf ähnlich Bestandteile ankommt.
// Bestandteile sind durch Komma getrennt.
bool verbSpanish::vergleich(const QString& s1, const QString& s2) const
{
	bool gleich=false;
	unsigned int l=0;
	int r=s1.find(",", l);
	while ((l<s1.length()) && (false==gleich))
	{
		if (-1 == r)
		  r = s1.length();
		if (s2.find(s1.mid(l, r-l).simplifyWhiteSpace(), 0) != -1)
		{
		  gleich = true;
		}
		else
		{
			l = r+1;
			r=s1.find(",", l);
		};
	};
	return gleich;
}

// returns true if the form is automatic generated and has a '#' on the right end.
bool verbSpanish::isAutoForm(QString& s) const
{
	if ("#" == s.right(1))
		return true;
	return false;
}

// löscht das '#' einer automatisch generierten Form
QString verbSpanish::delAutoForm(QString s) const
{
	if (isAutoForm(s))
		s = s.left(s.length()-1);
	return s;
}

// does the changes for group verbs
//  The verbs are divided in several groups:
//   	 0 -> regular
//     1 -> irregular
//     2 -> <e -> ie>
//     3 -> <o -> ue>
//     4 -> <u -> ue>
//     5 -> <e -> i>
//     6 -> <c -> qu>
//     7 -> <g -> gu>
//     8 -> <z -> c>
//     9 -> <gu -> gü>
//    10 -> <c -> z>
//    11 -> <g -> j>
//    12 -> <gu -> g>
//    13 -> <qu -> c>
//    14 -> <accent like 'enviar'>
//    15 -> <accent like 'continuar'>
//    16 -> <missing i>
//
bool verbSpanish::groupVerb(QString &s, int time/*=-1*/, int person/*=-1*/)
{
	bool good = true;
	int pos;
	switch (group)
	{
	case 2: // <e -> ie>
		// betrifft den Presente und den Subjuntivo Presente und einige Imperativformen
		// dabei jeweils die Personen 1-3 im Singular und die 3. Person Plural.
		// Subjuntivo Presente wird hier nicht bearbeitet, weil der von der ersten Person
		// Singular abgeleitet wird und die sollte schon richtig sein.
		if ((presente == time) || (imperativo == time))
		{
			if ((nosotros != person) && (vosotros != person))
			{
				pos = s.findRev("e");
				if (pos>=0)
					s.replace(pos, 1, "ie");
			};
		};
		break;
	case 3: // <o -> ue>
		// betrifft den Presente und den Subjuntivo Presente und einige Imperativformen
		// dabei jeweils die Personen 1-3 im Singular und die 3. Person Plural.
		// Subjuntivo Presente wird hier nicht bearbeitet, weil der von der ersten Person
		// Singular abgeleitet wird und die sollte schon richtig sein.
		if ((presente == time) || (imperativo == time))
		{
			if ((nosotros != person) && (vosotros != person))
			{
				pos = s.findRev("o", -3);
				if (pos>=0)
					s.replace(pos, 1, "ue");
			};
		};	
		break;
	case 4: // <u -> ue>
		// betrifft den Presente und den Subjuntivo Presente und einige Imperativformen
		// dabei jeweils die Personen 1-3 im Singular und die 3. Person Plural.
		// Subjuntivo Presente wird hier nicht bearbeitet, weil der von der ersten Person
		// Singular abgeleitet wird und die sollte schon richtig sein.
		if ((presente == time) || (imperativo == time))
		{
			if ((nosotros != person) && (vosotros != person))
			{
				pos = s.findRev("u");
				if (pos>=0)
					s.replace(pos, 1, "ue");
			};
		};	
		break;
	case 5: // <e -> i>
		// Betrifft Präsens (1.-3. Pers Singular und 3. Person Plural)
		// die Gerundform, die wird allerdings nicht hier behandelt, sondern in conjugateAll()
		// indefinido 3. Person Sg und Pl.
		// alle subjuntivo Formen und den imperativo
		if ((presente == time) && (person != nosotros) && (person != vosotros))
		{
			QString st = getStem();
			pos = st.findRev("e");
			if (pos>=0)
			{
				st.replace(pos, 1, "i");	
				s = st + verbEndings[time][getEnding(good)][person];
			};
		}
		else
		if ((indefinido == time) && ((el == person)||(ellos == person)))
		{
			QString st = getStem();
			pos = st.findRev("e");
			if (pos>=0)
			{
				st.replace(pos, 1, "i");	
				s = st + verbEndings[time][getEnding(good)][person];
			};	
		}
		else
			if (imperativo == time)
			{
				good = false;
				// qWarning("Imperativo for group verbs isn't ready.");
			};	
		break;
	case 6: // <c -> qu> (vor e wird c zu qu)
		pos = s.findRev("ce");
		if (pos>=0)
			s.replace(pos, 2, "que");			
		else
		{
			pos = s.findRev("cé");
			if (pos>=0)
				s.replace(pos, 2, "qué");
		}
		break;
	case 7: // <g -> gu> (vor e wird g zu gu)
		pos = s.findRev("ge");
		if (pos>=0)
			s.replace(pos, 2, "gue");		
		else
		{
			pos = s.findRev("gé");
			if (pos>=0)
				s.replace(pos, 2, "gué");
		}
		break;
	case 8: // <z -> c> (vor e wird z zu c)
		pos = s.findRev("ze");
		if (pos>=0)
			s.replace(pos, 2, "ce");	
		else
		{
			pos = s.findRev("zé");
			if (pos>=0)
				s.replace(pos, 2, "cé");
		}
		break;
	case 9: // <gu -> gü> (vor e wird gu zu gü)
		pos = s.findRev("gue");
		if (pos>=0)
			s.replace(pos, 3, "güe");
		else
		{
			pos = s.findRev("gué");
			if (pos>=0)
				s.replace(pos, 3, "güé");
		}
	break;
	case 10: // <c -> z> (vor o und a wird c zu z)
		pos = s.findRev("co");
		if (pos>=0)
			s.replace(pos, 2, "zo");
		else
		{
			pos = s.findRev("ca");
			if (pos>=0)
				s.replace(pos, 2, "za");
		};
		break;
	case 11: // <g -> j> (vor o und a wird g zu j)
		pos = s.findRev("ga");
		if (pos>=0)
			s.replace(pos, 2, "ja");
		else
		{
			pos = s.findRev("go");
			if (pos>=0)
				s.replace(pos, 2, "jo");
		};
		break;
	case 12: // <gu -> g> (vor o und a wird gu zu g)
		pos = s.findRev("guo");
		if (pos>=0)
			s.replace(pos, 3, "go");
		else
		{
			pos = s.findRev("gua");
			if (pos>=0)
				s.replace(pos, 3, "ga");
		};
		break;
	case 13: // <qu -> c> (vor o und a wird qu zu c)
		pos = s.findRev("quo");
		if (pos>=0)
			s.replace(pos, 3, "co");
		else
		{
			pos = s.findRev("qua");
			if (pos>=0)
				s.replace(pos, 3, "ca");
		};
		break;
	case 14: // <accent like 'enviar'>
		// Änderungen betreffen die Präsensformen außer nosotros und vosotros. Außerdem den
		// Subjuntivo presente, der von der 1.Person singular abgeleitet wird und daher hier nicht
		// behandelt werden muss. Des Weiteren sind einige Imperativformen betroffen.
		if (presente == time)
		{
			switch (person)
			{
			case yo:
				s = s.left(s.length()-3) + "ío#";
				break;
			case tu:
				s = s.left(s.length()-4) + "ías#";
				break;
			case el:
				s = s.left(s.length()-3) + "ía#";
				break;
			case ellos:
				s = s.left(s.length()-4) + "ían#";
				break;
			default:
				{};
			};
		}
		else
			if (imperativo == time)
			{
				switch (person)
				{
				case tu:
					s = s.left(s.length()-3) + "ía#";
					break;
				case el:
					s = s.left(s.length()-3) + "íe#";
					break;
				case ellos:
					s = s.left(s.length()-4) + "íen#";
					break;
				default:
					{};
				};	
			};	
		break;
	case 15: // <accent like 'continuar'>
		// Änderungen betreffen die Präsensformen außer nosotros und vosotros. Außerdem den
		// Subjuntivo presente, der von der 1.Person singular abgeleitet wird und daher hier nicht
		// behandelt werden muss. Des Weiteren sind einige Imperativformen betroffen.
		if (presente == time)
		{
			switch (person)
			{
			case yo:
				s = s.left(s.length()-3) + "úo#";
				break;
			case tu:
				s = s.left(s.length()-4) + "úas#";
				break;
			case el:
				s = s.left(s.length()-3) + "úa#";
				break;
			case ellos:
				s = s.left(s.length()-4) + "úan#";
				break;
			default:
				{};
			};
		}
		else
			if (imperativo == time)
			{
				switch (person)
				{
				case tu:
					s = s.left(s.length()-3) + "úa#";
					break;
				case el:
					s = s.left(s.length()-3) + "úe#";
					break;
				case ellos:
					s = s.left(s.length()-4) + "úen#";
					break;
				default:
					{};
				};	
			};
		break;
	case 16: // <missing i>
		// Änderungen betreffen die Gerund-Form, den Indefinido in der 3. Person Singular
		// und Plural und den Subjuntivo Pasado. Hier wird der Subjuntivo nicht bearbeitet,
		// weil er von der 3. Person des Indefinido abgeleitet wird. Diese Form sollte bereits
		// geändert sein. Ebenso wird der Gerund in der ConjugateAll()-Funktion behandelt.
		if ("ió#" == s.right(3))		
		{
		 	s = s.left(s.length()-3) + "ó#";
		}
		else
			if ("ieron#" == s.right(6))		
			{
				s = s.left(s.length()-6) + "eron#";
			};	
		break;
	default:
		good = false;	  	
	};
	return good;
}




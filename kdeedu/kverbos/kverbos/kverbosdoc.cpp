/***************************************************************************
                          kverbosdoc.cpp  -  description
                             -------------------
    begin                : Fre Dez 14 19:28:09 CET 2001
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

// include eines Standardheaders
#include <iostream>
#include <ctime>
#include <cstdlib>

// include files for Qt
#include <qmessagebox.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>

// application specific includes
#include "verbspanish.h"
#include "kverbosdoc.h"
#include "kverbosview.h"
#include "kverbosuser.h"




QPtrList<KVerbosView> *KVerbosDoc::pViewList = 0L;

KVerbosDoc::KVerbosDoc(QWidget *parent, const char *name) : QObject(parent, name)
{
	if(!pViewList)
	{
		pViewList = new QPtrList<KVerbosView>();
	};
	pViewList->setAutoDelete(true);
	// Zufallsgenerator mit einem seed-Wert versorgen
	srand((unsigned int)time((time_t *)NULL));
	// Den Zeiger auf einen Benutzer initialisieren
	benutzer = new KVerbosUser(getList(), DEFAULTUSER);
}

KVerbosDoc::~KVerbosDoc()
{
	if (benutzer != 0)
	{
		int result, number;
		pViewList->first()->userChanged(result, number);
		benutzer->saveUser(result, number);
		delete benutzer;
		benutzer = 0;
	};
	verbList2.clear();
}

void KVerbosDoc::addView(KVerbosView *view)
{
	pViewList->append(view);
	connect(this, SIGNAL(changeView()), view, SLOT(zeigeVerb()));
}

void KVerbosDoc::removeView(KVerbosView *view)
{
	pViewList->remove(view);
}

void KVerbosDoc::setURL(const KURL &url)
{
	doc_url=url;
}

const KURL& KVerbosDoc::URL() const
{
	return doc_url;
}

void KVerbosDoc::slotUpdateAllViews(KVerbosView *sender)
{
	KVerbosView *w;
	if(pViewList)
		for(w=pViewList->first(); w!=0; w=pViewList->next())
			if(w!=sender)
				w->repaint();
}

bool KVerbosDoc::saveModified()
{
  bool completed=true;

  if(modified)
  {
    KVerbosApp *win=(KVerbosApp *) parent();
    int want_save = KMessageBox::warningYesNoCancel(win,
                i18n("The current file has been modified.\nDo you want to save it?"),
                i18n("Warning"),KStdGuiItem::save(),KStdGuiItem::discard());
    switch(want_save)
    {
      // ich habe die Nummern gegenüber den originalen Nummern in der case-Anweisung
      // vertauscht, weil die originalen Nummern verkehrt zugeordnet sind!.
      // In der Dokumentation zu KMessageBox kann man die richtige Zuordnung der Buttons finden.
      case 2: // CANCEL
           completed=false;
           break;
      case 3: // YES
           if (doc_url.fileName() == i18n("Untitled"))
           {
             win->slotFileSaveAs();
           }
           else
           {
             saveDocument(URL());
       	   };

       	   deleteContents();
           completed=true;
           break;
      case 4: // NO
           setModified(false);
           deleteContents();
           completed=true;
           break;	
      default: // Verhalten wie CANCEL
           completed=false;
           break;
    }
  }

  return completed;
}

void KVerbosDoc::closeDocument()
{
	deleteContents();
}

bool KVerbosDoc::newDocument()
{
	// Die Verbenlist wird gelöscht, damit sie sicher leer ist.
	verbList2.clear();

	modified=false;
	doc_url.setFileName(i18n("Untitled"));
	emit anzahlVerbenGeaendert(getAnzahl());
	return true;
}

bool KVerbosDoc::openDocument(const KURL& url, const char *format /*=0*/)
{
	bool b = false;
	QString tmpfile;
	KIO::NetAccess::download( url, tmpfile );

	QFile file(url.path());
	if ( file.open(IO_ReadOnly) ) {    // file opened successfully
		deleteContents();
	if ((url.fileName().right(4) == "html")||(url.fileName().right(3) == "htm"))
	{
		// qWarning("öffnen eines HTML-files");
		b = openDocHTML(file);
	}
	else
	{
		// qWarning("öffnen eines .verbos-files");
		b = openDocVERBOS(file);
	};
	file.close();
	// Liste der Verben sortieren
    spanishVerbList vl;
    spanishVerbList::Iterator it1 = verbList2.begin();
    spanishVerbList::Iterator it2;
   	while (it1 != verbList2.end())
    {
			it2 = vl.begin();
   		bool inserted = false;
   		while (it2  != vl.end())
     	{
				if (*it1 < *it2)
    		{
        	vl.insert(it2, *it1);
         	inserted = true;
          it2 = vl.end();
      	}
	      else
  	      it2++;
      };
      if (!inserted)
      	vl.insert(it2, *it1);
      it1++;
    };
    verbList2.clear();
    verbList2 = vl;
    benutzer->fillList(getList());  	
    emit changeView();
  };

  KIO::NetAccess::removeTempFile( tmpfile );

  modified=false;
  emit anzahlVerbenGeaendert(getAnzahl());
  return b;
}

// Liesteine Datei als HTML-Dokument ein
bool KVerbosDoc::openDocHTML(QFile &f)
{
	bool result = true;
	QTextStream t( &f );          // use a text stream
	int count1;
	int count2;
	QString s;
	QString v[3];
	QString forms[60];
	verbSpanish NewVerb;
	while (!f.atEnd())
	{
		count1 = 0;
		// drei einlesen, das sind der Infinitiv, die deutsche Bedeutung, und der Typ
		while ((count1<3) && (!f.atEnd()))
		{
			s = t.readLine();
			if (s.left(9) == "<td width")
			{
				int a = s.find('>', 22, true);
				a++;
				int e = s.find('<', a, true);
				v[count1] = s.mid(a, e-a);
				count1++;
			};
		}; // while ((count1<3) && (!f.atEnd()))
		if (3 == count1)
		{
			NewVerb.setSpanish(v[0]);
			NewVerb.setForeign(v[1]);
			if (v[2] != "unregelm&auml;&szlig;ig")
			{
				// read a regular or a group verb
				int g = 0;
				if (v[2] == "regelm&auml;&szlig;ig")
					g = 0;
				else if (v[2] == "&lt;e &gt; ie&gt;")
					g = 2;
				else if (v[2] == "&lt;o &gt; ue&gt;")
					g = 3;
				else if (v[2] == "&lt;u &gt; ue&gt;")
					g = 4;
				else if (v[2] == "&lt;e &gt; i&gt;")
					g = 5;
				else if (v[2] == "&lt;c &gt; qu&gt;")
					g = 6;
				else if (v[2] == "&lt;g &gt; gu&gt;")
					g = 7;
				else if (v[2] == "&lt;z &gt; c&gt;")
					g = 8;
				else if (v[2] == "&lt;gu &gt; gü&gt;")
					g = 9;
				else if (v[2] == "&lt;c &gt; z&gt;")
					g = 10;
				else if (v[2] == "&lt;g &gt; j&gt;")
					g = 11;
				else if (v[2] == "&lt;gu &gt; g&gt;")
					g = 12;
				else if (v[2] == "&lt;qu &gt; c&gt;")
					g = 13;
				else if (v[2] == "&lt;accent like 'enviar'&gt;")
					g = 14;
				else if (v[2] == "&lt;accent like 'continuar&gt;")
					g = 15;
				else if (v[2] == "&lt;missing i&gt;")
					g = 16;
				else
					result = false;
				NewVerb.setGroup(g);
			}
			else
			{
				// ein unregelmäßiges Verb
				NewVerb.setGroup(IRREGULAR);
				// weitere 60 Zeilen einlesen, das sind die ganzen Formen
				count2 = 0;
				while ((count2<60) && (!f.atEnd()))
				{
					s = t.readLine();
					if ((s.left(4) == "<td>") && (s != "<td></td><td></td><td></td><td></td><td></td></tr><tr>"))
					{
						int a = s.find('>', 2, true);
						a++;
						int e = s.find('<', a, true);
						forms[count2] = s.mid(a, e-a);
						count2++;
						if (14 == count2)
						{
							forms[count2] = "participio";
							count2++;
						};
					};
				}; // while ((count2<60) && (!f.atEnd()))
				int i;
				// den ersten Tabellenteil übertragen
				for (i=0; (i<30) && (i<count2); i++)
				{
					int a = i % 5;
					int b = i / 5;
					// In der fünften ( a == 4 ) Spalte stehen die Gerundform und das Partizip
					if (a == 4)
					{
						if (b == 0)
						NewVerb.setGerund(forms[i]);
						if (b == 3)
						NewVerb.setPartizip(forms[i]);
					}
					else
					{
						// kommt ein Leerzeichen vor, muss man die Form aufteilen: In das
						// Reflexivpronomen und das eigentliche Verb
						if (forms[i].find(' ') >= 0)
						{
							NewVerb.setForm(a, b, forms[i].right(forms[i].length()-forms[i].find(' ')-1));
						}
						else
						{
							NewVerb.setForm(a, b, forms[i]);
						}
					};
				};
				// den zweiten Tabellenteil übertragen
				for (i=0; (i<30) && (i+30<count2); i++)
				{
					int a = i % 5 + 4;
					int b = i / 5;
					// In der Spalte 3 der zweiten Tabellenhälfte steht der subjuntivo futuro,
					// den ich vergessen habe und der deshalb zur Zeit ausgelassen wird.
					int offs = 30;
					// kommt ein Leerzeichen vor, muss man die Form aufteilen: In das
					// Reflexivpronomen und das eigentliche Verb      	
					if (forms[i+offs].find(' ') >= 0)
					{
						NewVerb.setForm(a, b, forms[i+offs].right(forms[i+offs].length()-forms[i+offs].find(' ')-1));
					}
					else
					{
						NewVerb.setForm(a, b, forms[i+offs]);
					};	
				};
			}; // if (v[2] == "unregelm&auml;&szlig;ig")
			appendVerb(NewVerb, false); // an die Liste anhängen
		};
	}; // while (!f.atEnd())
	emit anzahlVerbenGeaendert(getAnzahl());
	return result;
}

bool KVerbosDoc::openDocVERBOS(QFile &f)
{
	QTextStream t( &f );          // use a text stream
	int count;
	QString v[3];
	while (!f.atEnd())
	{
		count = 0;
		// drei einlesen, das sind der Infinitiv, die deutsche Bedeutung, und der Typ
		while ((count<3) && (!f.atEnd()))
		{
			v[count] = t.readLine();
			count++;
		}; // while ((count<3) && (!f.atEnd()))
		if (3 == count)
		{
			verbSpanish NewVerb;
			NewVerb.setSpanish(v[0]);
			NewVerb.setForeign(v[1]);
			int	g = v[2].toInt();
			if (IRREGULAR != g)
			{
				// ein regelmäßiges Verb
				NewVerb.setGroup(g);
			}
			else
			{
				// ein unregelmäßiges Verb
				NewVerb.setGroup(IRREGULAR);
				// erst den Gerund und das Partizip einlesen und dann die restlichen Formen.
				NewVerb.setGerund(t.readLine());
				NewVerb.setPartizip(t.readLine());
				count = 0;
				for (int i = presente; i <= imperativo; i++)
					for (int j = yo; j <= ellos; j++)
						if (!f.atEnd())
						{
							NewVerb.setForm(i, j, t.readLine());
							count++;
						};
				if (count < 54)
				{
					QMessageBox mb("kverbos warning", "An uncomplete verb has been found in the file",
									QMessageBox::Warning, 1, 0, 0, 0);
					mb.exec();
					//qWarning("Ein unvollständiges Verb kam vor!");
				};
			};
			appendVerb(NewVerb, false); // an die Liste anhängen
		};
	}; // while (!f.atEnd())
	emit anzahlVerbenGeaendert(getAnzahl());
	return true;
}

// sichert die Datei. Die Verben werden in einer Datei im HTML Format gespeichert.
bool KVerbosDoc::saveDocument(const KURL& url, const char *format /*=0*/)
{
	bool b = false;
	QFile file(url.path());
	if ( file.open(IO_WriteOnly) )
	{    // file opened successfully
		if ((url.fileName().right(4) == "html")||(url.fileName().right(3) == "htm"))
		{
			// qWarning("als HTML speichern");
			b = saveDocHTML(file);
		}
		else
		{
			// qWarning("als .verbos speichern");    	
			b = saveDocVERBOS(file);
		};
	};
	file.close();
	modified=false;
	return b;
}

// Die Verben als HTML-Datei abspeichern
bool KVerbosDoc::saveDocHTML(QFile &f)
{
	bool good = true;
  QTextStream t( &f );           // use a text stream
  // Den Anfang der HTML Datei schreiben
  t << "<html>" << endl;
  t << "<head>" << endl;
  t << "<title>verbos espa&ntilde;oles</title>" << endl;
  t << "<style type=\"text/css\">" << endl;
	t << "<!--" << endl;
 	t << "h1 { font-size:48pt; color:#FF0000; font-family:sans-serif; text-align:center; }" << endl;
 	t << ".n { font-size:12pt; color:#000000; font-family:sans-serif; margin:0pt; }" << endl;
 	t << ".v   { font-size:16pt; color:#0000ff; font-family:sans-serif; }" << endl;
	t << ".r   { font-size:16pt; color:#b0b0ff; font-family:sans-serif; }" << endl; 	
 	t << ".t { background-color:#f0f0f0; padding:4pt; font-size:12pt; color:#000000; font-family:sans-serif; margin:0pt; }" << endl;
	t << "td { background-color:#ffffff; padding:4pt; font-size:12pt; color:#000000; font-family:sans-serif; margin:0pt; }" << endl;
 	t << "//-->" << endl;
  t << "</style>" << endl;
  t << "</head>" << endl;
  t << "<body>" << endl;
  t << "<h1>verbos espa&ntilde;oles</h1>" << endl;
 	t << "<p class=\"n\">";
 	t << "Als regelm&auml;&szlig;ig gelten in diesem Programm alle Verben, die sich " << endl;
 	t << "ohne geringste Abweichung an das Konjugationsschema für die Verben mit den " << endl;
 	t << "Endungen -ar, -er und -ir halten. Die Gruppenverben, die auch eine gewisse " << endl;
 	t << "Regelm&auml;&szlig;keit f&uuml;r eine Gruppe von Verben erkennen lassen, " << endl;
 	t << "m&uuml;ssen als unregelm&auml;&szlig;ige eingegeben werden. </p>" << endl;
 	t << "<p class=\"n\">";
 	t << "Die Formen des 'subjuntivo futuro' werden einstweilen nicht mit in das " << endl;
 	t << "Programm aufgenommen. </p>" << endl;
 	t << "<p class=\"n\">";
 	t << "Diese Datei wurde automatisch durch das Programm KVerbos erstellt. Die " << endl;
 	t << "Verben wurden dabei in einem Format ausgegeben, die das Programm auch " << endl;
 	t << "wieder einlesen kann. Deshalb sollten an der Datei keine &Auml;nderungen " << endl;
 	t << "vorgenommen werden. Durch &Auml;nderungen kann die Datei f&uuml;r das Programm " << endl;
 	t << "unlesbar werden. <br></p>" << endl;
  // Die Verbenliste in die Datei schreiben.
  if (verbList2.isEmpty())
  {
    t << "<p class=\"n\">Es wurden keine Verben eingegeben.</p>" << endl;
  }
  else
  {
    spanishVerbList::Iterator it;
    for (it = verbList2.begin(); it != verbList2.end(); ++it)
    {
    	// erst die spanische und die deutsche Bedeutung abspeichern und den Typ
     	t << "<hr color=#000000 size=3 noshade>" << endl;
   		t << "<table width=100% border=\"0\"><tr>" << endl;
   		t << "<td width=37% class=\"v\">" << (*it).getSpanish() << "</td>" << endl;
     	t << "<td width=37% class=\"v\">" << (*it).getForeign() << "</td>" << endl;
     	if ((*it).isGroupVerb())
     	{
     	  t << "<td width=26% class=\"r\">";
				switch ((*it).getGroup())
				{
					case 0:
						t << "regelm&auml;&szlig;ig</td>" << endl; break;
					case 2:
						t << "&lt;e &gt; ie&gt;</td>" << endl; break;
					case 3:
						t << "&lt;o &gt; ue&gt;</td>" << endl; break;
					case 4:
						t << "&lt;u &gt; ue&gt;</td>" << endl; break;
					case 5:
						t << "&lt;e &gt; i&gt;</td>" << endl; break;
					case 6:
						t << "&lt;c &gt; qu&gt;</td>" << endl; break;
					case 7:
						t << "&lt;g &gt; gu&gt;</td>" << endl; break;
					case 8:
						t << "&lt;z &gt; c&gt;</td>" << endl; break;
					case 9:
						t << "&lt;gu &gt; gü&gt;</td>" << endl; break;
					case 10:
						t << "&lt;c &gt; z&gt;</td>" << endl; break;
					case 11:
						t << "&lt;g &gt; j&gt;</td>" << endl; break;
					case 12:
						t << "&lt;gu &gt; g&gt;</td>" << endl; break;
					case 13:
						t << "&lt;qu &gt; c&gt;</td>" << endl; break;
					case 14:
						t << "&lt;accent like 'enviar'&gt;</td>" << endl; break;
					case 15:
						t << "&lt;accent like 'continuar&gt;</td>" << endl; break;
					case 16:	
						t << "&lt;missing i&gt;</td>" << endl; break;
					default:
						good = false;
						//qWarning("Error! Wrong group!!");
				};
     	}
     	else
     	{
     	  t << "<td width=26% class=\"r\">unregelm&auml;&szlig;ig</td>" << endl;
     	};
     	t << "</tr></table>" << endl;
     	// Im Falle eines unregelmäßigen Verbes, die Formen sichern
     	if ((*it).isGroupVerb() == false)
     	{
   			t << "<table width=100%  border=\"0\">" << endl;
 				t << "<colgroup><col width=20%><col width=20%><col width=20%><col width=20%><col width=20%></colgroup>" << endl;
 				// erster Teil der Tabelle
 				t << "<tr>" << endl;
 				t << "<td class=\"t\">presente</td>" << endl;
 				t << "<td class=\"t\">imperfecto</td>" << endl;
 				t << "<td class=\"t\">indefinido</td>" << endl;
 				t << "<td class=\"t\">futuro</td>" << endl;
 				t << "<td class=\"t\">gerundio</td>" << endl;
 				t << "</tr>" << endl;
 				int i;
 				for (i = yo; i <= ellos; i++)
 				{
 				  t << "<tr>" << endl;
 				  for (int j = presente; j <= futuro; j++)
 				  {
   				  // Formen presente, imperfecto, indefinido, futuro
 	  			  t << "<td>";
		  		  if ((*it).isReflexiv())
              t << (*it).getRefPron(i) << ' ';
 				    t << (*it).getForm(j, i) << "</td>" << endl;
 				  };
 				  // Sonderformen gerundio und participo
 				  switch (i)
 				  {
 				    case 0:
 	            t << "<td>" << (*it).getGerund() << "</td>" << endl;					  					
 							break;
 						case 2:
 						  t << "<td class=\"t\">" << "participo</td>" << endl;					  					
 							break;
 						case 3:
 						  t << "<td>" << (*it).getPartizip() << "</td>" << endl;					  					
 							break;
 						default:
 						  t << "<td></td>" << endl;	
 				  }
 				  t << "</tr>" << endl;
 				}					
 				t << "<tr>" << endl;
 				t << "<td></td><td></td><td></td><td></td><td></td>";
 				t << "</tr>";
        // zweiter Teil der Tabelle
 				t << "<tr>" << endl;
 				t << "<td class=\"t\">condicional</td>" << endl;
 				t << "<td class=\"t\">subjuntivo presente</td>" << endl;
 				t << "<td class=\"t\">subjuntivo imperfecto</td>" << endl;
 				t << "<td class=\"t\">subjuntivo futuro</td>" << endl;
 				t << "<td class=\"t\">imperativo</td>" << endl;
 				t << "</tr>" << endl;
 				for (i = yo; i <= ellos; i++)
 				{
 				  t << "<tr>" << endl;
 				  for (int j = condicional; j <= imperativo; j++)
 				  {
   				  // Formen condicional, subjuntivo presente, subjuntivo imperfecto
   				  // subjuntivo futuro, imperativo
 	  			  t << "<td>";
		  		  if ((*it).isReflexiv())
              t << (*it).getRefPron(i) << ' ';
 				    t << (*it).getForm(j, i) << "</td>" << endl;
			    };
 				  t << "</tr>" << endl;
 				}					
 				t << "</table>" << endl;
     	}
 		  t << "<p></p>" << endl;
 		};
 	};
  // Das Ende der HTML Datei anfügen.
  t << "</body>" << endl;
  t << "</html>" << endl;
  return good;
}

bool KVerbosDoc::saveDocVERBOS(QFile &f)
{
  QTextStream t( &f );           // use a text stream
  // Die Verbenliste in die Datei schreiben.
  if (verbList2.isEmpty())
  {
    t << "Es wurden keine Verben eingegeben." << endl;
  }
  else
  {
    spanishVerbList::Iterator it;
    for (it = verbList2.begin(); it!=verbList2.end(); ++it)
    {
    	// erst die spanische und die deutsche Bedeutung abspeichern und den Typ
   		t << (*it).getSpanish() << endl;
     	t << (*it).getForeign() << endl;
     	t << (*it).getGroup() << endl;	
     	// Im Falle eines unregelmäßigen Verbes, die Formen sichern
     	if ((*it).isGroupVerb() == false)
     	{
			  t << (*it).getGerund() << endl;	
			  t << (*it).getPartizip() << endl;
			  for (int j = presente; j <= imperativo; j++)
 				{
   				for (int i = yo; i <= ellos; i++)
 				  {
 				    t << (*it).getForm(j, i) << endl;
 				  };
 				};
 			};
 		};
 	};
  return true;
}

void KVerbosDoc::deleteContents()
{
 	verbList2.clear();
	emit anzahlVerbenGeaendert(getAnzahl());
}

// Ein Verb wird der Liste hinzugefügt.
void KVerbosDoc::appendVerb(const verbSpanish v, bool emitStatus /*= true*/)
{
  verbList2.append(v);
  if (emitStatus)
    emit anzahlVerbenGeaendert(getAnzahl());
}

// es wird geprüft, ob die Liste leer ist.
bool KVerbosDoc::isEmpty() const
{
  return verbList2.isEmpty();
}
	
// Gibt die Liste des Dokumentes zurück.
spanishVerbList* KVerbosDoc::getList()
{
  return &verbList2;
}

// Diese Funktion ist auch die einzige über die diese Abfrage läuft, deshalb erhält sie auch
// die Aufgabe eine Auswahl anzuregen.
// Reihenfolge in dem Stringfeld: yo, tu, el, nosotros, vosotros, ellos, aleman, espanol
// Wird "- - - - - -" zurückgegeben, dann gibt es kein Verb in der Liste. Normalerweise
// wird die spanische Verbform zurückgegeben.
QString KVerbosDoc::getAuswahl(QString s[][3], int &t)
{
	bool verbOK = true;
	// feststellen, wie viele aktivierte Zeiten in den Optionen vorhanden sind
	int ti[MAX_TIEMPOS*MAX_SUBTYPES], ty[MAX_TIEMPOS*MAX_SUBTYPES];
	bool op[MAX_TIEMPOS][MAX_SUBTYPES];
	int zeitenZahl = 0;
	KVerbosApp *app = dynamic_cast<KVerbosApp*>(parent());
	if (app) {
		app->getTimeOptions(op);
	}
	for (int i=0; i<MAX_TIEMPOS; i++)
		for (int j=0; j<MAX_SUBTYPES; j++)
			if (op[i][j])
			{
				ti[zeitenZahl] = i;
				ty[zeitenZahl] = j;
				zeitenZahl++;
			};
	if ((0 == verbList2.count()) || (0 == zeitenZahl))
	{
		// Die Liste ist leer oder keine Zeit ist ausgewählt.
		for (int i=0; i<PERSONEN+1; i++)
		{
			s[i][0] = "- - - - - -";
			s[i][1] = "- - - - - -";
			s[i][2] = "- - - - - -";
			t = 0;
		};
	}
	else
	{
		spanishVerbList::Iterator it = findVerb(benutzer->suggestVerb());
		if (it == verbList2.end())
		{
			it = verbList2.at(rand() % verbList2.count());
		};
		zeitenZahl = rand() % zeitenZahl;
		// die deutsche und die spanische Form übernehmen
		s[6][0] = (*it).getSpanish();
		s[6][1] = (*it).getForeign();
		// die einzelnen Formen zusammenstellen
		for (int i=0; i<PERSONEN; i++)
		{
			verbOK = verbOK && (*it).getForm(ti[zeitenZahl], i, ty[zeitenZahl], s[i]);
		};
		t = ti[zeitenZahl] + ty[zeitenZahl]*MAX_TIEMPOS;
	};
	if (!verbOK)
	{
		QMessageBox mb("kverbos warning", "There is a problem with the verb " + s[6][0] +
						".\nPlease check the verb in the edit dialog.", QMessageBox::Warning, 1, 0, 0, 0);
		mb.exec();	
		for (int i=0; i<PERSONEN+1; i++)
		{
			s[i][0] = "- - - - - -";
			s[i][1] = "- - - - - -";
			s[i][2] = "- - - - - -";
			t = 0;
		};	
	};
	return s[6][1];
}

spanishVerbList::Iterator KVerbosDoc::findVerb(const QString& s)
{
	spanishVerbList::Iterator it = verbList2.begin();	
	while ((it != verbList2.end()) && (!((*it) == s)))
		it++;
	return it;
}

void KVerbosDoc::solved(const bool b)
{
	if (benutzer!=0)
		if (b)
		  benutzer->right();
		else
		  benutzer->wrong();
}

// sets a new user name, and deletes the old one. If the old one is differet from
// the default name'-------' then some information is stored about the user. The
// Program look if some information about the user is stored.
void KVerbosDoc::setNewUser(const QString n)
{
	if (0 != benutzer)
	{
		int result, number;
		pViewList->first()->userChanged(result, number);
		benutzer->saveUser(result, number);
		delete benutzer;
	};
	// prüfen ob über den Nutzer schon was gespeichert ist.
	benutzer = new KVerbosUser(getList(), n);
}

#include "kverbosdoc.moc"

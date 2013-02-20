/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2001 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002-2003 by StanislavVsinovsky
			    <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */
#include "catalog.h"
#include "catalogsettings.h"
#include "editcmd.h"
#include "dictchooser.h"
#include "kbabeldictbox.h"
#include "regexpextractor.h"
#include "roughtransdlg.h"

#include <qmemarray.h>
#include <qcheckbox.h>
#include <qhbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qtimer.h>
#include <qvgroupbox.h>
#include <qvbox.h>
#include <qwhatsthis.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogress.h>


#include <kdebug.h>

using namespace KBabel;

RoughTransDlg::RoughTransDlg(KBabelDictBox *dict, Catalog *cat
                , QWidget *parent,const char *name)
        : KDialogBase(parent,name,true
                ,i18n("Caption of dialog","Rough Translation") 
                , User1|User2|User3|Close)
        ,catalog(cat)
        ,active(false)
        ,stop(false)
        ,cancel(false)
        ,dictBox(dict)
        ,exactTransCounter(0)
        ,partTransCounter(0)
        ,totalTried(0)
{
    setButtonBoxOrientation(Vertical);
    setButtonText(User1,i18n("&Start"));
    setButtonText(User2,i18n("S&top"));
    setButtonText(User3,i18n("C&ancel"));

    enableButton(User2,false);
    enableButton(User3,false);
    
    QWidget *mw = new QWidget(this);
    setMainWidget(mw);

    QVBoxLayout *mainLayout = new QVBoxLayout(mw);

    configWidget = new QVBox(mw);
    mainLayout->addWidget(configWidget);

    QVGroupBox *box = new QVGroupBox(i18n("What to Translate"),configWidget);

    QHButtonGroup *bBox = new QHButtonGroup(box);
    bBox->setMargin(0);
    bBox->setFrameStyle(QFrame::NoFrame);
    whatBox = bBox;
    untransButton = new QCheckBox(i18n("U&ntranslated entries"),bBox);
    fuzzyButton = new QCheckBox(i18n("&Fuzzy entries"),bBox);
    transButton = new QCheckBox(i18n("T&ranslated entries"),bBox);

    connect(bBox,SIGNAL(clicked(int)),this,SLOT(msgButtonClicked(int)));

    QWhatsThis::add(bBox,i18n("<qt><p><b>What entries to translate</b></p>"
                "<p>Choose here, for which entries of the file KBabel "
                "tries to find a translation. Changed entries are always "
                "marked as fuzzy, no matter which option you choose.</p></qt>"));
    
    box = new QVGroupBox(i18n("How to Translate"),configWidget);
    bBox = new QHButtonGroup(box);
    bBox->setFrameStyle(QFrame::NoFrame);
    bBox->setMargin(0);
    
    searchMatchButton = new QCheckBox(i18n("&Use dictionary settings")
	    ,bBox);

    fuzzyMatchButton = new QCheckBox(i18n("Fu&zzy translation (slow)")
            ,bBox);
    singleWordButton = new QCheckBox(i18n("&Single word translation")
            ,bBox);

    QWhatsThis::add(bBox,i18n("<qt><p><b>How messages get translated</b></p>"
                "<p>Here you can define if a message can only get translated "
                "completely, if similar messages are acceptable or if KBabel "
                "is supposed to try translating "
                "the single words of a message if no translation of the "
                "complete message or similar message was found.</p></qt>"));
 

    box = new QVGroupBox(i18n("Options"),configWidget);

    markFuzzyButton = new QCheckBox(i18n("&Mark changed entries as fuzzy"),box);
    markFuzzyButton->setChecked(true);
    QWhatsThis::add(markFuzzyButton,
            i18n("<qt><p><b>Mark changed entries as fuzzy</b></p>"
          "<p>When a translation for a message is found, the entry "
          "will be marked <b>fuzzy</b> by default. This is because the "
          "translation is just guessed by KBabel and you should always "
          "check the results carefully. Deactivate this option only if "
          "you know what you are doing.</p></qt>"));


    connect(markFuzzyButton, SIGNAL(toggled(bool))
            , this, SLOT(fuzzyButtonToggled(bool)));

    kdeButton = new QCheckBox(i18n("Initialize &KDE-specific entries"),box);
    kdeButton->setChecked(true);
    QWhatsThis::add(kdeButton,
            i18n("<qt><p><b>Initialize KDE-specific entries</b></p>"
          "<p>Initialize \"Comment=\" and \"Name=\" entries "
	  "if a translation is not found. Also, \"NAME OF TRANSLATORS\" "
	  "and \"EMAIL OF TRANSLATORS\" is filled with identity settings.</p></qt>"));

    QVGroupBox *dBox = new QVGroupBox(i18n("Dictionaries"),configWidget);
    configWidget->setStretchFactor(dBox,1);

    QPtrList<ModuleInfo> moduleList = dict->moduleInfos();

    KConfig *config = KGlobal::config();
    KConfigGroupSaver gs(config,"RoughTranslation");
    QStringList selectedList=config->readListEntry("Selected");
    if(selectedList.isEmpty())
    {
        int a = dict->activeModule();
        ModuleInfo *mi = moduleList.at(a);
        if(mi)
        {
            selectedList.append(mi->id);
        }
    }
    dictChooser = new DictChooser(dict,selectedList,dBox,"dictChooser");

    QWhatsThis::add(dictChooser,i18n("<qt><p><b>Dictionaries</b></p>"
                "<p>Choose here, which dictionaries have to be used for "
                "finding a translation. If you select more than one "
                "dictionary, they are used in the same order as they "
                "are displayed in the list.</p>"
		"<p>The <b>Configure</b> button allows you to temporarily "
		"configure selected dictionary. The original settings "
		"will be restored after closing the dialog.</p></qt>"));

    QLabel* label = new QLabel( i18n("Messages:"), mw );
    progressbar = new KProgress(mw,"progressbar");
    progressbar->setTextEnabled(true);
    progressbar->setFormat("%v/%m (%p%)");
    QHBoxLayout* pblayout= new QHBoxLayout(mainLayout);
    pblayout->add(label);
    pblayout->add(progressbar);

    transButton->setChecked(config->readBoolEntry("Translated",false));
    untransButton->setChecked(config->readBoolEntry("Untranslated",true));
    fuzzyButton->setChecked(config->readBoolEntry("Fuzzies",false));

    bool flag = config->readBoolEntry("fuzzyMatch",true);
    fuzzyMatchButton->setChecked(flag);

    flag = config->readBoolEntry("searchMatch",true);
    searchMatchButton->setChecked(flag);

    flag = config->readBoolEntry("singleWord",true);
    singleWordButton->setChecked(flag);

    flag = config->readBoolEntry("kdeSpecific",true);
    kdeButton->setChecked(flag);

    msgButtonClicked(0);
}

RoughTransDlg::~RoughTransDlg()
{
    KConfig *config=KGlobal::config();
    KConfigGroupSaver gs(config,"RoughTranslation");
    config->writeEntry("Selected",dictChooser->selectedDicts());

    bool flag=transButton->isChecked();
    config->writeEntry("Translated",flag);
    flag=untransButton->isChecked();
    config->writeEntry("Untranslated",flag);
    flag=fuzzyButton->isChecked();
    config->writeEntry("Fuzzies",flag);
    flag=singleWordButton->isChecked();
    config->writeEntry("singleWord",flag);
    flag=fuzzyMatchButton->isChecked();
    config->writeEntry("fuzzyMatch",flag);
    flag=searchMatchButton->isChecked();
    config->writeEntry("searchMatch",flag);
    flag=kdeButton->isChecked();
    config->writeEntry("kdeSpecific",flag);

}

void RoughTransDlg::slotUser1()
{
    configWidget->setEnabled(false);
    enableButton(User1,false);
    enableButton(Close,false);
    enableButton(User2,true);
    enableButton(User3,true);

    active=true;
    stop=false;
    cancel=false;
    
    exactTransCounter=0;
    partTransCounter=0;
    totalTried=0;

    QTimer::singleShot(0,this, SLOT(translate()));
}

void RoughTransDlg::translate()
{
    bool markFuzzy = markFuzzyButton->isChecked();
    bool translated = transButton->isChecked();
    bool untranslated = untransButton->isChecked();
    bool fuzzies = fuzzyButton->isChecked();
    bool kdeSpecific=kdeButton->isChecked();

    int total=catalog->numberOfEntries();
    progressbar->setTotalSteps(total);

    QStringList dictList = dictChooser->selectedDicts();

    catalog->applyBeginCommand(0,Msgstr,0);

    bool singleWords=singleWordButton->isChecked();
    bool fuzzyMatch=fuzzyMatchButton->isChecked();
    bool searchMatch=searchMatchButton->isChecked();
    QRegExp contextReg=catalog->miscSettings().contextInfo;
    QChar accelMarker=catalog->miscSettings().accelMarker;
    QRegExp endPunctReg("[\\.?!: ]+$");
    

    for(int i = 0; i < total; i++)
    {
        progressbar->setProgress(i+1);
        kapp->processEvents(100);

        if(stop || cancel) break;

        // FIXME: should care about plural forms
	QString msg=catalog->msgid(i,true).first();
        QString translation;

        // this is KDE specific:
	if( kdeSpecific )
	{
	    if( catalog->pluralForm(i) == NoPluralForm )
	    {
		QString origTrans = catalog->msgstr(i).first();
    		if(msg.find("_: NAME OF TRANSLATORS\\n")==0)
    		{
		    QString authorName;
		    if( !catalog->identitySettings().authorLocalizedName.isEmpty() )
			authorName = catalog->identitySettings().authorLocalizedName;
		    else // fallback to non-localized name
			if( !catalog->identitySettings().authorName.isEmpty() )
			    authorName = catalog->identitySettings().authorName;
			else continue; // there is no name to be inserted
		
		    if( !QStringList::split(',', origTrans).contains(authorName) )
		    {
        		if(origTrans.isEmpty() ) translation=authorName;
			else translation+=origTrans+","+authorName;
		    }
    		}
    		else if(msg.find("_: EMAIL OF TRANSLATORS\\n")==0)
    		{
		    // skip, if email is not specified in settings
		    if( catalog->identitySettings().authorEmail.isEmpty() ) continue;
		
		    if( !QStringList::split(',', origTrans).contains(catalog->identitySettings().authorEmail) )
		    {
			if(origTrans.isEmpty() ) translation=catalog->identitySettings().authorEmail;
			else translation=origTrans+","+catalog->identitySettings().authorEmail;
		    }
    		}
                else if (msg.find("ROLES_OF_TRANSLATORS") == 0)
                {
                  QString temp = "<othercredit role=\\\"translator\\\">\n<firstname></firstname>"
                    "<surname></surname>\n<affiliation><address><email>" + 
                    catalog->identitySettings( ).authorEmail+"</email></address>\n"
                    "</affiliation><contrib></contrib></othercredit>";
                  if (origTrans.isEmpty( ))
                    translation = temp;
                  else if (origTrans.find(catalog->identitySettings( ).authorEmail) < 0)
                    translation = origTrans + "\n" + temp;
                }
                else if (msg.find("CREDIT_FOR_TRANSLATORS") == 0)
                {
                  QString authorName;
                  if (!catalog->identitySettings( ).authorLocalizedName.isEmpty( ))
                    authorName = catalog->identitySettings( ).authorLocalizedName;
                  else if (!catalog->identitySettings( ).authorName.isEmpty( ))
                    authorName = catalog->identitySettings( ).authorName;
                  QString temp = "<para>" + authorName + "\n" + "<email>" + 
                    catalog->identitySettings( ).authorEmail + "</email></para>";
                  if (origTrans.isEmpty( ))
                    translation = temp;
                  else if (origTrans.find(authorName) < 0 && 
                           origTrans.find(catalog->identitySettings( ).authorEmail) < 0)
                    translation = origTrans + "\n" + temp;
                }
	    }
	} 
	else  // not kdeSpecific 
	{
	    // skip KDE specific texts
	    if( msg.find("_: EMAIL OF TRANSLATORS\\n")==0 || msg.find("_: NAME OF TRANSLATORS\\n")==0 || 
                msg.find("ROLES_OF_TRANSLATORS")==0 || msg.find("CREDIT_FOR_TRANSLATORS")==0)
		continue;
	}

	if( translation.isEmpty() ) // KDE-specific translation didn't work
	{
		if( !untranslated && catalog->isUntranslated(i) ) continue;
		if( !translated && !catalog->isUntranslated(i) && !catalog->isFuzzy(i) ) continue;
		if( !fuzzies && catalog->isFuzzy(i) ) continue;
	}

	totalTried++;

        if(msg.contains(contextReg))
        {
            msg.replace(contextReg,"");
        }

	// try exact translation
	QStringList::Iterator dit = dictList.begin();
	while(translation.isEmpty() && dit != dictList.end())
	  {
	    dictBox->setActiveModule(*dit);
	    translation = dictBox->translate(msg);

	    ++dit;
	  }

	if(!translation.isEmpty())
	  {
	    exactTransCounter++;
	  }

	// try search settings translation
	else if (searchMatch) { 
	  QString tr;
	  int score, best_score = 0;
	  dit = dictList.begin();
	  while(dit != dictList.end())
	    {
	      dictBox->setActiveModule(*dit);
	      tr = dictBox->searchTranslation(msg,score);
	      kdDebug() << "Found: " << tr << ", score " << score << endl;

	      if (score > best_score) {
	        kdDebug() << "Best score" << endl;
		translation = tr;
		best_score = score;
	      }

	      ++dit;
	    }

	  if(!translation.isEmpty())
	    {
	      partTransCounter++;
	    }
	}

	// try fuzzy translation
	else if (fuzzyMatch) { 
	  QString tr;
	  int score, best_score = 0;
	  dit = dictList.begin();
	  while(dit != dictList.end())
	    {
	      dictBox->setActiveModule(*dit);
	      tr = dictBox->fuzzyTranslation(msg,score);

	      if (score > best_score) {
		translation = tr;
		best_score = score;
	      }

	      ++dit;
	    }

	  if(!translation.isEmpty())
	    {
	      partTransCounter++;
	    }
	}
	
	kdDebug() << "Best translation so far: " << translation << endl;

	// try single word translation
        if(translation.isEmpty() && singleWords)
	  {
            QStringList wordList;
            QChar accel;
            QString endingPunctuation;
            int pos = msg.findRev(endPunctReg);
            if(pos >= 0)
	      {
                endingPunctuation = msg.right(msg.length()-pos);
	      }
	    
            msg=msg.simplifyWhiteSpace();
            msg=msg.stripWhiteSpace();


            RegExpExtractor te(catalog->tagSettings().tagExpressions);
	    te.setString(msg);
            msg=te.matchesReplaced(" KBABELTAG ");
	    
            QString word;
            int length = msg.length();
            QRegExp digitReg("^[0-9]*$");
            for(int index=0; index < length; index++)
            {
                QChar c=msg[index];
                
                if(c==accelMarker)
                {
                    index++;
                    if(index < length)
                    {
                        if(msg[index].isLetterOrNumber())
                        {
                            word+=msg[index];
                            accel=msg[index];
                        }
                        else if(!word.isEmpty() )
                        {
                            if(!word.contains(digitReg))
                                wordList.append(word);

                            word=QString::null;
                        }
                    }
                    else if(!word.isEmpty())
                    {
                        if(!word.contains(digitReg))
                            wordList.append(word);

                        word=QString::null;
                    }
                        
                }
                else if(c.isLetterOrNumber())
                {
                    word+=c;
                }
                else if(c == '\\')
                {
                    if(index < length-2)
                    {
                        if(msg[index+1]=='n' && msg[index+2].isSpace())
                        {
                            if(!word.isEmpty() && !word.contains(digitReg))
                                wordList.append(word);

                            word=QString::null;
                            
                            wordList.append("\\n\n");
                            index+=2;
                        }
                        else if(!word.isEmpty() )
                        {
                            if(!word.contains(digitReg))
                                wordList.append(word);

                            word=QString::null;
                        }
                    }
                    else if(!word.isEmpty())
                    {
                        if(!word.contains(digitReg))
                            wordList.append(word);

                        word=QString::null;
                    }
                }
                else if(!word.isEmpty())
                {
                    if(!word.contains(digitReg)) {
                        wordList.append(word);
		    }

                    word=QString::null;
                }
            }
	    
	    // handle the last word as well
	    if( !word.isEmpty() ) wordList.append(word);
            
            dit = dictList.begin();
            int wordCounter=0;
            while(wordCounter==0 && dit != dictList.end())
            {
                dictBox->setActiveModule(*dit);

                for(QStringList::Iterator it=wordList.begin();
                        it!=wordList.end(); ++it)
                {
                    if( (*it)=="\\n\n" )
                    {
                        translation+="\\n\n";
                    }
                    else if( (*it)=="KBABELTAG" )
                    {
                        translation+=te.nextMatch();
                    }
                    else
                    {
                        QString trans = dictBox->translate(*it);

                        if(!trans.isEmpty())
                        {
                            wordCounter++;
                            if(!translation.isEmpty())
                            {
                                translation += ' ';
                            }
                            translation += trans;
                        }
                    }
                }

                if(wordCounter==0)
                    translation=QString::null;

                ++dit;
            }

            if(!translation.isEmpty())
            {
                partTransCounter++;
                // try to set the correct keyboard accelerator
                if(!accel.isNull())
                {
                    int index = translation.find(accel,0,false);
                    if(index >= 0)
                    {
                        translation.insert(index,accelMarker);
                    }
                }

                translation+=endingPunctuation;
            }
        }

	// this is KDE specific:
	if(kdeSpecific && translation.isEmpty())
	{
	    if( msg.startsWith("Name=") ) {
		translation="Name=";
		partTransCounter++;
	    }
	    if( msg.startsWith("Comment=") ) {
		translation="Comment=";
		partTransCounter++;
	    }
        }

        if(!translation.isEmpty())
        {
            if(!catalog->isUntranslated(i))
            {
		QStringList msgs = catalog->msgstr(i);
		uint counter = 0;
		for( QStringList::Iterator it = msgs.begin() ; it != msgs.end() ; ++it)
		{
            	    DelTextCmd* delCmd = new DelTextCmd(0
                        ,(*it),counter++);
            	    delCmd->setPart(Msgstr);
            	    delCmd->setIndex(i);
            	    catalog->applyEditCommand(delCmd,0);
		}
            }

	    for( int count=0; count < catalog->numberOfPluralForms(i) ; count++ )
	    {
		InsTextCmd* insCmd = new InsTextCmd(0,translation,count);
        	insCmd->setPart(Msgstr);
        	insCmd->setIndex(i);
        	catalog->applyEditCommand(insCmd,0);
	    }

            if(markFuzzy)
            {
                catalog->setFuzzy(i,true);
            }
        }
    }

    catalog->applyEndCommand(0,Msgstr,0);

    if(stop || cancel)
    {
        if(cancel)
        {
            catalog->undo();
        }
        else
        {
            msgButtonClicked(0);
        }
        progressbar->setProgress(0);
        configWidget->setEnabled(true);
        active = false;
        
        enableButton(User1,true);
        enableButton(Close,true);
        enableButton(User2,false);
        enableButton(User3,false);
        
        return;
    }
    
    showStatistics();
}

void RoughTransDlg::showStatistics()
{
    int nothing=totalTried-partTransCounter-exactTransCounter;
    KLocale *locale = KGlobal::locale();
    QString statMsg = i18n("Result of the translation:\n"
            "Edited entries: %1\n"
            "Exact translations: %2 (%3%)\n"
            "Approximate translations: %4 (%5%)\n"
            "Nothing found: %6 (%7%)")
            .arg( locale->formatNumber(totalTried,0) )
            .arg( locale->formatNumber(exactTransCounter,0) )
            .arg( locale->formatNumber( ((double)(10000*exactTransCounter/QMAX(totalTried,1)))/100) )
            .arg( locale->formatNumber(partTransCounter,0) )
            .arg( locale->formatNumber(((double)(10000*partTransCounter/QMAX(totalTried,1)))/100) )
            .arg( locale->formatNumber(nothing,0) )
            .arg( locale->formatNumber(((double)(10000*nothing/QMAX(totalTried,1)))/100) );

    KMessageBox::information(this, statMsg
            , i18n("Rough Translation Statistics"));
    
    dictChooser->restoreConfig();
    accept();
}

void RoughTransDlg::slotClose()
{
    if(active)
    {
        cancel = true;
        return;
    }
    else
    {
       dictChooser->restoreConfig();
       accept(); 
    }
}

void RoughTransDlg::slotUser2()
{
    stop=true;
}

void RoughTransDlg::slotUser3()
{
    cancel=true;
}

void RoughTransDlg::msgButtonClicked(int id)
{
    if(!transButton->isChecked() && !untransButton->isChecked() 
            && !fuzzyButton->isChecked())
    {
        QButton *button = whatBox->find(id);
        if(button == transButton)
        {
            transButton->setChecked(true);
        }
        else if(button == untransButton)
        {
            untransButton->setChecked(true);
        }
        else if(button == fuzzyButton)
        {
            fuzzyButton->setChecked(true);
        }
    }

    progressbar->setTotalSteps(catalog->numberOfEntries());

    enableButton(User1,catalog->numberOfEntries());
}

void RoughTransDlg::fuzzyButtonToggled(bool on)
{
    if(!on)
    {
        QString msg=i18n("<qt><p>"
          "When a translation for a message is found, the entry "
          "will be marked <b>fuzzy</b> by default. This is because the "
          "translation is just guessed by KBabel and you should always "
          "check the results carefully. Deactivate this option only if "
          "you know what you are doing.</p></qt>");

        KMessageBox::information(this, msg, QString::null,"MarkFuzzyWarningInRoughTransDlg");
    }
}

void RoughTransDlg::statistics(int &total, int& exact, int& part) const
{
    total = totalTried;
    exact = exactTransCounter;
    part = partTransCounter;
}

#include "roughtransdlg.moc"

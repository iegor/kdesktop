/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002-2004 by Stanislav Visnovsky
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

#include <kdatatool.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kinputdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kurl.h>
#include <kio/netaccess.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qvbox.h>

#include "catalogsettings.h"
#include "editcmd.h"
#include "tagextractor.h"
#include "kbabelview.h"
#include "kbabeldictbox.h"
#include "mymultilineedit.h"
#include "hidingmsgedit.h"
#include "roughtransdlg.h"
#include "kbabelsettings.h"
#include "kbprojectsettings.h"

#include "resources.h"

using namespace KBabel;

QPtrList<ModuleInfo> KBabelView::dictionaries()
{
   QPtrList<ModuleInfo> list = dictBox->moduleInfos();

   return list;
}

void KBabelView::configureDictionary(const QString id)
{
   dictBox->configure(id);
}

void KBabelView::editDictionary(const QString id)
{
   dictBox->edit(id);
}


void KBabelView::aboutDictionary(const QString id)
{
   dictBox->aboutModule(id);
}

void KBabelView::informDictionary()
{
    if( isSearching () )
	stopSearch ();

    if( msgstrEdit->isModified() )
	dictBox->setTextChanged(_catalog->msgid(_currentIndex,true)
	    ,*(_catalog->msgstr(_currentIndex).at(msgstrEdit->currentForm()))
	    ,msgstrEdit->currentForm()
	    ,_catalog->comment(_currentIndex));
}

void KBabelView::setNewLanguage()
{
    IdentitySettings s = _catalog->identitySettings();

    dictBox->setLanguage(s.languageCode, s.languageName);

    // setup new plural form number
    int form = msgstrEdit->currentForm();
    if( form >= s.numberOfPluralForms )
	form = s.numberOfPluralForms-1;

    msgstrEdit->setNumberOfPlurals (s.numberOfPluralForms);
    updateSettings();

    if (! _catalog->currentURL().isEmpty())
    {
	updateEditor( form );
    }
}


void KBabelView::wheelEvent(QWheelEvent *e)
{
    if( _catalog->numberOfEntries() == 0 ) return;

    if( (e->state() & ControlButton) && (e->state() & AltButton))
    {
        if(e->delta() > 0)
        {
            gotoPrevFuzzyOrUntrans();
        }
        else
        {
            gotoNextFuzzyOrUntrans();
        }
    }
    else if(e->state() & ControlButton)
    {
        if(e->delta() > 0)
        {
            gotoPrevFuzzy();
        }
        else
        {
            gotoNextFuzzy();
        }
    }
    else if(e->state() & AltButton)
    {
        if(e->delta() > 0)
        {
            gotoPrevUntranslated();
        }
        else
        {
            gotoNextUntranslated();
        }
    }
    else
    {
        if(e->delta() > 0)
        {
            gotoPrev();
        }
        else
        {
            gotoNext();
        }
    }

    e->accept();
}



void KBabelView::roughTranslation()
{
    RoughTransDlg *dlg = new RoughTransDlg(dictBox, _catalog, this
            , "roughtransDlg");

    dlg->exec();

    delete dlg;
}


void KBabelView::updateTags()
{
    bool hadTags = _tags.count() > 0;

    _tags = _catalog->tagList(_currentIndex);

    if(_tagsMenu)
    {
        _tagsMenu->clear();

        QStringList tList;
        QStringList::Iterator it;
        int counter=0;
        for(it=_tags.begin(); it!=_tags.end(); ++it)
        {
	    QString s = *it;
	    if( s.startsWith("&") ) s = "&"+s;
            if(!tList.contains(s))
            {
                _tagsMenu->insertItem(s,counter);
                tList.append(s);
            }
            counter++;
        }
    }

    bool haveTags = (_tags.count() > 0);

    if(isReadOnly())
        haveTags=false;

    if(haveTags != hadTags)
    {
        emit signalNextTagAvailable(haveTags);
        emit signalTagsAvailable(haveTags);
    }

    _currentTag = 0;

    if(haveTags)
    {
	_tagExtractor->setString(_catalog->msgid(_currentIndex).first());
    }
    // if there is no tag, it will set invalid tag
    selectTag();
}

void KBabelView::skipToNextTag()
{
    if( (uint)_currentTag >= _tags.count()-1 ) return;
    ++_currentTag;
    selectTag();
}

void KBabelView::skipToPreviousTag()
{
    if( _currentTag == 0 ) return;
    --_currentTag;
    selectTag();
}

void KBabelView::selectTag()
{
    if( _tagExtractor->countMatches() == 0 ) {
	// no tags, select none
	kdDebug() << "No tags" << endl;
	msgidLabel->selectTag(0,0);
	return;
    }

    // count number of eofs in tag
    uint diff=0;
    // FIXME: what about plural forms
    QString msgid = _catalog->msgid(_currentIndex).first();

    for( uint i = _tagExtractor->matchIndex(_currentTag); i < _tagExtractor->matchIndex(_currentTag)+_tags[_currentTag].length()+1; i++ )
    {
	if( msgid[i] == '\n' ) diff++;
    }
    msgidLabel->selectTag(_tagExtractor->matchIndex(_currentTag),_tags[_currentTag].length()+diff);
    emit signalNextTag (_currentTag);
}

void KBabelView::setTagsMenu(QPopupMenu *menu)
{
    _tagsMenu=menu;

    connect(_tagsMenu,SIGNAL(activated(int)),this,SLOT(insertTag(int)));
}

void KBabelView::modifyMsgstrText(const uint offset, const QString& text, bool clearFirst)
{
    _catalog->applyBeginCommand( _currentIndex, Msgstr ,this);

    if( clearFirst ) msgstrEdit->clear();

    InsTextCmd* insCmd = new InsTextCmd(offset,text,msgstrEdit->currentForm());
    insCmd->setPart(Msgstr);
    insCmd->setIndex(_currentIndex);

    msgstrEdit->processCommand(insCmd,false);
    forwardMsgstrEditCmd(insCmd);

    _catalog->applyEndCommand(_currentIndex, Msgstr,this);

    autoCheck( true ); // check it NOW - it should not be needed, but it is, I don't know why :-(
}

void KBabelView::insertTag(int n)
{
    QString tag = _tagsMenu->text(n);
    if( tag.startsWith( "&&" ) ) tag = tag.remove(0,1); // replace && -> &. && is used for correct menu display

    modifyMsgstrText( msgstrEdit->currentIndex(), tag );
}

void KBabelView::insertNextTag()
{
    if(_currentTag >= (int)_tags.count())
    {
	KNotifyClient::beep();
	return;
    }

    int offset = msgstrEdit->currentIndex();

    _currentTag++;
    selectTag();

    modifyMsgstrText( offset, _tags[_currentTag-1] );
}

void KBabelView::insertNextTagMsgid()
{
    TagExtractor extractor;

    int offset = msgstrEdit->beginOfLastMarkedText(); //msgstrEdit->currentIndex();

    QString s = (*_catalog->msgstr(_currentIndex).at(msgstrEdit->currentForm())).left(offset);

    QString t;

    if( _catalog->pluralForm( _currentIndex ) == KDESpecific )
    {
	    int pos = msgstrEdit->currentIndex();
            int currentFormBegin=s.findRev("\\n",pos);
	    if( currentFormBegin == -1 ) currentFormBegin=0;
	    else currentFormBegin+=3; // skip the newline
	    int currentFormEnd=s.find("\\n",pos);
	    if( currentFormEnd == -1 ) currentFormEnd=s.length();

	    s=s.mid(currentFormBegin,currentFormEnd-currentFormBegin);
    }

    extractor.setString(s);
    uint num= extractor.countMatches();
    if(num >= _tags.count())
    {
	KNotifyClient::beep();
	return;
    }

    t=_tags[num];

    modifyMsgstrText( offset, t );
}

void KBabelView::showTagsMenu()
{
    if(_tagsMenu && _tags.count() > 0)
    {
        int y=msgstrEdit->height()/2;
        int x=msgstrEdit->width()/2;
        _tagsMenu->exec(msgstrEdit->mapToGlobal( QPoint(x,y) ) );

        return;
    }
}


void KBabelView::updateArgs()
{
    bool hadArgs = _args.count() > 0;

    _args = _catalog->argList(_currentIndex);

    if(_argsMenu)
    {
        _argsMenu->clear();

        QStringList tList;
        QStringList::Iterator it;
        int counter=0;
        for(it=_args.begin(); it!=_args.end(); ++it)
        {
	    QString s = *it;
            if(!tList.contains(s))
            {
                _argsMenu->insertItem(s,counter);
                tList.append(s);
            }
            counter++;
        }
    }

    bool haveArgs = (_args.count() > 0);

    if(isReadOnly())
        haveArgs=false;

    if(haveArgs != hadArgs)
    {
        emit signalNextArgAvailable(haveArgs);
        emit signalArgsAvailable(haveArgs);
    }
}

void KBabelView::setArgsMenu(QPopupMenu *menu)
{
    _argsMenu=menu;

    connect(_argsMenu,SIGNAL(activated(int)),this,SLOT(insertArg(int)));
}


void KBabelView::insertArg(int n)
{
    QString arg = _argsMenu->text(n);

    modifyMsgstrText( msgstrEdit->currentIndex(), arg );
}

void KBabelView::insertNextArg()
{
    int offset = msgstrEdit->currentIndex();

    QString s = (*_catalog->msgstr(_currentIndex).at(msgstrEdit->currentForm())).left(offset);

    if( _catalog->pluralForm( _currentIndex ) == KDESpecific )
    {
	int pos = msgstrEdit->currentIndex();
    	int currentFormBegin=s.findRev("\\n",pos);
	if( currentFormBegin == -1 ) currentFormBegin=0;
	else currentFormBegin+=3; // skip the newline
	int currentFormEnd=s.find("\\n",pos);
	if( currentFormEnd == -1 ) currentFormEnd=s.length();

	s=s.mid(currentFormBegin,currentFormEnd-currentFormBegin);
    }

    _argExtractor->setString(s);
    uint num=_argExtractor->countMatches();
    if(num >= _args.count())
    {
    	KNotifyClient::beep();
        return;
    }

    QString t=_args[num];

    modifyMsgstrText( offset,t );
}

void KBabelView::showArgsMenu()
{
    if(_argsMenu && _args.count() > 0)
    {
        int y=msgstrEdit->height()/2;
        int x=msgstrEdit->width()/2;
        _argsMenu->exec(msgstrEdit->mapToGlobal( QPoint(x,y) ) );

        return;
    }
}


void KBabelView::diff()
{
    diffInternal(false);
    msgstrEdit->setFocus();
}

void KBabelView::diffShowOrig()
{
    msgidLabel->setText(_catalog->msgid(_currentIndex));
    msgidLabel->forceUpdate();
    msgstrEdit->setFocus();
}

void KBabelView::toggleAutoDiff(bool on)
{
    if(on != _diffEnabled)
    {
        _diffEnabled = on;

        if(on)
        {
            diff();
        }
        else
        {
            diffShowOrig();
        }
    }
}

void KBabelView::autoDiff()
{
    diffInternal(true);
}

void KBabelView::diffInternal(bool autoDf)
{
    if(_diffing || _loadingDiffFile)
    {
        return;
    }

    _diffing = true;
    uint diffIndex = _currentIndex;

    QString diffString;

    Catalog::DiffResult r = _catalog->diff(_currentIndex, &diffString);

    if(r == Catalog::DiffNeedList)
    {
	switch( _project->settings()->useDBForDiff() )
	{
	    case 1:
    	    {
        	_loadingDiffFile=true;
    		bool wasEnabled=_diffEnabled;
    	        _diffEnabled=false;

        	QValueList<DiffEntry> diffList;
                QString error;
	        QString package = _catalog->packageName()+".po";
    	        kdDebug(KBABEL) << "getting list for " << package << endl;

        	if(dictBox->messagesForPackage(package,diffList,error))
        	{
                    kdDebug(KBABEL) << "got " << diffList.count()
	                << " messages" << endl;
    	            _catalog->setDiffList(diffList);
        	}
                else
	        {
    	            KMessageBox::sorry(this
        	        ,i18n("An error occurred while trying to get the list "
                        "of messages for this file from the database:\n"
                        "%1").arg(error));

            	    _diffing=false;
                    _diffEnabled=false;
	            _loadingDiffFile=false;
    	            emit signalDiffEnabled(false);

        	    return;
        	}

                _diffEnabled=wasEnabled;
	        _loadingDiffFile=false;
		break;
    	    }
	    case 0:
    	    {
        	_diffing=false;
                if(!openDiffFile(true))
	        {
    		    _diffEnabled=false;
    	             emit signalDiffEnabled(false);

            	    _diffing=false;
                    return;
	        }

    	        _diffing = true;
		break;
            }
	    case 2:
	    {
		// get the list of all entries
        	QValueList<DiffEntry> diffList = _catalog->asDiffList();

		QValueList<DiffEntry> resultList;

		// swap msgstr and msgid
		QValueList<DiffEntry>::iterator it;
		DiffEntry entry;

		 for ( it = diffList.begin(); it != diffList.end(); ++it )
		 {
		    entry.msgstr = (*it).msgid;
		    // if there is no translation, do not show difference
		    if( !(*it).msgstr.isEmpty() )
		    {
			entry.msgid = (*it).msgstr;
		    }
		    else
		    {
			entry.msgid = (*it).msgid;
		    }
		    resultList.append(entry);
		 }

		// set as a source for diff
    	        _catalog->setDiffList(resultList);

            	_diffing=false;
                _diffEnabled=true;
	        _loadingDiffFile=false;
    	        emit signalDiffEnabled(true);
	    }
	}

        diffIndex = _currentIndex;
        r = _catalog->diff(_currentIndex, &diffString);
    }

    // if the index changed in the meanwhile
    while(diffIndex != _currentIndex)
    {
        diffIndex=_currentIndex;
        r = _catalog->diff(_currentIndex,&diffString);
    }

    if(r == Catalog::DiffOk)
    {
	msgidLabel->setText(diffString);
	msgidLabel->forceUpdate();

	// FIXME: should care about plural forms
        if(diffString == _catalog->msgid(_currentIndex).first() )
        {
            emit signalChangeStatusbar(i18n("No difference found"));
        }
        else
        {
            emit signalChangeStatusbar(i18n("Difference found"));
        }
    }
    else
    {
        if(!autoDf)
        {
            KMessageBox::information(this
                ,i18n("No corresponding message found."));
        }
        else
        {
            emit signalChangeStatusbar(
                    i18n("No corresponding message found"));
        }
    }

    _diffing = false;
}

bool KBabelView::openDiffFile()
{
    return openDiffFile(false);
}

bool KBabelView::openDiffFile(bool autoDiff)
{
    if(_diffing || _loadingDiffFile)
        return false;

    KURL url;

    if( autoDiff && ! _project->settings()->diffBaseDir().isEmpty() )
    {
        KURL fileURL = _catalog->currentURL();
	
	KURL poBaseURL( _project->catManSettings().poBaseDir );
	
        QString poBase = poBaseURL.path();
        int len = poBase.length();
        if(fileURL.path().left(len) == poBase)
        {
           QString fileRelPath = fileURL.path().mid(len);
           if(fileRelPath[0] == '/')
               fileRelPath=fileRelPath.mid(1);

           if(_project->settings()->diffBaseDir().right(1) != "/")
               _project->settings()->diffBaseDir() += '/';

           QString diffFilePath = _project->settings()->diffBaseDir() + fileRelPath;


           KURL diffFileURL(diffFilePath);

           if(diffFileURL.isValid() && KIO::NetAccess::exists(diffFileURL,true,NULL))
           {
               url = diffFileURL;

               kdDebug(KBABEL) << "using file " << diffFileURL.prettyURL()
                         << " as diff file" << endl;
           }
        }
    }


    if(url.isEmpty())
    {
        url = KFileDialog::getOpenURL(_project->settings()->diffBaseDir(), 
"application/x-gettext", this, i18n("Select File to Diff With"));
    }

    if(url.isEmpty())
        return false;

    _loadingDiffFile=true;
    bool wasEnabled=_diffEnabled;
    _diffEnabled=false;


    Catalog cat;

    connect(&cat,SIGNAL(signalProgress(int)),this,SIGNAL(signalProgress(int)));
    emit signalResetProgressBar(i18n("loading file for diff"),100);

    ConversionStatus stat = cat.openURL(url);

    emit signalClearProgressBar();


    if(stat != OK && stat != RECOVERED_PARSE_ERROR)
    {
        switch(stat)
        {
            case PARSE_ERROR:
            {
                KMessageBox::sorry(this
                    ,i18n("Error while trying to read file:\n %1\n"
                   "Maybe it is not a valid PO file.").arg(url.prettyURL()));
                break;
            }
            case NO_PERMISSIONS:
            {
                KMessageBox::sorry(this,i18n(
                "You do not have permissions to read file:\n %1")
                        .arg(url.prettyURL()));
                break;
            }
            case NO_FILE:
            {
                 KMessageBox::sorry(this,i18n(
                 "You have not specified a valid file:\n %1")
                         .arg(url.prettyURL()));
                break;
            }
	    case NO_PLUGIN:
	    {
	         KMessageBox::error(this,i18n(
	              "KBabel cannot find a corresponding plugin for the MIME type of the file:\n %1").arg(url.prettyURL()));
	         break;
	    }
	    case UNSUPPORTED_TYPE:
	    {
	          KMessageBox::error(this,i18n(
	               "The import plugin cannot handle this type of the file:\n %1").arg(url.prettyURL()));
                  break;
	    }
            default:
            {
                 KMessageBox::sorry(this,i18n(
                  "Error while trying to open file:\n %1")
                         .arg(url.prettyURL()));
                 break;
            }

        }

        _diffEnabled=wasEnabled;
        _loadingDiffFile=false;

        return false;
    }

    _catalog->setDiffList( cat.asDiffList() );

    _diffEnabled=wasEnabled;
    _loadingDiffFile=false;

    return true;
}

void KBabelView::showTryLaterMessageBox()
{
    if( !_showTryLaterBox ) return;

    KDialogBase *dialog= new KDialogBase(
		i18n("Information"),
		KDialogBase::Yes,
		KDialogBase::Yes, KDialogBase::Yes,
		this, "information", true, true,
		KStdGuiItem::ok() );

    QVBox *topcontents = new QVBox (dialog);
    topcontents->setSpacing(KDialog::spacingHint()*2);
    topcontents->setMargin(KDialog::marginHint()*2);

    QWidget *contents = new QWidget(topcontents);
    QHBoxLayout * lay = new QHBoxLayout(contents);
    lay->setSpacing(KDialog::spacingHint()*2);

    lay->addStretch(1);
    QLabel *label1 = new QLabel( contents);
    label1->setPixmap(QMessageBox::standardIcon(QMessageBox::Information));
    lay->add( label1 );
    QLabel *label2 = new QLabel( i18n("The search string has not been found yet.\n"
	         "However, the string might be found "
		 "in the files being searched at the moment.\n"
		 "Please try later."), contents);
    label2->setAlignment( Qt::AlignAuto | Qt::AlignVCenter | Qt::ExpandTabs | Qt::WordBreak );
    label2->setMinimumSize(label2->sizeHint());
    lay->add( label2 );
    lay->addStretch(1);

    QCheckBox *checkbox = new QCheckBox(i18n("Do not show in this find/replace session again"), topcontents);

    dialog->setMainWidget(topcontents);
    dialog->enableButtonSeparator(false);
    dialog->incInitialSize( QSize(50,0) );

    dialog->exec();

    _showTryLaterBox = !checkbox->isChecked();
    delete dialog;
}

void KBabelView::setFilePackage()
{
    bool result=false;
    QString p = KInputDialog::getText(QString::null, i18n("Enter new package for the current file:"),_catalog->package(),&result,this);
    if( result )
    {
	_catalog->setPackage(p);
	emit signalChangeCaption(p);
    }
}

void KBabelView::insertTagFromTool( const QString& tag )
{
    modifyMsgstrText(msgstrEdit->currentIndex(),tag);

    msgstrEdit->setFocus();
}

void KBabelView::skipToTagFromTool( int index )
{
    _currentTag = index;
    selectTag();
}

void KBabelView::plural2msgstr()
{
    int currentFormBegin, currentFormEnd, pos;
    uint i;

    QStringList msgs = _catalog->msgstr(_currentIndex);
    QString text= *msgs.at(msgstrEdit->currentForm());
    uint numForms = _catalog->numberOfPluralForms(_currentIndex);

    if( text.isEmpty() || _catalog->pluralForm(_currentIndex) == NoPluralForm) return;


    QString result;

    switch( _catalog->pluralForm(_currentIndex) )
    {
	case Gettext:

	    _catalog->applyBeginCommand( _currentIndex, Msgstr ,this);

	    i=0;
	    for( QStringList::Iterator it=msgs.begin() ; it!=msgs.end() ; ++it )
	    {
		if( i!= msgstrEdit->currentForm() )
		{
		    // clear first
		    DelTextCmd* insCmd = new DelTextCmd(0,(*it),i);
		    insCmd->setPart(Msgstr);
		    insCmd->setIndex(_currentIndex);
		    msgstrEdit->processCommand(insCmd,false);
		    forwardMsgstrEditCmd(insCmd);

		    // insert text
		    insCmd = new InsTextCmd(0,text,i);
		    insCmd->setPart(Msgstr);
		    insCmd->setIndex(_currentIndex);
		    msgstrEdit->processCommand(insCmd,false);
		    forwardMsgstrEditCmd(insCmd);
		}
		i++;
	    }
	    
	    // fill the non-initialized ones
	    while (i != numForms)
	    {
		// insert text
		DelTextCmd* insCmd = new InsTextCmd(0,text,i);
		insCmd->setPart(Msgstr);
		insCmd->setIndex(_currentIndex);
		msgstrEdit->processCommand(insCmd,false);
		forwardMsgstrEditCmd(insCmd);
		i++;
	    }

	    _catalog->applyEndCommand( _currentIndex, Msgstr ,this);

	    break;

	case KDESpecific:
	    {
		pos = msgstrEdit->currentIndex();
    		currentFormBegin=text.findRev("\\n",pos);
		if( currentFormBegin == -1 ) currentFormBegin=0;
		else currentFormBegin+=3; // skip the newline
		currentFormEnd=text.find("\\n",pos);
		if( currentFormEnd == -1 ) currentFormEnd=text.length();

	        text=text.mid(currentFormBegin,currentFormEnd-currentFormBegin);

		QString result=text;
		for( i=1; i<numForms ; i++ )
		    result+="\\n\n"+text;

		modifyMsgstrText( 0, result, true );
	    }
	    break;

	case NoPluralForm: break;
    }

}

bool KBabelView::validateUsingTool( const KDataToolInfo & info, const QString &command )
{
    if(currentURL().isEmpty())
        return false;

    KDataTool* tool = info.createTool();
    if( !tool )
    {
	kdWarning() << "Cannot create tool" << endl;
	return false;
    }

    bool result=_catalog->checkUsingTool(tool);
    emitEntryState();

    QString checkName = *(info.userCommands().at( info.commands().findIndex(command) ));

    if(result)
    {
	KMessageBox::information(this
	    ,i18n("No mismatch has been found.")
	    ,checkName);
    }
    else
    {
	int index=0;
	DocPosition pos;

	if(!_catalog->hasError(0,pos))
	    index = _catalog->nextError(0,pos);
	if(index>=0)
	{
	    kdDebug(KBABEL) << "Going to " << pos.item << ", " << pos.form << endl;
	    gotoEntry(pos);
	}

	KMessageBox::error(this
	    ,i18n("Some mismatches have been found.\n"
	    "Please check the questionable entries by using "
	    "Go->Next error")
	    ,checkName);
    }
    delete tool;

    return result;
}

void KBabelView::modifyUsingTool( const KDataToolInfo & info, const QString &command )
{
    KDataTool* tool = info.createTool();
    if( !tool )
    {
	kdWarning() << "Cannot create tool" << endl;
	return;
    }

    // do some stuff on all entries
    _catalog->modifyUsingTool(tool, command);

    delete tool;
}

void KBabelView::modifyCatalogUsingTool( const KDataToolInfo & info, const QString &command )
{
    KDataTool* tool = info.createTool();
    if( !tool )
    {
	kdWarning() << "Cannot create tool" << endl;
	return;
    }

    // do some stuff on the catalog
    tool->run(command, _catalog, "Catalog", "application/x-kbabel-catalog");

    delete tool;
}

void KBabelView::insertChar( QChar ch )
{
    if( isReadOnly() || _catalog->package().isEmpty() )
	return;

    modifyMsgstrText(msgstrEdit->currentIndex(),ch);
    msgstrEdit->setFocus();
}

void KBabelView::wordCount()
{
    uint total, untranslated, fuzzy;
    
    _catalog->wordCount( total, fuzzy, untranslated );
    
    KMessageBox::information( this
	, i18n("Total words: %1\n\n"
"Words in untranslated messages: %2\n\n"
"Words in fuzzy messages: %3").arg(total).arg(untranslated).arg(fuzzy)
	, i18n("Word Count") );
}

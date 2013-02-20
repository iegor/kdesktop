/***************************************************************************
                          keducaview.cpp  -  description
                             -------------------
    begin                : Thu May 24 2001
    copyright            : (C) 2001 by Javier Campos
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "keducaview.h"
#include "keducaview.moc"
#include "settings.h"
#include "../libkeduca/fileread.h"
#include "kquestion.h"
#include "kgroupeduca.h"

#include <stdlib.h>

#include <klocale.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <kio/netaccess.h>

#include <qtimer.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtextedit.h>

KEducaView::KEducaView( QWidget *parent, const char *name )
    : QWidgetStack( parent, name ), _keducaFile( 0 ), _timeoutTimer( 0 )
{
    init();
}

KEducaView::~KEducaView()
{
    delete _questionText;
    delete _buttonGroup;
    delete _split;
    delete _buttonNext;
    delete _buttonSave;
    delete _viewResults;
    delete _keducaFile;
}

/** Init graphical interface */
void KEducaView::init()
{
    // Intro Screen
    _introWidget = new QLabel( this, "introScreen" );
    _introWidget->setBackgroundColor( Qt::white );
    _introWidget->setAlignment( AlignCenter );
    _introWidget->setPixmap( QPixmap( locate("data","keduca/pics/keduca_init.png") ) );
    addWidget( _introWidget, 0 );

    // Info Widget
    _infoWidget = new QVBox( this, "infoWidget" );
    _viewInfo = new QTextEdit( _infoWidget );
    _viewInfo->setReadOnly( true );
    _buttonStartTest = new KPushButton( i18n( "&Start Test" ), _infoWidget );
    connect( _buttonStartTest, SIGNAL( clicked() ), SLOT( slotButtonNext() ) );
    addWidget( _infoWidget, 1 );

    // Question Widget
    _questionWidget = new QVBox( this, "questionWidget" );
    _split  = new QSplitter( QSplitter::Vertical, _questionWidget );
    _questionText = new KQuestion( _split, "kquestion" );
    _buttonGroup = new KGroupEduca( _split, "ButtonGroup" );
    _buttonGroup->setRadioButtonExclusive( true );
    _buttonNext = new KPushButton( i18n( "&Next >>" ), _questionWidget, "ButtonNext" );
    connect( _buttonNext, SIGNAL( clicked() ), SLOT( slotButtonNext() ) );
    addWidget( _questionWidget, 2 );

    // Results Widget
    _resultsWidget = new QVBox( this, "resultsWidget" );
    _viewResults = new QTextEdit( _resultsWidget );
    _viewResults->setReadOnly( true );
    _buttonSave = new KPushButton( i18n( "&Save Results..." ), _resultsWidget );
    connect( _buttonSave, SIGNAL( clicked() ), SLOT( slotButtonSave() ) );
    /* FIXME: This is a hack
     * The results widget can be shown only in the end of the test (when you should be shown the
     * option to save the results) or at the middle of the test, after each question (when there should
     * be a "Next" button).
     */
    _buttonResultsNext = new KPushButton( i18n( "&Next >>" ), _resultsWidget );
    connect( _buttonResultsNext, SIGNAL( clicked() ), SLOT( slotButtonNext() ) );
    _buttonResultsNext->hide();
    addWidget( _resultsWidget, 3 );

    raiseWidget( _introWidget );

    // Restore splitter size
    _split->setSizes( Settings::splitter_size() );
}

void KEducaView::slotButtonStartTest()
{
    raiseWidget( _questionWidget );
    showRecord();
}

/** Button Next action */
void KEducaView::slotButtonNext()
{
    // stop the timer
    if (_timeoutTimer)
    {
      _currentTime += _questionText->getCurrentTime();
      _timeoutTimer->stop();
      _questionText->countdown(0);
    }

    if( ( visibleWidget() == _questionWidget ) && !_isInitStatus )
      setResults();

    _buttonGroup->clearAnswers();

    if( ( Settings::resultFinish()== Settings::EnumResultFinish::afterEachQuestion ) 
        && _questionText->isVisible() )
    {
        showResults( _currentResults + "<HR>" + currentStatusPoints() );
    }
    else
    {
        if( questionNext() )
          showRecord();
        else
          {
          configWrite();
          showResults( setFinalResult() + currentStatusPoints() + "<HR><P>" + _results + "</HTML>"
);
          }
    }
}

/** Button Save action */
void KEducaView::slotButtonSave()
{
    KFileDialog *dialog=new KFileDialog(QString::null, QString::null, this, "file dialog", true);
    dialog->setCaption( i18n("Save Results As") );
    dialog->setKeepLocation( true );
    dialog->setOperationMode( KFileDialog::Saving );
    QStringList mimeFilter( "text/html");
    dialog->setMimeFilter( mimeFilter );
    KURL newURL;
    QString outputFormat ("text/html");

    bool bOk;
    do {
        bOk=true;
        if(dialog->exec()==QDialog::Accepted) {
            newURL=dialog->selectedURL();
            outputFormat=dialog->currentMimeFilter();
        }
        else
        {
            bOk = false;
            break;
        }
        kdDebug()<<"Requested saving to file "<<newURL.prettyURL() <<endl;

        if ( QFileInfo( newURL.path() ).extension().isEmpty() ) {
            // No more extensions in filters. We need to get it from the mimetype.
            QString extension = ".html";
            newURL.setPath( newURL.path() + extension );
        }

        if ( KIO::NetAccess::exists( newURL, false, this ) ) { // this file exists => ask for confirmation
            bOk = KMessageBox::warningContinueCancel( this,
                                              i18n("A document with this name already exists.\n"\
                                                   "Do you want to overwrite it?"),
                                              i18n("Warning"), i18n("Overwrite") ) == KMessageBox::Continue;
        }
    } while ( !bOk );

    delete dialog;  
    if( bOk ) {
        if (!_keducaFile->saveResults( newURL, _viewResults->text() ))
          {
          kdDebug()<< "saving of file failed" <<endl;
          KMessageBox::sorry( this, i18n("Save failed.") ); 
          }
        }else {
        kdDebug()<< "KControlDoc: no valid filename selected" <<endl;
    }
}

/** Show record, returns false if there is no record to show. */
void KEducaView::showRecord()
{
    QString questionTextTemp = "";
    
    raiseWidget( _questionWidget );
    
    _keducaFileIndex++;
    bool MultiAnswer = _keducaFile->isMultiAnswer();

    // SHOW QUESTION *******************************************************************
    _questionText->setPixmap( _keducaFile->getPicturePixmap() );
    questionTextTemp =  "<table width=100%><tr><td><b>"
                        + i18n("Question %1").arg(_keducaFileIndex) + "</b>";

    if( _keducaFile->getTotalPoints() > 0 )
      questionTextTemp  += "</td><td align=center bgColor=#336699><font color=#ffffff><b>"
                        + _keducaFile->getQuestion( FileRead::QF_POINTS )
                        + " " + i18n("Points")
                        + "</b></font>";
                       
    questionTextTemp    += "</tr></td></table><hr/><table><tr><td>"
                        + _keducaFile->getQuestion( FileRead::QF_TEXT )
                        + "</td></tr></table>";

    _questionText->setText( questionTextTemp );
                            
    // SHOW ANSWERS ********************************************************************
    if( MultiAnswer )
        _buttonGroup->setType( KGroupEduca::Check );
    else
        _buttonGroup->setType( KGroupEduca::Radio );


    if( Settings::randomAnswers() )
      {
      _randomAnswers.clear();
      QValueList<unsigned int> tmpRandom;
      unsigned int index;
      for( index = 0 ; index < _keducaFile->recordAnswerCount() ; index ++ )
        { tmpRandom.append( index ); }
      for( index = 0 ; index < _keducaFile->recordAnswerCount() ; index ++ )
        {
        unsigned int random = rand() % tmpRandom.count();
        QValueList<unsigned int>::iterator itTmpRandom = tmpRandom.begin();
        for( unsigned int i = 0; i < random; i++) { ++itTmpRandom; }
        _keducaFile->recordAnswerAt( (*itTmpRandom) );
        _randomAnswers.append( (*itTmpRandom) );
        _buttonGroup->insertAnswer( _keducaFile->getAnswer(FileRead::AF_TEXT) );
        tmpRandom.remove( itTmpRandom );
        }
      }
    else
      {
        _keducaFile->recordAnswerFirst();
        while( !_keducaFile->recordAnswerEOF() )
        {
            if( !_keducaFile->getAnswer(FileRead::AF_TEXT).isEmpty() )
                _buttonGroup->insertAnswer( _keducaFile->getAnswer(FileRead::AF_TEXT) );
            _keducaFile->recordAnswerNext();
        };
      }

    
    // START THE TIMER *****************************************************************
    if( _keducaFile->getQuestionInt(FileRead::QF_TIME) > 0 )
    {
        KMessageBox::information( this,
                                  i18n( "You have %1 seconds to complete this question.\n\n"
                                        "Press OK when you are ready." )
                                  .arg( _keducaFile->getQuestion(FileRead::QF_TIME) ) );
     }

    int timeout = _keducaFile->getQuestionInt(FileRead::QF_TIME);
    if (timeout > 0)
    {
      if (!_timeoutTimer)
      {
        _timeoutTimer = new QTimer(this);
        connect(_timeoutTimer, SIGNAL(timeout()),
                this, SLOT(questionTimedOut()));
      }
      _timeoutTimer->start(1000*timeout);
      _questionText->countdown(timeout);
      _questionText->countdownVisible(true);
    }
    else
    {
      _questionText->countdownVisible(false);
    }
}

void KEducaView::questionTimedOut()
{
  _currentTime += _keducaFile->getQuestionInt(FileRead::QF_TIME);
  slotButtonNext();
}

/** Show results */
void KEducaView::showResults( const QString &text )
{
    _viewResults->setText( text );
   
    if ( _keducaFile->recordEOF() )
    {
        _buttonSave->show();
	   _buttonResultsNext->hide();
    }
    else
    {
        _buttonSave->hide();
	   _buttonResultsNext->show();
    }
    
    raiseWidget( _resultsWidget );
}

/** Set results */
void KEducaView::setResults()
{
    bool isCorrect = true;
    QString yourAnswer = "";
    QString correctAnswer = "";
    QValueList<uint>::iterator itRandom = _randomAnswers.begin();
    
    Settings::randomAnswers() ? _keducaFile->recordAnswerAt(*itRandom) : _keducaFile->recordAnswerFirst();

    for( unsigned index = 0 ; index < _keducaFile->recordAnswerCount() ; index++ )
    {
        QString answertext = _keducaFile->getAnswer( FileRead::AF_TEXT );
        if( !answertext.isEmpty() )
        {
            if(_buttonGroup->isChecked(index) != _keducaFile->getAnswerValue())
            {
                isCorrect=false;
                if(_buttonGroup->isChecked(index)) yourAnswer += "<BR><FONT COLOR=#b84747><B>" + answertext + "</B></FONT>";
                if(_keducaFile->getAnswerValue()) correctAnswer += "<BR><FONT COLOR=#006b6b><B>" + answertext + "</B></FONT>";
            }
            else
            {
                if(_buttonGroup->isChecked(index)) yourAnswer += "<BR><B>" + answertext + "</B>";
                if(_keducaFile->getAnswerValue()) correctAnswer += "<BR><FONT COLOR=#006b6b><B>" + answertext + "</B></FONT>";
            }
        }
        if( Settings::randomAnswers() )
          {
          ++itRandom;
          _keducaFile->recordAnswerAt(*itRandom);
          }
        else
          _keducaFile->recordAnswerNext();
    }

    if( isCorrect)
      {
      _correctAnswer++;
      if( _keducaFile->getTotalPoints() > 0 )
        _correctPoints += _keducaFile->getQuestionInt(FileRead::QF_POINTS);
      }
    else
      {
      _incorrectAnswer++;
      if( _keducaFile->getTotalPoints() > 0 )
        _incorrectPoints += _keducaFile->getQuestionInt(FileRead::QF_POINTS);
      }
    
  _results += getTableQuestion( isCorrect, correctAnswer, yourAnswer );
}

/** Set results */
QString KEducaView::setFinalResult()
{
  QString finalResult = "";
  
  if( !_keducaFile->isResult() ) return "";

  finalResult = "<TABLE WIDTH=100% BORDER=0 BGCOLOR=#EEEEDD CELLSPACING=0><TR><TD BGCOLOR=#DDDDCC COLSPAN=2 ALIGN=CENTER><FONT COLOR=#222211><B>"
              + i18n("Result")
              + "</B></FONT></TD></TR>";
  
  _keducaFile->recordResultFirst();
  while( !_keducaFile->recordResultEOF() )
    {
        if( _correctPoints >= _keducaFile->getResultInt(FileRead::RS_MIN) &&
            _correctPoints <= _keducaFile->getResultInt(FileRead::RS_MAX) )
          {
             if( _keducaFile->getResult(FileRead::RS_PICTURE).isEmpty() )
               finalResult += "<TR><TD COLSPAN=2 ALIGN=CENTER>";
               else
               finalResult += "<TR><TD><IMG SRC=" + _keducaFile->getResult(FileRead::RS_PICTURE) + " : </TD><TD>";
               
             finalResult += _keducaFile->getResult(FileRead::RS_TEXT) + "</TD><TR>";
          }
        _keducaFile->recordResultNext();
    }

  finalResult += "</TABLE>";

  return finalResult;
}

/** Open url */
bool KEducaView::openURL( const KURL &url)
{
    _keducaFile = new FileRead();
    if ( !_keducaFile->openFile( url ) )
        return false;

    _isInitStatus = true;
    _results = "<HTML>";
    _correctAnswer = 0;
    _correctPoints = 0;
    _currentTime = 0;
    _incorrectAnswer = 0;
    _incorrectPoints = 0;
    _keducaFileIndex = 0;

    if( Settings::randomQuestions() )
      {
      unsigned int index;
      for( index = 0 ; index < _keducaFile->getTotalQuestions() ; index ++ )
            {   _randomQuestions.append( index ); }
      }

    _keducaFile->recordFirst();
    _keducaFile->recordAnswerFirst();
    
    _viewInfo->setText( getInformation() );
    raiseWidget( _infoWidget );
    
    return true;
}

/** Write settings */
void KEducaView::configWrite()
{
    Settings::setSplitter_size( _split->sizes() );
    Settings::writeConfig();
}

/** current Status Points */
QString KEducaView::currentStatusPoints()
{
  QString tempStatus;
 
  tempStatus = insertTable()
              + insertRow( i18n("Statistics"), true, 4)
              + insertRow( i18n("Correct questions"),
                           QString().setNum( _correctAnswer ),
                           i18n("Incorrect questions"),
                           QString().setNum( _incorrectAnswer ), true);

  if( _keducaFile->getTotalPoints() > 0 )
    tempStatus += insertRow( i18n("Total points"), true, 4 )
                + insertRow( i18n("Correct points"),
                             QString().setNum( _correctPoints ),
                             i18n("Incorrect points"),
                             QString().setNum( _incorrectPoints ), true );

  if( _keducaFile->getTotalTime() > 0 )
    tempStatus += insertRow( i18n("Time"), true, 4 )
                + insertRow( i18n("Total time"),
                             QString().setNum( _keducaFile->getTotalTime() ),
                             i18n("Time in tests"),
                             QString().setNum( _currentTime ), true );
  tempStatus += insertTableClose();
  return tempStatus;
}

/** Set HTML table view with correct and incorrect answers */
QString KEducaView::getTableQuestion( bool isCorrect, const QString &correct, const QString &incorrect)
{
  _currentResults = "<TABLE WIDTH=100% BORDER=0><TR><TD VALIGN=TOP WIDTH=70><IMG WIDTH=64 HEIGHT=64 SRC=";                     
  if( isCorrect )
    _currentResults += locate("data","keduca/pics/keduca_correct_64.png");
    else
    _currentResults += locate("data","keduca/pics/keduca_incorrect_64.png");

  _currentResults += "></TD><TD VALIGN=TOP>";

  _currentResults += "<B><FONT COLOR=#336699>" + _keducaFile->getQuestion( FileRead::QF_TEXT ) + "</FONT></B>";

  if( _keducaFile->getQuestionInt(FileRead::QF_POINTS) > 0 )
    _currentResults += "<SMALL> ( " + _keducaFile->getQuestion(FileRead::QF_POINTS) + " " + i18n("Points") + " )</SMALL>";
    
  _currentResults += "<HR><SMALL>";

  if( isCorrect )
    _currentResults += i18n( "The answer is: ");
    else
    _currentResults += i18n( "The correct answer is: ");

  _currentResults += correct + "<BR>";

  if( !isCorrect )
    {
        _currentResults += i18n( "Your answer was: ");
        _currentResults += incorrect;
    }
 
  _currentResults += "</SMALL></TD></TR></TABLE><P>";
  return _currentResults;
}

/** Define the next questions (random, secuencial) */
bool KEducaView::questionNext()
{
  if( !Settings::randomQuestions() && !_isInitStatus )
    {
    _keducaFile->recordNext();
    return _keducaFile->recordEOF() ? false : true;
    }

  if( _isInitStatus )
    {
    _isInitStatus = false;
    if( !Settings::randomQuestions() ) return true;
    }
 
  if( _randomQuestions.count() > 0 )
    { 
    unsigned int randomQuestion = rand() % _randomQuestions.count();
    QValueList<uint>::iterator itRandom = _randomQuestions.begin();
    for( unsigned int i = 0; i < randomQuestion; ++i)
      { ++itRandom; }
    _keducaFile->recordAt( (*itRandom) );
    _randomQuestions.remove( itRandom );
    return true;
    }
    else
    return false;
}

/** Get document information */
QString KEducaView::getInformation()
{

  QString tmp = "";
  
  if ( _keducaFile )
  {
    if( !(_keducaFile->getHeader("startupinfo")).isEmpty() )
    {
      tmp += insertTable( "<img src=" + locate("data","keduca/pics/alert.png") + ">", 2 )
//          + insertRow( i18n("Additional Information")+"<br>", true, 2 )
          + insertRow( _keducaFile->getHeader("startupinfo") + "<br>", false )
          + insertTableClose();
    }

    tmp += insertTable( "<img src=" + locate("data","keduca/pics/info.png") + ">", 2 );
    tmp += insertRow( i18n("Information")+"<br>", true, 2);
    tmp += insertRow( i18n("Title"), _keducaFile->getHeader("title"), true );
    tmp += insertRow( i18n("Category"), _keducaFile->getHeader("category"), true );
    tmp += insertRow( i18n("Type"), _keducaFile->getHeader("type"), true );
    tmp += insertRow( i18n("Language"), _keducaFile->getHeader("language"), true );
    tmp += insertTableClose();
  }
  else
  {
    tmp += "<html><body>";
    tmp += "<img src=" + locate("data","keduca/pics/keduca_init.png") + "/>";
    tmp += "</body></html>";
  }

  return tmp;
}

/** Init table with title and colspan */
QString KEducaView::insertTable( const QString &title, unsigned int col){
  QString tmp;

  tmp = "<TABLE WIDTH=100% BORDER=0 CELLSPACING=0 BGCOLOR=#EEEEDD>";

  if( !title.isEmpty() ) {
    tmp += "<TR><TD ";

    if( col > 0 ) tmp += "COLSPAN=" + QString().setNum( col ) + " ";

    tmp += "ALIGN=CENTER><FONT COLOR=#222211><B>"
        + title
        + "</B></FONT></TD></TR>";
    }
  return tmp;
}

/** Insert row */
QString KEducaView::insertRow( const QString &text, bool title, unsigned colSpan ){
  QString tmp;
  tmp = "<TR><TD";
  if( colSpan>0 ) tmp+= " COLSPAN=" + QString().setNum(colSpan);
  if( title ) tmp += " ALIGN=CENTER";
  tmp += ">";
  if( title ) tmp += "<B>";
  tmp += text;
  if( title ) tmp += "</B>";
  tmp += "</TD></TR>";
  return tmp;
}

/** Insert row */
QString KEducaView::insertRow( const QString &label, const QString &field, bool formBased ){
  QString tmp;
  tmp = "<TR><TD";
  if( formBased ) tmp += " ALIGN=RIGHT";
  tmp += ">" + label;
  if( formBased ) tmp += ": ";
  tmp += "</TD><TD>" + field + "</TD></TR>";
  return tmp;
}

QString KEducaView::insertRow( const QString &label1, const QString &field1, const QString &label2, const QString &field2, bool formBased ){
  QString tmp;
  tmp = "<TR><TD";
  if( formBased ) tmp += " ALIGN=RIGHT";
  tmp += ">" + label1;
  if( formBased ) tmp += ": ";
  tmp += "</TD><TD>" + field1 + "</TD>";
  tmp += "<TD";
  if( formBased ) tmp += " ALIGN=RIGHT";
  tmp += ">" + label2;
  if( formBased ) tmp += ": ";
  tmp += "</TD><TD>" + field2 + "</TD></TR>";
  return tmp;
}


/** Close current table */
QString KEducaView::insertTableClose(){
  return "</TABLE>";
}


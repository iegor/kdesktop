/***************************************************************************
                          fileread.cpp  -  description
                             -------------------
    begin                : Wed May 23 2001
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

#include "fileread.h"

#include <assert.h>

#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <kmimetype.h>
#include <kdebug.h>
#include <ktempfile.h>

#include <qfileinfo.h>
#include <qpixmap.h>
#include <qbuffer.h>

FileRead::FileRead( QObject *parent, const char *name )
    :QObject(parent, name),
     _changed(false),
     _tmpfile(0)
{
}

FileRead::~FileRead()
{
}

bool FileRead::openFile(const KURL &url) {
    QString tmpFile;
    bool returnval=false;
    if( KIO::NetAccess::download( url, tmpFile, 0 ) )
    {
        returnval=loadFile( tmpFile );
        if (returnval) {
            _currentURL = url;
            kdDebug()<<"... load successful: "<<_currentURL.url()<<endl;
        }

        KIO::NetAccess::removeTempFile( tmpFile );
    }else
        kdDebug()<<"FileRead::openFile(): download NOT successful: "<<url.url()<<endl;

    return returnval;
}

bool FileRead::loadFile(const QString &filename)
{
    QDomDocument doc("document.xml");

    KMimeType::Ptr type = KMimeType::findByFileContent(filename);

	kdDebug() << "FileRead::loadFile(): MIME-Type is " << type->name() << endl;

    QFile file(filename);

    if(!file.open(IO_ReadOnly))
    {
        return false;
    }

    if (type->name() == "text/html") // Non-compressed files are recognized as HTML...
    {
        doc.setContent(&file);
    }
    else
    {
        doc.setContent(qUncompress(file.readAll()));
    }

     QDomElement docElem = doc.documentElement();
   if( (doc.doctype().isNull()) || (doc.doctype().name() != "educa") )   { 
    	file.close(); 
	return false;
	}

    QDomNode n = docElem.firstChild();

    QDomNodeList dnList = n.childNodes();
    for( unsigned int i = 0; i < dnList.count(); ++i)
    {
        // ------------------- INFORMATION BODY -----------------------
        QDomElement element = dnList.item(i).toElement();
        if( element.tagName() == "default" || element.tagName() == "author" )
        {
            if( element.tagName() == "default" ) {	_header.insert( "image", element.attribute( "image", "default.png" ) ); }
            if( element.tagName() == "author" )	{
                QDomNodeList AuthordnList = element.childNodes();
                for( unsigned int i = 0; i < AuthordnList.count(); ++i) {
                    QDomElement authorelement = AuthordnList.item(i).toElement();
                    _header.insert( authorelement.tagName(), authorelement.text() );
                }
            }
        } else {
            _header.insert( element.tagName(), element.text() );
        }
    }

    n = n.nextSibling();

    dnList = n.childNodes();
    for( unsigned int i = 0; i < dnList.count(); ++i)
    {
        insertQuestion();
        // --------------------- QUESTION ATTRIBUTE------------------------
        QDomElement elementNODE = dnList.item(i).toElement();
        setQuestion( QF_TYPE, 		elementNODE.attribute( "type", "1" ).toInt() );
        setQuestion( QF_PICTURE, 	elementNODE.attribute( "image", "" ) );
        setQuestion( QF_TIME, 		elementNODE.attribute( "time", "0" ).toInt() );
        setQuestion( QF_POINTS, 	elementNODE.attribute( "points", "0" ).toInt() );

        QDomNodeList quList = elementNODE.childNodes();
        for( unsigned int x = 0; x < quList.count(); ++x)
        {
            // --------------------- QUESTION AND RESPONSES------------------
            QDomElement element = quList.item(x).toElement();
            if( element.tagName() == "text" ) 		setQuestion( QF_TEXT, element.text() );
            if( element.tagName() == "true" )		  setAnswer( element.text(), true, element.attribute( "points", "0" ).toInt() );
            if( element.tagName() == "false" )		setAnswer( element.text(), false,element.attribute( "points", "0" ).toInt() );
            if( element.tagName() == "tip" ) 			setQuestion( QF_TIP, element.text() );
            if( element.tagName() == "explain" ) 	setQuestion( QF_EXPLAIN, element.text() );
        }
    }

    n = n.nextSibling();

    dnList = n.childNodes();

    if( dnList.count() > 0 )
      {
      for( unsigned int i = 0; i < dnList.count(); ++i)
        {
        insertResult();
        // --------------------- QUESTION ATTRIBUTE------------------------
        QDomElement elementNODE = dnList.item(i).toElement();
        setResult( RS_TEXT, 		elementNODE.text() );
        setResult( RS_PICTURE, 	elementNODE.attribute( "image", "" ) );
        setResult( RS_MIN, 		  elementNODE.attribute( "min", "0" ).toInt() );
        setResult( RS_MAX, 	    elementNODE.attribute( "max", "0" ).toInt() );
        }
      }
    
    file.close();

    refreshData();
    
    _changed=false;
    return true;
}

void FileRead::setQuestion( QuestionField field, const QString& text)
{
// QF_text, QF_picture, QF_type, QF_time, QF_tip, QF_explain
    switch( field )
    {
    case QF_TEXT:
        (*_recordQuestions).text = text;
        break;
    case QF_PICTURE:
        (*_recordQuestions).picture = text;
        break;
    case QF_TIP:
        (*_recordQuestions).tip = text;
        break;
    case QF_EXPLAIN:
        (*_recordQuestions).explain = text;
        break;
    default:
        kdDebug()<<"FileRead::setQuestion(QuestionField field, QString text) called for not handled field value "<<field <<endl;
        break;
    }
    _changed=true;
}

void FileRead::setQuestion( QuestionField field, int value )
{
    switch( field )
    {
    case QF_TYPE:
        (*_recordQuestions).type = value;
        break;
    case QF_TIME:
        (*_recordQuestions).time = value;
        break;
    case QF_POINTS:
        (*_recordQuestions).points = value;
        break;
    default:
        kdDebug()<<"FileRead::setQuestion(QuestionField field, int value) called for not handled field value "<<field <<endl;
        break;
    }
    _changed=true;
}

void FileRead::setResult( ResultField field, const QString& text)
{
// RS_text, QF_picture
    switch( field )
    {
    case RS_TEXT:
        (*_recordResults).text = text;
        break;
    case RS_PICTURE:
        (*_recordResults).picture = text;
        break;
    default:
        kdDebug()<<"FileRead::setResult(ResultField field, QString text) called for not handled field value "<<field <<endl;
        break;
    }
    _changed=true;
}

void FileRead::setResult( ResultField field, int value )
{
    switch( field )
    {
    case RS_MIN:
        (*_recordResults).min = value;
        break;
    case RS_MAX:
        (*_recordResults).max = value;
        break;
    default:
        kdDebug()<<"FileRead::setResultInt(ResultField field, int value) called for not handled field value "<<field <<endl;
        break;
    }
    _changed=true;
}

void FileRead::setAnswer( const QString& text, bool value, int points)
{
    Answers tmpAnswers;

    tmpAnswers.text = text;
    tmpAnswers.value = value;
    tmpAnswers.points = points;

    (*_recordQuestions).listAnswers.append( tmpAnswers );
    _changed=true;
}

void FileRead::insertQuestion()
{
    Questions tempQuestions;
    tempQuestions.text = "";
    _listQuestions.append( tempQuestions );
    recordLast();
    _changed=true;
}

void FileRead::insertResult()
{
    Results tempResults;
    tempResults.text = "";
    _listResults.append( tempResults );
    recordResultLast();
    _changed=true;
}

void FileRead::recordFirst()
{
    if( _fileEOF = true ) _fileEOF = false;
    if( _fileBOF = false ) _fileBOF = true;
    _recordQuestions = _listQuestions.begin();
}

void FileRead::recordLast()
{
    if( _fileBOF = true ) 	_fileBOF = false;
    if( _fileEOF = false ) _fileEOF = true;
    _recordQuestions = _listQuestions.end();
    --_recordQuestions;
}

void FileRead::recordNext()
{
    ++_recordQuestions;
    if( _recordQuestions == _listQuestions.end() )
      {
      _fileEOF = true;
      --_recordQuestions;
      }
    else
      if( _fileBOF = true ) _fileBOF = false;
}

void FileRead::recordPrevious()
{
    if( _recordQuestions == _listQuestions.begin() )
      _fileBOF = true;
    else
    {
      if( _fileEOF = true ) _fileEOF = false;
      --_recordQuestions;
    }
}

void FileRead::recordResultFirst()
{
    if( _fileResultEOF = true ) _fileResultEOF = false;
    if( _fileResultBOF = false ) _fileResultBOF = true;
    _recordResults = _listResults.begin();
}

void FileRead::recordResultLast()
{
    if( _fileResultBOF = true ) 	_fileResultBOF = false;
    if( _fileResultEOF = false ) _fileResultEOF = true;
    _recordResults = _listResults.end();
    --_recordResults;
}

void FileRead::recordResultNext()
{
    ++_recordResults;
    if( _recordResults == _listResults.end() )
    {
        _fileResultEOF = true;
        --_recordResults;
    }
    else
    {
        if( _fileBOF = true ) _fileBOF = false;
    }
}

void FileRead::recordResultPrevious()
{
    if( _recordResults == _listResults.begin() )
    {
        _fileResultBOF = true;
    }
    else
    {
        if( _fileResultEOF = true ) _fileResultEOF = false;
        --_recordResults;
    }
}

void FileRead::recordAnswerFirst()
{
    if( _fileAnswerEOF = true ) _fileAnswerEOF = false;
    if( _fileAnswerBOF = false ) _fileAnswerBOF = true;
    (*_recordQuestions).recordAnswers = (*_recordQuestions).listAnswers.begin();
}

void FileRead::recordAnswerLast()
{
    if( _fileAnswerBOF = true ) 	_fileAnswerBOF = false;
    if( _fileAnswerEOF = false ) _fileAnswerEOF = true;
    (*_recordQuestions).recordAnswers = (*_recordQuestions).listAnswers.end();
    --(*_recordQuestions).recordAnswers;
}

void FileRead::recordAnswerNext()
{
    ++(*_recordQuestions).recordAnswers;
    if( (*_recordQuestions).recordAnswers == (*_recordQuestions).listAnswers.end() )
    {
        _fileAnswerEOF = true;
        --(*_recordQuestions).recordAnswers;
    }
    else
    {
        if( _fileAnswerBOF = true ) _fileAnswerBOF = false;
    }
}

void FileRead::recordAnswerPrevious()
{
    if( (*_recordQuestions).recordAnswers == (*_recordQuestions).listAnswers.begin() )
    {
        _fileBOF = true;
    }
    else
    {
        if( _fileAnswerEOF = true ) _fileAnswerEOF = false;
        --(*_recordQuestions).recordAnswers;
    }
}

void FileRead::recordAnswerAt( unsigned int index)
{
    (*_recordQuestions).recordAnswers = (*_recordQuestions).listAnswers.begin();
    for( unsigned int i = 0; i < index; ++i)
      ++(*_recordQuestions).recordAnswers;
}

unsigned int FileRead::recordAnswerCount()
{
  return (*_recordQuestions).listAnswers.count();
}

QString FileRead::getQuestion( QuestionField field )
{
// QF_text, QF_picture, QF_type, QF_time, QF_tip, QF_explain
    switch( field )
    {
    case QF_TEXT:
        return (*_recordQuestions).text;
        break;
    case QF_PICTURE:
//        return getPictureLocal( (*_recordQuestions).picture );
        return( (*_recordQuestions).picture );
        break;
    case QF_POINTS:
        return QString().setNum( (*_recordQuestions).points );
        break;
    case QF_TIME:
        return QString().setNum( (*_recordQuestions).time );
        break;
    case QF_TIP:
        return (*_recordQuestions).tip;
        break;
    case QF_EXPLAIN:
        return (*_recordQuestions).explain;
        break;
    default:
        kdDebug()<<"FileRead::getQuestion() called for not handled field value "<<field <<endl;
        return "";
        break;
    }
}

int FileRead::getQuestionInt( QuestionField field )
{
    switch( field )
    {
    case QF_TYPE:
        return (*_recordQuestions).type;
        break;
    case QF_TIME:
        return (*_recordQuestions).time;
        break;
    case QF_POINTS:
        return (*_recordQuestions).points;
        break;
    default:
        kdDebug()<<"FileRead::getQuestionInt() called for not handled field value "<<field <<endl;
        return 0;
    }
}

QString FileRead::getAnswer( AnswerField field )
{
    // AField { AF_text, AF_value, AF_picture, AF_point };
    switch( field )
    {
    case AF_TEXT:
        return (*(*_recordQuestions).recordAnswers).text;
        break;
//		case AF_VALUE:
//			(*(*_recordQuestions).RecordAnswers).Value ? return i18n("True") : return i18n("False");
//			break;
    case AF_POINT:
        return QString().setNum( (*(*_recordQuestions).recordAnswers).points );
        break;
    default:
        kdDebug()<<"FileRead::getAnswer() called for not handled field value "<<field <<endl;
        return "";
    }
}

bool FileRead::getAnswerValue()
{
    return (*(*_recordQuestions).recordAnswers).value;
}

int FileRead::getAnswerPoints()
{
    return (*(*_recordQuestions).recordAnswers).points;
}

QString FileRead::getResult( ResultField field )
{
    switch( field )
    {
    case RS_TEXT:
        return (*_recordResults).text;
        break;
    case RS_PICTURE:
        return( (*_recordResults).picture );
        break;
    case RS_MIN:
        return QString().setNum( (*_recordResults).min );
        break;
    case RS_MAX:
        return QString().setNum( (*_recordResults).max );
        break;
    default:
        kdDebug()<<"FileRead::getResult() called for not handled field value "<<field <<endl;
        return "";
        break;
    }
}

int FileRead::getResultInt( ResultField field )
{
    switch( field )
    {
    case RS_MIN:
        return (*_recordResults).min;
        break;
    case RS_MAX:
        return (*_recordResults).max;
        break;
    default:
        kdDebug()<<"FileRead::getResult() called for not handled field value "<<field <<endl;
        return 0;
        break;
    }
}

bool FileRead::recordEOF()
{
    return _fileEOF;
}

bool FileRead::recordBOF()
{
    return _fileBOF;
}

bool FileRead::recordResultEOF()
{
    return _fileResultEOF;
}

bool FileRead::recordResultBOF()
{
    return _fileResultBOF;
}

bool FileRead::recordAnswerEOF()
{
    return _fileAnswerEOF;
}

bool FileRead::recordAnswerBOF()
{
    return _fileAnswerBOF;
}

QString FileRead::getPicture()
{
  QString picture;

    if( !getQuestion(QF_PICTURE).isEmpty() )
      picture = getQuestion(QF_PICTURE);
      else if( !(_header["image"]).isEmpty() )
        picture = _header["image"];
          else
            return locate("data","keduca/pics/default.png");

  if( _currentURL.isLocalFile() && !KURL(picture).isValid() )
    {
    if( !QFileInfo(picture).exists() )
      picture = _currentURL.directory(false,true) + picture;
    } else if( !_currentURL.isLocalFile() && !KURL(picture).isValid() )
       picture = _currentURL.protocol() + "://" + _currentURL.host() + _currentURL.directory(false,true) + picture;
    
  kdDebug()<< picture <<endl;
            
  return picture;
}

QPixmap FileRead::getPicturePixmap()
{
  
  KURL picture ( getPicture() );
  QPixmap pict;
    
  if( KIO::NetAccess::download( picture, _tmpfileImage, 0 ) )
  {
    kdDebug()<<"... load successful: "<< _tmpfileImage <<endl;
    pict = QPixmap( _tmpfileImage );
    KIO::NetAccess::removeTempFile( _tmpfileImage );
  }
  else
  {
    kdDebug()<<"FileRead::openFile(): download NOT successful: "<< _tmpfileImage <<endl;
    pict = NULL;
  }

  return pict;
}

void FileRead::clearAnswers()
{
    ((*_recordQuestions).listAnswers).clear();
    _changed=true;
}

void FileRead::recordDelete()
{
    _listQuestions.remove( _recordQuestions );
    _changed=true;
}

void FileRead::recordSwap( bool moveup )
{
    Questions listTMP;
    Questions listNEW;

    listTMP = (*_recordQuestions);

    if( moveup )
    {
        recordPrevious();
        listNEW = (*_recordQuestions);
        (*_recordQuestions) = listTMP;
        recordNext();
        (*_recordQuestions) = listNEW;
    }
    else
    {
        recordNext();
        listNEW = (*_recordQuestions);
        (*_recordQuestions) = listTMP;
        recordPrevious();
        (*_recordQuestions) = listNEW;
    }
    _changed=true;
}

bool FileRead::saveFile( const KURL &url, bool copyimages, bool saveCompressed )
{
    if (url.isValid())
    {
        _currentURL = url;
    }
    kdDebug()<<"FileRead::saveFile() to "<<_currentURL.url()<<endl;
    // Local file
    if ( _currentURL.isLocalFile() )
    {
        if ( _tmpfile != 0 ) // get rid of a possible temp file first
        {              // (happens if previous _currentURL was remote)
             _tmpfile->unlink();
             delete _tmpfile;
             _tmpfile = 0;
        }
        if( saveFile(_currentURL.path(), copyimages, saveCompressed) ) {
            emit completed();
            emit setWindowCaption( _currentURL.prettyURL() );
            return true;
        }
    }
    else
    { // Remote file
        // We haven't saved yet, or we did but locally - provide a temp file
        if ( _tmpfile == 0 )
        {
             _tmpfile = new KTempFile;
        }
        // otherwise, we already had a temp file
        if( saveFile(_tmpfile->name(), copyimages, saveCompressed) ) {
            // upload the file
             KIO::Job * job = KIO::file_copy( KURL::fromPathOrURL( _tmpfile->name() ), _currentURL, -1, true /*overwrite*/ );
             connect( job, SIGNAL( result( KIO::Job * ) ), this, SLOT( slotUploadFinished (KIO::Job *) ) );
            return true;
        }
    }

    // Save local file and upload local file
    return false;
}

bool FileRead::saveFile( const QString &filename, bool copyimages, bool saveCompressed )
{
    QDomDocument doc("document.xml");
    QTextStream stream;
    QString line;
    QByteArray data;
    QBuffer buffer(data);
    QFile file(filename);
    QStringList copyJOB;

    stream.setDevice(&buffer);

    if ( (!file.open(IO_WriteOnly)) || (!buffer.open(IO_WriteOnly)) )
    {
        return false;
    }
    /*
    stream.setDevice(&file);

    if(!file.open(IO_WriteOnly))
    {
        // No puede abrir la base
        return false;
    }
    */

    QString head( "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?><!DOCTYPE educa>" );
    doc.setContent( head );

    QDomElement Root = doc.createElement("Document");
    doc.appendChild( Root );

    QDomElement NodeInfo = doc.createElement("Info");
    Root.appendChild( NodeInfo );

    insertXML( doc, NodeInfo, "title", _header["title"] );
    insertXML( doc, NodeInfo, "category", _header["category"] );
    insertXML( doc, NodeInfo, "type", _header["type"] );
    insertXML( doc, NodeInfo, "level", _header["level"] );
    insertXML( doc, NodeInfo, "language", _header["language"] );

    if( !(_header["image"]).isEmpty() )
    {
        QDomElement Nodedefault = doc.createElement("default");

        if( copyimages  )
        {
            copyJOB.append( _header["image"] );
            Nodedefault.setAttribute( "image", QFileInfo(_header["image"]).fileName() );
        } else {
            Nodedefault.setAttribute( "image", _header["image"]);
        }
        NodeInfo.appendChild( Nodedefault );
    }

    if( !_header["name"].isEmpty() || !_header["email"].isEmpty() || !_header["www"].isEmpty() )
    {
        QDomElement Nodeauthor = doc.createElement("author");
        NodeInfo.appendChild( Nodeauthor );
        if( !_header["name"].isEmpty() ) 	insertXML( doc, Nodeauthor, "name", _header["name"] );
        if( !_header["email"].isEmpty() )	insertXML( doc, Nodeauthor, "email", _header["email"] );
        if( !_header["www"].isEmpty() )		insertXML( doc, Nodeauthor, "www", _header["www"] );
    }

    QDomElement NodeData = doc.createElement("Data");
    Root.appendChild( NodeData );

    recordFirst();
    while ( !recordEOF() )
    {
        QDomElement question = doc.createElement("question");
        if( !getQuestion( QF_PICTURE ).isEmpty() )
        {
            if( copyimages )
            {
                copyJOB.append( getQuestion( QF_PICTURE ) );
                question.setAttribute("image", QFileInfo( getQuestion( QF_PICTURE ) ).fileName() );
            } else {
                question.setAttribute("image",  getQuestion( QF_PICTURE ) );
            }
        }
        question.setAttribute( "type", getQuestionInt( QF_TYPE ) );
        if( getQuestionInt( QF_POINTS ) > 0 ) question.setAttribute( "points", getQuestion( QF_POINTS ) );
        if( getQuestionInt( QF_TIME ) > 0 ) question.setAttribute( "time", getQuestion( QF_TIME ) );
        insertXML( doc, question, "text", getQuestion( QF_TEXT ) );

        recordAnswerFirst();
        while( !recordAnswerEOF() )
        {
            if( getAnswerValue() )
            {
                QDomElement domELEMENT = doc.createElement( "true" );
                if( getAnswerPoints() > 0 ) domELEMENT.setAttribute("points", getAnswerPoints() );
                QDomText DATAelement = doc.createTextNode( getAnswer( AF_TEXT ) );
                question.appendChild( domELEMENT );
                domELEMENT.appendChild( DATAelement );
            }	else {
//				insertXML( doc, question, "false", getAnswer( AF_text ) );
                QDomElement domELEMENT = doc.createElement( "false" );
                if( getAnswerPoints() > 0 ) domELEMENT.setAttribute("points", getAnswerPoints() );
                QDomText DATAelement = doc.createTextNode( getAnswer( AF_TEXT ) );
                question.appendChild( domELEMENT );
                domELEMENT.appendChild( DATAelement );
            }
            recordAnswerNext();
        };

        if( !getQuestion( QF_TIP ).isEmpty() ) insertXML( doc, question, "tip", getQuestion( QF_TIP ) );
        if( !getQuestion( QF_EXPLAIN ).isEmpty() ) insertXML( doc, question, "explain", getQuestion( QF_EXPLAIN ) );

        NodeData.appendChild( question );
        recordNext();
    }

    doc.save( stream, 4);
    buffer.close();
    if ( saveCompressed )
        file.writeBlock(qCompress(data));
    else
        file.writeBlock(data);
    file.close();

    if( copyimages == true && copyJOB.count() > 0 )
    {
        KURL::List	KurlLIST( copyJOB );
        KIO::CopyJob *copyjob;

        copyjob = KIO::copy( KurlLIST, KURL( _currentURL.directory(false,true) ), true);
    }
    _changed=false;
    return true;

}

/** Save results */
bool FileRead::saveResults( const KURL &url, const QString &results )
{
    if (url.isValid())
    {
        _currentURL = url;
    }
    kdDebug()<<"FileRead::saveResults() to "<<_currentURL.url()<<endl;
    // Local file
    if ( _currentURL.isLocalFile() )
    {
        if ( _tmpfile != 0 ) // get rid of a possible temp file first
        {              // (happens if previous _currentURL was remote)
             _tmpfile->unlink();
             delete _tmpfile;
             _tmpfile = 0;
        }
        if( saveResults(_currentURL.path(), results) ) {
            emit completed();
            emit setWindowCaption( _currentURL.prettyURL() );
            return true;
        }
    }
    else
    { // Remote file
        // We haven't saved yet, or we did but locally - provide a temp file
        if ( _tmpfile == 0 )
        {
             _tmpfile = new KTempFile;
        }
        // otherwise, we already had a temp file
        if( saveResults(_tmpfile->name(), results) ) {
            // upload the file
             KIO::Job * job = KIO::file_copy( KURL::fromPathOrURL( _tmpfile->name() ), _currentURL, -1, true /*overwrite*/ );
             connect( job, SIGNAL( result( KIO::Job * ) ), this, SLOT( slotUploadFinished (KIO::Job *) ) );
            return true;
        }
    }

    // Save local file and upload local file
    return false;
}

bool FileRead::saveResults( const QString &filename, const QString &results )
{
    QTextStream stream;
    QFile file(filename);
    QStringList copyJOB;

    stream.setDevice(&file);

    if(!file.open(IO_WriteOnly))
    {
        // Cannot open
        return false;
    }
    stream << results;
    file.close();
    return true;
}

/** Insert xml format data */
void FileRead::insertXML( QDomDocument &doc, QDomElement &parent, const QString &tagName, const QString &data)
{
    QDomElement domELEMENT = doc.createElement( tagName );
    QDomText DATAelement = doc.createTextNode( data );

    parent.appendChild( domELEMENT );
    domELEMENT.appendChild( DATAelement );
}

/** Insert xml data format */
void FileRead::insertXML( QDomDocument &doc, QDomElement &parent, const QString &data)
{
    QDomText DATAelement = doc.createTextNode( data );
    parent.appendChild( DATAelement );
}

/** Get Header */
QString FileRead::getHeader(const QString &head)
{
    return _header[head];
}

/** Set header data */
void FileRead::setHeader( const QString field, const QString value)
{
    _changed = _header[field]!=value;

    if( (_header[field]).isEmpty() )
        _header.insert( field, value );
    else
        _header.replace( field, value );
}

/** is Multi Answer */
bool FileRead::isMultiAnswer()
{
    int numOKanswer = 0;

    recordAnswerFirst();

    while( !recordAnswerEOF() )
    {
        if( (*(*_recordQuestions).recordAnswers).value == true ) numOKanswer++;
        recordAnswerNext();
    }

    if( numOKanswer > 1 )	{
        return true;
    }	else {
        return false;
    }
}

/** is Multi Answer */
bool FileRead::isResult()
{
  return _listResults.count() > 0 ? true : false; 
}

void FileRead::slotUploadFinished( KIO::Job * job )
{
  if (job->error()) {
    emit canceled( job->errorString() );
    kdDebug()<< "FileRead::slotUploadFinished(): " <<job->errorString()<<endl;
  }
  else
  {
    if ( _tmpfile!=0 ) // We're finished with this document -> remove temp file
    {
       _tmpfile->unlink();
       delete _tmpfile;
       _tmpfile=0;
    }
    emit setWindowCaption( _currentURL.prettyURL() );
    emit completed();
  }
}

/** Record at index */
void FileRead::recordAt( uint index )
{
    _recordQuestions = _listQuestions.begin();
    for( unsigned int i = 0; i < index; ++i)
      ++_recordQuestions;   
}

/** Total questions */
uint FileRead::getTotalQuestions()
{
  return _totalQuestions;
}

/** Total points */
uint FileRead::getTotalPoints()
{
  return _totalPoints;
}

/** Total time */
uint FileRead::getTotalTime()
{
  return _totalTime;
}

/** Refresh stadistical data - Points, number questions and total points */
void FileRead::refreshData()
{
  _totalTime = 0;
  _totalPoints = 0;
  _totalQuestions = 0;

  _recordQuestions = _listQuestions.begin();
  while ( _recordQuestions != _listQuestions.end() )
    {
    _totalQuestions++;
    _totalPoints += (*_recordQuestions).points;
    _totalTime += (*_recordQuestions).time;
    ++_recordQuestions;
    } 
}

#include "fileread.moc"

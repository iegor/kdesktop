/***************************************************************************
                          fileread.h  -  description
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

#ifndef FILEREAD_H
#define FILEREAD_H

#include <kio/job.h>

#include <qdom.h>

// forward declarations
class KTempFile;

class QFile;

/**File Read operations. This class read a keduca file an pass the records.
 *@author Javier Campos
 *@author Klas Kalass
 */

class FileRead : public QObject {
    Q_OBJECT

public:
    FileRead( QObject *parent=0, const char *name=0 );
    virtual ~FileRead();
    enum QuestionField { QF_TEXT, QF_PICTURE, QF_POINTS, QF_TYPE, QF_TIME, QF_TIP, QF_EXPLAIN };
    enum AnswerField { AF_TEXT, AF_VALUE, AF_POINT };
    enum ResultField { RS_TEXT, RS_MIN, RS_MAX, RS_PICTURE };
    
    /** Open keduca file. This function can open a remote or local url. */
    bool openFile( const KURL &url );
    /** Set value to questions fields */
    void setQuestion( QuestionField field, const QString& text);
    /** Set value to questions fields */
    void setQuestion( QuestionField field, int value );
    /** Set value to results fields */
    void setResult( ResultField field, const QString& text);
    /** Set value to results fields */
    void setResult( ResultField field, int value );
    /** Insert an answer field */
    void setAnswer( const QString& text, bool value, int points=0);
    /** Go to the Last Record */
    void recordLast();
    /** Go to the First record */
    void recordFirst();
    /** Go to the Last Record */
    void recordResultLast();
    /** Go to the First record */
    void recordResultFirst();
    /** Insert a blank question */
    void insertQuestion();
    /** Insert a blank result */
    void insertResult();
    /** Record Next */
    void recordNext();
    /** Record previous */
    void recordPrevious();
    /** Record Next */
    void recordResultNext();
    /** Record previous */
    void recordResultPrevious();
    /** Go to the Last Record */
    void recordAnswerLast();
    /** Go to the First record */
    void recordAnswerFirst();
    /** Record Next */
    void recordAnswerNext();
    /** Record previous */
    void recordAnswerPrevious();
    /** Move to record */
    void recordAnswerAt( unsigned int index);
    /** Record answer count */
    unsigned int recordAnswerCount();
    /** Get the value of the field question */
    QString getQuestion( QuestionField field );
    /** Get the value of the field questions in integer */
    int getQuestionInt( QuestionField field );
    /** Get Answer field */
    QString getAnswer( AnswerField field );
    /** Get answer field */
    int getAnswerPoints();
    /** Get Answer field */
    bool getAnswerValue();
    /** Get the value of the field result */
    QString getResult( ResultField field );
    /** Get the value of the field result */
    int getResultInt( ResultField field );
    /** Returns if this record is a begin of file */
    bool recordBOF();
    /** Returns if this record is a end of file */
    bool recordEOF();
    /** Returns if this record is a begin of file */
    bool recordResultBOF();
    /** Returns if this record is a end of file */
    bool recordResultEOF();
    /** Returns if this record is a begin of file */
    bool recordAnswerBOF();
    /** Returns if this record is a end of file */
    bool recordAnswerEOF();
    /** Clear answers */
    void clearAnswers();
    /** Delete current record */
    void recordDelete();
    /** Swap two variables */
    void recordSwap( bool );
    /** Save file */
    bool saveFile( const KURL &url = KURL(), bool copyimages = false, bool saveCompressed = true );
    /** Save results */
    bool saveResults( const KURL &url, const QString &results );
    /** Get real picture */
    QString getPicture();
    /** Get real picture */
    QPixmap getPicturePixmap();
    /** Set header data */
    void setHeader( const QString field, const QString value);
    /** Get Header */
    QString getHeader(const QString &head);
    /** is Multi Answer */
    bool isMultiAnswer();
    /** is Result */
    bool isResult();
    KURL const &getCurrentURL()const{return _currentURL;}
    /** @return true: The document has changed and should be saved*/
    bool changed() {return _changed;}
    /** Record at index */
    void recordAt( uint index );
    /** Refresh stadistical data - Points, number questions and total points */
    void refreshData();
    /* get total questions */
    uint getTotalQuestions();
    /* get total Points */
    uint getTotalPoints();
    /* get total Time */
    uint getTotalTime();
private:

    // Private attributes

    bool _changed;
    /** List of General Variables of the header */
    QMap<QString,QString> _header;

    struct Answers
    {
        QString text;
        bool value;
        int points;
    };

    struct Results
    {
        QString text;
        QString picture;
        int min;
        int max;
    };
    
    struct Questions
    {
        QString text;
        int type;
        QString picture;
        int time;
        int points;
        QString tip;
        QString explain;
        QValueList<Answers> listAnswers;
        QValueListIterator<Answers> recordAnswers;
    };

    /** List to Data struct */
    QValueList<Questions> _listQuestions;
    QValueListIterator<Questions> _recordQuestions;

    QValueList<Results> _listResults;
    QValueListIterator<Results> _recordResults;
    
    /** Returns if the database finds End Of File */
    bool _fileEOF;
    /** Returns if the database finds Bof Of File */
    bool _fileBOF;
    /** Returns if the database finds End Of File */
    bool _fileAnswerEOF;
    /** Returns if the database finds Bof Of File */
    bool _fileAnswerBOF;
    /** Returns if the database finds End Of File */
    bool _fileResultEOF;
    /** Returns if the database finds Bof Of File */
    bool _fileResultBOF;
    /** Current url */
    KURL _currentURL;
    /** the temporary file to which the document is saved, NULL if no temporary file is needed */
    KTempFile *_tmpfile;
    /** the temporary image file */
    QString _tmpfileImage;
    /** Total Questions */
    uint _totalQuestions;
   /** Total Time */
    uint _totalTime;
    /** Total Points */
    uint _totalPoints;
    
    // Private methods

    /** Insert xml format data */
    void insertXML( QDomDocument &doc, QDomElement &parent, const QString &tagName, const QString &data);
    /** Insert xml data format */
    void insertXML( QDomDocument &doc, QDomElement &parent, const QString &data);

protected:
    /** Open keduca file. This function does the actual work and expects a local filename . */
    bool loadFile( const QString &filename );
    /** Save KEduca file to the file*/
    bool saveFile( const QString &filename, bool copyimages, bool saveCompressed = true );
    /** Save Results file to the file */
    bool saveResults( const QString &filename, const QString &results );

protected slots:
    void slotUploadFinished( KIO::Job * job );

signals:
/* Stolen from kparts */

  /**
   * Emit this when you have completed loading data.
   * Hosting apps will want to know when the process of loading the data
   * is finished, so that they can access the data when everything is loaded.
   **/
  void completed();

  /**
   * Same as the above signal except besides indicating that the data has
   * been completely loaded it also informs the host, by setting the flag,
   * that a pending action has been generated as a result of completing the
   * requested task (loading the data).  An example of this is meta-refresh
   * tags on HTML pages which result in the page either being refreshed or
   * the viewer being redirected to another page.  By emitting this signal
   * after appropriately setting the flag, the part can tell the host of the
   * pending scheduled action inorder to give it a chance to accept or cancel
   * that action.
   *
   * @p pendingAction  if true, a pending action exists (ex: a scheduled refresh)
   */
  void completed( bool pendingAction );

  /**
   * Emit this if loading is canceled by the user or by an error.
   **/
  void canceled( const QString &errMsg );

    /**
     * Emitted by the part, to set the caption of the window(s)
     * hosting this part
     */
    void setWindowCaption( const QString & caption );

};

#endif

/***************************************************************************
                          kcontroladdedit.cpp  -  description
                             -------------------
    begin                : Sun May 27 2001
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

#include "kcontroladdedit.h"
#include "kcontroladdedit.moc"

// KDE includes
#include <klocale.h>
#include <kconfig.h>
#include <kurlrequester.h>
#include <kcombobox.h>
#include <knuminput.h>
#include <klistview.h>
//#include <.h>

// Qt includes
#include <qlabel.h>
#include <qtextedit.h>

KControlAddEdit::KControlAddEdit(QWidget *parent, const char *name, bool modal, WFlags f)
    : KControlAddEditBase(parent, name, modal, f),
      _currentAnswerItem(0)
{
    configRead();
}

KControlAddEdit::~KControlAddEdit()
{
}

/** Init _keducaFile pointer and EditMode */
void KControlAddEdit::init(FileRead *keducaFile, bool edit)
{
    setCurrentAnswerItem(0);
    _keducaFile = keducaFile;
    _editMode = edit;

    _listAnswers->setSorting(-1);
    
    if( _editMode )
    {
        setCaption(i18n("Modify Question"));
        fillPage();
    }else{
        setCaption(i18n("Add Questions"));
        slotQuestionTypeChanged( 0 );
    }

    // settings for Question page
    setHelpEnabled ( _pageQuestion, false );
    setNextEnabled( _pageQuestion, false );

    // settings for Answer page
    setHelpEnabled ( _pageAnswer, false );
    setFinishEnabled( _pageAnswer, true );
}

/** Fill page of current record */
void KControlAddEdit::fillPage()
{
    _questionText->setText( 		_keducaFile->getQuestion( FileRead::QF_TEXT ) 		);
    if( !_keducaFile->getQuestion( FileRead::QF_PICTURE ).isEmpty() )
    {
        _questionPreview->setPixmap(_keducaFile->getQuestion(FileRead::QF_PICTURE));
        _answerPreview->setPixmap(_keducaFile->getQuestion(FileRead::QF_PICTURE));
        _questionPicture->setURL(	_keducaFile->getQuestion( FileRead::QF_PICTURE ) );
    }
    _questionType->setCurrentItem( _keducaFile->getQuestionInt( FileRead::QF_TYPE )-1 );
    slotQuestionTypeChanged( _questionType->currentItem() );
    _questionPoint->setValue( 		_keducaFile->getQuestionInt( FileRead::QF_POINTS )	);
    _questionTip->setText( 			_keducaFile->getQuestion( FileRead::QF_TIP ) 		);
    _questionExplain->setText( 	_keducaFile->getQuestion( FileRead::QF_EXPLAIN ) );
    _questionTime->setValue( 		_keducaFile->getQuestionInt( FileRead::QF_TIME )	);

    _listAnswers->clear();
    _keducaFile->recordAnswerFirst();
    while( !_keducaFile->recordAnswerEOF() )
    {
        QListViewItem *newItem = new QListViewItem(_listAnswers);
        newItem->setText(0,_keducaFile->getAnswer( FileRead::AF_TEXT ));
        newItem->setText(1,_keducaFile->getAnswerValue()?i18n("True"):i18n("False"));
        newItem->setText(2,QString::number(_keducaFile->getAnswerPoints()));

        _keducaFile->recordAnswerNext();
    };
}

void KControlAddEdit::setCurrentAnswerItem(QListViewItem *item)
{
    _currentAnswerItem = item;
    if (item)
        _buttonInsert->setText(i18n("&Apply"));
    else
        _buttonInsert->setText(i18n("&Add"));
}

/** Button insert */
void KControlAddEdit::slotAddAnswer()
{
    if( !_answerText->text().isEmpty() )
    {
        // add new listview item
        QListViewItem *newItem = _currentAnswerItem
                                 ? _currentAnswerItem
                                 : new QListViewItem(_listAnswers,_listAnswers->lastItem());
        newItem->setText(0,_answerText->text());
        newItem->setText(1,_answerValue->currentItem() == 1 ?i18n("True"):i18n("False"));
        newItem->setText(2,QString::number(_answerPoints->value()));

        // reset values
        newItem->setSelected(false);
        resetAnswerValues();
    }
}

void KControlAddEdit::resetAnswerValues()
{
    _answerText->setText("");
    _answerValue->setCurrentItem( 0 );
    _answerPoints->setValue(0);
    _answerText->setFocus();
    setCurrentAnswerItem(0);
    _buttonUp->setEnabled(false);
    _buttonDown->setEnabled(false);
    _buttonRemove->setEnabled(false);
    _buttonInsert->setEnabled(false);
}

/** Remove answer */
void KControlAddEdit::slotRemoveAnswer()
{
    delete _listAnswers->currentItem();
    resetAnswerValues();
}

/** Button Up */
void KControlAddEdit::slotMoveUpAnswer()
{
    QListViewItem *item = _listAnswers->currentItem();

    if (item && item->itemAbove())
        item->itemAbove()->moveItem(item);

    _buttonUp->setEnabled(item->itemAbove()!=0);
    _buttonDown->setEnabled(item->itemBelow()!=0);
}

/** Button Down */
void KControlAddEdit::slotMoveDownAnswer()
{
    QListViewItem *item = _listAnswers->currentItem();

    if (item)
        item->moveItem(item->itemBelow());

    _buttonUp->setEnabled(item->itemAbove()!=0);
    _buttonDown->setEnabled(item->itemBelow()!=0);
}

void KControlAddEdit::slotAnswerSelected(QListViewItem *item)
{
    if (!item)
        return;

    setCurrentAnswerItem(item);

    // set the answer fields to the value of the selected answer
    _answerText->setText(item->text(0));
    _answerValue->setCurrentItem(item->text(1) == i18n("True")?1:0);
    _answerPoints->setValue(item->text(2).toInt());

    // enable/disable the buttons as appropriate
    _buttonRemove->setEnabled(true);
    _buttonInsert->setEnabled(true);
    _buttonDown->setEnabled(item->itemBelow()!=0);
    _buttonUp->setEnabled(item->itemAbove()!=0);
}

/** Accept changes */
void KControlAddEdit::accept()
{
    if( !_editMode )
        _keducaFile->insertQuestion();

    addQuestion();
    configWrite();
    done( QDialog::Accepted );
}

/** Add question with form data */
void KControlAddEdit::addQuestion()
{
    _keducaFile->setQuestion( FileRead::QF_TEXT, _questionText->text() );
    _keducaFile->setQuestion( FileRead::QF_TYPE, _questionType->currentItem()+1 );
    _keducaFile->setQuestion( FileRead::QF_PICTURE, _questionPicture->url() );
    _keducaFile->setQuestion( FileRead::QF_POINTS, _questionPoint->value() );
    _keducaFile->setQuestion( FileRead::QF_TIME, _questionTime->value() );
    _keducaFile->setQuestion( FileRead::QF_TIP, _questionTip->text() );
    _keducaFile->setQuestion( FileRead::QF_EXPLAIN, _questionExplain->text() );

    _keducaFile->clearAnswers();
    QListViewItem *item = _listAnswers->firstChild();
    while (item) {
        _keducaFile->setAnswer(
            item->text(0), // The Answer text
            item->text(1)==i18n("True"), // the validity of the answer
            item->text(2).toInt()); // the points
        item = item->nextSibling();
    }
}
/** Read config settings */
void KControlAddEdit::configRead()
{
    KConfig *config = KGlobal::config();
    config->setGroup("AddModify Window");
    QSize defaultSize(500,400);
    resize( config->readSizeEntry("Geometry", &defaultSize ) );
}

/** Write config settings */
void KControlAddEdit::configWrite()
{
    KConfig *config = KGlobal::config();
    config->setGroup("AddModify Window");
    config->writeEntry("Geometry", size() );
    config->sync();
}

/** Question data changed */
void KControlAddEdit::slotDataChanged()
{
    if( _pageQuestion == currentPage() )
    {
        nextButton()->setEnabled( !_questionText->text().isEmpty() );
    }
    if( _pageAnswer == currentPage() )
    {
        _buttonInsert->setEnabled( !_answerText->text().isEmpty() );
    }
}


/** Preview image */
void KControlAddEdit::slotPreviewImage( const QString &text)
{
    if( text.isEmpty() ) { _questionPreview->setText(""); _answerPreview->setText(""); return; }

    _questionPreview->setPixmap( QPixmap( text ) );
    _answerPreview->setPixmap( QPixmap( text ) );
}

/** show current Page */
void KControlAddEdit::showPage(QWidget *page)
{
    QWizard::showPage(page);
    slotDataChanged();

    if ( page == _pageQuestion ) {
        _questionText->setFocus();
    } else if ( page == _pageAnswer ) {
        _answerText->setFocus();
    }
}

/** Change question type */
void KControlAddEdit::slotQuestionTypeChanged( int index )
{
    switch( index+1 )
    {
    case 1:
        _questionPoint->setEnabled( false );
        _answerPoints->setEnabled( false );
        _questionPoint->setValue(0);
        _answerPoints->setValue(0);
        break;
    case 2:
        _questionPoint->setEnabled( true );
        _answerPoints->setEnabled( false );
        _answerPoints->setValue(0);
        break;
    case 3:
        _questionPoint->setEnabled( false );
        _answerPoints->setEnabled( true );
        _questionPoint->setValue(0);
        break;
    default:
        break;
    }
}

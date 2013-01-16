/* EditWidget.h
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
** Contributions by M. Laurent and W. Bastian.
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#ifndef _EDITWIDGET_H_
#define _EDITWIDGET_H_ 1
#include <qhbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
class EditWidget:public QHBox
{
	Q_OBJECT
public:
	EditWidget(QString const label="", QString const text="", bool isFile=false, QWidget *parent=0, const char *name=0, WFlags f=0, bool allowLines=true);
	void setLabel(QString const &label) { lbl->setText(label); };
	QString text() const { return line->text(); };
	QString displayText() const { return line->displayText(); };
	int maxLength() const { return line->maxLength(); };
	virtual void setMaxLength(int l) { line->setMaxLength(l); };
	virtual void setFrame(bool b) { line->setFrame(b); };
	bool frame() const { return line->frame(); };
	virtual void setEchoMode(QLineEdit::EchoMode e) { line->setEchoMode(e); };
	QLineEdit::EchoMode echoMode() const { return line->echoMode(); };
	void setReadOnly(bool b) const { line->setReadOnly(b); };
	bool isReadOnly() const { return line->isReadOnly(); };
	virtual void setValidator(const QValidator *v) { line->setValidator(v); };
	const QValidator *validator() const { return line->validator(); };
	virtual void setSelection(int s, int e) { line->setSelection(s, e); };
	virtual void setCursorPosition(int p) { line->setCursorPosition(p); };
	int cursorPosition() const { return line->cursorPosition(); };
	bool validateAndSet(const QString &s, int a, int b, int c) { return line->validateAndSet(s, a, b, c); };
	void cut() { line->cut(); };
	void copy() const { line->copy(); };
	void paste() { line->paste(); };
	void setAlignment(int flag) { line->setAlignment(flag); };
	int alignment() const { return line->alignment(); };
	void cursorLeft(bool mark, int steps=1) { line->cursorLeft(mark, steps); };
	void cursorRight(bool mark, int steps=1) { line->cursorRight(mark, steps); };
	void cursorWordForward(bool mark) { line->cursorWordForward(mark); };
	void cursorWordBackward(bool mark) { line->cursorWordBackward(mark); };
	void backspace() { line->backspace(); };
	void del() { line->del(); };
	void home(bool mark) { line->home(mark); };
	void end(bool mark) { line->end(mark); };
	void setEdited(bool e) { line->setEdited(e); };
	bool edited() const { return line->edited(); };
	bool hasMarkedText() const { return line->hasMarkedText(); };
	QString markedText() const { return line->markedText(); };
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
public slots:
	virtual void setText(const QString &text) { line->setText(text); };
	void selectAll() { line->selectAll(); };
	void deselect() { line->deselect(); };
	void clearValidator() { line->clearValidator(); };
	void insert(const QString &s) { line->insert(s); };
	void clear() { line->clear(); };
	void selectFileClicked();
signals:
	void textChanged(const QString &);
	void returnPressed();
private:
	QLabel		*lbl;
	QLineEdit	*line;
	QPushButton	*select;
};
#endif

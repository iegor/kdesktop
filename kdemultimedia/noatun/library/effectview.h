// Copyright (c) 2001 Charles Samuels <charles@kde.org>
// Copyright (c) 2001 Neil Stevens <neil@qualityassistant.com>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef EFFECTVIEW_H
#define EFFECTVIEW_H

#include <kdialogbase.h>
#include <klistview.h>

class EffectList;
class Effect;
class KComboBox;
class QToolButton;

class EffectView : public KDialogBase
{
Q_OBJECT
public:
	EffectView();

	virtual void show();

public slots:
	void added(Effect *);
	void removed(Effect *);
	void moved(Effect *);

	// buttons
	void moveDown();
	void moveUp();
	void removeEffect();
	void addEffect();
	void configureEffect();

	void activeChanged(QListViewItem *);

protected slots:
	void activeDrop(QDropEvent *, QListViewItem *);

private:
	QListViewItem *toListItem(Effect *) const;

	void init(void);
	bool initialized;

	KComboBox *available;

	QToolButton *up, *down, *configure, *remove;

	EffectList *active;
};

class EffectList : public KListView
{
Q_OBJECT
public:
	EffectList(QWidget *parent);
	virtual bool acceptDrag(QDropEvent *) const;
	virtual QDragObject *dragObject() const;
};

#endif

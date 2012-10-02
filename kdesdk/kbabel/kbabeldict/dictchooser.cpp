/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2001 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2003	  by Stanislav Visnovsky
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
#include "dictchooser.h"
#include "resources.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <kconfig.h>
#include <klistbox.h>
#include <klocale.h>
#include <kdialog.h>
#include <ktempfile.h>

DictChooser::DictChooser(KBabelDictBox*b, QStringList selected
                    , QWidget *parent, const char *name)
           : QWidget(parent,name), box(b)
{
    tempConfig.clear();
    tempConfig.setAutoDelete(true);

    dictList = box->moduleInfos();
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(KDialog::spacingHint());


    QLabel *label = new QLabel(i18n("dictionary to not use","Do not use:"),this);
    layout->addWidget(label,0,0);

    label = new QLabel(i18n("dictionary to use","Use:"),this);
    layout->addWidget(label,0,2);

    unselectedBox = new KListBox(this,"unselectedBox");
    layout->addWidget(unselectedBox,1,0);


    QVBoxLayout *bLayout = new QVBoxLayout();
    selectBtn = new QPushButton("  &>>  ", this);
    selectBtn->setEnabled(false);
    bLayout->addWidget(selectBtn);
    unselectBtn = new QPushButton("  &<<  ", this);
    unselectBtn->setEnabled(false);
    bLayout->addWidget(unselectBtn);
    bLayout->addStretch();
    layout->addLayout(bLayout,1,1);

    selectedBox = new KListBox(this,"selectedBox");
    layout->addWidget(selectedBox,1,2);


    bLayout = new QVBoxLayout();
    upBtn = new QPushButton(i18n("Move &Up"), this);
    upBtn->setEnabled(false);
    bLayout->addWidget(upBtn);
    downBtn = new QPushButton(i18n("Move &Down"), this);
    downBtn->setEnabled(false);
    bLayout->addWidget(downBtn);
    configureBtn = new QPushButton(i18n("Con&figure..."), this);
    bLayout->addWidget(configureBtn);
    bLayout->addStretch();
    layout->addLayout(bLayout,1,3);


    dictList.setAutoDelete(true);
    ModuleInfo *mi;
    for(QStringList::Iterator it=selected.begin(); it!=selected.end();
            ++it)
    {
        for(mi = dictList.first(); mi != 0; mi = dictList.next())
        {
            if(mi->id==*it)
            {
                selectedBox->insertItem(mi->name);
            }
        }
    }

    for(mi = dictList.first(); mi != 0; mi = dictList.next())
    {
        if(!selected.contains(mi->id))
        {
            unselectedBox->insertItem(mi->name);
        }
    }

    if(selectedBox->count() == 0 && unselectedBox->count() > 0)
    {
        selectedBox->insertItem(unselectedBox->text(0));
        unselectedBox->removeItem(0);
    }


    connect(selectedBox,SIGNAL(highlighted(int)), this
            , SLOT(selectedMarked(int)));
    connect(unselectedBox,SIGNAL(highlighted(int)), this
            , SLOT(unselectedMarked(int)));

    connect(selectBtn, SIGNAL(clicked()), this, SLOT(select()));
    connect(unselectBtn, SIGNAL(clicked()), this, SLOT(unselect()));
    connect(upBtn, SIGNAL(clicked()), this, SLOT(up()));
    connect(downBtn, SIGNAL(clicked()), this, SLOT(down()));
    connect(configureBtn,SIGNAL(clicked()), this
            , SLOT(configureSelected()));


    selectedBox->installEventFilter(this);
    unselectedBox->installEventFilter(this);

    int min = minimumHeight();
    if(min < 100)
        setMinimumHeight(100);
}


QStringList DictChooser::selectedDicts()
{
    QStringList selected;

    for(int i = 0; i < (int)selectedBox->count(); i++)
    {
        QString text = selectedBox->text(i);

        ModuleInfo *mi;
        for(mi = dictList.first(); mi!=0; mi = dictList.next())
        {
            if(mi->name == text)
            {
                selected.append(mi->id);
            }
        }
    }

    return selected;
}


void DictChooser::selectedMarked(int index)
{
    unselectedBox->selectAll(false);

    unselectBtn->setEnabled(selectedBox->count() > 1);
    selectBtn->setEnabled(false);
    downBtn->setEnabled((int)selectedBox->count()-1 > index);
    upBtn->setEnabled(index > (int)0);
}

void DictChooser::unselectedMarked(int)
{
    selectedBox->selectAll(false);

    selectBtn->setEnabled(true);
    unselectBtn->setEnabled(false);
    downBtn->setEnabled(false);
    upBtn->setEnabled(false);
}


void DictChooser::select()
{
    int i = unselectedBox->currentItem();
    if(i >= 0)
    {
        QString text = unselectedBox->text(i);
        selectedBox->insertItem(text);

        unselectedBox->removeItem(i);

        if(unselectedBox->count() == 0)
        {
            selectBtn->setEnabled(false);
        }
        else
        {
            unselectedBox->setSelected(i,true);
        }
    }
}


void DictChooser::unselect()
{
    int i = selectedBox->currentItem();
    if(i >= 0 && selectedBox->count() > 1)
    {
        QString text = selectedBox->text(i);
        unselectedBox->insertItem(text);

        selectedBox->removeItem(i);

        if(i < (int)selectedBox->count()-1)
        {
            selectedBox->setSelected(i,true);
        }
        else
        {
            selectedBox->setSelected(i-1,true);
        }
    }
}


void DictChooser::up()
{
    int i = selectedBox->currentItem();
    if(i > 0)
    {
        QString text = selectedBox->text(i);
        selectedBox->changeItem(selectedBox->text(i-1),i);
        selectedBox->changeItem(text,i-1);
        selectedBox->setSelected(i-1,true);
    }
}


void DictChooser::down()
{
    int i = selectedBox->currentItem();
    if(i < (int)selectedBox->count()-1)
    {
        QString text = selectedBox->text(i);
        selectedBox->changeItem(selectedBox->text(i+1),i);
        selectedBox->changeItem(text,i+1);
        selectedBox->setSelected(i+1,true);
    }
}


bool DictChooser::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::FocusIn)
    {
        if(object==selectedBox)
        {
            int i = selectedBox->currentItem();
            selectedBox->setSelected(i,true);
            selectedMarked(i);
            unselectedBox->clearSelection();
        }
        else
        {
            int i = unselectedBox->currentItem();
            if(i >= 0)
            {
                unselectedBox->setSelected(i,true);
                unselectedMarked(i);
                selectedBox->clearSelection();
            }
        }

        return true;
    }

    return false;
}

void DictChooser::configureSelected()
{
    int ci = selectedBox->currentItem();
    if( ci == -1 ) ci = 0; // there is always at least one item
    QString module = selectedBox->text(ci);

    ModuleInfo *mi;
    for(mi = dictList.first(); mi!=0; mi = dictList.next())
    {
        if(mi->name == module)
        {
	    // do backup first
	    if( !tempConfig[mi->id])
	    {
		KTempFile* tmp=new KTempFile();
		tmp->setAutoDelete(true);
		tempConfig.insert(mi->id ,tmp);

		kdDebug(KBABEL_SEARCH) << "Temp file: " << tmp->name() << endl;
		KConfig config(tmp->name());
		config.setGroup(mi->id);
		box->saveSettings(mi->id, &config);
	    }

	    // now let user change configuration
	    box->configure(mi->id,true);
	    break;
        }
    }
}

void DictChooser::restoreConfig()
{
    kdDebug( KBABEL_SEARCH ) << "Cleanup" << endl;
    // cleanup
    QDictIterator<KTempFile> it( tempConfig ); // See QDictIterator
    for( ; it.current(); ++it )
    {
        KConfig config( it.current()->name() );
	config.setGroup( it.currentKey() );
	box->readSettings( it.currentKey(), &config);
    }

    // there is no temporary configs
    tempConfig.clear();
}

#include "dictchooser.moc"

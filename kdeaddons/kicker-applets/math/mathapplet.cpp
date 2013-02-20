/*****************************************************************

Based on code 'Run' applet code, copyright (c) 2000 Matthias Elter <elter@kde.org>

Modifications made by Andrew Coles, 2004 <andrew_coles@yahoo.co.uk>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qlabel.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qpushbutton.h>
#include <qhbox.h>

#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kcombobox.h>
#include <kurifilter.h>
#include <kdialog.h>
#include <krun.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>

#include "parser.h"



#include "mathapplet.h"
#include "mathapplet.moc"

extern "C"
{
  KDE_EXPORT KPanelApplet* init(QWidget *parent, const QString& configFile)
  {
    KGlobal::locale()->insertCatalogue("kmathapplet");
    return new MathApplet(configFile, KPanelApplet::Stretch, 0, parent, "kmathapplet");
  }
}

MathApplet::MathApplet(const QString& configFile, Type type, int actions,
                             QWidget *parent, const char *name)
  : KPanelApplet(configFile, type, actions, parent, name),
    m_hasFocus(false)
{
  //  setBackgroundMode(X11ParentRelative);
    setBackgroundOrigin( AncestorOrigin );
    // setup label
    _label = new QLabel(i18n("Evaluate:"), this);
    QFont f(_label->font());
    f.setPixelSize(12);
//    _label->setBackgroundMode(X11ParentRelative);
    _label->setBackgroundOrigin( AncestorOrigin );
    _label->setFixedHeight(14);
    _label->setFont(f);

    // setup popup button
    _btn = new QPushButton(this);
    f = _btn->font();
    f.setPixelSize(12);
    _btn->setFont(f);
    connect(_btn, SIGNAL(clicked()), SLOT(popup_combo()));

    // setup history combo
    _input = new KHistoryCombo(this);
    _input->setFocus();
    _input->clearEdit();
    watchForFocus(_input->lineEdit());
    connect(_input, SIGNAL(activated(const QString&)),
	    SLOT(evaluate(const QString&)));

    initContextMenu();    
    useDegrees();
	    
    KConfig *c = config();
    c->setGroup("General");

    
    // restore history and completion list
    QStringList list = c->readListEntry("Completion list");
    _input->completionObject()->setItems(list);
    list = c->readListEntry("History list");
    _input->setHistoryItems(list);
    int mode = c->readNumEntry( "CompletionMode", KGlobalSettings::completionMode() );
    _input->setCompletionMode( (KGlobalSettings::Completion) mode );

    _hbox = new QHBox( 0, 0, WStyle_Customize | WType_Popup );
    _hbox->setFixedSize(120, 22);
    
    
}

void MathApplet::initContextMenu()
{
    mContextMenu = new KPopupMenu(this);
    mContextMenu->setCheckable(true);
    mContextMenu->insertItem(i18n("Use &Degrees"), this, SLOT(useDegrees()), 0, 0, 0);
    mContextMenu->insertItem(i18n("Use &Radians"), this, SLOT(useRadians()), 0, 1, 1);
    setCustomMenu(mContextMenu);
}


MathApplet::~MathApplet()
{
    KConfig *c = config();
    c->setGroup("General");

    // save history and completion list
    QStringList list = _input->completionObject()->items();
    c->writeEntry("Completion list", list);
    list = _input->historyItems();
    c->writeEntry("History list", list);
    c->writeEntry( "CompletionMode", (int) _input->completionMode() );
    c->sync();

    KGlobal::locale()->removeCatalogue("kmathapplet");
}

void MathApplet::useDegrees() {

	mContextMenu->setItemChecked(0, true);
	mContextMenu->setItemChecked(1, false);
	Parser dummy;
	dummy.setAngleMode(1);
}

void MathApplet::useRadians() {
	mContextMenu->setItemChecked(0, false);
	mContextMenu->setItemChecked(1, true);
	Parser dummy;
	dummy.setAngleMode(0);
}

void MathApplet::resizeEvent(QResizeEvent*)
{
    if(orientation() == Horizontal)
	{
	    _btn->hide();
	    _input->reparent(this, QPoint(0,0), true);
	    _label->setGeometry(0,0, width(), _label->height());

	    if(height() >= _input->sizeHint().height() + _label->height())
		{
                    int inputVOffset = height() - _input->sizeHint().height() - 2;
                    int labelHeight = _label->sizeHint().height();
		    _label->setGeometry(0, inputVOffset - labelHeight,
                                        width(), labelHeight);
		    _input->setGeometry(0, inputVOffset,
					width(), _input->sizeHint().height());
		    _label->show();
		}
	    else
		{
		    _label->hide();

                    // make it as high as the combobox naturally wants to be
                    // but no taller than the panel is!
                    // don't forget to center it vertically either.
                    int newHeight = _input->sizeHint().height();
                    if (newHeight > height())
                        newHeight = height();
		    _input->setGeometry(0, (height() - newHeight) / 2,
                                        width(), newHeight);
		}
	}
    else
	{
	    _btn->show();
	    _btn->setFixedSize(width(), 22);
	    _input->reparent( _hbox, QPoint(0, 0), false);
	    _label->hide();
	}
    setButtonText();
}

void MathApplet::positionChange(KPanelApplet::Position)
{
    setButtonText();
}

void MathApplet::setButtonText()
{
    QString t;

    if (position() == pLeft)
	{
	    if (width() >= 42)
		t = i18n("< Eval");
	    else
		t = "<";
	}
    else
	{
	    if(width() >= 42)
		t = i18n("Eval >");
	    else
		t = ">";
	}

    _btn->setText(t);
}

int MathApplet::widthForHeight(int ) const
{
    return 110;
}

int MathApplet::heightForWidth(int ) const
{
    return 22;
}

void MathApplet::popup_combo()
{
    QPoint p;
    if (position() == pLeft)
	p = mapToGlobal(QPoint(-_input->width()-1, 0));
    else
	p = mapToGlobal(QPoint(width()+1, 0));
    _hbox->move(p);
    _hbox->show();
    _input->setFocus();
}

void MathApplet::evaluate(const QString& command)
{
    QString exec;

    Parser evaluator;
    
    kapp->propagateSessionManager();

    _input->addToHistory(command);
    

    QString cmd = command;

    // Nothing interesting. Quit!
    if ( cmd.isEmpty() ){
	KMessageBox::sorry(0L, i18n("You have to enter an expression to be evaluated first."));
        needsFocus(true);
    } else {
    	double answer = evaluator.eval(command);
	if (evaluator.errmsg() == 0) {
		QString ansAsString = QString::number(answer);
		_input->clearEdit();
		_input->setEditText(ansAsString);
	} else {
		_input->removeFromHistory(_input->currentText());
		needsFocus(true);
	}
    }
    
    if (orientation() == Vertical)
	_hbox->hide();
}

void MathApplet::mousePressEvent(QMouseEvent *e)
{
    if ( e->button() != RightButton )
    {
        KPanelApplet::mousePressEvent( e );
        return;
    }

    mContextMenu->exec(e->globalPos());
}

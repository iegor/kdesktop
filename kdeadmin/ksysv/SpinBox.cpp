// (c) 2000 Peter Putzer

#include <qlineedit.h>

#include <kdebug.h>

#include "ksv_core.h"
#include "SpinBox.h"

KSVSpinBox::KSVSpinBox (QWidget* parent, const char* name)
  : QSpinBox (0, 99, 1, parent, name),
    KCompletionBase (),
    mClearedSelection (false)
{
  KCompletion* comp = ksv::numberCompletion();
  setCompletionObject (comp, true);

  editor()->installEventFilter (this);

  connect (editor(), SIGNAL (textChanged (const QString&)),
           comp, SLOT (slotMakeCompletion (const QString&)));
  connect (comp, SIGNAL (match (const QString&)),
           this, SLOT (handleMatch (const QString&)));
}

KSVSpinBox::~KSVSpinBox ()
{
}

QString KSVSpinBox::mapValueToText (int value)
{
  QString result;

  if (value < 10)
    result.sprintf("%.2i", value);
  else
    result.setNum (value);
  
  return result;
}

void KSVSpinBox::setCompletedText (const QString& text)
{
  QLineEdit* e = editor ();
  const int pos = e->cursorPosition();

  e->setText (text);

  e->setSelection (pos, text.length());
  e->setCursorPosition (pos);
}

void KSVSpinBox::setCompletedItems (const QStringList& /*items*/)
{
  // dont know what is supposed to be in here but it has to be defined
  // because else the lack of this damn thing is making it abstract
}

void KSVSpinBox::handleMatch (const QString& match)
{
  if (!match.isNull() && editor()->text().length() < 2 && !mClearedSelection)
    setCompletedText (match);
}

bool KSVSpinBox::eventFilter (QObject* o, QEvent* e)
{
  Q_UNUSED(o);
  if (e->type() == QEvent::KeyPress)
    {
      QKeyEvent* ke = static_cast<QKeyEvent*> (e);
      
      switch (ke->key())
        {
        case Key_BackSpace:
        case Key_Delete:
          mClearedSelection = true;
          break;

        default:
          mClearedSelection = false;
        }
    }

  return false;
}

#include "SpinBox.moc"

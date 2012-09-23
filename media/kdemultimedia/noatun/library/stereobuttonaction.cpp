#include "stereobuttonaction.h"

namespace NoatunStdAction
{

StereoButtonAction::StereoButtonAction(const QString& text, int accel, QObject* parent, const char* name )
	: KAction(text, accel, parent, name)
{}

StereoButtonAction::StereoButtonAction(const QString& text, int accel, const QObject* receiver, const char* slot, QObject* parent, const char* name )
	: KAction(text, accel, receiver, slot, parent, name)
{}

StereoButtonAction::StereoButtonAction(const QString& text, const QIconSet& pix, int accel, QObject* parent, const char* name )
	: KAction(text, pix, accel, parent, name)
{}

StereoButtonAction::StereoButtonAction(const QString& text, const QString& pix, int accel, QObject* parent, const char* name )
	: KAction(text, pix, accel, parent, name)
{}

StereoButtonAction::StereoButtonAction(const QString& text, const QIconSet& pix, int accel, const QObject* receiver, const char* slot, QObject* parent, const char* name )
	: KAction(text, pix, accel, receiver, slot, parent, name)
{}

StereoButtonAction::StereoButtonAction(const QString& text, const QString& pix, int accel, const QObject* receiver, const char* slot, QObject* parent, const char* name )
	: KAction(text, pix, accel, receiver, slot, parent, name)
{}

void StereoButtonAction::disable(void)
{
	setEnabled(false);
}

void StereoButtonAction::enable(void)
{
	setEnabled(true);
}

void StereoButtonAction::toggleEnabled(void)
{
	setEnabled(!isEnabled());
}

}

#include "stereobuttonaction.moc"

#ifndef STEREOBUTTONACTION_H
#define STEREOBUTTONACTION_H

#include <kaction.h>

namespace NoatunStdAction
{

/**
 * No, I never owned StereoButtonAction, but I'm tired
 * and PlayAction is already taken.
 */
class StereoButtonAction : public KAction
{
Q_OBJECT
public:
	StereoButtonAction(const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );
	StereoButtonAction(const QString& text, int accel, const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
	StereoButtonAction(const QString& text, const QIconSet& pix, int accel = 0, QObject* parent = 0, const char* name = 0 );
	StereoButtonAction(const QString& text, const QString& pix, int accel = 0, QObject* parent = 0, const char* name = 0 );
	StereoButtonAction(const QString& text, const QIconSet& pix, int accel, const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
	StereoButtonAction(const QString& text, const QString& pix, int accel, const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
public slots:
	void disable(void);
	void enable(void);
	void toggleEnabled(void);
};

}

#endif

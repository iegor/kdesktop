#ifndef KEYZ_H
#define KEYZ_H

#include <noatun/pref.h>
#include <noatun/plugin.h>

class Keyz : public QObject, public Plugin
{
    Q_OBJECT
    NOATUNPLUGIND

public:
    Keyz();
    ~Keyz();

    static KGlobalAccel *accel() { return s_accel; }

public slots:
void slotVolumeUp();
    void slotVolumeDown();
    void slotMute();

    void slotForward();
    void slotBackward();
    void slotNextSection();
    void slotPrevSection();
    void slotCopyTitle();

private:
    static KGlobalAccel *s_accel;
    int preMuteVol;
};


class KeyzPrefs : public CModule
{
    Q_OBJECT

public:
    KeyzPrefs( QObject *parent );
    virtual void save();

private:
    class KKeyChooser* m_chooser;

};

#endif // KEYZ_H

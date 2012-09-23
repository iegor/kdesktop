#include <noatun/engine.h>
#include <noatun/player.h>
#include <noatun/app.h>
#include <noatun/playlist.h>

#include <qlayout.h>

#include <kglobalaccel.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <qclipboard.h>

#include "keyz.h"

extern "C"
{
    KDE_EXPORT Plugin *create_plugin()
    {
        return new Keyz();
    }
}

KGlobalAccel * Keyz::s_accel = 0L;

Keyz::Keyz() : QObject( 0L, "Keyz" ), Plugin(), preMuteVol(0)
{
    NOATUNPLUGINC(Keyz);
    Player *player = napp->player();

    if ( !s_accel )
    {
        s_accel = new KGlobalAccel( this, "noatunglobalaccel" );
        s_accel->insert( "PlayPause", i18n("Play/Pause"), QString::null,
                         CTRL+ALT+Key_P, KKey::QtWIN+CTRL+Key_P,
                         player, SLOT( playpause() ));
        s_accel->insert( "Stop", i18n("Stop Playing"), QString::null,
                         CTRL+ALT+Key_S, KKey::QtWIN+CTRL+Key_S,
                         player, SLOT( stop() ));
        s_accel->insert( "Back", i18n("Back"), QString::null,
                         CTRL+ALT+Key_Left, KKey::QtWIN+CTRL+Key_Left,
                         player, SLOT( back() ));
        s_accel->insert( "Forward", i18n("Forward"), QString::null,
                         CTRL+ALT+Key_Right, KKey::QtWIN+CTRL+Key_Right,
                         player, SLOT( forward() ));
        s_accel->insert( "Playlist", i18n("Show/Hide Playlist"), QString::null,
                         CTRL+ALT+Key_L, KKey::QtWIN+CTRL+Key_L,
                         player, SLOT( toggleListView() ));
        s_accel->insert( "OpenFile", i18n("Open File to Play"), QString::null,
                         CTRL+ALT+Key_O, KKey::QtWIN+CTRL+Key_O,
                         napp, SLOT( fileOpen() ));
        s_accel->insert( "Effects", i18n("Effects Configuration"), QString::null,
                         CTRL+ALT+Key_E, KKey::QtWIN+CTRL+Key_E,
                         napp, SLOT( effectView() ));
        s_accel->insert( "Preferences", i18n("Preferences"), QString::null,
                         CTRL+ALT+Key_F, KKey::QtWIN+CTRL+Key_F,
                         napp, SLOT( preferences() ));
        s_accel->insert( "VolumeUp", i18n("Volume Up"), QString::null,
                         CTRL+ALT+SHIFT+Key_Up, KKey::QtWIN+CTRL+SHIFT+Key_Up,
                         this, SLOT( slotVolumeUp() ));
        s_accel->insert( "VolumeDown", i18n("Volume Down"), QString::null,
                         CTRL+ALT+SHIFT+Key_Down, KKey::QtWIN+CTRL+SHIFT+Key_Down,
                         this, SLOT( slotVolumeDown() ));
        s_accel->insert( "Mute", i18n("Mute"), QString::null,
                         CTRL+ALT+Key_M, KKey::QtWIN+CTRL+Key_M,
                         this, SLOT( slotMute() ));
        s_accel->insert( "SeekForward", i18n("Seek Forward"), QString::null,
                         CTRL+ALT+SHIFT+Key_Right, KKey::QtWIN+CTRL+SHIFT+Key_Right,
                         this, SLOT( slotForward() ));
        s_accel->insert( "SeekBackward", i18n("Seek Backward"), QString::null,
                         CTRL+ALT+SHIFT+Key_Left, KKey::QtWIN+CTRL+SHIFT+Key_Left,
                         this, SLOT( slotBackward() ));
        s_accel->insert( "NextSection", i18n("Next Section"), QString::null,
                         0, 0,
                         this, SLOT( slotNextSection() ));
        s_accel->insert( "PrevSection", i18n("Previous Section"), QString::null,
                         0, 0,
                         this, SLOT( slotPrevSection() ));
        s_accel->insert( "CopyTitle", i18n("Copy Song Title to Clipboard"), QString::null,
                         CTRL+ALT+Key_C, KKey::QtWIN+CTRL+Key_C,
                         this, SLOT( slotCopyTitle() ));

        s_accel->insert( "ToggleGUI", i18n("Show/Hide Main Window"), QString::null,
                         CTRL+ALT+Key_W, KKey::QtWIN+CTRL+Key_W,
                         napp, SLOT( toggleInterfaces() ));

        s_accel->readSettings();
        s_accel->updateConnections();
    }

    new KeyzPrefs(this);
}

Keyz::~Keyz()
{
    delete s_accel;
    s_accel = 0L;
}

void Keyz::slotVolumeUp()
{
    int vol = napp->player()->volume();
    if ( vol >= 100 )
        return;

    napp->player()->setVolume( vol + 4 );
}

void Keyz::slotVolumeDown()
{
    int vol = napp->player()->volume();
    if ( vol <= 0 )
        return;

    napp->player()->setVolume( vol - 4 );
}

void Keyz::slotForward()
{
    napp->player()->skipTo( QMIN(napp->player()->getLength(), napp->player()->getTime() + 3000) ); // + 3 seconds
}

void Keyz::slotBackward()
{
    napp->player()->skipTo( QMAX( 0, napp->player()->getTime() - 3000 )); // - 3 seconds
}

void Keyz::slotNextSection()
{
    Playlist *list = napp->playlist();
    if ( list ) {
        PlaylistItem item = list->nextSection();
        if ( !item.isNull() )
            napp->player()->play( item );
    }
}

void Keyz::slotPrevSection()
{
    Playlist *list = napp->playlist();
    if ( list ) {
        PlaylistItem item = list->previousSection();
        if ( !item.isNull() )
            napp->player()->play( item );
    }
}

void Keyz::slotCopyTitle()
{
    if (napp->player()->current())
        KApplication::kApplication()->clipboard()->setText(napp->player()->current().title());
}


void Keyz::slotMute()
{
    int vol = napp->player()->volume();

    if ( vol <= 0 ) // already muted
    {
        vol = preMuteVol;
    }
    else // we should mute
    {
        preMuteVol = vol;
        vol = 0;
    }

    napp->player()->setVolume( vol );
}

///////////////////////////////////////////////////////////////////

KeyzPrefs::KeyzPrefs( QObject *parent ) :
    CModule( i18n("Keyz"), i18n("Shortcut Configuration"), "key_bindings",
             parent )
{
    QVBoxLayout *layout = new QVBoxLayout( this );
    m_chooser = new KKeyChooser( Keyz::accel(), this );
    layout->addWidget( m_chooser );
}

void KeyzPrefs::save()
{
    m_chooser->commitChanges();
    Keyz::accel()->updateConnections();
    Keyz::accel()->writeSettings();
}

#include "keyz.moc"

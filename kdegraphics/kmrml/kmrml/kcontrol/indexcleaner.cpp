#include <kdebug.h>
#include <kprocess.h>

#include <kmrml_config.h>
#include "indexcleaner.h"

#include <kdeversion.h>
#if KDE_VERSION < 306
  #define QUOTE( x ) x
#else
  #define QUOTE( x ) KProcess::quote( x )
#endif

using namespace KMrmlConfig;

IndexCleaner::IndexCleaner( const QStringList& dirs,
                            const KMrml::Config *config,
                            QObject *parent, const char *name )
    : QObject( parent, name ),
      m_dirs( dirs ),
      m_config( config ),
      m_process( 0L )
{
    m_stepSize = 100 / dirs.count();
}

IndexCleaner::~IndexCleaner()
{
    if ( m_process )
    {
        m_process->kill();
        delete m_process;
        m_process = 0L;
    }
}

void IndexCleaner::start()
{
    startNext();
}

void IndexCleaner::slotExited( KProcess *proc )
{
    emit advance( m_stepSize );

    if ( !proc->normalExit() )
        kdWarning() << "Error removing old indexed directory" << endl;

    m_process = 0L;

    startNext();
}

void IndexCleaner::startNext()
{
    if ( m_dirs.isEmpty() )
    {
        emit advance( 100 );
        emit finished();
        return;
    }

#if KDE_VERSION < 306
    m_process = new KShellProcess();
#else
    m_process = new KProcess();
    m_process->setUseShell( true );
#endif
    connect( m_process, SIGNAL( processExited( KProcess * )),
             SLOT( slotExited( KProcess * ) ));

    QString cmd = m_config->removeCollectionCommandLine();

    QString dir = m_dirs.first();
    m_dirs.pop_front();

    int index = cmd.find( "%d" );
    if ( index != -1 )
        cmd.replace( index, 2, QUOTE( dir ) );
    else // no %d? What else can we do?
        cmd.append( QString::fromLatin1(" ") + QUOTE( dir ) );

    *m_process << cmd;

    if ( !m_process->start() )
    {
        kdWarning() << "Error starting: " << cmd << endl;

        delete m_process;
        m_process = 0L;

        startNext();
    }
}

#include "indexcleaner.moc"

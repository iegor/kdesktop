
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kactioncollection.h>
#include <kdebug.h>

#include <dbtest.h>
#include <dbtest.moc>

dBTestWidget::dBTestWidget( QWidget* p, const char* n ) : QWidget( p,n ), dB2VolCalc( -24,6 ) {
kdDebug() << k_funcinfo << endl;
	( void* ) KStdAction::quit( this, SLOT( close() ), new KActionCollection( this ) );

	for ( float i=0; i<=1; i+=0.25 )
		kdDebug() << i << " : " << amptodb( i ) << "dB" <<endl;

	for ( int i=-24; i<=0; i++ )
		kdDebug() << i <<"db : " << dbtoamp( i ) << endl;
}
dBTestWidget::~dBTestWidget() {
kdDebug() << k_funcinfo << endl;
}

int main( int argc, char* argv[] ) {

	KAboutData aboutData(  "dbtest", I18N_NOOP( "dBTest" ),
		"0.1", "", KAboutData::License_GPL,
		"(c) 2002, Arnold Krille" );
	aboutData.addAuthor( "Arnold Krille", I18N_NOOP(  "Creator" ), "arnold@arnoldarts.de");
	KCmdLineArgs::init( argc, argv, &aboutData );

	KApplication app;

	dBTestWidget* w = new dBTestWidget( 0 );
	w->show();
	app.setMainWidget( w );

	return app.exec();
}

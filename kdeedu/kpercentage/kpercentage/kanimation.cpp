// C/C++ includes
#include <stdlib.h>

// Qt includes
#include <qfile.h>

// KDE includes
#include <kdebug.h>

// local includes
#include "kanimation.h"

///////////
// KFrame

KFrame::KFrame( double nx, double ny, int nf )
{
	x = nx;
	y = ny;
	f = nf;
}

KFrame::~KFrame()
{
}


////////////
// KScene

KScene::KScene()
{
	current_frame_number = 0;
	setAutoDelete( TRUE );
	following = new QStrList( TRUE ); // deep copy
	following->setAutoDelete( TRUE );
}

KScene::~KScene()
{
	delete following;
}

KFrame *KScene::nextFrame()
{
	current_frame_number++;
	if ( current_frame_number < count() )
		return at( current_frame_number );
	else
		return 0;
}

KFrame *KScene::currentFrame()
{
	if ( current_frame_number < count() )
		return at( current_frame_number );
	else
		return 0;
}

void KScene::addFollowingScene( const QString scene_name )
{
	following->append( scene_name.latin1() );
}

void KScene::followingSceneName( QString& scene_name )
{
	if ( following->count() == 0 )
		scene_name = "";
	else
		scene_name = following->at( rand() % following->count() );
}

void KScene::setToStart()
{
	current_frame_number = 0;
}


/////////////
// KStoryBoard

KStoryBoard::KStoryBoard( const QString filename ) : QDict<KScene>()
{
	current_scene = 0;

	QFile f( filename );
	KScene *c_scene = 0;

	if ( f.open( IO_ReadOnly ) )           // file opened successfully
	{
		QTextStream t( &f );        // use a text stream

		kdDebug() << "loading the story board..." << endl;

		while ( !t.eof() )               // until end of file...
		{
			QString s = t.readLine();       // line of text excluding '\n'
			QString dbgString = s;
			int pos = s.find ( "#");
			if (pos==-1)
				pos = s.find ( ";");
			if ( pos > -1 )
			{
				dbgString = s.mid( pos +1 );
				kdDebug() << "Comment: " << dbgString << endl;
				s = s.left( pos );
			}
			if ( !s.isEmpty() )
			{
					QString command_word = s.section( " ", 0, 0 );
					// new scene beginning
					if ( command_word == "name" )
					{
						QString scene_name = s.section( " ", 1, 1 );
						c_scene = new KScene();
						insert( scene_name, c_scene );
						kdDebug() << "scene found: " << scene_name << endl;
					}
					// new frame
					if ( c_scene && command_word == "move" )
					{
						QString parameters = s.section( " ", 1, 1 );
						c_scene->append( new KFrame(
									parameters.section( ",", 0, 0 ).toDouble(),
									parameters.section( ",", 1, 1 ).toDouble(),
									parameters.section( ",", 2, 2 ).toInt() ) );
						kdDebug() << parameters << endl;
					}
					// new following scene
					if ( c_scene && command_word == "following" )
					{
						kdDebug() << "following st..." << endl;
						c_scene->addFollowingScene( s.section( " ", 1, 1 ) );
					}
				}
			}
		}
		f.close();
		// TODO check, if all scenes called by "following" exist
		setToStart();
	}

	KStoryBoard::~KStoryBoard()
	{
	}

	KFrame *KStoryBoard::currentFrame()
	{
		if ( current_scene )
			return current_scene->currentFrame();
		else
			return 0;
	}

	KFrame *KStoryBoard::nextFrame()
	{
		if ( current_scene )
		{
			if ( current_scene->nextFrame() )
				return current_scene->currentFrame();
			else
			{
				QString scene_name;
				current_scene->followingSceneName( scene_name );
				current_scene = find( scene_name );
				if ( current_scene )
				{
					current_scene->setToStart();
					return current_scene->currentFrame();
				}
				else
					return 0;
			}
		}
		else
		{
			return 0;
		}
	}

	void KStoryBoard::setToStart()
	{
		current_scene = find( "init" );
		if ( current_scene )
			current_scene->setToStart();
	}

	//////////
	// KAnimation

	KAnimation::KAnimation( const QString story_filename,
	                        QCanvasPixmapArray * a, QCanvas * canvas ) :
			QCanvasSprite( a, canvas )
	{
		story = new KStoryBoard( story_filename );
	}

	KAnimation::~KAnimation()
	{
		delete story;
	}

	void KAnimation::advance( int phase )
	{
		if ( phase == 1 )
		{
			KFrame * f_ = story->nextFrame();
			if ( f_ )
				move( f_->x, f_->y, f_->f );
		}
	}

	void KAnimation::setToStart()
	{
		story->setToStart();
		KFrame *f_ = story->currentFrame();
		if ( f_ )
			move( f_->x, f_->y, f_->f );
	}





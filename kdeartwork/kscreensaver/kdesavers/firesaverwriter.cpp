/***************************************************************************
 *   Copyright (C) 2004 by E.Ros                                           *
 *   rosenric@dei.unipd.it                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <qimage.h>
#include <qgl.h>
#include <qfile.h>
#include <qstring.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kdeversion.h>
#include <klocale.h>
#include "firesaverwriter.h"


/* Word: SINGLE WORD */

Word::Word( const char * _text, QMap<char, Symbol *> * sMap, float _scale )
    : width(0), scale(_scale), cX(0), cY(0), vScale(0), vX(0), vY(0),
    activateTime(0.0), lifeTime(2), currentTime(0)
{
	for ( ; *_text != 0 && *_text != ' '; _text++ )
	{
		char c = *_text;
		if ( !sMap->contains(c) )	//search for a symbol in the map
			continue;
		Symbol * symbol = (*sMap)[c];	//get the symbol*
		width += symbol->scale;		//increase word's half-width
		symbolList.append( symbol );	//insert it to the list
	}
	color[0] = 0;
	color[1] = 0.8 * drand48();
	color[2] = 0.2 + 0.8 * drand48();
	color[3] = 1;
}

inline void Word::renderWord( double dT )
{
	if ( (currentTime += dT) < activateTime )
		return;

	//update coloring
	if ( activateTime >= 0 ) {
		if ( currentTime < activateTime + 0.4 )
			color[3] = (currentTime - activateTime) / 0.4;
		else
			color[3] = 1 - (currentTime - activateTime - 0.4) / (lifeTime - 0.4);
	} else
		color[3] = 1 - currentTime / lifeTime;

	//word's global transforms
	glPushMatrix();
	glTranslatef( cX - scale * width, cY, 0 );
	glScalef( scale, scale, 1 );
	glColor4fv( color );

	//for each symbol draw it!
	Symbol * symbol = symbolList.first();
	for( ; symbol; symbol = symbolList.next() )
		symbol->renderSymbol();
	glPopMatrix();

	//physical update to position and scale
	cX += vX * dT;
	cY += vY * dT;
	scale += scale * vScale * dT;
}

inline bool Word::isDead()
{
	if ( activateTime > 0 )
		return (currentTime - activateTime) >= lifeTime;
	return currentTime >= lifeTime;
}



/* Writer: engine that spawns and manages words */

Writer::Writer( QString descFileName )
    : numTextures(0)
{
	wordList.setAutoDelete( true );

	if ( !loadMap( descFileName ) )
		return;

	QString welcomeString = i18n("Welcome to KDE %1.%2.%3")
	    .arg(KDE_VERSION_MAJOR)
	    .arg(KDE_VERSION_MINOR)
	    .arg(KDE_VERSION_RELEASE);
	spawnWords(welcomeString, Fun1);
}

Writer::~ Writer()
{
	glDeleteTextures( numTextures, texArray );
	wordList.clear();
	QMap<char, Symbol *>::Iterator it = symbolMap.begin();
	for ( ; it != symbolMap.end(); ++it )
		delete (Symbol *)it.data();
}

void Writer::spawnWords( QString phrase, effectType fX )
{
	int wordCount = 0;
	float xCenter = 0,
	      yCenter = drand48()*40 - 20,
	      wordsWidth = 0;
	QPtrList<Word> localWords;
	while ( phrase.length() > 0 )
	{
		QString letters = phrase.section(" ",0,0);
		Word * word = new Word( letters.latin1(), &symbolMap );
		wordList.append( word );
		localWords.append( word );
		word->cX = xCenter;
		word->cY = yCenter;
		switch ( fX ) {
		    case Fun1:{
			float	angle = 2*M_PI * drand48(),
				module = 0.25 * (drand48() + drand48());
			word->vX = module * cos( angle );
			word->vY = module * sin( angle );
			word->vScale = 0.6;
			word->scale = 0.7 + 0.3*(drand48() + drand48());}
			word->activateTime = 0.3 * wordCount;
			//fall to the case below for word spacing
		    default:
		    case NoEffect:
			wordsWidth += word->width;
			word->cX += wordsWidth;
			wordsWidth += word->width + 1;
			break;
		    case Sequence:
			word->lifeTime = 1.2;
			word->activateTime = 0.6 + 0.9 * wordCount;
//			word->vY = -5;
			break;
		}
		wordCount ++;
		phrase.remove(0, letters.length() + 1);
	}
	if ( localWords.count() < 1 )
		return;
	//some computations to 'center' the string
	float displace = -(wordsWidth - 1) / 2;
	Word * word = localWords.first();
	for( ; word; word = localWords.next() )
		word->cX += displace;
}

void Writer::render( double dT )
{
    if ( !numTextures )
	return;

    glEnable( GL_TEXTURE_2D );

    glPushMatrix();
    glScalef( 0.6, 0.6, 1.0 );
    Word * word = wordList.first();
    while( word ) {
	word->renderWord( dT );
	if ( word->isDead() ) {
		wordList.remove();
		word = wordList.current();
	} else
		word = wordList.next();
    }
    glPopMatrix();
}

/* loadMap()
 *  parses the description file to create the internal symbols map.
 *  This map is then used when building words.
 **/
bool Writer::loadMap( QString descFile )
{
    QFile desc( locate("data","kfiresaver/"+descFile) );
    if ( !desc.open( IO_ReadOnly ) )
	return false;

    unsigned int currentNumber;
    float xres = 0, yres = 0;
    bool generatedFirst = false;

    while ( !desc.atEnd() )
    {
	QString line;
	int count = desc.readLine( line, 100 );
	//skip comments / invalid lines
	if ( count < 6 || line.at(0) == '#')
	    continue;
	//load texture maps
	if ( line.at(0) == '"' && numTextures < 15 )
	{
	    //load and generate texture
	    QString fileName = line.section("\"", 1,1 );
	    QImage tmp;
	    if ( !tmp.load( locate("data","kfiresaver/"+fileName) ) ) {
		kdWarning() << "can't load filename:" << fileName << endl;
		generatedFirst = false;
		continue;
	    }
	    glGenTextures( 1, &currentNumber );
	    texArray[ numTextures++ ] = currentNumber;
	    glBindTexture(GL_TEXTURE_2D, currentNumber);
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    QImage texture = QGLWidget::convertToGLFormat( tmp );
	    xres = (float)texture.width();
	    yres = (float)texture.height();
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)xres, (int)yres, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
	    generatedFirst = true;
	    continue;
	}
	if  ( !generatedFirst )
	    continue;
	if ( line.contains(' ') != 4 ) {
	    kdWarning() << "wrong line on symbols.desc (4 spaces expected):" << endl;
	    kdWarning() << " '" << line << "'" << endl;
	    continue;
	}
	//parse the line describing a symbol and create it
	char p = *(line.latin1());
	if ( symbolMap.contains(p) )
	    continue;
	float left = (float)(line.section(" ",1,1).toInt())/xres,
	      top = (float)(line.section(" ",2,2).toInt())/yres,
	      right = (float)(line.section(" ",3,3).toInt() + 1)/xres,
	      bottom = (float)(line.section(" ",4,4).toInt() + 1)/yres;
	symbolMap[p] = new Symbol( currentNumber, left,top,right,bottom );
    }

    return symbolMap.size() > 0;
}

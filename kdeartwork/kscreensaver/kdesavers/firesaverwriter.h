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

#ifndef FIRESAVER_WRITER_H
#define FIRESAVER_WRITER_H

#include <qgl.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qstring.h>

class Symbol
{
    public:
	Symbol( unsigned int textureNumber, float l, float t, float r, float b )
	    : scale((r - l) / (b - t)), texNum(textureNumber), L(l), T(1-t), R(r), B(1-b)
	{
	    v1[0] = -scale;	v1[1] =  1;
	    v2[0] = -scale;	v2[1] = -1;
	    v3[0] = scale;	v3[1] =  1;
	    v4[0] = scale;	v4[1] = -1;
	}

	float scale;

	inline void renderSymbol()
	{
	    //draw the symbol and update "cursor"'s position
	    glBindTexture( GL_TEXTURE_2D, texNum );
	    glTranslatef( scale, 0, 0 );
	    glBegin( GL_TRIANGLE_STRIP );
	     glTexCoord2f( L, T );
	     glVertex2fv( v1 );
	     glTexCoord2f( L, B );
	     glVertex2fv( v2 );
	     glTexCoord2f( R, T );
	     glVertex2fv( v3 );
	     glTexCoord2f( R, B );
	     glVertex2fv( v4 );
	    glEnd();
	    glTranslatef( scale, 0, 0 );
	}

    private:
	float v1[2], v2[2], v3[2], v4[2];
	unsigned int texNum;	//number of texture to activate
	float L, T, R, B;	//coordinates for mapping
};


class Word
{
    friend class Writer;
    public:
	Word( const char * text, QMap<char, Symbol *> * map, float scale = 1.0 );

	inline void renderWord( double dT );
	inline bool isDead();

    private:
	float width, scale, cX, cY;
	float vScale, vX, vY;
	float activateTime, lifeTime, currentTime;
	float color[4];
	QPtrList<Symbol> symbolList;
};


/*
 *
 **/
class Writer
{
    public:
	Writer( QString descFileName );
	~Writer();

	//types of effects implemented
	enum effectType { NoEffect = 0, Sequence, Fun1, Fun2 };

	//call this function to add a sentence to the renderer
	void spawnWords( QString phrase, effectType fx = NoEffect );

	//called to get the words on screen using OpenGL
	//Note: the context must be set up. Words are drawn on XY plane
	//inside a rectangle with 10 units's side.
	void render( double dT );

    private:
	//misc utility functions
	bool loadMap( QString );

	//texture 'references' used by GL to delete allocated textures
	int numTextures;
	unsigned int texArray[16];

	//list of words and map of symbols
	QPtrList<Word> wordList;
	QMap<char, Symbol *> symbolMap;

	//disables standard constructor
	Writer();
};

#endif

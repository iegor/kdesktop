/****************************************************************************
** 
**
** ???
**
** Copyright (C) 2002-2003 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "QtUnicode.h"

using namespace T2P;
// START OF GENERATED DATA

// copied form qfont.h, as we can't include it in tools. Do not modify without
// changing the script enum in qfont.h aswell.
const unsigned char QtUnicode::otherScripts [120] = {
#define SCRIPTS_02 0
    0xaf, Latin, 0xff, SpacingModifiers, 			// row 0x02, index 0
#define SCRIPTS_03 4
    0x6f, CombiningMarks, 0xff, Greek, 			// row 0x03, index 4
#define SCRIPTS_05 8
    0x2f, Cyrillic, 0x8f, Armenian, 0xff, Hebrew,	// row 0x05, index 8
#define SCRIPTS_07 14
    0x4f, Syriac, 0x7f, Unicode, 0xbf, Thaana,
    0xff, Unicode, 						// row 0x07, index 14
#define SCRIPTS_10 22
    0x9f, Myanmar, 0xff, Georgian,			// row 0x10, index 20
#define SCRIPTS_13 26
    0x7f, Ethiopic, 0x9f, Unicode, 0xff, Cherokee,	// row 0x13, index 24
#define SCRIPTS_16 32
    0x7f, CanadianAboriginal, 0x9f, Ogham,
    0xff, Runic, 						// row 0x16 index 30
#define SCRIPTS_17 38
    0x1f, Tagalog, 0x3f, Hanunoo, 0x5f, Buhid,
    0x7f, Tagbanwa, 0xff, Khmer,				// row 0x17, index 36
#define SCRIPTS_18 48
    0xaf, Mongolian, 0xff, Unicode,		       	// row 0x18, index 46
#define SCRIPTS_20 52
    0x0b, Unicode, 0x0d, UnknownScript, 0x6f, Unicode, 0x9f, NumberForms,
    0xab, CurrencySymbols, 0xac, Latin,
    0xcf, CurrencySymbols, 0xff, CombiningMarks,		// row 0x20, index 50
#define SCRIPTS_21 68
    0x4f, LetterlikeSymbols, 0x8f, NumberForms,
    0xff, MathematicalOperators,					// row 0x21, index 62
#define SCRIPTS_24 74
    0x5f, TechnicalSymbols, 0xff, EnclosedAndSquare,	// row 0x24, index 68
#define SCRIPTS_2e 78
    0x7f, Unicode, 0xff, Han,				// row 0x2e, index 72
#define SCRIPTS_30 82
    0x3f, Han, 0x9f, Hiragana, 0xff, Katakana,	// row 0x30, index 76
#define SCRIPTS_31 88
    0x2f, Bopomofo, 0x8f, Hangul, 0x9f, Han,
    0xff, Unicode,						// row 0x31, index 82
#define SCRIPTS_fb 96
    0x06, Latin, 0x1c, Unicode, 0x4f, Hebrew,
    0xff, Arabic,						// row 0xfb, index 90
#define SCRIPTS_fe 104
    0x1f, Unicode, 0x2f, CombiningMarks, 0x6f, Unicode,
    0xff, Arabic,						// row 0xfe, index 98
#define SCRIPTS_ff 112
    0x5e, Katakana, 0x60, Unicode,        		// row 0xff, index 106
    0x9f, KatakanaHalfWidth, 0xff, Unicode
};

// (uc-0x0900)>>7
const unsigned char QtUnicode::indicScripts [] =
{
    Devanagari, Bengali,
    Gurmukhi, Gujarati,
    Oriya, Tamil,
    Telugu, Kannada,
    Malayalam, Sinhala,
    Thai, Lao
};


// 0x80 + x: x is the offset into the otherScripts table
const unsigned char QtUnicode::scriptTable[256] =
{
    Latin, Latin, 0x80+SCRIPTS_02, 0x80+SCRIPTS_03,
    Cyrillic, 0x80+SCRIPTS_05, Arabic, 0x80+SCRIPTS_07,
    Unicode, SCRIPTS_INDIC, SCRIPTS_INDIC, SCRIPTS_INDIC,
    SCRIPTS_INDIC, SCRIPTS_INDIC, SCRIPTS_INDIC, Tibetan,

    0x80+SCRIPTS_10, Hangul, Ethiopic, 0x80+SCRIPTS_13,
    CanadianAboriginal, CanadianAboriginal, 0x80+SCRIPTS_16, 0x80+SCRIPTS_17,
    0x80+SCRIPTS_18, Unicode, Unicode, Unicode,
    Unicode, Unicode, Latin, Greek,

    0x80+SCRIPTS_20, 0x80+SCRIPTS_21, MathematicalOperators, TechnicalSymbols,
    0x80+SCRIPTS_24, GeometricSymbols, MiscellaneousSymbols, MiscellaneousSymbols,
    Braille, Unicode, Unicode, Unicode,
    Unicode, Unicode, 0x80+SCRIPTS_2e, Han,

    0x80+SCRIPTS_30, 0x80+SCRIPTS_31, EnclosedAndSquare, EnclosedAndSquare,
    Han, Han, Han, Han,
    Han, Han, Han, Han,
    Han, Han, Han, Han,

    Han, Han, Han, Han, Han, Han, Han, Han,
    Han, Han, Han, Han, Han, Han, Han, Han,

    Han, Han, Han, Han, Han, Han, Han, Han,
    Han, Han, Han, Han, Han, Han, Han, Han,

    Han, Han, Han, Han, Han, Han, Han, Han,
    Han, Han, Han, Han, Han, Han, Han, Han,

    Han, Han, Han, Han, Han, Han, Han, Han,
    Han, Han, Han, Han, Han, Han, Han, Han,


    Han, Han, Han, Han, Han, Han, Han, Han,
    Han, Han, Han, Han, Han, Han, Han, Han,

    Han, Han, Han, Han, Han, Han, Han, Han,
    Han, Han, Han, Han, Han, Han, Han, Han,

    Yi, Yi, Yi, Yi, Yi, Unicode, Unicode, Unicode,
    Unicode, Unicode, Unicode, Unicode, Hangul, Hangul, Hangul, Hangul,

    Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul,
    Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul,

    Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul,
    Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul,

    Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul, Hangul,
    Unicode, Unicode, Unicode, Unicode, Unicode, Unicode, Unicode, Unicode,

    Unicode, Unicode, Unicode, Unicode, Unicode, Unicode, Unicode, Unicode,
    Unicode, Unicode, Unicode, Unicode, Unicode, Unicode, Unicode, Unicode,

    Unicode, Unicode, Unicode, Unicode, Unicode, Unicode, Unicode, Unicode,
    Unicode, Han, Han, 0x80+SCRIPTS_fb, Arabic, Arabic, 0x80+SCRIPTS_fe, 0x80+SCRIPTS_ff
};


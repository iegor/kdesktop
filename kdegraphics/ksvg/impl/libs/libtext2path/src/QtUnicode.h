/*
    Copyright (C) 2003 Nikolas Zimmermann <wildfox@kde.org>
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef T2P_QT_UNICODE_H
#define T2P_QT_UNICODE_H

namespace T2P
{
	enum Script 
	{
		// European Alphabetic Scripts
		Latin,
		Greek,
		Cyrillic,
		Armenian,
		Georgian,
		Runic,
		Ogham,
		SpacingModifiers,
		CombiningMarks,

		// Middle Eastern Scripts
		Hebrew,
		Arabic,
		Syriac,
		Thaana,

		// South and Southeast Asian Scripts
		Devanagari,
		Bengali,
		Gurmukhi,
		Gujarati,
		Oriya,
		Tamil,
		Telugu,
		Kannada,
		Malayalam,
		Sinhala,
		Thai,
		Lao,
		Tibetan,
		Myanmar,
		Khmer,

		// East Asian Scripts
		Han,
		Hiragana,
		Katakana,
		Hangul,
		Bopomofo,
		Yi,

		// Additional Scripts
		Ethiopic,
		Cherokee,
		CanadianAboriginal,
		Mongolian,

		// Symbols
		CurrencySymbols,
		LetterlikeSymbols,
		NumberForms,
		MathematicalOperators,
		TechnicalSymbols,
		GeometricSymbols,
		MiscellaneousSymbols,
		EnclosedAndSquare,
		Braille,

		Unicode,

		// some scripts added in Unicode 3.2
		Tagalog,
		Hanunoo,
		Buhid,
		Tagbanwa,

		KatakanaHalfWidth,

		// End
		NScripts,
		UnknownScript = NScripts
	};

	class QtUnicode
	{
	public:
		QtUnicode() { }
		~QtUnicode() { }

		static int scriptForChar(unsigned short uc)
		{
			unsigned char script = QtUnicode::scriptTable[(uc >> 8)];
			if(script >= QtUnicode::SCRIPTS_INDIC)
			{
				if(script == QtUnicode::SCRIPTS_INDIC)
					script = QtUnicode::indicScripts[(uc - 0x0900) >> 7];
				else
				{
					// 0x80 + SCRIPTS_xx
					unsigned char index = script - 0x80;
					unsigned char cell = uc & 0xff;
					while(QtUnicode::otherScripts[index++] < cell)
						index++;
					script = QtUnicode::otherScripts[index];
				}
			}
	
			return script;
		}

	private:
	    enum
		{
			SCRIPTS_INDIC = 0x7e
		};

		static const unsigned char otherScripts[];
		static const unsigned char indicScripts[];
		static const unsigned char scriptTable[];
	};
}

#define SCRIPT_FOR_CHAR(script, c) \
if(c < 0x100) \
	script = T2P::Latin; \
else \
	script = (T2P::Script) QtUnicode::scriptForChar(c); 

#endif

// vim:ts=4:noet

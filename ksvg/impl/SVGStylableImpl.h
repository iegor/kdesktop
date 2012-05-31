/*
    Copyright (C) 2001-2003 KSVG Team
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
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef SVGStylableImpl_H
#define SVGStylableImpl_H

#include <qstring.h>
#include <qstringlist.h>

#include <dom/dom_string.h>

#include "ksvg_lookup.h"

class QRect;

// from libart, but now no dependency
typedef enum
{
  PATH_STROKE_JOIN_MITER,
  PATH_STROKE_JOIN_ROUND,
  PATH_STROKE_JOIN_BEVEL
} PathStrokeJoinType;

typedef enum
{
  PATH_STROKE_CAP_BUTT,
  PATH_STROKE_CAP_ROUND,
  PATH_STROKE_CAP_SQUARE
} PathStrokeCapType;

#define SVG_STYLE_FLAG_NONE							0x0000000000000ULL
#define SVG_STYLE_FLAG_BASELINE_SHIFT				0x0000000000001ULL
#define SVG_STYLE_FLAG_CLIP_PATH					0x0000000000002ULL
#define SVG_STYLE_FLAG_CLIP_RULE					0x0000000000004ULL
#define SVG_STYLE_FLAG_COLOR						0x0000000000008ULL
#define SVG_STYLE_FLAG_COLOR_INTERPOLATION			0x0000000000010ULL
#define SVG_STYLE_FLAG_COLOR_INTERPOLATION_FILTERS	0x0000000000020ULL
#define SVG_STYLE_FLAG_COLOR_PROFILE				0x0000000000040ULL
#define SVG_STYLE_FLAG_COLOR_RENDERING				0x0000000000080ULL
#define SVG_STYLE_FLAG_CURSOR						0x0000000000100ULL
#define SVG_STYLE_FLAG_DIRECTION					0x0000000000200ULL
#define SVG_STYLE_FLAG_DISPLAY						0x0000000000400ULL
#define SVG_STYLE_FLAG_FILL							0x0000000000800ULL
#define SVG_STYLE_FLAG_FILL_OPACITY					0x0000000001000ULL
#define SVG_STYLE_FLAG_FILL_RULE					0x0000000002000ULL
#define SVG_STYLE_FLAG_FONT_FAMILY					0x0000000004000ULL
#define SVG_STYLE_FLAG_FONT_SIZE					0x0000000008000ULL
#define SVG_STYLE_FLAG_FONT_SIZE_ADJUST				0x0000000010000ULL
#define SVG_STYLE_FLAG_FONT_STRETCH					0x0000000020000ULL
#define SVG_STYLE_FLAG_FONT_STYLE					0x0000000040000ULL
#define SVG_STYLE_FLAG_FONT_VARIANT					0x0000000080000ULL
#define SVG_STYLE_FLAG_FONT_WEIGHT					0x0000000100000ULL
#define SVG_STYLE_FLAG_GLYPH_ORIENTATION_HORIZONTAL	0x0000000200000ULL
#define SVG_STYLE_FLAG_GLYPH_ORIENTATION_VERTICAL	0x0000000400000ULL
#define SVG_STYLE_FLAG_IMAGE_RENDERING				0x0000000800000ULL
#define SVG_STYLE_FLAG_KERNING						0x0000001000000ULL
#define SVG_STYLE_FLAG_LETTER_SPACING				0x0000002000000ULL
#define SVG_STYLE_FLAG_MARKER						0x0000004000000ULL
#define SVG_STYLE_FLAG_MARKER_END					0x0000008000000ULL
#define SVG_STYLE_FLAG_MARKER_MID					0x0000010000000ULL
#define SVG_STYLE_FLAG_MARKER_START					0x0000020000000ULL
#define SVG_STYLE_FLAG_OPACITY						0x0000040000000ULL
#define SVG_STYLE_FLAG_POINTER_EVENTS				0x0000080000000ULL
#define SVG_STYLE_FLAG_SHAPE_RENDERING				0x0000100000000ULL
#define SVG_STYLE_FLAG_STOP							0x0000200000000ULL
#define SVG_STYLE_FLAG_STROKE_DASH_ARRAY			0x0000400000000ULL
#define SVG_STYLE_FLAG_STROKE_DASH_OFFSET			0x0000800000000ULL
#define SVG_STYLE_FLAG_STROKE_LINE_CAP				0x0001000000000ULL
#define SVG_STYLE_FLAG_STROKE_LINE_JOIN				0x0002000000000ULL
#define SVG_STYLE_FLAG_STROKE_MITER_LIMIT			0x0004000000000ULL
#define SVG_STYLE_FLAG_STROKE_OPACITY				0x0008000000000ULL
#define SVG_STYLE_FLAG_STROKE						0x0010000000000ULL
#define SVG_STYLE_FLAG_STROKE_WIDTH					0x0020000000000ULL
#define SVG_STYLE_FLAG_TEXT_ANCHOR					0x0040000000000ULL
#define SVG_STYLE_FLAG_TEXT_DECORATION				0x0080000000000ULL
#define SVG_STYLE_FLAG_TEXT_DIRECTION				0x0100000000000ULL
#define SVG_STYLE_FLAG_TEXT_RENDERING				0x0200000000000ULL
#define SVG_STYLE_FLAG_TEXT_UNICODE_BIDI			0x0400000000000ULL
#define SVG_STYLE_FLAG_TEXT_WRITING_MODE			0x0800000000000ULL
#define SVG_STYLE_FLAG_VISIBILITY					0x1000000000000ULL
#define SVG_STYLE_FLAG_WORD_SPACING					0x2000000000000ULL
#define SVG_STYLE_FLAG_OVERFLOW						0x4000000000000ULL
#define SVG_STYLE_FLAG_MASK								0x0008000000000000ULL

//?#define SVG_STYLE_FLAG_MASK_INHERIT				0x0010000000000000ULL

// Helper macros
#define STYLE_GET(Return, Name, Member) \
protected:\
 Return Member;\
public:\
Return get ##Name () const { return Member; }

#define STYLE_GET_OPTIM(Return, Name, Member, Optim) \
protected:\
 Return Member : Optim;\
public:\
Return get ##Name () const { return Member; }

#define STYLE_HAS(Name, Member) \
bool has##Name () { return ! Member .isEmpty(); }

namespace KSVG
{

enum ECursor
{
	CURSOR_AUTO,
	CURSOR_CROSSHAIR,
	CURSOR_DEFAULT,
	CURSOR_POINTER,
	CURSOR_MOVE,
	CURSOR_E_RESIZE,
	CURSOR_NE_RESIZE,
	CURSOR_N_RESIZE,
	CURSOR_NW_RESIZE,
	CURSOR_SE_RESIZE,
	CURSOR_SW_RESIZE,
	CURSOR_S_RESIZE,
	CURSOR_W_RESIZE,
	CURSOR_TEXT,
	CURSOR_WAIT,
	CURSOR_HELP
};

enum ETextDirection
{
	LTR, RTL
};

enum ETextDecoration
{
	TDNONE = 0, UNDERLINE = 1, OVERLINE = 2, LINE_THROUGH = 4
};

enum ETextAnchor
{
	TASTART, TAMIDDLE, TAEND
};

enum EUnicodeBidi
{
	UBNORMAL, EMBED, OVERRIDE, INHERIT
};

enum EFontStyle
{
	FSNORMAL, ITALIC, OBLIQUE
};

enum ETextWritingMode
{
	LR, RL, TB
};

enum EWindRule
{
	RULE_NONZERO,
	RULE_EVENODD
};

enum EPointerEvents
{
	PE_NONE,
	PE_STROKE,
	PE_FILL,
	PE_PAINTED,
	PE_VISIBLE,
	PE_VISIBLE_STROKE,
	PE_VISIBLE_FILL,
	PE_VISIBLE_PAINTED,
	PE_ALL,
	PE_INHERIT
};

enum EColorInterpolation
{
	CI_SRGB,
	CI_LINEARRGB
};

class SVGColorImpl;
class SVGPaintImpl;
class SVGLengthImpl;
class SVGAnimatedLengthImpl;
class SVGAnimatedLengthListImpl;
class SVGMatrixImpl;
class SVGStringListImpl;
class SVGElementImpl;
class SVGStylableImpl
{
public:
	SVGStylableImpl(SVGElementImpl *object);
	virtual ~SVGStylableImpl();
	
	static void setColor(const QString &, SVGColorImpl *);

	STYLE_GET(SVGAnimatedLengthImpl *, DashOffset, m_dashOffset)
	STYLE_GET(SVGAnimatedLengthListImpl *, DashArray, m_dashArray)
	STYLE_GET(SVGPaintImpl *, StrokeColor, m_strokeColor)
	STYLE_GET(SVGPaintImpl *, FillColor, m_fillColor)
	STYLE_GET(SVGColorImpl *, StopColor, m_stopColor)
	STYLE_GET(SVGColorImpl *, Color, m_color)
	STYLE_GET(SVGAnimatedLengthImpl *, StrokeWidth, m_strokeWidth)
	STYLE_GET(SVGStringListImpl *, FontFamily, m_fontFamily);
	STYLE_GET(float, FontSize, m_fontSize)
	STYLE_GET(QString, LetterSpacing, m_letterSpacing)
	STYLE_GET(QString, WordSpacing, m_wordSpacing)
	STYLE_GET(QString, FontWeight, m_fontWeight)
	STYLE_GET(QString, ClipPath, m_clipPath)
	STYLE_GET(QString, Mask, m_mask)
	STYLE_GET(QString, StartMarker, m_startMarker)
	STYLE_GET(QString, MidMarker, m_midMarker)
	STYLE_GET(QString, EndMarker, m_endMarker)
	STYLE_GET(QString, BaselineShift, m_baselineShift)
	STYLE_GET(QString, GlyphOrientationVertical, m_glyphOrientationVertical)
	STYLE_GET(QString, GlyphOrientationHorizontal, m_glyphOrientationHorizontal)
	STYLE_GET(float, Opacity, m_opacity)
	STYLE_GET(float, FillOpacity, m_fillOpacity)
	STYLE_GET(float, StrokeOpacity, m_strokeOpacity)
	STYLE_GET(unsigned int, StrokeMiterlimit, m_strokeMiterlimit)

	STYLE_GET_OPTIM(EPointerEvents, PointerEvents, m_pointerEvents, 4) 
	STYLE_GET_OPTIM(ECursor, Cursor, m_cursor, 4) 
	STYLE_GET_OPTIM(EWindRule, FillRule, m_fillRule, 1)
	STYLE_GET_OPTIM(EWindRule, ClipRule, m_clipRule, 1)
	STYLE_GET_OPTIM(PathStrokeCapType, CapStyle, m_capStyle, 2)
	STYLE_GET_OPTIM(PathStrokeJoinType, JoinStyle, m_joinStyle, 2)
	STYLE_GET_OPTIM(ETextDirection, TextDirection, m_textDirection, 1)
	STYLE_GET_OPTIM(unsigned int, TextDecoration, m_textDecoration, 3)
	STYLE_GET_OPTIM(ETextAnchor, TextAnchor, m_textAnchor, 2)
	STYLE_GET_OPTIM(EUnicodeBidi, TextUnicodeBidi, m_textUnicodeBidi, 2)
	STYLE_GET_OPTIM(ETextWritingMode, TextWritingMode, m_textWritingMode, 2)
	STYLE_GET_OPTIM(EFontStyle, FontStyle, m_fontStyle, 2)
	STYLE_GET_OPTIM(bool, Overflow, m_overflow, 1)
	STYLE_GET_OPTIM(bool, Visible, m_visible, 1)
	STYLE_GET_OPTIM(bool, Display, m_display, 1)
	STYLE_GET_OPTIM(EColorInterpolation, ColorInterpolation, m_colorInterpolation, 1)
	
	STYLE_HAS(ClipPath, m_clipPath)
	STYLE_HAS(Mask, m_mask)
	STYLE_HAS(StartMarker, m_startMarker)
	STYLE_HAS(MidMarker, m_midMarker)
	STYLE_HAS(EndMarker, m_endMarker)

	// Special "set" cases
	void setStartMarker(const QString &);
	void setMidMarker(const QString &);
	void setEndMarker(const QString &);
	void setMarkers(const QString &);

	// Special "has" cases
	bool hasMarkers() const;

	// Special "is" cases
	bool isStroked() const;
	bool isFilled() const;

	// Function which sets default values
	void processStyle();
	
	// Special virtual functions
	virtual void setClip(const QString &clip);
	virtual QRect clip();

protected:
	float fontSizeForText(const QString &);

	SVGElementImpl *m_object;

private:
	void setPaint(const QString &, SVGPaintImpl *);
	QString extractUrlId(const QString& string);

	unsigned long long m_flags;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		ClassName, Style,
		StrokeWidth, StrokeMiterlimit, StrokeLineCap, StrokeLineJoin,
	 	Stroke, Fill, Color, StopColor, FontSize, FontFamily, FontWeight,
		FontStyle, TextDecoration, TextAnchor, Direction, WritingMode,
		UnicodeBidi, Opacity, FillOpacity, StrokeOpacity, ClipPath,
		MarkerStart, MarkerMid, MarkerEnd, Marker, Cursor, Display,
		Overflow, Clip, Visibility, FillRule, ClipRule,
		StrokeDashOffset, StrokeDashArray, ColorProfile, BaselineShift,
		LetterSpacing, WordSpacing, PointerEvents,
		GlyphOrientationVertical, GlyphOrientationHorizontal,
		ColorInterpolation, Mask,
		// Functions
		GetStyle
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

KSVG_DEFINE_PROTOTYPE(SVGStylableImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGStylableImplProtoFunc, SVGStylableImpl)

#endif

// vim:ts=4:noet

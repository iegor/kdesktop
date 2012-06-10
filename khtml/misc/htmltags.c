/* ANSI-C code produced by gperf version 3.0.1 */
/* Command-line: gperf -a -L ANSI-C -P -D -E -C -l -o -t -k '*' -NfindTag -Hhash_tag -Wwordlist_tag -Qspool_Tag htmltags.gperf  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "htmltags.gperf"

/* This file is automatically generated from htmltags.in by maketags, do not edit */
/* Copyright 1999 Lars Knoll */
#include "htmltags.h"
#line 6 "htmltags.gperf"
struct tags {
    int name;
    int id;
};
/* maximum key range = 345, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_tag (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
       70,  60,  55,  40,  25,  15, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346,  15,  10,  35,
       10,   5,  10,   0,  55,  40,  70,  30,   5,  15,
       30,  50,   5,  50,  20,   0,   0,  90, 120,   5,
       15,  35, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346, 346, 346, 346,
      346, 346, 346, 346, 346, 346, 346
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]+1];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct spool_Tag_t
  {
    char spool_Tag_str0[sizeof("s")];
    char spool_Tag_str1[sizeof("tr")];
    char spool_Tag_str2[sizeof("p")];
    char spool_Tag_str3[sizeof("td")];
    char spool_Tag_str4[sizeof("b")];
    char spool_Tag_str5[sizeof("br")];
    char spool_Tag_str6[sizeof("pre")];
    char spool_Tag_str7[sizeof("a")];
    char spool_Tag_str8[sizeof("dd")];
    char spool_Tag_str9[sizeof("dl")];
    char spool_Tag_str10[sizeof("del")];
    char spool_Tag_str11[sizeof("base")];
    char spool_Tag_str12[sizeof("map")];
    char spool_Tag_str13[sizeof("samp")];
    char spool_Tag_str14[sizeof("table")];
    char spool_Tag_str15[sizeof("em")];
    char spool_Tag_str16[sizeof("area")];
    char spool_Tag_str17[sizeof("label")];
    char spool_Tag_str18[sizeof("i")];
    char spool_Tag_str19[sizeof("th")];
    char spool_Tag_str20[sizeof("dfn")];
    char spool_Tag_str21[sizeof("meta")];
    char spool_Tag_str22[sizeof("col")];
    char spool_Tag_str23[sizeof("font")];
    char spool_Tag_str24[sizeof("frame")];
    char spool_Tag_str25[sizeof("q")];
    char spool_Tag_str26[sizeof("xmp")];
    char spool_Tag_str27[sizeof("form")];
    char spool_Tag_str28[sizeof("hr")];
    char spool_Tag_str29[sizeof("frameset")];
    char spool_Tag_str30[sizeof("code")];
    char spool_Tag_str31[sizeof("small")];
    char spool_Tag_str32[sizeof("select")];
    char spool_Tag_str33[sizeof("address")];
    char spool_Tag_str34[sizeof("wbr")];
    char spool_Tag_str35[sizeof("body")];
    char spool_Tag_str36[sizeof("embed")];
    char spool_Tag_str37[sizeof("legend")];
    char spool_Tag_str38[sizeof("ol")];
    char spool_Tag_str39[sizeof("bdo")];
    char spool_Tag_str40[sizeof("nobr")];
    char spool_Tag_str41[sizeof("param")];
    char spool_Tag_str42[sizeof("h6")];
    char spool_Tag_str43[sizeof("img")];
    char spool_Tag_str44[sizeof("thead")];
    char spool_Tag_str45[sizeof("li")];
    char spool_Tag_str46[sizeof("kbd")];
    char spool_Tag_str47[sizeof("layer")];
    char spool_Tag_str48[sizeof("script")];
    char spool_Tag_str49[sizeof("h5")];
    char spool_Tag_str50[sizeof("big")];
    char spool_Tag_str51[sizeof("abbr")];
    char spool_Tag_str52[sizeof("title")];
    char spool_Tag_str53[sizeof("applet")];
    char spool_Tag_str54[sizeof("noembed")];
    char spool_Tag_str55[sizeof("textarea")];
    char spool_Tag_str56[sizeof("u")];
    char spool_Tag_str57[sizeof("tt")];
    char spool_Tag_str58[sizeof("ins")];
    char spool_Tag_str59[sizeof("head")];
    char spool_Tag_str60[sizeof("image")];
    char spool_Tag_str61[sizeof("h4")];
    char spool_Tag_str62[sizeof("span")];
    char spool_Tag_str63[sizeof("iframe")];
    char spool_Tag_str64[sizeof("dt")];
    char spool_Tag_str65[sizeof("dir")];
    char spool_Tag_str66[sizeof("tfoot")];
    char spool_Tag_str67[sizeof("center")];
    char spool_Tag_str68[sizeof("ul")];
    char spool_Tag_str69[sizeof("noframes")];
    char spool_Tag_str70[sizeof("h3")];
    char spool_Tag_str71[sizeof("fieldset")];
    char spool_Tag_str72[sizeof("cite")];
    char spool_Tag_str73[sizeof("keygen")];
    char spool_Tag_str74[sizeof("h2")];
    char spool_Tag_str75[sizeof("nolayer")];
    char spool_Tag_str76[sizeof("basefont")];
    char spool_Tag_str77[sizeof("h1")];
    char spool_Tag_str78[sizeof("sup")];
    char spool_Tag_str79[sizeof("sub")];
    char spool_Tag_str80[sizeof("plaintext")];
    char spool_Tag_str81[sizeof("tbody")];
    char spool_Tag_str82[sizeof("ilayer")];
    char spool_Tag_str83[sizeof("link")];
    char spool_Tag_str84[sizeof("style")];
    char spool_Tag_str85[sizeof("noscript")];
    char spool_Tag_str86[sizeof("isindex")];
    char spool_Tag_str87[sizeof("menu")];
    char spool_Tag_str88[sizeof("listing")];
    char spool_Tag_str89[sizeof("var")];
    char spool_Tag_str90[sizeof("html")];
    char spool_Tag_str91[sizeof("caption")];
    char spool_Tag_str92[sizeof("acronym")];
    char spool_Tag_str93[sizeof("input")];
    char spool_Tag_str94[sizeof("strike")];
    char spool_Tag_str95[sizeof("strong")];
    char spool_Tag_str96[sizeof("object")];
    char spool_Tag_str97[sizeof("marquee")];
    char spool_Tag_str98[sizeof("div")];
    char spool_Tag_str99[sizeof("button")];
    char spool_Tag_str100[sizeof("colgroup")];
    char spool_Tag_str101[sizeof("option")];
    char spool_Tag_str102[sizeof("anchor")];
    char spool_Tag_str103[sizeof("optgroup")];
    char spool_Tag_str104[sizeof("blockquote")];
  };
static const struct spool_Tag_t spool_Tag_contents =
  {
    "s",
    "tr",
    "p",
    "td",
    "b",
    "br",
    "pre",
    "a",
    "dd",
    "dl",
    "del",
    "base",
    "map",
    "samp",
    "table",
    "em",
    "area",
    "label",
    "i",
    "th",
    "dfn",
    "meta",
    "col",
    "font",
    "frame",
    "q",
    "xmp",
    "form",
    "hr",
    "frameset",
    "code",
    "small",
    "select",
    "address",
    "wbr",
    "body",
    "embed",
    "legend",
    "ol",
    "bdo",
    "nobr",
    "param",
    "h6",
    "img",
    "thead",
    "li",
    "kbd",
    "layer",
    "script",
    "h5",
    "big",
    "abbr",
    "title",
    "applet",
    "noembed",
    "textarea",
    "u",
    "tt",
    "ins",
    "head",
    "image",
    "h4",
    "span",
    "iframe",
    "dt",
    "dir",
    "tfoot",
    "center",
    "ul",
    "noframes",
    "h3",
    "fieldset",
    "cite",
    "keygen",
    "h2",
    "nolayer",
    "basefont",
    "h1",
    "sup",
    "sub",
    "plaintext",
    "tbody",
    "ilayer",
    "link",
    "style",
    "noscript",
    "isindex",
    "menu",
    "listing",
    "var",
    "html",
    "caption",
    "acronym",
    "input",
    "strike",
    "strong",
    "object",
    "marquee",
    "div",
    "button",
    "colgroup",
    "option",
    "anchor",
    "optgroup",
    "blockquote"
  };
#define spool_Tag ((const char *) &spool_Tag_contents)
#ifdef __GNUC__
__inline
#endif
const struct tags *
findTag (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 105,
      MIN_WORD_LENGTH = 1,
      MAX_WORD_LENGTH = 10,
      MIN_HASH_VALUE = 1,
      MAX_HASH_VALUE = 345
    };

  static const unsigned char lengthtable[] =
    {
       1,  2,  1,  2,  1,  2,  3,  1,  2,  2,  3,  4,  3,  4,
       5,  2,  4,  5,  1,  2,  3,  4,  3,  4,  5,  1,  3,  4,
       2,  8,  4,  5,  6,  7,  3,  4,  5,  6,  2,  3,  4,  5,
       2,  3,  5,  2,  3,  5,  6,  2,  3,  4,  5,  6,  7,  8,
       1,  2,  3,  4,  5,  2,  4,  6,  2,  3,  5,  6,  2,  8,
       2,  8,  4,  6,  2,  7,  8,  2,  3,  3,  9,  5,  6,  4,
       5,  8,  7,  4,  7,  3,  4,  7,  7,  5,  6,  6,  6,  7,
       3,  6,  8,  6,  6,  8, 10
    };
  static const struct tags wordlist_tag[] =
    {
#line 87 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str0, ID_S},
#line 106 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str1, ID_TR},
#line 82 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str2, ID_P},
#line 100 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str3, ID_TD},
#line 17 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str4, ID_B},
#line 24 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str5, ID_BR},
#line 85 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str6, ID_PRE},
#line 11 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str7, ID_A},
#line 32 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str8, ID_DD},
#line 37 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str9, ID_DL},
#line 33 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str10, ID_DEL},
#line 18 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str11, ID_BASE},
#line 69 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str12, ID_MAP},
#line 88 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str13, ID_SAMP},
#line 98 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str14, ID_TABLE},
#line 39 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str15, ID_EM},
#line 16 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str16, ID_AREA},
#line 64 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str17, ID_LABEL},
#line 55 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str18, ID_I},
#line 103 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str19, ID_TH},
#line 34 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str20, ID_DFN},
#line 72 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str21, ID_META},
#line 30 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str22, ID_COL},
#line 42 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str23, ID_FONT},
#line 44 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str24, ID_FRAME},
#line 86 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str25, ID_Q},
#line 112 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str26, ID_XMP},
#line 43 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str27, ID_FORM},
#line 53 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str28, ID_HR},
#line 45 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str29, ID_FRAMESET},
#line 29 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str30, ID_CODE},
#line 91 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str31, ID_SMALL},
#line 90 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str32, ID_SELECT},
#line 14 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str33, ID_ADDRESS},
#line 111 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str34, ID_WBR},
#line 23 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str35, ID_BODY},
#line 40 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str36, ID_EMBED},
#line 66 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str37, ID_LEGEND},
#line 79 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str38, ID_OL},
#line 20 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str39, ID_BDO},
#line 73 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str40, ID_NOBR},
#line 83 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str41, ID_PARAM},
#line 51 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str42, ID_H6},
#line 58 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str43, ID_IMG},
#line 104 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str44, ID_THEAD},
#line 67 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str45, ID_LI},
#line 62 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str46, ID_KBD},
#line 65 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str47, ID_LAYER},
#line 89 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str48, ID_SCRIPT},
#line 50 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str49, ID_H5},
#line 21 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str50, ID_BIG},
#line 12 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str51, ID_ABBR},
#line 105 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str52, ID_TITLE},
#line 15 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str53, ID_APPLET},
#line 74 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str54, ID_NOEMBED},
#line 101 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str55, ID_TEXTAREA},
#line 108 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str56, ID_U},
#line 107 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str57, ID_TT},
#line 60 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str58, ID_INS},
#line 52 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str59, ID_HEAD},
#line 114 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str60, ID_IMG},
#line 49 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str61, ID_H4},
#line 92 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str62, ID_SPAN},
#line 56 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str63, ID_IFRAME},
#line 38 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str64, ID_DT},
#line 35 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str65, ID_DIR},
#line 102 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str66, ID_TFOOT},
#line 27 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str67, ID_CENTER},
#line 109 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str68, ID_UL},
#line 75 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str69, ID_NOFRAMES},
#line 48 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str70, ID_H3},
#line 41 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str71, ID_FIELDSET},
#line 28 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str72, ID_CITE},
#line 63 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str73, ID_KEYGEN},
#line 47 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str74, ID_H2},
#line 77 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str75, ID_NOLAYER},
#line 19 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str76, ID_BASEFONT},
#line 46 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str77, ID_H1},
#line 97 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str78, ID_SUP},
#line 96 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str79, ID_SUB},
#line 84 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str80, ID_PLAINTEXT},
#line 99 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str81, ID_TBODY},
#line 57 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str82, ID_ILAYER},
#line 68 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str83, ID_LINK},
#line 95 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str84, ID_STYLE},
#line 76 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str85, ID_NOSCRIPT},
#line 61 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str86, ID_ISINDEX},
#line 71 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str87, ID_MENU},
#line 115 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str88, ID_PRE},
#line 110 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str89, ID_VAR},
#line 54 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str90, ID_HTML},
#line 26 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str91, ID_CAPTION},
#line 13 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str92, ID_ACRONYM},
#line 59 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str93, ID_INPUT},
#line 93 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str94, ID_STRIKE},
#line 94 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str95, ID_STRONG},
#line 78 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str96, ID_OBJECT},
#line 70 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str97, ID_MARQUEE},
#line 36 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str98, ID_DIV},
#line 25 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str99, ID_BUTTON},
#line 31 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str100, ID_COLGROUP},
#line 81 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str101, ID_OPTION},
#line 113 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str102, ID_A},
#line 80 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str103, ID_OPTGROUP},
#line 22 "htmltags.gperf"
      {(int)(long)&((struct spool_Tag_t *)0)->spool_Tag_str104, ID_BLOCKQUOTE}
    };

  static const signed char lookup[] =
    {
       -1,   0,   1,  -1,  -1,  -1,   2,   3,  -1,  -1,
       -1,   4,   5,   6,  -1,  -1,   7,   8,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,   9,  10,  11,
       -1,  -1,  -1,  12,  13,  14,  -1,  15,  -1,  16,
       17,  18,  19,  20,  21,  -1,  -1,  -1,  22,  23,
       24,  25,  -1,  26,  27,  -1,  -1,  28,  29,  30,
       31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
       41,  -1,  42,  43,  -1,  44,  -1,  45,  46,  -1,
       47,  48,  49,  50,  51,  52,  53,  54,  55,  -1,
       -1,  56,  57,  58,  59,  60,  -1,  61,  -1,  62,
       -1,  63,  64,  65,  -1,  66,  67,  68,  69,  -1,
       -1,  -1,  70,  71,  72,  -1,  73,  74,  -1,  -1,
       -1,  -1,  75,  76,  -1,  -1,  -1,  77,  78,  -1,
       -1,  -1,  -1,  79,  80,  81,  82,  -1,  -1,  83,
       84,  -1,  -1,  85,  -1,  -1,  -1,  86,  -1,  87,
       -1,  -1,  88,  89,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  90,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  91,  -1,  -1,
       -1,  -1,  92,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       93,  94,  -1,  -1,  -1,  -1,  95,  -1,  -1,  -1,
       -1,  96,  97,  98,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  99,  -1, 100,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 101,  -1,  -1,  -1,
       -1, 102,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1, 103,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 104
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_tag (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              if (len == lengthtable[index])
                {
                  register const char *s = wordlist_tag[index].name + spool_Tag;

                  if (*str == *s && !memcmp (str + 1, s + 1, len - 1))
                    return &wordlist_tag[index];
                }
            }
        }
    }
  return 0;
}
#line 116 "htmltags.gperf"



static const char tagStable[] = {
   "\000/a\000/abbr\000/acronym\000/address\000/applet\000/area\000/b"
   "\000/base\000/basefont\000/bdo\000/big\000/blockquote\000/body"
   "\000/br\000/button\000/caption\000/center\000/cite\000/code\000/col"
   "\000/colgroup\000/dd\000/del\000/dfn\000/dir\000/div\000/dl\000/dt"
   "\000/em\000/embed\000/fieldset\000/font\000/form\000/frame\000/frameset"
   "\000/h1\000/h2\000/h3\000/h4\000/h5\000/h6\000/head\000/hr\000/html"
   "\000/i\000/iframe\000/ilayer\000/img\000/input\000/ins\000/isindex"
   "\000/kbd\000/keygen\000/label\000/layer\000/legend\000/li\000/link"
   "\000/map\000/marquee\000/menu\000/meta\000/nobr\000/noembed\000/noframes"
   "\000/noscript\000/nolayer\000/object\000/ol\000/optgroup\000/option"
   "\000/p\000/param\000/plaintext\000/pre\000/q\000/s\000/samp\000/script"
   "\000/select\000/small\000/span\000/strike\000/strong\000/style"
   "\000/sub\000/sup\000/table\000/tbody\000/td\000/textarea\000/tfoot"
   "\000/th\000/thead\000/title\000/tr\000/tt\000/u\000/ul\000/var"
   "\000/wbr\000/xmp\000/text\000/comment\000"
};

static const unsigned short tagSList[] = {
  0,

     2,   5,  11,  20,  29,  37,  43,  46,  52,  62,  67,  72,
    84,  90,  94, 102, 111, 119, 125, 131, 136, 146, 150, 155,
   160, 165, 170, 174, 178, 182, 189, 199, 205, 211, 218, 228,
   232, 236, 240, 244, 248, 252, 258, 262, 268, 271, 279, 287,
   292, 299, 304, 313, 318, 326, 333, 340, 348, 352, 358, 363,
   372, 378, 384, 390, 399, 409, 419, 428, 436, 440, 450, 458,
   461, 468, 479, 484, 487, 490, 496, 504, 512, 519, 525, 533,
   541, 548, 553, 558, 565, 572, 576, 586, 593, 597, 604, 611,
   615, 619, 622, 626, 631, 636, 641, 647,   1,   4,  10,  19,
    28,  36,  42,  45,  51,  61,  66,  71,  83,  89,  93, 101,
   110, 118, 124, 130, 135, 145, 149, 154, 159, 164, 169, 173,
   177, 181, 188, 198, 204, 210, 217, 227, 231, 235, 239, 243,
   247, 251, 257, 261, 267, 270, 278, 286, 291, 298, 303, 312,
   317, 325, 332, 339, 347, 351, 357, 362, 371, 377, 383, 389,
   398, 408, 418, 427, 435, 439, 449, 457, 460, 467, 478, 483,
   486, 489, 495, 503, 511, 518, 524, 532, 540, 547, 552, 557,
   564, 571, 575, 585, 592, 596, 603, 610, 614, 618, 621, 625,
   630, 635, 640, 646,    0
};

const char* KDE_NO_EXPORT getTagName(unsigned short id)
{
    if(id > ID_CLOSE_TAG*2) id = ID_CLOSE_TAG+1;
    return &tagStable[tagSList[id]];
}

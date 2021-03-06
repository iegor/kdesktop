/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** An tokenizer for SQL
**
** This file contains C code that splits an SQL input string up into
** individual tokens and sends those tokens one-by-one over to the
** parser for analysis.
**
** $Id: tokenize.c 373135 2004-12-24 12:55:20Z cramdal $
*/
#include "sqliteInt.h"
#include "os.h"
#include <ctype.h>
#include <stdlib.h>

/*
** This function looks up an identifier to determine if it is a
** keyword.  If it is a keyword, the token code of that keyword is 
** returned.  If the input is not a keyword, TK_ID is returned.
**
** The implementation of this routine was generated by a program,
** mkkeywordhash.c, located in the tool subdirectory of the distribution.
** The output of the mkkeywordhash.c program was manually cut and pasted
** into this file.  When the set of keywords for SQLite changes, you
** must modify the mkkeywordhash.c program (to add or remove keywords from
** the data tables) then rerun that program to regenerate this function.
*/
int sqlite3KeywordCode(const char *z, int n){
  static const char zText[519] =
    "ABORTAFTERALLANDASCATTACHBEFOREBEGINBETWEENBYCASCADECASECHECK"
    "COLLATECOMMITCONFLICTCONSTRAINTCREATECROSSDATABASEDEFAULTDEFERRABLE"
    "DEFERREDDELETEDESCDETACHDISTINCTDROPEACHELSEENDEXCEPTEXCLUSIVE"
    "EXPLAINFAILFOREIGNFROMFULLGLOBGROUPHAVINGIGNOREIMMEDIATEINDEX"
    "INITIALLYINNERINSERTINSTEADINTERSECTINTOISNULLJOINKEYLEFTLIKE"
    "LIMITMATCHNATURALNOTNULLNULLOFFSETONORDEROUTERPRAGMAPRIMARYRAISE"
    "REFERENCESREPLACERESTRICTRIGHTROLLBACKROWSELECTSETSTATEMENTTABLE"
    "TEMPORARYTHENTRANSACTIONTRIGGERUNIONUNIQUEUPDATEUSINGVACUUMVALUES"
    "VIEWWHENWHERE";
  static const unsigned char aHash[154] = {
       0,  75,  82,   0,   0,  97,  80,   0,  83,   0,   0,   0,   0,
       0,   0,   6,   0,  95,   4,   0,   0,   0,   0,   0,   0,   0,
       0,  96,  86,   8,   0,  26,  13,   7,  19,  15,   0,   0,  32,
      25,   0,  21,  31,  41,   0,   0,   0,  34,  27,   0,   0,  30,
       0,   0,   0,   9,   0,  10,   0,   0,   0,   0,  51,   0,  44,
      43,   0,  45,  40,   0,  29,  39,  35,   0,   0,  20,   0,  59,
       0,  16,   0,  17,   0,  18,   0,  55,  42,  72,   0,  33,   0,
       0,  61,  66,  56,   0,   0,   0,   0,   0,   0,   0,  54,   0,
       0,   0,   0,   0,  74,  50,  76,  64,  52,   0,   0,   0,   0,
      68,  84,   0,  47,   0,  58,  60,  92,   0,   0,  48,   0,  93,
       0,  63,  71,  98,   0,   0,   0,   0,   0,  67,   0,   0,   0,
       0,  87,   0,   0,   0,   0,   0,  90,  88,   0,  94,
  };
  static const unsigned char aNext[98] = {
       0,   0,   0,   0,   2,   0,   0,   0,   0,   0,   0,   0,   0,
       0,  12,   0,   0,   0,   0,   0,   0,  11,   0,   0,   0,   0,
       0,   0,   0,  14,   3,  24,   0,   0,   0,   1,  22,   0,   0,
      36,  23,  28,   0,   0,   0,   0,   0,   0,   0,   0,   5,   0,
       0,  49,  37,   0,   0,   0,  38,   0,  53,   0,  57,  62,   0,
       0,   0,   0,   0,   0,  70,  46,   0,  65,   0,   0,   0,   0,
      69,  73,   0,  77,   0,   0,   0,   0,   0,   0,  81,  85,   0,
      91,  79,  78,   0,   0,  89,   0,
  };
  static const unsigned char aLen[98] = {
       5,   5,   3,   3,   2,   3,   6,   6,   5,   7,   2,   7,   4,
       5,   7,   6,   8,  10,   6,   5,   8,   7,  10,   8,   6,   4,
       6,   8,   4,   4,   4,   3,   6,   9,   7,   4,   3,   7,   4,
       4,   4,   5,   6,   6,   9,   2,   5,   9,   5,   6,   7,   9,
       4,   2,   6,   4,   3,   4,   4,   5,   5,   7,   3,   7,   4,
       2,   6,   2,   2,   5,   5,   6,   7,   5,  10,   7,   8,   5,
       8,   3,   6,   3,   9,   5,   4,   9,   4,  11,   7,   5,   6,
       6,   5,   6,   6,   4,   4,   5,
  };
  static const unsigned short int aOffset[98] = {
       0,   5,  10,  13,  16,  16,  19,  25,  31,  36,  43,  45,  52,
      56,  61,  68,  74,  82,  92,  98, 103, 111, 118, 128, 136, 142,
     146, 152, 160, 164, 168, 172, 175, 181, 190, 197, 201, 201, 208,
     212, 216, 220, 225, 231, 237, 246, 246, 251, 260, 265, 271, 278,
     287, 291, 291, 297, 301, 304, 308, 312, 317, 322, 329, 329, 336,
     340, 340, 346, 348, 348, 353, 358, 364, 371, 376, 386, 393, 401,
     406, 414, 417, 423, 426, 435, 440, 440, 449, 453, 464, 471, 476,
     482, 488, 493, 499, 505, 509, 513,
  };
  static const unsigned char aCode[98] = {
    TK_ABORT,      TK_AFTER,      TK_ALL,        TK_AND,        TK_AS,         
    TK_ASC,        TK_ATTACH,     TK_BEFORE,     TK_BEGIN,      TK_BETWEEN,    
    TK_BY,         TK_CASCADE,    TK_CASE,       TK_CHECK,      TK_COLLATE,    
    TK_COMMIT,     TK_CONFLICT,   TK_CONSTRAINT, TK_CREATE,     TK_JOIN_KW,    
    TK_DATABASE,   TK_DEFAULT,    TK_DEFERRABLE, TK_DEFERRED,   TK_DELETE,     
    TK_DESC,       TK_DETACH,     TK_DISTINCT,   TK_DROP,       TK_EACH,       
    TK_ELSE,       TK_END,        TK_EXCEPT,     TK_EXCLUSIVE,  TK_EXPLAIN,    
    TK_FAIL,       TK_FOR,        TK_FOREIGN,    TK_FROM,       TK_JOIN_KW,    
    TK_GLOB,       TK_GROUP,      TK_HAVING,     TK_IGNORE,     TK_IMMEDIATE,  
    TK_IN,         TK_INDEX,      TK_INITIALLY,  TK_JOIN_KW,    TK_INSERT,     
    TK_INSTEAD,    TK_INTERSECT,  TK_INTO,       TK_IS,         TK_ISNULL,     
    TK_JOIN,       TK_KEY,        TK_JOIN_KW,    TK_LIKE,       TK_LIMIT,      
    TK_MATCH,      TK_JOIN_KW,    TK_NOT,        TK_NOTNULL,    TK_NULL,       
    TK_OF,         TK_OFFSET,     TK_ON,         TK_OR,         TK_ORDER,      
    TK_JOIN_KW,    TK_PRAGMA,     TK_PRIMARY,    TK_RAISE,      TK_REFERENCES, 
    TK_REPLACE,    TK_RESTRICT,   TK_JOIN_KW,    TK_ROLLBACK,   TK_ROW,        
    TK_SELECT,     TK_SET,        TK_STATEMENT,  TK_TABLE,      TK_TEMP,       
    TK_TEMP,       TK_THEN,       TK_TRANSACTION,TK_TRIGGER,    TK_UNION,      
    TK_UNIQUE,     TK_UPDATE,     TK_USING,      TK_VACUUM,     TK_VALUES,     
    TK_VIEW,       TK_WHEN,       TK_WHERE,      
  };
  int h, i;
  if( n<2 ) return TK_ID;
  h = (sqlite3UpperToLower[((unsigned char*)z)[0]]*5 + 
      sqlite3UpperToLower[((unsigned char*)z)[n-1]]*3 +
      n) % 154;
  for(i=((int)aHash[h])-1; i>=0; i=((int)aNext[i])-1){
    if( aLen[i]==n && sqlite3StrNICmp(&zText[aOffset[i]],z,n)==0 ){
      return aCode[i];
    }
  }
  return TK_ID;
}

/*
** If X is a character that can be used in an identifier and
** X&0x80==0 then isIdChar[X] will be 1.  If X&0x80==0x80 then
** X is always an identifier character.  (Hence all UTF-8
** characters can be part of an identifier).  isIdChar[X] will
** be 0 for every character in the lower 128 ASCII characters
** that cannot be used as part of an identifier.
**
** In this implementation, an identifier can be a string of
** alphabetic characters, digits, and "_" plus any character
** with the high-order bit set.  The latter rule means that
** any sequence of UTF-8 characters or characters taken from
** an extended ISO8859 character set can form an identifier.
*/
static const char isIdChar[] = {
/* x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  /* 3x */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 4x */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,  /* 5x */
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 6x */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  /* 7x */
};

#define IdChar(C)  (((c=C)&0x80)!=0 || (c>0x2f && isIdChar[c-0x30]))

/*
** Return the length of the token that begins at z[0]. 
** Store the token type in *tokenType before returning.
*/
static int sqliteGetToken(const unsigned char *z, int *tokenType){
  int i, c;
  switch( *z ){
    case ' ': case '\t': case '\n': case '\f': case '\r': {
      for(i=1; isspace(z[i]); i++){}
      *tokenType = TK_SPACE;
      return i;
    }
    case '-': {
      if( z[1]=='-' ){
        for(i=2; (c=z[i])!=0 && c!='\n'; i++){}
        *tokenType = TK_COMMENT;
        return i;
      }
      *tokenType = TK_MINUS;
      return 1;
    }
    case '(': {
      *tokenType = TK_LP;
      return 1;
    }
    case ')': {
      *tokenType = TK_RP;
      return 1;
    }
    case ';': {
      *tokenType = TK_SEMI;
      return 1;
    }
    case '+': {
      *tokenType = TK_PLUS;
      return 1;
    }
    case '*': {
      *tokenType = TK_STAR;
      return 1;
    }
    case '/': {
      if( z[1]!='*' || z[2]==0 ){
        *tokenType = TK_SLASH;
        return 1;
      }
      for(i=3, c=z[2]; (c!='*' || z[i]!='/') && (c=z[i])!=0; i++){}
      if( c ) i++;
      *tokenType = TK_COMMENT;
      return i;
    }
    case '%': {
      *tokenType = TK_REM;
      return 1;
    }
    case '=': {
      *tokenType = TK_EQ;
      return 1 + (z[1]=='=');
    }
    case '<': {
      if( (c=z[1])=='=' ){
        *tokenType = TK_LE;
        return 2;
      }else if( c=='>' ){
        *tokenType = TK_NE;
        return 2;
      }else if( c=='<' ){
        *tokenType = TK_LSHIFT;
        return 2;
      }else{
        *tokenType = TK_LT;
        return 1;
      }
    }
    case '>': {
      if( (c=z[1])=='=' ){
        *tokenType = TK_GE;
        return 2;
      }else if( c=='>' ){
        *tokenType = TK_RSHIFT;
        return 2;
      }else{
        *tokenType = TK_GT;
        return 1;
      }
    }
    case '!': {
      if( z[1]!='=' ){
        *tokenType = TK_ILLEGAL;
        return 2;
      }else{
        *tokenType = TK_NE;
        return 2;
      }
    }
    case '|': {
      if( z[1]!='|' ){
        *tokenType = TK_BITOR;
        return 1;
      }else{
        *tokenType = TK_CONCAT;
        return 2;
      }
    }
    case ',': {
      *tokenType = TK_COMMA;
      return 1;
    }
    case '&': {
      *tokenType = TK_BITAND;
      return 1;
    }
    case '~': {
      *tokenType = TK_BITNOT;
      return 1;
    }
    case '\'': case '"': {
      int delim = z[0];
      for(i=1; (c=z[i])!=0; i++){
        if( c==delim ){
          if( z[i+1]==delim ){
            i++;
          }else{
            break;
          }
        }
      }
      if( c ) i++;
      *tokenType = TK_STRING;
      return i;
    }
    case '.': {
      *tokenType = TK_DOT;
      return 1;
    }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
      *tokenType = TK_INTEGER;
      for(i=1; isdigit(z[i]); i++){}
      if( z[i]=='.' && isdigit(z[i+1]) ){
        i += 2;
        while( isdigit(z[i]) ){ i++; }
        *tokenType = TK_FLOAT;
      }
      if( (z[i]=='e' || z[i]=='E') &&
           ( isdigit(z[i+1]) 
            || ((z[i+1]=='+' || z[i+1]=='-') && isdigit(z[i+2]))
           )
      ){
        i += 2;
        while( isdigit(z[i]) ){ i++; }
        *tokenType = TK_FLOAT;
      }
      return i;
    }
    case '[': {
      for(i=1, c=z[0]; c!=']' && (c=z[i])!=0; i++){}
      *tokenType = TK_ID;
      return i;
    }
    case '?': {
      *tokenType = TK_VARIABLE;
      for(i=1; isdigit(z[i]); i++){}
      return i;
    }
    case ':': {
      for(i=1; IdChar(z[i]); i++){}
      *tokenType = i>1 ? TK_VARIABLE : TK_ILLEGAL;
      return i;
    }
    case '$': {
      *tokenType = TK_VARIABLE;
      if( z[1]=='{' ){
        int nBrace = 1;
        for(i=2; (c=z[i])!=0 && nBrace; i++){
          if( c=='{' ){
            nBrace++;
          }else if( c=='}' ){
            nBrace--;
          }
        }
        if( c==0 ) *tokenType = TK_ILLEGAL;
      }else{
        int n = 0;
        for(i=1; (c=z[i])!=0; i++){
          if( isalnum(c) || c=='_' ){
            n++;
          }else if( c=='(' && n>0 ){
            do{
              i++;
            }while( (c=z[i])!=0 && !isspace(c) && c!=')' );
            if( c==')' ){
              i++;
            }else{
              *tokenType = TK_ILLEGAL;
            }
            break;
          }else if( c==':' && z[i+1]==':' ){
            i++;
          }else{
            break;
          }
        }
        if( n==0 ) *tokenType = TK_ILLEGAL;
      }
      return i;
    } 
    case 'x': case 'X': {
      if( (c=z[1])=='\'' || c=='"' ){
        int delim = c;
        *tokenType = TK_BLOB;
        for(i=2; (c=z[i])!=0; i++){
          if( c==delim ){
            if( i%2 ) *tokenType = TK_ILLEGAL;
            break;
          }
          if( !isxdigit(c) ){
            *tokenType = TK_ILLEGAL;
            return i;
          }
        }
        if( c ) i++;
        return i;
      }
      /* Otherwise fall through to the next case */
    }
    default: {
      if( !IdChar(*z) ){
        break;
      }
      for(i=1; IdChar(z[i]); i++){}
      *tokenType = sqlite3KeywordCode((char*)z, i);
      return i;
    }
  }
  *tokenType = TK_ILLEGAL;
  return 1;
}

/*
** Run the parser on the given SQL string.  The parser structure is
** passed in.  An SQLITE_ status code is returned.  If an error occurs
** and pzErrMsg!=NULL then an error message might be written into 
** memory obtained from malloc() and *pzErrMsg made to point to that
** error message.  Or maybe not.
*/
int sqlite3RunParser(Parse *pParse, const char *zSql, char **pzErrMsg){
  int nErr = 0;
  int i;
  void *pEngine;
  int tokenType;
  int lastTokenParsed = -1;
  sqlite3 *db = pParse->db;
  extern void *sqlite3ParserAlloc(void*(*)(int));
  extern void sqlite3ParserFree(void*, void(*)(void*));
  extern int sqlite3Parser(void*, int, Token, Parse*);

  db->flags &= ~SQLITE_Interrupt;
  pParse->rc = SQLITE_OK;
  i = 0;
  pEngine = sqlite3ParserAlloc((void*(*)(int))malloc);
  if( pEngine==0 ){
    sqlite3SetString(pzErrMsg, "out of memory", (char*)0);
    return 1;
  }
  assert( pParse->sLastToken.dyn==0 );
  assert( pParse->pNewTable==0 );
  assert( pParse->pNewTrigger==0 );
  assert( pParse->nVar==0 );
  assert( pParse->nVarExpr==0 );
  assert( pParse->nVarExprAlloc==0 );
  assert( pParse->apVarExpr==0 );
  pParse->zTail = pParse->zSql = zSql;
  while( sqlite3_malloc_failed==0 && zSql[i]!=0 ){
    assert( i>=0 );
    pParse->sLastToken.z = &zSql[i];
    assert( pParse->sLastToken.dyn==0 );
    pParse->sLastToken.n = sqliteGetToken((unsigned char*)&zSql[i], &tokenType);
    i += pParse->sLastToken.n;
    switch( tokenType ){
      case TK_SPACE:
      case TK_COMMENT: {
        if( (db->flags & SQLITE_Interrupt)!=0 ){
          pParse->rc = SQLITE_INTERRUPT;
          sqlite3SetString(pzErrMsg, "interrupt", (char*)0);
          goto abort_parse;
        }
        break;
      }
      case TK_ILLEGAL: {
        if( pzErrMsg ){
          sqliteFree(*pzErrMsg);
          *pzErrMsg = sqlite3MPrintf("unrecognized token: \"%T\"",
                          &pParse->sLastToken);
        }
        nErr++;
        goto abort_parse;
      }
      case TK_SEMI: {
        pParse->zTail = &zSql[i];
        /* Fall thru into the default case */
      }
      default: {
        sqlite3Parser(pEngine, tokenType, pParse->sLastToken, pParse);
        lastTokenParsed = tokenType;
        if( pParse->rc!=SQLITE_OK ){
          goto abort_parse;
        }
        break;
      }
    }
  }
abort_parse:
  if( zSql[i]==0 && nErr==0 && pParse->rc==SQLITE_OK ){
    if( lastTokenParsed!=TK_SEMI ){
      sqlite3Parser(pEngine, TK_SEMI, pParse->sLastToken, pParse);
      pParse->zTail = &zSql[i];
    }
    sqlite3Parser(pEngine, 0, pParse->sLastToken, pParse);
  }
  sqlite3ParserFree(pEngine, free);
  if( sqlite3_malloc_failed ){
    pParse->rc = SQLITE_NOMEM;
  }
  if( pParse->rc!=SQLITE_OK && pParse->rc!=SQLITE_DONE && pParse->zErrMsg==0 ){
    sqlite3SetString(&pParse->zErrMsg, sqlite3ErrStr(pParse->rc),
                    (char*)0);
  }
  if( pParse->zErrMsg ){
    if( pzErrMsg && *pzErrMsg==0 ){
      *pzErrMsg = pParse->zErrMsg;
    }else{
      sqliteFree(pParse->zErrMsg);
    }
    pParse->zErrMsg = 0;
    if( !nErr ) nErr++;
  }
  if( pParse->pVdbe && pParse->nErr>0 ){
    sqlite3VdbeDelete(pParse->pVdbe);
    pParse->pVdbe = 0;
  }
  sqlite3DeleteTable(pParse->db, pParse->pNewTable);
  sqlite3DeleteTrigger(pParse->pNewTrigger);
  sqliteFree(pParse->apVarExpr);
  if( nErr>0 && (pParse->rc==SQLITE_OK || pParse->rc==SQLITE_DONE) ){
    pParse->rc = SQLITE_ERROR;
  }
  return nErr;
}

/*
** Token types used by the sqlite3_complete() routine.  See the header
** comments on that procedure for additional information.
*/
#define tkEXPLAIN 0
#define tkCREATE  1
#define tkTEMP    2
#define tkTRIGGER 3
#define tkEND     4
#define tkSEMI    5
#define tkWS      6
#define tkOTHER   7

/*
** Return TRUE if the given SQL string ends in a semicolon.
**
** Special handling is require for CREATE TRIGGER statements.
** Whenever the CREATE TRIGGER keywords are seen, the statement
** must end with ";END;".
**
** This implementation uses a state machine with 7 states:
**
**   (0) START     At the beginning or end of an SQL statement.  This routine
**                 returns 1 if it ends in the START state and 0 if it ends
**                 in any other state.
**
**   (1) EXPLAIN   The keyword EXPLAIN has been seen at the beginning of 
**                 a statement.
**
**   (2) CREATE    The keyword CREATE has been seen at the beginning of a
**                 statement, possibly preceeded by EXPLAIN and/or followed by
**                 TEMP or TEMPORARY
**
**   (3) NORMAL    We are in the middle of statement which ends with a single
**                 semicolon.
**
**   (4) TRIGGER   We are in the middle of a trigger definition that must be
**                 ended by a semicolon, the keyword END, and another semicolon.
**
**   (5) SEMI      We've seen the first semicolon in the ";END;" that occurs at
**                 the end of a trigger definition.
**
**   (6) END       We've seen the ";END" of the ";END;" that occurs at the end
**                 of a trigger difinition.
**
** Transitions between states above are determined by tokens extracted
** from the input.  The following tokens are significant:
**
**   (0) tkEXPLAIN   The "explain" keyword.
**   (1) tkCREATE    The "create" keyword.
**   (2) tkTEMP      The "temp" or "temporary" keyword.
**   (3) tkTRIGGER   The "trigger" keyword.
**   (4) tkEND       The "end" keyword.
**   (5) tkSEMI      A semicolon.
**   (6) tkWS        Whitespace
**   (7) tkOTHER     Any other SQL token.
**
** Whitespace never causes a state transition and is always ignored.
*/
int sqlite3_complete(const char *zSql){
  u8 state = 0;   /* Current state, using numbers defined in header comment */
  u8 token;       /* Value of the next token */

  /* The following matrix defines the transition from one state to another
  ** according to what token is seen.  trans[state][token] returns the
  ** next state.
  */
  static const u8 trans[7][8] = {
                     /* Token:                                                */
     /* State:       **  EXPLAIN  CREATE  TEMP  TRIGGER  END  SEMI  WS  OTHER */
     /* 0   START: */ {       1,      2,    3,       3,   3,    0,  0,     3, },
     /* 1 EXPLAIN: */ {       3,      2,    3,       3,   3,    0,  1,     3, },
     /* 2  CREATE: */ {       3,      3,    2,       4,   3,    0,  2,     3, },
     /* 3  NORMAL: */ {       3,      3,    3,       3,   3,    0,  3,     3, },
     /* 4 TRIGGER: */ {       4,      4,    4,       4,   4,    5,  4,     4, },
     /* 5    SEMI: */ {       4,      4,    4,       4,   6,    5,  5,     4, },
     /* 6     END: */ {       4,      4,    4,       4,   4,    0,  6,     4, },
  };

  while( *zSql ){
    switch( *zSql ){
      case ';': {  /* A semicolon */
        token = tkSEMI;
        break;
      }
      case ' ':
      case '\r':
      case '\t':
      case '\n':
      case '\f': {  /* White space is ignored */
        token = tkWS;
        break;
      }
      case '/': {   /* C-style comments */
        if( zSql[1]!='*' ){
          token = tkOTHER;
          break;
        }
        zSql += 2;
        while( zSql[0] && (zSql[0]!='*' || zSql[1]!='/') ){ zSql++; }
        if( zSql[0]==0 ) return 0;
        zSql++;
        token = tkWS;
        break;
      }
      case '-': {   /* SQL-style comments from "--" to end of line */
        if( zSql[1]!='-' ){
          token = tkOTHER;
          break;
        }
        while( *zSql && *zSql!='\n' ){ zSql++; }
        if( *zSql==0 ) return state==0;
        token = tkWS;
        break;
      }
      case '[': {   /* Microsoft-style identifiers in [...] */
        zSql++;
        while( *zSql && *zSql!=']' ){ zSql++; }
        if( *zSql==0 ) return 0;
        token = tkOTHER;
        break;
      }
      case '"':     /* single- and double-quoted strings */
      case '\'': {
        int c = *zSql;
        zSql++;
        while( *zSql && *zSql!=c ){ zSql++; }
        if( *zSql==0 ) return 0;
        token = tkOTHER;
        break;
      }
      default: {
        int c;
        if( IdChar((u8)*zSql) ){
          /* Keywords and unquoted identifiers */
          int nId;
          for(nId=1; IdChar(zSql[nId]); nId++){}
          switch( *zSql ){
            case 'c': case 'C': {
              if( nId==6 && sqlite3StrNICmp(zSql, "create", 6)==0 ){
                token = tkCREATE;
              }else{
                token = tkOTHER;
              }
              break;
            }
            case 't': case 'T': {
              if( nId==7 && sqlite3StrNICmp(zSql, "trigger", 7)==0 ){
                token = tkTRIGGER;
              }else if( nId==4 && sqlite3StrNICmp(zSql, "temp", 4)==0 ){
                token = tkTEMP;
              }else if( nId==9 && sqlite3StrNICmp(zSql, "temporary", 9)==0 ){
                token = tkTEMP;
              }else{
                token = tkOTHER;
              }
              break;
            }
            case 'e':  case 'E': {
              if( nId==3 && sqlite3StrNICmp(zSql, "end", 3)==0 ){
                token = tkEND;
              }else if( nId==7 && sqlite3StrNICmp(zSql, "explain", 7)==0 ){
                token = tkEXPLAIN;
              }else{
                token = tkOTHER;
              }
              break;
            }
            default: {
              token = tkOTHER;
              break;
            }
          }
          zSql += nId-1;
        }else{
          /* Operators and special symbols */
          token = tkOTHER;
        }
        break;
      }
    }
    state = trans[state][token];
    zSql++;
  }
  return state==0;
}

/*
** This routine is the same as the sqlite3_complete() routine described
** above, except that the parameter is required to be UTF-16 encoded, not
** UTF-8.
*/
int sqlite3_complete16(const void *zSql){
  sqlite3_value *pVal;
  char const *zSql8;
  int rc = 0;

  pVal = sqlite3ValueNew();
  sqlite3ValueSetStr(pVal, -1, zSql, SQLITE_UTF16NATIVE, SQLITE_STATIC);
  zSql8 = sqlite3ValueText(pVal, SQLITE_UTF8);
  if( zSql8 ){
    rc = sqlite3_complete(zSql8);
  }
  sqlite3ValueFree(pVal);
  return rc;
}

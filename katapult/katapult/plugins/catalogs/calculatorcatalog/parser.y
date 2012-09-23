/* Author: Tobi Vollebregt */

/* Infix notation calculator. */

%{
#define YYLEX_PARAM parsercontrol

#include <math.h>
#include <stdio.h>
#include "calculatorcatalog.h"

//void yyerror(char const *);
#define yyerror(a,b)
%}

/* Be a reentrant parser. */
%pure_parser

%parse-param {CalculatorCatalog::ParserControl* parsercontrol}

/* Bison declarations.  */
%union {
	double    val;   /* For returning numbers.  */
	FunPtr   fptr;   /* For returning function pointers.  */
	int        id;   /* For returning variables. */
}

%token <val>  NUM    /* Simple double precision number.  */
%token <fptr> FUN    /* Function.  */
%token <id>   VAR    /* Variable. */
%type  <val>  exp

%right '='    /* assignment */
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */

%{
static int yylex(YYSTYPE* lvalp, CalculatorCatalog::ParserControl* parsercontrol);
%}
		
%% /* The grammar follows.  */

line: exp            { parsercontrol->result = $1; }
| VAR '=' exp        { if (parsercontrol->assignments) parsercontrol->catalog->setVar($1, $3); }
;

exp: NUM             { $$ = $1;          }
| exp '+' exp        { $$ = $1 + $3;     }
| exp '-' exp        { $$ = $1 - $3;     }
| exp '*' exp        { $$ = $1 * $3;     }
| exp '/' exp        { $$ = $1 / $3;     }
| '-' exp  %prec NEG { $$ = -$2;         }
| exp '^' exp        { $$ = pow($1, $3); }
| '(' exp ')'        { $$ = $2;          }
| FUN '(' exp ')'    { $$ = (*$1)($3);   }
| VAR                { if ($1 == -1) {yyerror(parsercontrol, "undeclared variable"); YYABORT;} $$ = parsercontrol->catalog->getVar($1); }
;

%%

/* The lexical analyzer returns a double floating point
number on the stack and the token NUM, or the numeric code
of the character read if not a number.  It skips all blanks
and tabs, and returns 0 for end-of-input.  */

#include <ctype.h>
#include <string.h>

#define exp (parsercontrol->expression)

static int yylex(YYSTYPE* lvalp, CalculatorCatalog::ParserControl* parsercontrol)
{
	int c;

	/* Skip white space.  */
	while ((c = *(exp++)) == ' ') {
	}
	/* Return end-of-input.  */
	if (c == EOF) {
		return 0;
	}
	/* Process numbers.  */
	if (c == '.' || isdigit(c)) {
		char *endptr = 0;
		lvalp->val = strtod(exp - 1, &endptr);
		exp = endptr;
		return NUM;
	}
	/* Process IDs. */
	if (isalpha(c)) {
		int length = 0;
		char f[128];

		--exp;
		do {
			f[length] = tolower(c);
			++length;
		} while ((c = exp[length]) != 0 && isalpha(c));
		f[length] = 0;

		/* Process functions. */
		const CalculatorCatalog::Function* const functions = parsercontrol->catalog->functionTable();
		for (int i = 0; functions[i].name; ++i) {
			if (length == functions[i].length && strcmp(f, functions[i].name) == 0) {
				lvalp->fptr = functions[i].fptr;
				exp += length;
				return FUN;
			}
		}

		/* Process variables. */
		lvalp->id = parsercontrol->catalog->getVarID(f);
		exp += length;
		return VAR;
	}
	/* Return a single char.  */
	return c;
}

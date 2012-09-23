
/*
  This file is a part of KBabel
  
  Copyright (c) 2005 by Stanislav Visnovsky <visnovsky@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

*/

%{
#define YY_NO_UNPUT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "tokens.h"


%}

%option prefix="GettextBase"

%option noyywrap

/* Any end of line */
EOL		[\r\n]
/* No end of line */	
NOEOL		[^\r\n]

%%



[ ]*{EOL}+	; /* This catches empty lines or a LF after a CR */

msgid		{
		    return T_MSGID;
		}
msgid_plural	{
		    return T_MSGIDPLURAL;
		}
msgstr\[[0-9]+\]	{
		    return T_MSGSTR;
		}
msgstr		{
		    return T_MSGSTR;
		}
msgctxt         {
                    return T_MSGCTXT;
                }
#~{NOEOL}*	{
		   return T_OBSOLETE;
		}

#{NOEOL}*	{
		   return T_COMMENT;
		}
		
\"({NOEOL}*(\\\")?)*\"{EOL}	{
			yytext[strlen(yytext)-2] = 0;
			yytext++;
		   return T_STRING;
		}

[a-z]+          |
.               ;

<<EOF>>		{
		    return T_EOF;
		}

%%

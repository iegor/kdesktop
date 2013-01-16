// KDat - a tar-based DAT archiver
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <kglobal.h>
#include <qmessagebox.h>
#include <klocale.h>
//#include <kstandarddirs.h>
//#include <kapplication.h>

void error_handler(int err_sig);

/* include the following in main():
 signal(SIGHUP, error_handler);
 signal(SIGINT, error_handler);
 signal(SIGFPE, error_handler);
 signal(SIGSEGV, error_handler);
 signal(SIGTERM, error_handler);   

 printf("Error_handler installed\n");
*/

// from /usr/include/bits/signum.h:
// #define SIGHUP          1       /* Hangup (POSIX).  */
// #define SIGINT          2       /* Interrupt (ANSI).  */    
// #define SIGFPE          8       /* Floating-point exception (ANSI).  */  
// #define SIGSEGV         11      /* Segmentation violation (ANSI).  */ 
// #define SIGTERM         15      /* Termination (ANSI).  */    

void error_handler(int err_sig)
{
  QString base1a, base1b, base2a, base2b, msg1, msg2, msg3;
  int     i;

  base1a = i18n( " caught.\nExit the program from File->Quit or do "
                 "\"kill -9 <pid>\" if you like.\n"  );
  base1b = base1a;  // deep copy

  base2a = i18n( "You can dump core by selecting the \"Abort\" button.\n"
                 "Please notify the maintainer (see Help->About KDat)." );
  base2b = base2a;  // deep copy

  msg1 = base1a.append(base2a);
  msg2 = base1b.append("It is strongly recommended to do this!\n");
  msg2 = msg2.append(base2b);
  msg3 = i18n( "An Error Signal was Received" );

  switch (err_sig) {
   case SIGHUP:
     fprintf(stderr, "kdat: SIGHUP signal (\"Hangup (POSIX)\") caught\n");
     i = QMessageBox::critical( (QWidget *)NULL, 
        msg3, 
        i18n( "SIGHUP signal (\"Hangup (POSIX)\")" ).append( msg1 ), 
        QMessageBox::Ignore, QMessageBox::Abort, QMessageBox::NoButton);
     if( i == QMessageBox::Abort ){ abort(); }
     break;
  case SIGINT:
     fprintf(stderr, "kdat: SIGINT signal (\"Interrupt (ANSI)\") caught\n");
     i = QMessageBox::critical( (QWidget *)NULL, 
         msg3, 
         i18n( "SIGINT signal (\"Interrupt (ANSI)\")" ).append( msg1 ), 
         QMessageBox::Ignore, QMessageBox::Abort, QMessageBox::NoButton);
     if( i == QMessageBox::Abort ){ abort(); }
     break;
  case SIGFPE:
    fprintf(stderr, "kdat: SIGFPE signal (\"Floating-point exception (ANSI)\") caught\n");
    i = QMessageBox::critical( (QWidget *)NULL, 
        msg3, 
        i18n("SIGFPE signal (\"Floating-point exception (ANSI)\")").append (msg2), 
         QMessageBox::Ignore, QMessageBox::Abort, QMessageBox::NoButton);
    if( i == QMessageBox::Abort ){ abort(); }
    break;
  case SIGSEGV:
    fprintf(stderr, "kdat: SIGSEGV signal (\"Segmentation violation (ANSI)\") caught\n");
    i = QMessageBox::critical( (QWidget *)NULL, 
          msg3, 
          i18n( "SIGSEGV signal (\"Segmentation violation (ANSI)\")").append(msg2), 
          /* button1,           button2 */
          QMessageBox::Ignore, QMessageBox::Abort, QMessageBox::NoButton);
    if( i == QMessageBox::Abort ){ abort(); }
    break;
  case SIGTERM:
    fprintf(stderr, "kdat: SIGTERM signal (\"Termination (ANSI)\") caught\n");
    i = QMessageBox::critical( (QWidget *)NULL, 
        msg3, 
        i18n( "SIGTERM signal (\"Termination (ANSI)\")").append(msg1), 
          QMessageBox::Ignore, QMessageBox::Abort, QMessageBox::NoButton);
    if( i == QMessageBox::Abort ){ abort(); }
    break;
  }

        // Deinstall the signal handlers
	// signal(SIGHUP, SIG_DFL);
        // signal(SIGINT, SIG_DFL);
	// signal(SIGFPE, SIG_DFL);
        // signal(SIGSEGV, SIG_DFL);
        // signal(SIGTERM, SIG_DFL);
}                  

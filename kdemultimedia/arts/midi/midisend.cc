/*

Copyright (C) 1999 Emmeran Seehuber
                   the_emmy@gmx.de

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

*/

/*
  Changes:
  16.09.1999 Emmeran "Emmy" Seehuber <the_emmy@gmx.de>
    - Implementeted mapping of channels and pitches.
    - Reworked option parsing, now using getopt(). 
      Note: The parameters of the programms have changed !
*/

/*
** This program was in original by David G. Slomin.
** It was released to the Public Domain on 1/25/99.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "midisend.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

int input_fd = -1, test = 0, verbose = 0;
char cFileName[1025];
int optch;
CMidiMap Map;

void usage(char *prog)
{
	fprintf(stderr,"\n");
	fprintf(stderr,"Usage: %s [ -f <mididevice> ] [ -m <mapfile> ] [ -v ] [ -t <loop> ]\n",prog);
	fprintf(stderr,"   -f   the mididevice to read the input from.\n");
	fprintf(stderr,"        Default is /dev/midi. If you specify a dash, it is stdin\n");
  fprintf(stderr,"   -m   the mapfile to load.\n");
  fprintf(stderr,"   -v   verbose output.\n");
  fprintf(stderr,"   -t   test mode. Generates a testoutput on the midibus\n");
  fprintf(stderr,"   -l   long test mode. Generates a testoutput on the midibus\n");
	exit(1);
}

void parseArgs(int argc, char** argv)
{
  // Setup default
  strcpy(cFileName,"/dev/midi");

	while((optch = getopt(argc,argv,"m:f:vtl")) > 0)
	{
		switch(optch)
		{
			case 'm': if( !Map.readMap(optarg) ) 
                  fprintf(stderr,"%s: can't read file %s!\n",argv[0],optarg);
				break;
			case 't': test = 1;
				break;
			case 'l': test = 2;
				break;
#ifdef VERSION
			case 'v': verbose = 1; printf("MidiSend %s\n", VERSION );
				break;
#endif
			case 'f': strncpy(cFileName,optarg,1024);
				break;
			default: usage(argv[0]);
				break;
		}
	}
}


#ifdef COMMON_BINARY
int midisend_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
   Arts::Dispatcher dispatcher;
   Arts::MidiManager manager = Arts::Reference("global:Arts_MidiManager");

   if (manager.isNull())
   {
      fprintf(stderr, "%s trouble: No midimanager object found; please start "
         			  "artsd.\n",argv[0]);
      exit(EXIT_FAILURE);
   }

   /*
   ** MIDI input initialization.
   */

   parseArgs(argc,argv);

   string title = string("midisend (") + cFileName +")";
   Arts::MidiClient client
       = manager.addClient(Arts::mcdPlay,Arts::mctApplication,title,"midisend");
   Arts::MidiPort port = client.addOutputPort();

   if(test)
   {
      if( verbose )
        printf("performing test ...\n");
      unsigned long i,max=5000;
      if(test==2) max = 20000;
      for(i=0;i<max;i++)
      {
         port.processCommand(
             Arts::MidiCommand(Arts::mcsNoteOn, 60+(i%12), 100));
         port.processCommand(
             Arts::MidiCommand(Arts::mcsNoteOff,60+(i%12), 0));
      }
      exit(0);
   }

   if( verbose )
     printf("trying to open %s ...", cFileName );

   input_fd = open(cFileName,O_RDONLY);

   if(input_fd == -1)
   {
      fprintf(stderr,"\n%s trouble: can't open input device!\n",argv[0]);
      exit(EXIT_FAILURE);
   }
   else if( verbose )
     printf(" ok!\n");


   /*
   ** Main loop.
   */

   if(verbose)
     printf("beginning loop ...\n");

   unsigned char msg[3];

   while (1)
   {
      midimsgRead(input_fd, msg);
      switch (midimsgGetMessageType(msg))
      {
         case MIDIMSG_NOTE_OFF:
            Map.mapMsg(msg);
            port.processCommand(
				Arts::MidiCommand(Arts::mcsNoteOff|midimsgGetChannel(msg),
								midimsgGetPitch(msg), midimsgGetVelocity(msg)));
            if( verbose ) 
              printf("NoteOff: Channel %d, Pitch %3d\n", midimsgGetChannel(msg),midimsgGetPitch(msg));
            break;
         case MIDIMSG_NOTE_ON:
            Map.mapMsg(msg);
            port.processCommand(
				Arts::MidiCommand(Arts::mcsNoteOn|midimsgGetChannel(msg),
								midimsgGetPitch(msg), midimsgGetVelocity(msg)));
            if( verbose )
              printf("NoteOn : Channel %d, Pitch %3d, Velocity %2d\n", midimsgGetChannel(msg),
                     midimsgGetPitch(msg),midimsgGetVelocity(msg));
            break;
		case MIDIMSG_PITCH_WHEEL:
            Map.mapMsg(msg);
            port.processCommand(
				Arts::MidiCommand(Arts::mcsPitchWheel|midimsgGetChannel(msg),
								midimsgGetPitch(msg), midimsgGetVelocity(msg)));
            if( verbose )
              printf("PitchWheel : Channel %d, LSB %3d MSB %3d\n", midimsgGetChannel(msg),
                     midimsgGetPitch(msg),midimsgGetVelocity(msg));
 			break;
      }
   }
}

//--------------------------------------------
// The mapping stuff
//--------------------------------------------

bool CMidiMap::readMap(const char* pszFileName)
{
  if( verbose )
    printf("reading mapfile %s ...\n", pszFileName);

  FILE *file = fopen(pszFileName,"r");
  if( !file )
     return false;

  char cBuffer[1024+1];
  char* pszLine;
  int nLine = 0;
  while( (pszLine = fgets(cBuffer,1024,file)) ) {
    nLine++;
    parseLine(pszLine,pszFileName,nLine);
  }
  fclose(file);

  return true;
}

bool CMidiMap::getNextWord(char*& pszLine, char*& pszWord)
{
  // First skip all leading blanks, etc.
  bool bCont = true;
  while(bCont) {
    char cHelp = *pszLine;
    switch( cHelp ) 
    {
      case 0: return false; // Out of line
      case ' ': 
      case '\n':
      case '\r':
      case '\t': pszLine++; break; // Goto next char
      default: bCont = false; break; // NonSpace character -> Word begins here.
    }
  }
  
  // The word starts here
  pszWord = pszLine;

  // And now, goto the end of the word.
  bCont = true;
  while(bCont) {
    char cHelp = *pszLine;
    switch( cHelp ) 
    {
      case 0: 
      case ',':
      case ';':
      case ' ': 
      case '\n':
      case '\r':
      case '\t': *pszLine++ = 0; bCont = false; break; // Goto next char
      default: pszLine++; break;
    }
  }

  return true;
}

void CMidiMap::parseLine(char* pszLine, const char* pszConfigFile, int nConfigLine )
{
  char* pszWord = 0;
  bool bOk = true;

  // Get first word of the line
  bOk = bOk && getNextWord(pszLine,pszWord);
  if( !bOk )
    return;

  // Skip comments
  if( *pszWord == '#' )
    return;

  if( strcmp(pszWord,"PRC") == 0 ) {
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nOrigChannel = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nStart = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nEnd = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nChannel = atol(pszWord);
    if( bOk ) {
      channelMaps[nOrigChannel].nChannel = nOrigChannel;
      for( int i = nStart; i <= nEnd; i++ ) {
        channelMaps[nOrigChannel].channelRemaps[i].nPitch = i;
        channelMaps[nOrigChannel].channelRemaps[i].nChannel = nChannel;
      }
    }
    else {
      printf("midisend: (PRC) missing parameters at %s:%d\n", pszConfigFile,nConfigLine);
    }
    return;
  }

  if( strcmp(pszWord,"PRD") == 0 ) {
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nOrigChannel = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nStart = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nEnd = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nPitchDiff = atol(pszWord);
    if( bOk ) {
      channelMaps[nOrigChannel].nChannel = nOrigChannel;
      for( int i = nStart; i <= nEnd; i++ ) {
        channelMaps[nOrigChannel].pitchRemaps[i].nPitch = i;
        channelMaps[nOrigChannel].pitchRemaps[i].nToPitch = i + nPitchDiff;
      }
    }
    else {
      printf("midisend: (PRD) missing parameters at %s:%d\n", pszConfigFile,nConfigLine);
    }
    return;
  }

  if( strcmp(pszWord,"PTC") == 0 ) {
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nOrigChannel = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nPitch = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nToChannel = atol(pszWord);
    if( bOk ) {
      channelMaps[nOrigChannel].nChannel = nOrigChannel;
      channelMaps[nOrigChannel].channelRemaps[nPitch].nPitch = nPitch;
      channelMaps[nOrigChannel].channelRemaps[nPitch].nChannel = nToChannel;
    }
    else {
      printf("midisend: (PTC) missing parameters at %s:%d\n", pszConfigFile,nConfigLine);
    }
    return;
  }

  if( strcmp(pszWord,"PTP") == 0 ) {
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nOrigChannel = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nPitch = atol(pszWord);
    bOk = bOk && getNextWord(pszLine,pszWord);
    int nToPitch = atol(pszWord);
    if( bOk ) {
      channelMaps[nOrigChannel].nChannel = nOrigChannel;
      channelMaps[nOrigChannel].pitchRemaps[nPitch].nPitch = nPitch;
      channelMaps[nOrigChannel].pitchRemaps[nPitch].nToPitch = nToPitch;
    }
    else {
      printf("midisend: (PTP) missing parameters at %s:%d\n", pszConfigFile,nConfigLine);
    }
    return;
  }

  printf("midisend: Unknown command at %s:%d\n", pszConfigFile,nConfigLine);
}

void CMidiMap::mapMsg(Byte* msg)
{
  // Get out the data for mapping
  int nChannel = midimsgGetChannel(msg);
  int nPitch = midimsgGetPitch(msg);

  // Is there something to map for this channel ?
  if( channelMaps.find(nChannel) != channelMaps.end() ) {
    // => Yes, than do it.

    // C/P => C
    if( channelMaps[nChannel].channelRemaps.find(nPitch) != channelMaps[nChannel].channelRemaps.end() ) 
      midimsgSetChannel(msg,channelMaps[nChannel].channelRemaps[nPitch].nChannel);

    // C/P => P
    if( channelMaps[nChannel].pitchRemaps.find(nPitch) != channelMaps[nChannel].pitchRemaps.end() ) {
      midimsgSetPitch(msg,channelMaps[nChannel].pitchRemaps[nPitch].nToPitch);
    }
  }
}

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
#ifndef MIDISEND_H
#define MIDISEND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "artsmidi.h"
#include <vector>
#include <iostream>
#include <string>

extern "C" 
{
#include "midimsg.h"
}

/*
  This class does the mapping of the
  channels and pitches
*/
class CMidiMap {
public:
  /*
    Reads in the mapfile pszFileName.
    Returns TRUE, if successful.
  */
  bool readMap(const char* pszFileName);
  /*
    Maps the given message according to 
    the actual read configuration.
  */
  void mapMsg(Byte* msg);

private:
  /*
    Parses a configuration line.
  */
  void parseLine(char* pszLine, const char* pszConfigFile, int nConfigLine );

  /*
    Gets the next word out of the line. throws
    the exception CEOutOfLine, if there is no 
    more word in the line.

    A word consists of all but \0, spacecharacter, ';' and
    ','
    
    pszLine is the pointer to the start of the
    line. This function modifies the pointer.
    pszWord is the pointer to the found word.
  */
  bool getNextWord(char*& pszLine, char*& pszWord);
private:

  /*
    For each channel one instance of this structure
    exists in the channelMaps. It holds the mapping information
    for nChannel.
  */
  struct ChannelMaps {
    int nChannel;

    struct ChannelRemap {
      int nPitch;
      int nChannel;
    };

    struct PitchRemap {
      int nPitch;
      int nToPitch;
    };

    typedef std::map<int,ChannelRemap> ChannelRemapMap;
    ChannelRemapMap channelRemaps;
    typedef std::map<int,PitchRemap> PitchRemapMap;
    PitchRemapMap pitchRemaps;
  };
  typedef std::map<int,ChannelMaps> ChannelMapsMap;
  ChannelMapsMap channelMaps;
};


#endif 

/*
  generic Implementation of a cd-player
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */




#ifndef __INPUTDECODERXPLAYER_H
#define __INPUTDECODERXPLAYER_H


#include "../yafcore/inputDecoder.h"
#include "yafOutputStream.h"



/** 
    major modes . it should be not necessary to define
    new modes.
    What are "major modes"?
    <p>
    The idea of the major modes, is that this is a very
    general description of an abstract cd player.
    When the decoder works yaf send these
    major modes to the frontend.
    <p>
    These Major modes describes the basic commands which
    every yaf-decoder implementation should support:
    <pre>
    on/off
    open/close
    pause/play
    </pre>
    Thus it is possible that a frontend works with
    a decoder even if the frontend  does not fully support all
    the commands (e.g: switch to mono etc...)

 <pre>
 If you think you have a new "mode" its almost a "feature"
 eg:  You have a command "jump" but this is not a "mode"
      because you can press "jump" on your cd player even
      if the cd player is off OR on OR playing OR paused
      In all these cases the command does not affect the
      state of your cd-player. Or do you have a player
      which switches itself on if you press e.g. "Track 1" ?
 </pre>     
*/

#define _PLAYER_MAJOR_MODE_OFF               1
#define _PLAYER_MAJOR_MODE_ON                2
#define _PLAYER_MAJOR_MODE_OPEN_TRACK        3
#define _PLAYER_MAJOR_MODE_CLOSE_TRACK       4
#define _PLAYER_MAJOR_MODE_PLAYING           5   
#define _PLAYER_MAJOR_MODE_PAUSE             6


/*    
 off: the cd player is switched off  (/dev/dsp is unlocked)


 on : (you may play the inserte cd) (/dev/dsp is locked) 

 playing: cd player plays something  (player is switched on)
 pause: does not play                        "

 open track : think of it that you "select a track" you
              can only open a track if the player is "ON"

 close track: eject the cd from player. 
              (This does not necessarily mean that /dev/dsp is unlocked)
*/
 

class CommandTableXPlayer;

class InputDecoderXPlayer : public InputDecoder {

  
 public:
  InputDecoderXPlayer(YafOutputStream* yafOutput);
  ~InputDecoderXPlayer();

  const char* processCommand(int command,const char* args);
  void doSomething();

  void setMajorMode(int mode);
  int getMajorMode();

  void setMajorModeInfo(int lDisplayMajorMode);
  int getMajorModeInfo();

  int getOn();
  void setOn(int lOn);

 private:

  int majorMode;
  CommandTableXPlayer* ct;
  int lDisplayMajorMode;
  OutputInterface* output;
  YafOutputStream* yafOutput;
  
  int isOn;
};


#endif



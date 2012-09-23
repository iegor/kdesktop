/*
  base class for the player plugin
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */





#ifndef __DECODERPLUGIN_H
#define __DECODERPLUGIN_H


 


#include "commandPipe.h"
#include "../input/inputPlugin.h"
#include "../output/outPlugin.h"
#include "../util/timeWrapper.h"

#include <kdemacros.h>

/**
   Note: streamstate can be "or'ed" for the waitStreamState call
*/

#define _STREAM_STATE_EOF             1
#define _STREAM_STATE_FIRST_INIT      4
#define _STREAM_STATE_INIT            8
#define _STREAM_STATE_PLAY           16
#define _STREAM_STATE_WAIT_FOR_END   32
#define _STREAM_STATE_RESYNC_COMMIT  64
#define _STREAM_STATE_ALL         1+4+8+16+32+64


#define _RUN_CHECK_FALSE              0
#define _RUN_CHECK_TRUE               1
#define _RUN_CHECK_CONTINUE           2



/**
   Here is the base class for the player plugin.
   we can set the output of the player, and the input.
   <p>
   This class offer you a thread which you have to use
   in derived class for the decoding work.
   <p>
   All calls to your decoder goes through this class
   which must make sure that there are no races.
   <p>
   The life of the decoder thread:
   ------------------------------

   We start every time a thread if you make an instance of this class.
   The thread then goes to the idleThread() method and waits
   until we wake him up.
   If we wake him um he graps the decoderMut and enters the 
   decode_loop() This method must be used for your plugin.
   Then this thread "polls" during decoding the decoderChangeMut
   (see runCheck())
   if he can get the decoderChangeMut,the thread is authenticated for another
   decode round if not, he waits until we wake him up again.
   If the stream ends or the user stops decoding we leave the decoder_loop
   and go back to the idleThread().
   If you delete this class the thread is joined. 
   
*/

class KDE_EXPORT DecoderPlugin {

 public:
  
  DecoderPlugin();
  virtual ~DecoderPlugin();

  //
  // you can submit the following commands to the decode_loop thread
  //
  virtual void close();
  virtual void pause();
  virtual int play();
  virtual int seek(int second);
  void insertAsyncCommand(Command* cmd);
  void insertSyncCommand(Command* cmd);
  
  // returns total length (lCurrent==false) or current time (true)
  // Note: you should not call this too often because
  //       it locks the decoder thread.
  //       Should be called only after the initialisation of the stream
  //       no need to override this, overide getTotalLength()
  int getTime(int lCurrent);



  virtual void setOutputPlugin(OutputStream* output);

  // This method can only be called _once_ after a "close"
  virtual int setInputPlugin(InputStream* input);
  virtual void config(const char* key,const char* value,void* user_data);
  virtual int getStreamState();

  /**
     Use this method to wait for a stream signal.
     For example: after setInputPlugin the thread starts with
                  the state _EOF then the state changes between:

     After setInput you can wait for (INIT | DECODE | WAIT_FOR_END | EOF) this
     make sure the first init is passed. (successful or not)
     return value is current streamState, which had a succesfull match.
     Note: after return the state may already have changed (nature of threads)
                                             
  */
  int waitForStreamState(int state);
  PluginInfo* getPluginInfo();

  // Needed for Video Embedding
  int x11WindowId() { return output->x11WindowId(); }
  
  // never use this!
  void* idleThread();

  // Note: can only be called by the decode_loop thread !!!
  void setStreamState(int streamState);

 protected:

  // override this if you have the total length in second
  virtual int getTotalLength();
  // implement this if your plugin supports seek
  // (called by decoder_loop thread
  virtual int seek_impl(int second);


  /**
     should be called from decoder_loop
     checks some mutex variables if user want decoder to pause/quit
     returns:
                false : quit decoding
                true  : continue decoding

     Note: method blocks if user "pauses" the stream
  */
  int runCheck();
 
  // this mut is set from the start of decoder_loop()
  // its _not_ set if the thread runs in the while() loop
  // it it set if the thread leaves the while loop (runCheck->false)
  // (its used internally to make the getTime() call safe
  void shutdownLock();
  void shutdownUnlock();
 

  // this is the method to override in your decoder
  virtual void decoder_loop();
  
  // this method handles the commands for the decode_loop thread
  virtual int processThreadCommand(Command* command);

  OutputStream* output;
  InputStream* input;

  abs_thread_t tr;
  abs_thread_mutex_t shutdownMut; 
  abs_thread_mutex_t streamStateMut; 
  abs_thread_cond_t streamStateCond;

 


  int lDecoderLoop;
  int lCreatorLoop;
  int linDecoderLoop;
  int lDecode;
  int streamState;
  int lhasLength;
  int lAutoPlay;
  int decode_loopCounter;     // count how much we started decode_loop
  int runCheck_Counter;        // count how much we called runCheck
  int instance;
  PluginInfo* pluginInfo;

 private:

  CommandPipe* commandPipe;
  Command* threadCommand;


 
};

#endif


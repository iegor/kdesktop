#ifndef _KMIDIFACE_H_
#define _KMIDIFACE_H_

#include <dcopobject.h>

class KMidIface : virtual public DCOPObject
{
   K_DCOP

k_dcop:
// File

   virtual int openURL( const QString s )=0;

 // Song
   virtual void play()=0;
   virtual void pause()=0;
   virtual void stop()=0;
   virtual void rewind()=0;
   virtual void forward()=0;
   virtual void seek(int ms)=0;
   virtual void prevSong()=0;
   virtual void nextSong()=0; 

   virtual void setSongLoop(int i)=0;
   virtual void setVolume(int i)=0;
   virtual void setTempo(int i)=0;
// GUI


   virtual void setSongEncoding( int i )=0;
   virtual void setLyricEvents( int i )=0;

// Collections 
   virtual void setCurrentSong(int i)=0;

   virtual void setActiveCollection( int i )=0;
   virtual void setCollectionPlayMode(int i)=0;

// Midi device

   virtual void setMidiDevice(int i)=0;

};
#endif


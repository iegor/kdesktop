/***************************************************************************
                          kfeedercontrol.h  -  description
                             -------------------
    begin                : Wed Apr 17 2002
    copyright            : (C) 2002 by Arnold Kraschinski
    email                : arnold.k67@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KFEEDERCONTROL_H
#define KFEEDERCONTROL_H

#include <kprocess.h>
#include <dcopclient.h>


/** This class is used to control the KFeeder.
  *@author Arnold Kraschinski
  */

class KFeederControl {
public: 
  KFeederControl(int ID, bool splash, bool automatic);
  ~KFeederControl();
  /** resets the KFeeder */
  bool reset();
  /** sets the type of food movement. 'true' the food is moved automatically.
    * 'false' it is necessary to trigger the food movement. */
  bool setAutomatic(bool a);
  /** sends one Point to the KFeeder */
  bool addOne();
  /** sends n points to the KFeeder */
  bool addN(int n);
  /** returns if the KProcess is still active. This is useful right after the start to
    * see if it was successful
    */
  bool isActive() { return active; }
  /** returns the last result of the latest dcop operation */
  bool getLastResult() { return lastResult; }
private:
  KProcess childFeeder;
  QCString feederID;
  bool active;
  bool lastResult;

  QCString appId;	
  DCOPClient* dcop;
};

#endif

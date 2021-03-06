/*
    LX200 Generic
    Copyright (C) 2003 Jasem Mutlaq (mutlaqja@ikarustech.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef LX200GENERIC_H
#define LX200GENERIC_H

#include "indidevapi.h"
#include "indicom.h"

#define	POLLMS		1000		/* poll period, ms */
#define mydev		"LX200 Generic" /* The device name */

class LX200Generic
{
 public:
 LX200Generic();
 virtual ~LX200Generic() {}

 virtual void ISGetProperties (const char *dev);
 virtual void ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n);
 virtual void ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n);
 virtual void ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n);
 virtual void ISPoll ();
 virtual void getBasicData();

 int checkPower(INumberVectorProperty *np);
 int checkPower(ISwitchVectorProperty *sp);
 int checkPower(ITextVectorProperty *tp);
 void handleError(ISwitchVectorProperty *svp, int err, const char *msg);
 void handleError(INumberVectorProperty *nvp, int err, const char *msg);
 void handleError(ITextVectorProperty *tvp, int err, const char *msg);
 bool isTelescopeOn(void);
 void powerTelescope();
 void slewError(int slewCode);
 void getAlignment();
 int handleCoordSet();
 int getOnSwitch(ISwitchVectorProperty *sp);
 void setCurrentDeviceName(const char * devName);
 void correctFault();
 void enableSimulation(bool enable);
 void updateTime();
 void updateLocation();
 

 protected:
  int timeFormat;
  int currentSiteNum;
  int trackingMode;

  double JD;
  double currentRA;
  double currentDEC;
  double targetRA;
  double targetDEC;
  double lastRA;
  double lastDEC;
  double UTCOffset;
  bool   fault;
  bool   simulation;

  struct tm *localTM;
  
  char thisDevice[64];

  int currentSet;
  int lastSet;
  int lastMove[4];

};

void changeLX200GenericDeviceName(const char * newName);
void changeAllDeviceNames(const char *newName);

#endif

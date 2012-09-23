 

#ifndef __YAF_CONTROL_H
#define __YAF_CONTROL_H



#include "inputInterface.h"
#include "outputInterface.h"
#include "inputDecoder.h"

#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <kdemacros.h>


// prototypes

//extern "C" void control_xplayer();
KDE_EXPORT void yaf_control(InputInterface* input,
		 OutputInterface* output,
		 InputDecoder* decoder);

#endif

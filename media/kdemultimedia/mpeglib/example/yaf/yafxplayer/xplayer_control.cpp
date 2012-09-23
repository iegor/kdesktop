/*
  generic interactive controller 
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "xplayer_control.h"
#include <iostream>

using namespace std;


extern "C" void control_xplayer() {
  InputInterface input;
  OutputInterface output(&cout);
  YafOutputStream yafOutput(&input);
  InputDecoderXPlayer decoder(&yafOutput);


  cout<< "Command:0 Msg:protocol yaf-0.1" << endl;
  cout<< "Command:0 Msg:decoder generic player demo Version:0.1" << endl;
  cout<< "Command:0 Msg:comment enter 'help' for list of commands" << endl;
  

  yaf_control(&input,&output,&decoder);
}


/*

int main() {
  control_xplayer();
}


*/

/*
  valid Commands for Input/Output
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __COMMANDTABLE_H
#define __COMMANDTABLE_H


extern"C" {
#include <string.h>
#include <stdio.h>
}

#include <kdemacros.h>

#define _CT_START 40






struct CommandDescriptionStruct {

  int lexternalUse;         // written when user enters "help"

  // this field can be used to deliver a delayed return msg
  int lReturn;              // true generate ret-msg automatically

  const char* longName;
  const char* shortName;
  int   number;
  const char* help;
};


typedef struct CommandDescriptionStruct CommandDescription;


class KDE_EXPORT CommandTable {

  int nCommandDesc;
  CommandDescription commandDesc[50];

 public:
  CommandTable();
  virtual ~CommandTable();
  void insert(CommandDescription* cmdDesc);


  int getNr(const char* name);
  const char* getCommand(const char* name);
  const char* getCommand(int nr);
  const char* getArgs(const char* command,const char* wholeLine);

  int getReturnFlag(int cmdNr);
  void print();
  void print (int i,int lWithHelp);

  int getCommandCounter();
  void join(CommandTable* table);  // join two tables (no deep join!)

 private:
  CommandDescription* getCommandDescription(int i);
  int getPos(int commandNr);
};


#endif




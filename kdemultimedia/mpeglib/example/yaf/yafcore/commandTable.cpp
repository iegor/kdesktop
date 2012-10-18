/*
  valid Commands for Input/Output
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "commandTable.h"
#include <iostream>

using namespace std;


CommandTable::CommandTable(){
  nCommandDesc=0;
}

CommandTable::~CommandTable(){
}


const char* CommandTable::getCommand(int nr) {
  int i;
  const char* back="";
  for(i=0;i<nCommandDesc;i++) {
    if (commandDesc[i].number == nr) {
      return commandDesc[i].longName;
    }
  }
  return back;
}


const char* CommandTable::getCommand(const char* name) {
  int i;
  int comp;
  unsigned int nlen;
  const char* back="";
  for(i=0;i<nCommandDesc;i++) {
    nlen=strlen(commandDesc[i].longName);
    comp=strncmp(commandDesc[i].longName,name,nlen);
    if (comp == 0) {
      if (strlen(name) == nlen) {
	return commandDesc[i].longName;
      } else if ((strlen(name) > nlen) && (name[nlen]==' ')) {
	return commandDesc[i].longName;
      }
    }

    if (strlen(commandDesc[i].shortName) > 0) {
      nlen=strlen(commandDesc[i].shortName);
      comp=strncmp(commandDesc[i].shortName,name,nlen);    

      if (comp == 0) {
	if (strlen(name) == nlen) {
	  return commandDesc[i].shortName;
	} else if ((strlen(name) > nlen) && (name[nlen]==' ')) {
	  return commandDesc[i].shortName;
	}
      }
    }
  }
  return back;
}


int CommandTable::getNr(const char* command) {
  int i;
  int comp;
  int back=-1;
  unsigned int nlen;
  for(i=0;i<nCommandDesc;i++) {
    nlen=strlen(commandDesc[i].longName);
    comp=strncmp(commandDesc[i].longName,command,nlen);
    if (comp == 0) {
      if (strlen(command) == nlen) {
	return commandDesc[i].number;
      } else if ((strlen(command) > nlen) && (command[nlen]==' ')) {
	  return commandDesc[i].number;
      }
    }

    if (strlen(commandDesc[i].shortName) > 0) {
      nlen=strlen(commandDesc[i].shortName);
      comp=strncmp(commandDesc[i].shortName,command,nlen);    

      if (comp == 0) {
	if (strlen(command) == nlen) {
	  return commandDesc[i].number;
	} else if((strlen(command) > nlen) && (command[nlen]==' ')){
	  return commandDesc[i].number;
	}
      }
    }
  }
  return back;
}  


const char* CommandTable::getArgs(const char* command,const char* wholeLine) {
  unsigned int i;
  unsigned int n;
  const char* back;
  back=wholeLine;
  n=strlen(command);
  if (n==0) return back;
  for(i=0;i<n;i++) {
    back++;
  }
  // we must take care that after every command there may be a
  // space
  if (strlen(wholeLine) > n) {
    back++;
  }
  return back;
}



void CommandTable::print() {
  int i;
  cout << "internal Help System V. 0.2\n";
  cout << "known commands are :\n\n";
  for(i=0;i<nCommandDesc;i++) {
    print(commandDesc[i].number,false);
  }
}


int CommandTable::getPos(int commandNr) {
  int i;
  int pos=-1;
  for (i=0;i<nCommandDesc;i++) {
    if (commandDesc[i].number == commandNr) {
      pos=i;
      break;
    }
  }
  return pos;
}

void CommandTable::print (int command,int lWithHelp) {
  int i=getPos(command);
  if (i<0) {
    cout << "unknown Command\n";
    return;
  }
  if (commandDesc[i].lexternalUse == false) return;
  
  cout    << commandDesc[i].longName << "("  ;
  if (strlen(commandDesc[i].shortName) == 0) {
    cout << "No";
  } else {
    cout <<  commandDesc[i].shortName;
  }
  cout << ") Nr :" <<  commandDesc[i].number <<" ";
  if (lWithHelp==true) {
    cout <<  commandDesc[i].help ;
  }
  cout <<"\n";
}



CommandDescription* CommandTable::getCommandDescription(int i) {
  return &(commandDesc[i]);
}



int CommandTable::getCommandCounter() {
  return nCommandDesc;
}


void CommandTable::join(CommandTable* table) {
  int i;
  int n=table->getCommandCounter();
  CommandDescription* cmdDesc;

  for (i=0;i<n;i++) {
    cmdDesc=table->getCommandDescription(i);
    insert(cmdDesc);

  }

}



    



void CommandTable::insert(CommandDescription* cmdDesc) {
  const char* lNameTest;
  const char* sNameTest;
  int pos=getPos(cmdDesc->number);

  if (pos != -1) {
    cout << "number "<< cmdDesc->number 
	 << " for command "<< cmdDesc->longName << " already defined!" << endl;
  }

  lNameTest=getCommand(cmdDesc->longName);
  if (strlen(lNameTest) > 0) {
    cout << "longName "<< cmdDesc->longName << " already defined."
         << "Previous definition has number : " 
	 << getNr(cmdDesc->longName) << endl;
  }    

  sNameTest=getCommand(cmdDesc->shortName);
  if (strlen(sNameTest) > 0) {
    cout << "shortName "<< cmdDesc->shortName << " already defined."
         << "Previous definition has number : " 
	 << getNr(cmdDesc->shortName) << endl;

  }    


  commandDesc[nCommandDesc].lexternalUse=cmdDesc->lexternalUse;
  commandDesc[nCommandDesc].lReturn=cmdDesc->lReturn;
  commandDesc[nCommandDesc].longName=cmdDesc->longName;
  commandDesc[nCommandDesc].shortName=cmdDesc->shortName;
  commandDesc[nCommandDesc].number=cmdDesc->number;
  commandDesc[nCommandDesc].help=cmdDesc->help;
  nCommandDesc++;
}


int CommandTable::getReturnFlag(int cmdNr) {
  int i=getPos(cmdNr);
  CommandDescription* cmdDesc;
  if (i == -1) {
    return -1;
  }
  cmdDesc=getCommandDescription(i);
  return cmdDesc->lReturn;
}



  
   






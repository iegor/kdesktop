/*
  frame dumper
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


typedef float REAL;

#define SSLIMIT      18
#define SBLIMIT      32



#include "dump.h"
#include <stdio.h>


Dump::Dump() {
  
}


Dump::~Dump() {
}


void Dump::dump(REAL out[SBLIMIT][SSLIMIT]) {
  FILE* f=fopen("dump.raw","a+");
  int i;
  int j;
  for(i=0;i<SBLIMIT;i++) {
    fprintf(f,"Line:%d\n",i);
    for(j=0;j<SSLIMIT;j++) {
      fprintf(f,"%.25f\n",out[i][j]);
    }
  }
  fclose(f);
}

void Dump::dump(REAL out[SBLIMIT*SSLIMIT]) {
  FILE* f=fopen("dump.raw","a+");
  int i;
  int line=0;
  for(i=0;i<SBLIMIT*SSLIMIT;i++) {
    if ( (i % SSLIMIT) == 0) {
      fprintf(f,"Line:%d\n",line++);
    }
    fprintf(f,"%.25f\n",out[i]);
  }
  fclose(f); 
}

void Dump::dump2(REAL out[SSLIMIT*SBLIMIT]) {
  FILE* f=fopen("dump.raw","a+");
  int i;
  int j;
  int line=0;
  for(i=0;i<SSLIMIT;i++) {
    fprintf(f,"Line:%d\n",line++);
    for(j=0;j<SBLIMIT;j++) {
      fprintf(f,"%.25f\n",out[i*SBLIMIT+j]);
    }
  }
  fclose(f); 
}

void Dump::dump(REAL out[SSLIMIT][SBLIMIT]) {
  FILE* f=fopen("dump.raw","a+");
  int i;
  int j;
  for(i=0;i<SBLIMIT;i++) {
    fprintf(f,"Line:%d\n",i);
    for(j=0;j<SSLIMIT;j++) {
      fprintf(f,"%.25f\n",out[j][i]);
    }
  }
  fclose(f); 
}

void Dump::dump(int out[SBLIMIT][SSLIMIT]) {
  FILE* f=fopen("dump.raw","a+");
  int i;
  int j;
  for(i=0;i<SBLIMIT;i++) {
    fprintf(f,"Line:%d\n",i);
    for(j=0;j<SSLIMIT;j++) {
      if (out[i][j] == 0) {
	fprintf(f," %d ",out[i][j]);
	continue;
      }
      if (out[i][j] < 0) {
	fprintf(f," -x");
	continue;
      }
      fprintf(f," +x");

    }
    fprintf(f," \n");

  }
  fclose(f);
}

void Dump::dump(char* ptr,int len,int ldelete) {
  FILE* f;
  if (ldelete) {
    f=fopen("/tmp/dump.raw","w+");
  } else {
    f=fopen("/tmp/dump.raw","a+");
  }
  fwrite(ptr,len,1,f);
  fclose(f);
}


void Dump::scale_zero(layer3scalefactor* out) {

  int i;
  int j;
  
  for(i=0;i<23;i++) {
    out->l[i]=0;
  }

  for(i=0;i<3;i++) {
    for(j=0;j<13;j++) {
      out->s[i][j]=0;
    }
  }
}


void Dump::dump(layer3scalefactor* out) {
  FILE* f=fopen("dump.raw","a+");
  int i;
  int j;
  
  for(i=0;i<23;i++) {
    fprintf(f,"l[%d]=%d\n",i,out->l[i]);
  }

  for(i=0;i<3;i++) {
    for(j=0;j<13;j++) {
      fprintf(f,"s[%d][%d]=%d\n",i,j,out->s[i][j]);
    }
  }
  fprintf(f,"---------\n");
  
  fclose(f); 
}

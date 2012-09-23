    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#include "sequenceutils.h"
#include <stdarg.h>
#include <stdio.h>
#include <config.h>

using namespace std;

#if 0
void sqprintf(ArtsCorba::StringSeq *list, const char *fmt, ...)
{
	char p[1024];
	va_list ap;
	va_start(ap, fmt);
	(void) vsnprintf(p, 1024, fmt, ap);
	va_end(ap);

	unsigned long len = list->length();
	list->length(len+1);
	(*list)[len] = CORBA::string_dup(p);
}
#endif

void sqprintf(vector<string> *list, const char *fmt, ...)
{
	char p[1024];
	va_list ap;
	va_start(ap, fmt);
	(void) vsnprintf(p, 1024, fmt, ap);
	va_end(ap);

	list->push_back(p);
}

int parse_line(const char *in, char *& cmd, char *& param)
{
	int i,cmdlen=0,paramlen=0;
	static char static_cmd[1000], static_param[1000];

	cmd = static_cmd;
	param = static_param;
	i = 0;

	while(in[i] == ' ' || in[i] == '\t') i++;

	if(in[i] == 0) return(0);

	while(in[i] != '=' && in[i] != 0) cmd[cmdlen++] = in[i++];
	if(in[i] != 0) i++;
	while(in[i] != 0) param[paramlen++] = in[i++];

	cmd[cmdlen] = 0;
	param[paramlen] = 0;

	if(paramlen) return(2);
	if(cmdlen) return(1);
	return(0);
}

int parse_line(const string& in, string& cmd, string& param)
{
	char *ccmd, *cparam;
	int result = parse_line(in.c_str(),ccmd,cparam);
	param = cparam;
	cmd = ccmd;
	return result;
}

#if 0
void addSubStringSeq(ArtsCorba::StringSeq *target, ArtsCorba::StringSeq *source)
{
	unsigned long i;

	sqprintf(target,"{");
	for(i=0;i<source->length();i++)
	{
		unsigned long len = target->length();
		target->length(len+1);
		string srcstring = string("  ") + string((*source)[i]);
		(*target)[len] = CORBA::string_dup(srcstring.c_str());
	}
	sqprintf(target,"}");
}
#endif

void addSubStringSeq(vector<string> *target, const vector<string> *source)
{
	sqprintf(target,"{");

	vector<string>::const_iterator i;
	for(i=source->begin();i != source->end();i++)
		target->push_back("  " + *i);

	sqprintf(target,"}");
}

#if 0
void appendStringSeq(ArtsCorba::StringSeq *target, ArtsCorba::StringSeq *source)
{
	unsigned long i;

	for(i=0;i<source->length();i++)
	{
		unsigned long len = target->length();
		target->length(len+1);
		(*target)[len] = CORBA::string_dup((*source)[i]);
	}
}
#endif

void appendStringSeq(vector<string> *target, const vector<string> *source)
{
	vector<string>::const_iterator i;
	for(i=source->begin();i != source->end();i++)
		target->push_back(*i);
}

#if 0
ArtsCorba::StringSeq *getSubStringSeq(const ArtsCorba::StringSeq *seq,unsigned long& i)
{
	ArtsCorba::StringSeq *result = new ArtsCorba::StringSeq;
	char empty[1] = {0};
	char *cmd = empty,*param;


	while(strcmp(cmd,"{") && i<seq->length())
		parse_line((*seq)[i++],cmd,param);

	int brackets = 1;
	
	while(i<seq->length())
	{
		parse_line((*seq)[i],cmd,param);
		if(strcmp(cmd,"{") == 0) brackets++;
		if(strcmp(cmd,"}") == 0) brackets--;
		if(brackets == 0) return(result);

		unsigned long len = result->length();
		result->length(len+1);
		(*result)[len] = CORBA::string_dup((*seq)[i]);
		i++;
	}
	return(result);
}
#endif

vector<string> *getSubStringSeq(const vector<string> *seq,unsigned long& i)
{
	vector<string> *result = new vector<string>;
	string cmd = "",param;

	while(cmd != "{" && i<seq->size())
		parse_line((*seq)[i++],cmd,param);

	int brackets = 1;
	
	while(i<seq->size())
	{
		parse_line((*seq)[i],cmd,param);
		if(cmd == "{") brackets++;
		if(cmd == "}") brackets--;
		if(brackets == 0) return(result);

		result->push_back((*seq)[i]);
		i++;
	}
	return result;
}

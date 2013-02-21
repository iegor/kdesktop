/* String.cc
**
** Copyright (C) 2000,2001 by Bernhard Rosenkraenzer
**
** Contributions by A. Seigo and W. Bastian.
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#define _GNU_SOURCE 1
#include <features.h>
#include <string.h>
#include <string>
#include "String.h"
#include <stdio.h>
#include <regex.h>
#include <stdlib.h>

using namespace std;

void String::sprintf(const char *format, ...)
{
	va_list arg;
	va_start(arg, format);
	char *buf=0;
	int size=vsnprintf(buf, 0, format, arg);
	if(size==-1) { /* ARGH!!! */
		cerr << "WARNING: Your C library (libc) does not conform to the ISO C99 standard!" << endl << "Consider upgrading to glibc 2.1 or higher!" << endl;
		int bufsiz=1024;
		while(size==-1) {
			buf=(char *) malloc(bufsiz);
			size=vsnprintf(buf, 0, format, arg);
			bufsiz+=1024;
			free(buf);
		}
	}
	buf=(char *) malloc(size+1);
	vsnprintf(buf, size+1, format, arg);
	string str=buf;
	*this=buf;
	va_end(arg);
	free(buf);
	return;
}
bool String::readfile(String filename)
{
	FILE *f=fopen(filename, "r");
	if(!f)
		return false;

	string str="";
	char *buf=(char *) malloc(1024);
	while(!feof(f) && !ferror(f)) {
		if(!fgets(buf, 1024, f))
			continue;
		str += buf;
	};
	*this=buf;
	free(buf);
	fclose(f);
	return true;
}
char *String::cstr() const
{
	char *a=new char[size()+1];
	a[size()]=0;
	strncpy(a, data(), size());
	return a;
}
bool String::cmp(char const * const s) const
{
	if(size() != strlen(s))
		return false;
	return (strncmp(data(), s, size())==0);
}
bool String::casecmp(char const * const s) const
{
	if(size() != strlen(s))
		return false;
	return (strncasecmp(data(), s, size())==0);
}
bool String::contains(String const &s, bool cs) const
{
	if(cs)
		if(strstr(cstr(), s.cstr()))
			return true;
		else
			return false;
	else
		if(strcasestr(cstr(), s.cstr()))
			return true;
		else
			return false;
}
int String::locate(String const &s, bool cs, unsigned int startat) const
{
	if(startat>=size())
		return -1;

	char *s0=cstr(), *s1=s.cstr(), *s2;
	int r;
	if(cs)
		s2=strstr(s0+startat, s1);
	else
		s2=strcasestr(s0+startat, s1);
	if(s2==NULL) {
		delete [] s0;
		delete [] s1;
		return -1;
	}
	r=(s2-s0);
	if(startat>0) r++;
	delete [] s0;
	delete [] s1;
	return r;
}
String const String::operator +(char const &s) {
	char a[2];
	a[0]=s;
	a[1]=0;
	String st=cstr();
	st+=a;
	return st;
}
String const String::operator +(char const * const s) {
	String st=cstr();
	st += s;
	return st;
}
bool String::operator ==(char s) {
	if(size()==1 && cstr()[0]==s)
		return true;
	else
		return false;
}
bool String::operator !=(char s) {
	if(size()!=1 || cstr()[0]!=s)
		return true;
	else
		return false;
}
String String::simplifyWhiteSpace() const {
	char *s=cstr();
	for(unsigned int i=0; i<size(); i++)
		if(isspace(s[i]))
			s[i]=' ';
	while(*s==' ')
		strcpy(s, s+1);
	int l = strlen(s);
	while(l && (s[l-1]==' '))
		s[--l]=0;
		
	while(strstr(s, "  "))
		strcpy(strstr(s, "  "), strstr(s, "  ")+1);
	return s;
}
String String::left(unsigned int num) const
{
	if(num==0) return "";
	char *s=cstr();
	if(size()<=num)
		return s;
	s[num]=0;
	return s;
}
String String::right(unsigned int num) const
{
	if(num==0) return "";
	char *s=cstr();
	if(size()<=num)
		return s;
	strcpy(s, s+strlen(s)-num);
	return s;
}
String String::mid(unsigned int start, unsigned int num) const
{
	if(start>=size())
		return "";
	char *s=cstr();
	start--;
	if(start>0)
		strcpy(s, s+start);
	if(num>0 && num<=strlen(s))
		s[num]=0;
	return s;
}
String &String::regex(String const &expr, bool cs) const
{
	regex_t regexp;
	int err;
	regmatch_t reg[1];
	String *ret=new String("");
	if((err=regcomp(&regexp, expr, cs?REG_EXTENDED:REG_EXTENDED|REG_ICASE))) {
		regfree(&regexp);
		return *ret;
	}
	err=regexec(&regexp, cstr(), 1, reg, 0);
	regfree(&regexp);
	if(err)
		return *ret;
	if(reg[0].rm_so!=-1) {
		char *s=strdup(cstr()+reg[0].rm_so);
		s[reg[0].rm_eo-reg[0].rm_so]=0;
		delete ret;
		ret=new String(s);
		free(s);
	}
	return *ret;
}
String &String::replace(String const &what, String const &with, bool all) const
{
	String *result;
	if(!contains(what)) {
		result=new String(*this);
		return *result;
	}
	result=new String;
	*result=left(locate(what));
	*result+=with;
	if(!all) {
     		*result+=right(size()-locate(what)-what.size());
	} else {
		unsigned int start=locate(what)+what.size()+1;
		int loc;
		while((loc=locate(what, true, start+1))!=-1) {
			*result+=mid(start, loc-start);
			*result+=with;
			start=locate(what, true, start)+what.size();
		}
		if(size()>start)
			*result+=right(size()-start+1);
	}
	return *result;
}
String String::escapeForRegExp(String const &s)
{
	static const char meta[] = "$()*+.?[\\]^{|}";
	String quoted = s;
	int i = 0;

	while ( i < (int) quoted.length() ) {
		if ( strchr(meta, quoted.at(i)) != 0 )
			quoted.insert( i++, "\\" );
		i++;
	}
	return quoted;
}
StringList::StringList(String const &s)
{
	clear();
	char *st=strdup((char const * const)s);
	char *tok;
	char *line=strtok_r(st, "\n", &tok);
	while(line) {
		if(line[strlen(line)-1]=='\r') // Handle sucking OSes
			line[strlen(line)-1]=0;
		insert(end(), line);
		line=strtok_r(NULL, "\n", &tok);
	}
	free(st);
}

StringList::StringList(char **strs, int num)
{
	clear();
	if(num>=0) {
		for(int i=0; i<num; i++)
			insert(end(), strs[i]);
	} else {
		for(int i=0; strs[i]!=NULL; i++)
			insert(end(), strs[i]);
	}
}

bool StringList::readfile(String const &filename)
{
	clear();
	FILE *f=fopen(filename, "r");
	if(!f)
		return false;
	char *buf=(char *) malloc(1024);
	while(!feof(f) && !ferror(f)) {
		if(!fgets(buf, 1024, f))
			continue;
		while(strlen(buf) && (buf[strlen(buf)-1]=='\n' || buf[strlen(buf)-1]=='\r'))
			buf[strlen(buf)-1]=0;
		insert(end(), buf);
	};
	free(buf);
	fclose(f);
	return true;
}
bool StringList::writefile(String const &filename) const
{
	FILE *f=fopen(filename, "w");
	if(!f)
		return false;
	for(const_iterator it=begin(); it!=end(); it++) {
		fputs(*it, f);
		fputs("\n", f);
	}
	fclose(f);
	return true;
}
void StringList::operator +=(StringList const &s)
{
	for(const_iterator it=s.begin(); it!=s.end(); it++)
		insert(end(), *it);
}
void StringList::operator +=(StringList const * const s)
{
	for(const_iterator it=s->begin(); it!=s->end(); it++)
		insert(end(), *it);
}
bool StringList::contains(String const &s) const
{
	for(const_iterator it=begin(); it!=end(); it++)
		if(*it == s)
			return true;
	return false;
}
void StringList::remove(String const &s)
{
	bool done=false;
	for(iterator it=begin(); !done && it!=end(); it++)
		if(*it==s) {
			erase(it);
			done=true;
		}
}
String const &StringList::grep(String const &s) const
{
	for(const_iterator it=begin(); it!=end(); it++)
		if(!(*it).regex(s).empty())
			return *it;
	String *r=new String;
	return *r;
}
int __stringlist_compare(const void *a, const void *b)
{
	if(a==0 && b==0)
		return 0;
	else if(a==0)
		return 1;
	else if(b==0)
		return -1;
	else
		return strcmp((const char *)a,(const char *)b);
}
int __stringlist_compare_noncs(const void *a, const void *b)
{
	if(a==0 && b==0)
		return 0;
	else if(a==0)
		return 1;
	else if(b==0)
		return -1;
	else
		return strcasecmp((const char *)a,(const char *)b);
}
void StringList::sort(bool cs)
{
	unsigned int i=0, s=size();
	char **strings=new char*[s];
	for(const_iterator it=begin(); it!=end(); it++)
		strings[i++]=(*it).cstr();
	if(cs)
		qsort(strings, s, sizeof(char*),  __stringlist_compare);
	else
		qsort(strings, s, sizeof(char*), __stringlist_compare_noncs);
	clear();
	for(i=0; i<s; i++) {
		insert(end(), strings[i]);
		delete [] strings[i];
	}
	delete [] strings;
}
StringList::operator String() const
{
	String s="";
	for(const_iterator it=begin(); it!=end(); it++) {
		s+=(*it);
		if(s.right()!=String("\n") && s.right()!=String("\r"))
			s+="\n";
	}
	return s;
}

ostream &operator <<(ostream &os, String const &s)
{
	if(!s.empty())
		os << (char const * const) s;
	return os;
}
ostream &operator <<(ostream &os, String const *s)
{
	if(!s->empty())
		os << (char const * const) *s;
	return os;
}
ostream &operator <<(ostream &os, StringList const &s)
{
	for(StringList::const_iterator it=s.begin(); it!=s.end(); it++) {
		os << *it;
		if((*it).right()!=String("\n") && (*it).right()!=String("\r"))
			os << endl;
	}
	return os;
}
ostream &operator <<(ostream &os, StringList const *s)
{
	for(StringList::const_iterator it=s->begin(); it!=s->end(); it++) {
		os << *it;
		if((*it).right()!=String("\n") && (*it).right()!=String("\r"))
			os << endl;
	}
	return os;
}

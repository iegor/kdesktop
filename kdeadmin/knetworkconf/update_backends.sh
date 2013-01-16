#!/bin/bash
echo 'Network backend updating script of KNetworkConf'
echo

if [ $# -eq 0 ]
then
	echo "Use: $0 src_path dest_path"
	echo
	exit 1
fi	
	
if [ "$1" = "" ]
then 
	echo 'You have to specify the source directory of the new scripts'
	echo
	exit 1
elif [ "$2" = "" ]
then
	echo 'You have to specify the destination directory for the new scripts'	
	echo
	exit 1 	
fi

if [ ! -d $1 ]
then	
	echo "$1 Isn't a directory"
	echo
	exit 1
elif [ ! -d $2 ]
then
        echo "$2 Isn't a directory"
        echo
        exit 1	
fi

diractual=`pwd`
cd $1;
cp -f AUTHORS debug.pl.in file.pl.in general.pl.in guess_system.sh mkinstalldirs network-conf.in network.pl.in NEWS parse.pl.in platform.pl.in README replace.pl.in report.pl.in service-list.pl.in service.pl.in system-tools-backends.pc.in type1inst util.pl.in xml.pl.in process.pl.in $2

echo 
echo 'Ready, dont forget to check configure.in.in or Makefile.am to see if they need further updating.' 
echo

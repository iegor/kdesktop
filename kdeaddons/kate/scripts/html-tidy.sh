#!/bin/bash

# Kate script interface to html tidy
# Version 1 - 13/10/03
# by David Ross http://www.rossy.co.uk
# modified by Christoph Cullmann for Kate 2.2 (KDE 3.2)
# Version 1.1 - 15/10/03
# Added a help message if tidy isn't installed.

################
# Start Config #
################

# Path to tidy - just use "tidy" if it is in your path
tidy="tidy"

# Specify any options you want to pass to tidy
tidyoptions="-iu --wrap 0 --doctype transitional"

# File to write errors to
errfile=/tmp/$$.err
infile=/tmp/$$.in
outfile=/tmp/$$.out

# Clean up when done or when aborting.
trap "rm -f $outfile; rm -f $infile; rm -f $errfile" 0 1 2 3 15

# Do you want a prompt before showing errors
# If you don't have "kdialog" set to 0
errprompt=1

##############
# Start Code #
##############

# Is tidy installed
if [ `$tidy -v | grep -c "HTML Tidy"` -ne 1 ];then
kdialog --msgbox "HTML Tidy could not be found.
Please download install HTML tidy into your path, tidy can be downloaded from http://tidy.sf.net"
else
# Get the active document number
doc=`dcop $1 KateDocumentManager activeDocumentNumber`

# Get the current html from kate
dcop $1 EditInterface#$doc text > $infile

# Run the html through tidy
cat $infile | $tidy $tidyoptions > $outfile 2> $errfile

# Only send back the tidied version if there is valid html
if [ `cat $outfile|grep -c "DOCTYPE"` -gt 0 ];then
dcop $1 EditInterface#$doc setText "`cat $outfile`"
fi

# Open a new document with the error code should errors arise
if [ `cat $errfile|grep -c "No warnings or errors were found"` -eq 0 ];then
if [ $errprompt -eq 1 ];then
kdialog --yesno "Errors were found. Do you want to see them?"
fi
if [ $? -eq 0 ] || [ $errprompt -eq 0 ];then
dcop $1 KateDocumentManager openURL "$errfile" ""
fi
fi
fi

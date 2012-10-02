#!/bin/bash
# $Id: app.sh 314274 2004-05-24 21:04:46Z geiseri $
# %{APPNAME} - Copyright (C) %{YEAR} %{AUTHOR} <%{EMAIL}>


hello()
{
	echo "Hello $NAME"
}

echo "This is a test"

LST="Ian Ben Rook Kaz"
for NAME in $LST
do
	hello
done

#!/bin/sh
echo "This script searches for improperly named .arts structures:" >&2

for i in *.arts
do
	grep ^name=$(echo $i|sed s/.arts//g) $i >/dev/null || echo $i
done

echo "... done" >&2

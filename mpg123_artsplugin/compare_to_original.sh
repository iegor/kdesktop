#!/bin/sh
diff \
"--exclude-from=distexclude" -Bbdu1 ~/cvs/mpg123/mpg123  mpg123 &> ~/mpg.diff

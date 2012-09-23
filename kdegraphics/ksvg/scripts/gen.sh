# try to find an appropriate awk interpreter
if which mawk > /dev/null 2>&1; then
  cmd_awk=`which mawk`;
else
  if which awk > /dev/null 2>&1; then
    cmd_awk=`which awk`;
  else
    echo "No suitable AWK interpreter found. Aborting."
	exit 1
  fi
fi

# make them
touch $1.h
touch $1.cc

#start with copyright notices
cat ../COPYRIGHTS > $1.h
cat ../COPYRIGHTS > $1.cc

# add define for multiple include problem
echo "" >> $1.h
echo "#ifndef $1_H" >> $1.h
echo "#define $1_H" >> $1.h
echo "" >> $1.h
echo "namespace KSVG" >> $1.h
echo "{" >> $1.h
echo "" >> $1.h

#include in .cc + namespace
echo "" >> $1.cc
echo "#include \"$1.h\"" >> $1.cc
echo "#include \"$1Impl.h\"" >> $1.cc
echo "" >> $1.cc
echo "using namespace KSVG;" >> $1.cc
echo "" >> $1.cc

#go go go
$cmd_awk -f ../makeheader $1
$cmd_awk -f ../makecc $1

#add end of define
echo "};" >> $1.h
echo "" >> $1.h
echo "#endif" >> $1.h
echo "" >> $1.h

echo "// vim:ts=4:noet" >> $1.h
echo "// vim:ts=4:noet" >> $1.cc

#done I hope

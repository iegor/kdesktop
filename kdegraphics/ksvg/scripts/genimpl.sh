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

# you cant touch this
touch $1Impl.h
touch $1Impl.cc

#start with copyright notices
cat ../COPYRIGHTS > $1Impl.h
cat ../COPYRIGHTS > $1Impl.cc

# add define for multiple include problem
echo "" >> $1Impl.h
echo "#ifndef $1Impl_H" >> $1Impl.h
echo "#define $1Impl_H" >> $1Impl.h

# namespace
echo "" >> $1Impl.h
echo "namespace KSVG" >> $1Impl.h
echo "{" >> $1Impl.h

#include in .cc + namespace
echo "" >> $1Impl.cc
echo "#include \"$1Impl.h\"" >> $1Impl.cc
echo "" >> $1Impl.cc
echo "using namespace KSVG;" >> $1Impl.cc
echo "" >> $1Impl.cc

#go go go
$cmd_awk -f ../makeimpl $1

#add end of define
echo "};" >> $1Impl.h
echo "" >> $1Impl.h
echo "#endif" >> $1Impl.h
echo "" >> $1Impl.h
echo "// vim:ts=4:noet" >> $1Impl.h

echo "// vim:ts=4:noet" >> $1Impl.cc
#done I hope

#!/bin/bash
mkdir nowind nopress notemp
for  file in `ls .`
do
	METAR=`cat $file | grep -v ":"`
#	echo testing data $METAR
	KWEATHER=`dcopfind kweather* weatherIface`
	dcop "$KWEATHER" parseWeather "$METAR" false
	WIND=`dcop "$KWEATHER"  get_wind`
	PRESS=`dcop "$KWEATHER" get_pressure`
	TEMP=`dcop "$KWEATHER" get_temperature`
	if `test "$WIND"=""`
		then
			cp $file nowind/$file
	fi
	if `test "$PRESS"=""`
		then
			cp $file nopress/$file
	fi
	if `test "$TEMP"=""`
		then
			cp $file notemp/$file
	fi

	echo $WIND $PRESS $TEMP
#	#sleep 1
done


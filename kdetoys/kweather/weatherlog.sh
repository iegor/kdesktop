#!/bin/sh

LOC=KEWR
TEMP=`dcop KWeatherService WeatherService temperature $LOC`
WIND=`dcop KWeatherService WeatherService wind $LOC`
VIS=`dcop KWeatherService WeatherService visibility $LOC`
DATE=`date`
echo "$DATE,$TEMP,$WIND,$VIS" >> $LOC.log

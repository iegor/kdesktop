#!/bin/sh

LOC=KPTW
TEMP=`dcop KWeatherService WeatherService temperature $LOC`
WIND=`dcop KWeatherService WeatherService wind $LOC`
DATE=`dcop KWeatherService WeatherService date $LOC`
VIS=`dcop KWeatherService WeatherService visibility $LOC`
NAME=`dcop KWeatherService WeatherService stationName $LOC`
EMAIL="geiseri@kde.org"
echo "===+<KWeather for KDE>+=+<http://www.kde.org>+==="
echo "   Report for $NAME" 
echo "		on $DATE"
echo "   $TEMP with winds at $WIND and $VIS of visibility."
echo "===============================+<$EMAIL>+==="

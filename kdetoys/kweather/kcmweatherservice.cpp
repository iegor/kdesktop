/*                                                                      
    This file is part of KWeather.                                  
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>
                                                                        
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or   
    (at your option) any later version.                                 
                                                                        
    This program is distributed in the hope that it will be useful,     
    but WITHOUT ANY WARRANTY; without even the implied warranty of      
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        
    GNU General Public License for more details.                        
                                                                        
    You should have received a copy of the GNU General Public License   
    along with this program; if not, write to the Free Software         
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           
                                                                        
    As a special exception, permission is given to link this program    
    with any edition of Qt, and distribute the resulting executable,    
    without including the source code for Qt in the source distribution.
*/                                                                      

#include <qlayout.h>

#include <kaboutdata.h>
#include <kdebug.h>
#include <klocale.h>

#include "serviceconfigwidget.h"

#include "kcmweatherservice.h"

extern "C"
{
  KDE_EXPORT KCModule *create_weatherservice( QWidget *parent, const char * ) {
    return new KCMWeatherService( parent, "kweather" );
  }
}

KCMWeatherService::KCMWeatherService( QWidget *parent, const char *name )
  : KCModule( parent, name )
{
  QVBoxLayout *layout = new QVBoxLayout( this );
  mWidget = new ServiceConfigWidget( this );

  // not needed, as a change immediately changes the service
  //connect(mWidget, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));

  layout->addWidget( mWidget );
  KAboutData *about = new KAboutData( "kcmweatherservice",
                                      I18N_NOOP( "KWeather Configure Dialog" ),
                                      0, 0, KAboutData::License_GPL,
                                      I18N_NOOP( "(c), 2003 Tobias Koenig" ) );

  about->addAuthor( "Tobias Koenig", 0, "tokoe@kde.org" );
  setAboutData(about);
}

void KCMWeatherService::load()
{
}

void KCMWeatherService::save()
{
}

void KCMWeatherService::defaults()
{
}

#include "kcmweatherservice.moc"

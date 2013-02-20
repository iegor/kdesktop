/*
    This file is part of KWeather.
    Copyright (c) 2004 Tobias Koenig <tokoe@kde.org>

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
*/

#include <qimage.h>
#include <qheader.h>

#include <dcopclient.h>
#include <dcopref.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klistview.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include "serviceconfigwidget.h"
#include "weatherservice_stub.h"

class StationItem : public QListViewItem
{
  public:
    StationItem( QListView *view, const QString &name, const QString &uid )
      : QListViewItem( view, name ), mUID( uid )
    {
    }

    StationItem( QListViewItem *item, const QString &name, const QString &uid )
      : QListViewItem( item, name ), mUID( uid )
    {
    }

    QString uid() const { return mUID; }

  private:
    QString mUID;
};

static void parseStationEntry( const QString &line, QString &name, QString &uid );

ServiceConfigWidget::ServiceConfigWidget( QWidget *parent, const char *name )
  : wsPrefs( parent, name ), mService(0)
{
  mService = new WeatherService_stub( "KWeatherService", "WeatherService" );
  connect( mAllStations, SIGNAL( doubleClicked ( QListViewItem *, const QPoint &, int ) ), SLOT( addStation() ) );
  connect( mSelectedStations, SIGNAL( doubleClicked ( QListViewItem *, const QPoint &, int ) ), SLOT( removeStation() ) );

  initGUI();
  loadLocations();
  scanStations();
}

ServiceConfigWidget::~ServiceConfigWidget()
{
  delete mService;
}

void ServiceConfigWidget::addStation()
{
  if ( !dcopActive() )
    return;

  StationItem *item = dynamic_cast<StationItem*>( mAllStations->selectedItem() );
  if ( item == 0 )
    return;

  mService->addStation( item->uid() );
  scanStations();

  modified();
}

void ServiceConfigWidget::removeStation()
{
  if ( !dcopActive() )
    return;

  StationItem *item = dynamic_cast<StationItem*>( mSelectedStations->selectedItem() );
  if ( item == 0 )
    return;

  mService->removeStation( item->uid() );
  scanStations();

  modified();
}

void ServiceConfigWidget::updateStations()
{
  if ( !dcopActive() )
    return;

  mService->updateAll( );
  scanStations();
}

void ServiceConfigWidget::exitWeatherService()
{
  if ( !dcopActive() )
    return;

  mService->exit();
  modified();
}

void ServiceConfigWidget::scanStations()
{
  if ( !dcopActive() )
    return;

  QStringList list = mService->listStations( );

  mSelectedStations->clear();
  for ( uint i = 0; i < list.count(); ++i ) {
    QPixmap pm = mService->icon( list[ i ] );
    QImage img = pm.convertToImage();
    img = img.smoothScale( 22, 22 );
    pm.convertFromImage( img );

    QString uid = list[ i ];
    if (mStationMap[ uid ].isEmpty())
    {
      mStationMap[ uid ] = uid;
    }
    StationItem *item = new StationItem( mSelectedStations, mStationMap[ uid ], uid );

    item->setPixmap( 0, pm );
  }
}

void ServiceConfigWidget::selectionChanged( QListViewItem *item )
{
  mRemoveButton->setEnabled( item != 0 );
}

void ServiceConfigWidget::modified()
{
  emit changed( true );
}

void ServiceConfigWidget::initGUI()
{
  mAllStations->header()->hide();
  mSelectedStations->header()->hide();
}

void ServiceConfigWidget::loadLocations()
{
  KConfig config( locate( "data", "kweatherservice/weather_stations.desktop" ) );

  config.setGroup( "Main" );
  QStringList regions = QStringList::split( ' ', config.readEntry( "regions" ) );

  QStringList::ConstIterator regionIt;
  for ( regionIt = regions.begin(); regionIt != regions.end(); ++regionIt ) {
    config.setGroup( *regionIt );
    QString name = config.readEntry( "name" );
    QStringList states = config.readListEntry( "states", ' ' );

    QListViewItem *regionItem = new QListViewItem( mAllStations, name );
    regionItem->setSelectable( false );

    QStringList::ConstIterator stateIt;
    for ( stateIt = states.begin(); stateIt != states.end(); ++stateIt ) {
      config.setGroup( *regionIt + "_" + *stateIt );
      QString name = config.readEntry( "name" );

      QListViewItem *stateItem = new QListViewItem( regionItem, name );
      stateItem->setSelectable( false );

      QMap<QString, QString> entries = config.entryMap( *regionIt + "_" + *stateIt );
      QMap<QString, QString>::ConstIterator entryIt;
      for ( entryIt = entries.begin(); entryIt != entries.end(); ++entryIt ) {
        if ( entryIt.key() != "name" ) {
          QString station, uid;
          // get station and uid from the data
          parseStationEntry( entryIt.data(), station, uid );
          new StationItem( stateItem, station, uid );
          mStationMap.insert( uid, QString( "%1, %2" )
              .arg( station ).arg( *stateIt ) );
        }
      }
    }
  }
}

bool ServiceConfigWidget::dcopActive()
{
  QString error;
  QCString appID;
  bool isGood = true;
  DCOPClient *client = kapp->dcopClient();
  if ( !client->isApplicationRegistered( "KWeatherService" ) ) {
    if ( KApplication::startServiceByDesktopName( "kweatherservice", QStringList(), &error, &appID ) )
      isGood = false;
  }

  return isGood;
}

void parseStationEntry( const QString &line, QString &name, QString &uid )
{
  QStringList list = QStringList::split( ' ', line );

  bool inName = true;

  for ( uint i = 0; i < list.count(); ++i ) {
    if ( inName ) {
      if ( list[ i ].endsWith( "\\" ) ) {
        name.append( list[ i ].replace( "\\", " " ) );
      } else {
        name.append( list[ i ] );
        inName = false;
      }
    } else {
      uid = list[ i ];
      return;
    }
  }
}

#include "serviceconfigwidget.moc"

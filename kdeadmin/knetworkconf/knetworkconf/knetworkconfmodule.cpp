/***************************************************************************
                          knetworkconfmodule.cpp  -  description
                             -------------------
    begin                : Tue Apr 1 2003
    copyright            : (C) 2003 by Juan Luis Baptiste
    email                : juancho@linuxmail.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <klocale.h>
#include <kcmodule.h>
#include <qlayout.h> 
#include <version.h>
#include "knetworkconfmodule.h"



KNetworkConfModule::KNetworkConfModule(QWidget* parent, const char *name/*, QStringList list*/)
  : KCModule(parent,name)
{
  QVBoxLayout *top = new QVBoxLayout(this);
  
  conf = new KNetworkConf(this);
  conf->setVersion(QString(VERSION));
  conf->setReadOnly(false);
  
  top->addWidget(conf);

  if (getuid() != 0){
    conf->setReadOnlySlot(true);
    conf->setReadOnly(true);
  }  

  connect(conf,SIGNAL(networkStateChanged(bool)),SLOT(configChanged(bool)));
  setButtons(KCModule::Apply|KCModule::Help); 
}

KNetworkConfModule::~KNetworkConfModule()
{
}

void KNetworkConfModule::configChanged(bool b)
{
  emit changed(b);
}

void KNetworkConfModule::load()
{
 // conf->loadNetworkDevicesInfo();
  //conf->loadRoutingAndDNSInfo();
  //conf->loadDNSInfoTab();
}

void KNetworkConfModule::save()
{
   conf->saveInfoSlot();
}
/*
int KNetworkConfModule::buttons()
{
  return KCModule::Ok|KCModule::Apply|KCModule::Help;
}
*/
bool KNetworkConfModule::useRootOnlyMsg() const
{
  return true;
}

QString KNetworkConfModule::rootOnlyMsg() const
{
  return "Changing the network configuration requires root access";
}

KAboutData* KNetworkConfModule::aboutData() const
{
  /*KAboutData* data = new KAboutData( "knetworkconf", I18N_NOOP("KNetworkConf"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2003, Juan Luis Baptiste", 0, "http://www.merlinux.org/knetworkconf/", "jbaptiste@merlinux.org");
  data->addAuthor("Juan Luis Baptiste",I18N_NOOP("Lead Developer"),
                      "jbaptiste@merlinux.org");
  data->addCredit("David Sansome",I18N_NOOP("Various bugfixes and features"),
                      "me@davidsansome.com");

  return data; // Memory leak, oh well...*/
  
KAboutData *aboutData = new KAboutData( "knetworkconf", I18N_NOOP("KNetworkConf"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2003 - 2005, Juan Luis Baptiste", 0, "http://www.merlinux.org/knetworkconf/", "juan.baptiste@kdemail.net");
  aboutData->addAuthor("Juan Luis Baptiste",I18N_NOOP("Lead Developer"),
                      "juan.baptiste@kdemail.net");
  aboutData->addCredit("Carlos Garnacho and the Gnome System Tools Team",I18N_NOOP("Provided the Network backend which KNetworkConf relies on."),
                      "garnacho@tuxerver.net","http://www.gnome.org/projects/gst/");
  aboutData->addCredit("Helio Chissini de Castro",I18N_NOOP("Conectiva Linux Support"),
                      "helio@conectiva.com.br");
  aboutData->addCredit("Christoph Eckert",I18N_NOOP("Documentation maintainer, and German translator"),
                      "mchristoph.eckert@t-online.de ");  
  aboutData->addCredit("David Sansome",I18N_NOOP("Various bugfixes and features"),
                      "me@davidsansome.com");                      
  aboutData->addCredit("Gustavo Pichorim Boiko",I18N_NOOP("Various bugfixes and Brazilian Portuguese translator"),"gustavo.boiko@kdemail.net");
  return aboutData;// Memory leak, oh well...                      
}

QString KNetworkConfModule::quickHelp() const
{
  return i18n("%1Network configuration%2This module allows you to configure your TCP/IP settings.%3").arg("<h1>").arg("</h1><p>").arg("</p>");
}

//#include "knetworkconfmodule.moc"

#include "knetworkconfmodule.moc"

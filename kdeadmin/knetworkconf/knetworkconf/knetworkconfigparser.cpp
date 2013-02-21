/***************************************************************************
                          knetworkconfigparser.cpp  -  description
                             -------------------
    begin                : Mon Jan 13 2003
    copyright            : (C) 2003 by Juan Luis Baptiste
    email                : jbaptiste@merlinux.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "knetworkconfigparser.h"
#include "knetworkconfigparser.moc"

KNetworkConfigParser::KNetworkConfigParser(){
  networkInfo = new KNetworkInfo();
  
  QString platform;
  bool askAgain = readAskAgain(platform);
    
  if (!askAgain || platform.length() > 0)
      runDetectionScript(platform);
  else 
    runDetectionScript(QString::null);
}
/** Runs the gst backend to get network values. You can pass to the script a specific platform to load using the platform parameter.  */
void KNetworkConfigParser::runDetectionScript(QString platform){
  KDetectDistroDlg* dialog = new KDetectDistroDlg(0, 0);
  dialog->show();
  procDetect = new QProcess(this);
  QString pathToProgram =  locate("data",BACKEND_PATH);
  if (pathToProgram.isEmpty())
  {
    KMessageBox::error(0,
                        i18n("Could not find the backend script for the network configuration detection. Something is wrong with your installation.\n Please check that  \n{KDE_PATH}/%1 \nfile is present.").arg(BACKEND_PATH),
                        i18n("Could Not Find Network Configuration Backend Script"));
      dialog->close();
      //kapp->quit();
  }
  else
  {
    procDetect->addArgument( pathToProgram );
    if (platform != QString::null)
    {
      procDetect->addArgument( "--platform" );
      procDetect->addArgument( platform );
    }
    procDetect->addArgument( "--get" );
    connect( this, SIGNAL(readyLoadingNetworkInfo()), dialog, SLOT(close()) );
    connect( this, SIGNAL(errorDetectingPlatform()), dialog, SLOT(close()) );
    connect( procDetect, SIGNAL(processExited()), this, SLOT(readNetworkInfo()) );
    connect( procDetect, SIGNAL(readyReadStdout()),this, SLOT(concatXMLOutputSlot()));
    connect( procDetect, SIGNAL(readyReadStderr()),this, SLOT(readXMLErrSlot()));

    if ( !procDetect->start() )
    {
    // error handling
      KMessageBox::error(0,
                        i18n("Could not execute backend script for the network configuration detection. Something is wrong with your installation."),
                        i18n("Could Not Launch Network Configuration Backend Script"));
      dialog->close();
    }
  }
}
/** runs gst to find out the state of network devices.It runs the command:
$knetworkconf_home/backends/networkconf [--platform platform] -d list_ifaces. */
void KNetworkConfigParser::listIfaces(const QString &platform){
  procDetect = new QProcess(this);
  procDetect->addArgument( locate("data",BACKEND_PATH) );
  if (platform != QString::null)
  {
    procDetect->addArgument( "--platform" );
    procDetect->addArgument( platform );
  }
  //procDetect->addArgument( "--get" );
  procDetect->addArgument( "-d" );
  procDetect->addArgument( "list_ifaces" );

  connect( procDetect, SIGNAL(processExited()), this, SLOT(readListIfacesSlot()) );
  connect( procDetect, SIGNAL(readyReadStdout()),this, SLOT(concatXMLOutputSlot()));
  connect( procDetect, SIGNAL(readyReadStderr()),this, SLOT(readXMLErrSlot()));

  xmlOuput = "";
  xmlErr = "";
 if ( !procDetect->start() )
  {
// error handling
      KMessageBox::error(0,
                        i18n("Could not execute backend script for the network configuration detection. Something is wrong with your installation."),
                        i18n("Could Not Launch Network Configuration Backend Script"));
  }
}

void KNetworkConfigParser::readListIfacesSlot(){
  QPtrList<KNetworkInterface> tempDeviceList;

  //The gst backend puts a \n at the beginning of the xml output, so
  //we have to erase it first before we parse it.
  xmlOuput = xmlOuput.section('\n',1);
  qDebug("XML -d list_ifaces: %s",xmlOuput.latin1());
    QString err;
    int x,y;
    QDomDocument doc( "network-ifaces");
    if ( !doc.setContent( xmlOuput.utf8(),false,&err,&x,&y  ) )
    {
      KMessageBox::error(0,
                      i18n("Could not parse the XML output from the network configuration backend."),
                      i18n("Error While Listing Network Interfaces"));
//        qDebug("error: %s %d,%d",err.latin1(),x,y);
    }
    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();
    
    while( !node.isNull() )
    {
      if ( node.isElement() && node.nodeName() == "interface" )
      {
        QDomElement interface = node.toElement();
        KNetworkInterface *tempDevice = new KNetworkInterface();
        tempDevice = getInterfaceInfo(interface,QString::null);

        if (tempDevice->getType().lower() != LOOPBACK_IFACE_TYPE)
        {
          KNetworkInterface *originalDevice = getDeviceInfo(tempDevice->getDeviceName());
          if (originalDevice == NULL)
          {
            node = node.nextSibling();
            continue;
          }
          originalDevice->setActive(tempDevice->isActive());
          if (!tempDevice->getBroadcast().isEmpty())
            originalDevice->setBroadcast(tempDevice->getBroadcast());
          if (!tempDevice->getDescription().isEmpty())
            originalDevice->setDescription(tempDevice->getDescription());
          if (!tempDevice->getIpAddress().isEmpty())
            originalDevice->setIpAddress(tempDevice->getIpAddress());
          if (!tempDevice->getMacAddress().isEmpty())
            originalDevice->setMacAddress(tempDevice->getMacAddress());
          if (!tempDevice->getNetmask().isEmpty())
            originalDevice->setNetmask(tempDevice->getNetmask());
          if (!tempDevice->getNetwork().isEmpty())
            originalDevice->setNetwork(tempDevice->getNetwork());
        }
      }
      node = node.nextSibling();
    }
    //networkInfo->setDeviceList(deviceList);
    //Tell to interested parties when the network info is ready.
   emit readyLoadingNetworkInfo();
}

KNetworkConfigParser::~KNetworkConfigParser(){
}

/** return tyhe number of configured devices. */
unsigned KNetworkConfigParser::numDevices(){
  return _numDevices;
}

void KNetworkConfigParser::setProgramVersion(QString ver)
{
  KNetworkConfigParser::programVersion = ver;
}


void KNetworkConfigParser::readIfconfigOutput(){
  QString s = proc->readStdout();
  ifconfigOutput = s;
}

/** 
  Reads /proc/net/route looking for the default gateway. 
  
  NOTE:We should use the gateway reported by gst, but if there's a
  gw in a config file and one of the network interfaces is 
  configured to use dhcp, gst returns the value of the config 
  file instead of the gw configured by dhcp.
*/
void KNetworkConfigParser::loadRoutingInfo( KRoutingInfo *routingInfo){
#ifndef Q_OS_FREEBSD
  QFile f( "/proc/net/route");
  if ( !f.open(IO_ReadOnly) )
  {
    KMessageBox::error(0,
                        i18n("Could not open file /proc/net/route."),
                        i18n("Could Not Open File"));
  }
  else
  {
    QTextStream t( &f );        // use a text stream
    QString s;
    while (!t.eof())
    {
      s = t.readLine();       // line of text excluding '\n'
      QString interface = s.section('\t',0,0);      
      QString destination = s.section('\t',1,1);
      QString gw = s.section('\t',2,2);
        
      if (destination == "00000000")
      {
        routingInfo->setGateway(hexIPv4ToDecIPv4(gw));
        routingInfo->setGatewayDevice(interface);
      }  
    }
  }
  f.close();
#endif
}
QString
KNetworkConfigParser::hexIPv4ToDecIPv4(const QString &hex)
{
  bool ok;
  QString dec = "";
  QString dec2 = "";
  QString temp = "";
  QString temp2 = "";

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
  temp = hex.mid(6,2);
  temp2 = temp2.setNum(temp.toInt(&ok,16));
  dec.append(temp2);
  dec.append('.');
  temp = hex.mid(4,2);
  temp2 = temp2.setNum(temp.toInt(&ok,16));  
  dec.append(temp2);
  dec.append('.');
  temp = hex.mid(2,2);
  temp2 = temp2.setNum(temp.toInt(&ok,16));  
  dec.append(temp2);
  dec.append('.');
  temp = hex.mid(0,2);
  temp2 = temp2.setNum(temp.toInt(&ok,16));
  dec.append(temp2);
#else
  temp = hex.mid(0,2);
  temp2 = temp2.setNum(temp.toInt(&ok,16));
  dec.append(temp2);
  dec.append('.');
  temp = hex.mid(2,2);
  temp2 = temp2.setNum(temp.toInt(&ok,16));  
  dec.append(temp2);
  dec.append('.');
  temp = hex.mid(4,2);
  temp2 = temp2.setNum(temp.toInt(&ok,16));  
  dec.append(temp2);
  dec.append('.');
  temp = hex.mid(6,2);
  temp2 = temp2.setNum(temp.toInt(&ok,16));
  dec.append(temp2);
#endif

  dec2 = dec;
  if (ok)
    return dec2;
  else
    return "";
}

void KNetworkConfigParser::saveNetworkInfo(KNetworkInfo *networkInfo)
{
  this->networkInfo = networkInfo;
  QPtrList<KNetworkInterface> devList = networkInfo->getDeviceList();
  QPtrList<KNetworkInfo> profileList = networkInfo->getProfilesList();
  dnsInfo = networkInfo->getDNSInfo();
  routingInfo = networkInfo->getRoutingInfo();  
  
  //Start xml file
  QDomDocument doc( "network []" );
  QDomProcessingInstruction instr = doc.createProcessingInstruction("xml","version=\"1.0\" ");
  doc.appendChild(instr);
  QDomElement root = doc.createElement( "network" );
  doc.appendChild( root );

  addRoutingInfoToXMLDoc(&doc, &root, routingInfo);
  addDNSInfoToXMLDoc(&doc, &root, dnsInfo);
  addNetworkInterfacesToXMLDoc(&doc, &root, devList);
  addNetworkProfilesToXMLDoc(&doc, &root, profileList);
  
  //If we don't add this comment to the end of the xml file, the gst process never exits!
  QDomComment endComment = doc.createComment(" GST: end of request ");
  doc.appendChild( endComment );

  QString xml = doc.toString();
  qDebug("--set XML:\n%s",xml.latin1());
  procSaveNetworkInfo = new QProcess(this);
  procSaveNetworkInfo->addArgument( locate("data",BACKEND_PATH) );

  if (!networkInfo->getPlatformName().isEmpty())
  {
    procSaveNetworkInfo->addArgument( "--platform" );
    procSaveNetworkInfo->addArgument( networkInfo->getPlatformName() );
  }
  procSaveNetworkInfo->addArgument( "--set" );

//  KDetectDistroDlg* dialog = new KDetectDistroDlg(0, 0, true,QDialog::WStyle_Customize|QDialog::WStyle_NormalBorder|QDialog::WStyle_Title|QDialog::WStyle_SysMenu); //made it semi-modal
  KDetectDistroDlg* dialog = new KDetectDistroDlg((QWidget*)parent(), 0, true);
  dialog->setCaption(i18n("Reloading Network"));
  dialog->text->setText(i18n("%1Please wait while saving the network settings...%2").arg("<center>").arg("</center>"));
  dialog->show();

  xmlOuput = "";

  connect( this, SIGNAL(readyLoadingNetworkInfo()), dialog, SLOT(close()) );
  connect(procSaveNetworkInfo,SIGNAL(readyReadStdout()),this,SLOT(readFromStdoutSaveNetworkInfo()));
  connect(procSaveNetworkInfo,SIGNAL(wroteToStdin()),this,SLOT(sendNetworkInfoSavedSignalSlot()));
  connect(procSaveNetworkInfo,SIGNAL(processExited()),this,SLOT(listIfacesSlot()));

  processRunning = true;
  connect( procSaveNetworkInfo, SIGNAL(processExited()), this, SLOT(processExitedSlot()) );
   
  if ( !procSaveNetworkInfo->start() )
  {
      KMessageBox::error(0,
                        i18n("Could not execute backend script for the network configuration detection. Something is wrong with your installation."),
                        i18n("Could Not Launch Network Configuration Backend Script"));
  }

  procSaveNetworkInfo->writeToStdin(xml);

  //wait around until the process has finished, otherwise it becomes a zombie
  while (processRunning) {  
    kapp->processEvents();
  }
}

void KNetworkConfigParser::processExitedSlot() {
  processRunning = false;
}

void KNetworkConfigParser::addNetworkProfilesToXMLDoc(QDomDocument *doc, QDomNode *root, QPtrList<KNetworkInfo> profileList)
{
  QPtrListIterator<KNetworkInfo> profileIt(profileList);  
  KNetworkInfo *profile;
  QDomElement tag = doc->createElement( "profiledb" );
  root->appendChild( tag );
  
  while ( (profile = profileIt.current()) != 0)
  {
    ++profileIt;
    QPtrList<KNetworkInterface> devList = profile->getDeviceList();
    KDNSInfo *dnsInfo = profile->getDNSInfo();
    KRoutingInfo *routingInfo = profile->getRoutingInfo();  

    QDomElement profileTag = doc->createElement( "profile" );
    tag.appendChild( profileTag );
    QDomElement innerTag = doc->createElement( "name" );
    profileTag.appendChild( innerTag );
    QDomText t = doc->createTextNode( profile->getProfileName() );
    innerTag.appendChild( t );

    addRoutingInfoToXMLDoc(doc, &profileTag, routingInfo);
    addDNSInfoToXMLDoc(doc, &profileTag, dnsInfo);
    addNetworkInterfacesToXMLDoc(doc, &profileTag, devList);
  }
}

void KNetworkConfigParser::addNetworkInterfacesToXMLDoc(QDomDocument *doc, QDomNode *root, QPtrList<KNetworkInterface> devList)
{
  KNetworkInterface *device;    
  QPtrListIterator<KNetworkInterface> devIt(devList);
  
    //Save in the configuration file the description of the interfaces as
  //the backend no longer handles this
  KSimpleConfig cfg("knetworkconfrc");
  cfg.setGroup("Interfaces");
  
  //Add the network interfaces list
  while ( (device = devIt.current()) != 0 )
  {
    ++devIt;
    // if protocol is not specified, then should not have entry in config
    if (device->getBootProto().isEmpty())
      continue;

    QDomElement tag = doc->createElement( "interface" );
    tag.setAttribute("type",device->getType());
    root->appendChild( tag );
    QDomElement configurationTag;
    configurationTag = doc->createElement( "configuration" );
    tag.appendChild( configurationTag );
    
    QDomElement innerTag;
    QDomText t;
    if ((device->getBootProto().lower() != "dhcp") && (device->getBootProto().lower() != "bootp"))
    {
      if (!device->getIpAddress().isEmpty())
      {
        innerTag = doc->createElement( "address" );
        configurationTag.appendChild( innerTag );
        t = doc->createTextNode( device->getIpAddress() );
        innerTag.appendChild( t );
      }
      if (!device->getGateway().isEmpty())
      {
        innerTag = doc->createElement( "gateway" );
        configurationTag.appendChild( innerTag );
        t = doc->createTextNode( device->getGateway() );
        innerTag.appendChild( t );
      }
      if (!device->getBroadcast().isEmpty())
      {
        innerTag = doc->createElement( "broadcast" );
        configurationTag.appendChild( innerTag );
        t = doc->createTextNode( device->getBroadcast() );
        innerTag.appendChild( t );
      }
      if (!device->getNetmask().isEmpty())
      {
        innerTag = doc->createElement( "netmask" );
        configurationTag.appendChild( innerTag );
        t = doc->createTextNode( device->getNetmask() );
        innerTag.appendChild( t );
      }
      if (!device->getNetwork().isEmpty())
      {
        innerTag = doc->createElement( "network" );
        configurationTag.appendChild( innerTag );
        t = doc->createTextNode( device->getNetwork() );
        innerTag.appendChild( t );
      }
    }
    innerTag = doc->createElement( "auto" );
    configurationTag.appendChild( innerTag );

    if (device->getOnBoot().lower() == "yes")
      t = doc->createTextNode( "1" );
    else
      t = doc->createTextNode( "0" );

    innerTag.appendChild( t );

    innerTag = doc->createElement( "bootproto" );
    configurationTag.appendChild( innerTag );
    if (device->getBootProto().lower() == "manual")
      t = doc->createTextNode( "none" );
    else
      t = doc->createTextNode( device->getBootProto().lower() );
    innerTag.appendChild( t );

    innerTag = doc->createElement( "file" );
    configurationTag.appendChild( innerTag );
    t = doc->createTextNode( device->getDeviceName() );
    innerTag.appendChild( t );
    
    innerTag = doc->createElement( "dev" );
    tag.appendChild( innerTag );
    t = doc->createTextNode( device->getDeviceName() );
    innerTag.appendChild( t );

    innerTag = doc->createElement( "enabled" );
    tag.appendChild( innerTag );
    if (device->isActive())
      t = doc->createTextNode( "1" );
    else  
      t = doc->createTextNode( "0" );
    innerTag.appendChild( t );

    innerTag = doc->createElement( "hwaddr" );
    tag.appendChild( innerTag );
    t = doc->createTextNode( device->getMacAddress() );
    innerTag.appendChild( t );        

    //Wireless settings
    
    if (device->getType() == WIRELESS_IFACE_TYPE)
    {
      KWirelessInterface *wifiDev = static_cast<KWirelessInterface*>(device);
      
      if (!wifiDev->getEssid().isEmpty())
      {
        innerTag = doc->createElement( "essid" );
        configurationTag.appendChild( innerTag );
        t = doc->createTextNode( wifiDev->getEssid() );
        innerTag.appendChild( t );
      }
      if (!wifiDev->getWepKey().isEmpty())
      {
        innerTag = doc->createElement( "key" );
        configurationTag.appendChild( innerTag );
        t = doc->createTextNode( wifiDev->getWepKey() );
        innerTag.appendChild( t );
      }  
      if (!wifiDev->getKeyType().isEmpty())
      {
        innerTag = doc->createElement( "key_type" );
        configurationTag.appendChild( innerTag );
        t = doc->createTextNode( wifiDev->getKeyType().lower() );
        innerTag.appendChild( t );
      }  
      
    }
    if (!device->getDescription().isEmpty() && device->getDeviceName()!= "lo")
        cfg.writeEntry(device->getDeviceName(),device->getDescription());
    
    cfg.sync();
  }
}

void KNetworkConfigParser::addDNSInfoToXMLDoc(QDomDocument *doc, QDomNode *root, KDNSInfo *dnsInfo)
{
  QStringList nameServerList = dnsInfo->getNameServers();
  QPtrList<KKnownHostInfo> knownHostsList = dnsInfo->getKnownHostsList();
  QPtrListIterator<KKnownHostInfo> knownHostsIt(knownHostsList);
  KKnownHostInfo *host;

  QDomElement tag = doc->createElement( "hostname" );
  root->appendChild( tag );
  QDomText t = doc->createTextNode( dnsInfo->getMachineName() );
  tag.appendChild( t );
  tag = doc->createElement( "domain" );
  root->appendChild( tag );
  t = doc->createTextNode( dnsInfo->getDomainName() );
  tag.appendChild( t );
  
  //Add the list of name servers
  for ( QStringList::Iterator it = nameServerList.begin(); it != nameServerList.end(); ++it ) {
    tag = doc->createElement( "nameserver" );
    root->appendChild( tag );
    t = doc->createTextNode( *it );
    tag.appendChild( t );
  }
  
  //Add the list of static hosts
  while ( (host = knownHostsIt.current()) != 0 )
  {
    ++knownHostsIt;
    tag = doc->createElement( "statichost" );
    root->appendChild( tag );
    QDomElement innerTag;
    if (!host->getIpAddress().isEmpty())
    {
      innerTag = doc->createElement( "ip" );
      tag.appendChild( innerTag );
      t = doc->createTextNode( host->getIpAddress() );
      innerTag.appendChild( t );
    } 
    QStringList aliases = host->getAliases(); 
      
    for ( QStringList::Iterator it = aliases.begin(); it != aliases.end(); ++it ) 
    {
      innerTag = doc->createElement( "alias" );        
      tag.appendChild( innerTag );
      t = doc->createTextNode( *it );
      innerTag.appendChild( t );
    }                
  }  
}

void KNetworkConfigParser::addRoutingInfoToXMLDoc(QDomDocument *doc, QDomNode *root, KRoutingInfo *routingInfo)
{
  QDomElement tag = doc->createElement( "gateway" );
  root->appendChild( tag );
  QDomText t = doc->createTextNode( routingInfo->getGateway() );
  tag.appendChild( t );

  tag = doc->createElement( "gatewaydev" );
  root->appendChild( tag );
  t = doc->createTextNode( routingInfo->getGatewayDevice() );
  tag.appendChild( t );
}
/** Loads the network info from a xml file generated by the gnome system tools
network backends that are included with this app. */
KNetworkInfo * KNetworkConfigParser::getNetworkInfo(){

    return networkInfo;
}

/** Parses all of the <interface>...</interface> entries in the xml configuration file. Returns a KWirelessInterface 
    object that contains all the info of the wireless interface. */
KWirelessInterface * KNetworkConfigParser::getWirelessInterfaceInfo(QDomElement interface, const QString &type){
  KWirelessInterface *wifiDevice = new KWirelessInterface();
  KNetworkInterface *tempDevice = NULL;
  
  //first we get the standard network information
  tempDevice = getInterfaceInfo(interface,type);
  //Then we copy the network interface info to the wireless object. I don't
  //know why it doesn't work with static_cast, after doing the cast, I can't
  //write to the KWirelessInterface memebers.
  memcpy(wifiDevice,tempDevice,sizeof(KNetworkInterface));
  //wifiDevice = static_cast<KWirelessInterface*>(tempDevice);
  
  QDomNode node = interface.firstChild();

  while ( !node.isNull() )
  {
    if ( node.isElement() )
    {
      QString nodeName =node.nodeName();
      
      //Parsing --get interfaces configuration
      if ( node.isElement() && node.nodeName() == "configuration" )
      {
        QDomNode configNode = node.firstChild();
        while ( !configNode.isNull() )
        {
          if ( configNode.isElement() )
          {
            QString configNodeName =configNode.nodeName();
            
            if ( configNodeName == "key" )
            {
              QDomElement e = configNode.toElement();
              wifiDevice->setWepKey(e.text());            
            }
            else if ( configNodeName == "essid" )
            {
              QDomElement e = configNode.toElement();
              wifiDevice->setEssid(e.text());
            }
            else if ( configNodeName == "key_type" )
            {
              QDomElement e = configNode.toElement();
              wifiDevice->setKeyType(e.text());
            }
            configNode = configNode.nextSibling();
          }
        }  
      }                  
    }
    node = node.nextSibling();
  }

  return wifiDevice;  
}

/** Parses all of the <interface>...</interface> entries in the xml configuration file. Returns a KNetworkInterface
  object with all the info of the interface.*/
KNetworkInterface * KNetworkConfigParser::getInterfaceInfo(QDomElement interface, const QString &type){
  QDomNode node = interface.firstChild();
  KNetworkInterface *tempDevice = new KNetworkInterface();
  //tempDevice->setDescription(i18n("Ethernet Network Device"));

  while ( !node.isNull() )
  {
    if ( node.isElement() )
    {
      QString nodeName =node.nodeName();
      
      //Parsing --get interfaces configuration
      if ( node.isElement() && node.nodeName() == "configuration" )
      {
        QDomNode configNode = node.firstChild();
        while ( !configNode.isNull() )
        {
          if ( configNode.isElement() )
          {
            QString configNodeName =configNode.nodeName();
            
            if ( configNodeName == "auto" )
            {
              QDomElement e = configNode.toElement();
              if (e.text() == "1")
                tempDevice->setOnBoot("yes");
              else
                tempDevice->setOnBoot("no");
            }
            else if ( configNodeName == "bootproto" )
            {
              QDomElement e = configNode.toElement();
              tempDevice->setBootProto(e.text());
            }
            if ( configNodeName == "address" || configNodeName == "addr")
            {
              QDomElement e = configNode.toElement();
              if (!e.text().isEmpty())
	              tempDevice->setIpAddress(e.text());
            }
            else if ( configNodeName == "gateway")
            {
              QDomElement e = configNode.toElement();
              if (!e.text().isEmpty())
            	  tempDevice->setGateway(e.text());
            }
            else if ( configNodeName == "netmask" || configNodeName == "mask")
            {
              QDomElement e = configNode.toElement();
              if (!e.text().isEmpty())
            	  tempDevice->setNetmask(e.text());
            }
            else if ( configNodeName == "network" )
            {
              QDomElement e = configNode.toElement();
              if (!e.text().isEmpty())
                tempDevice->setNetwork(e.text());
            }
            else if ( configNodeName == "broadcast" || configNodeName == "bdcast")
            {
              QDomElement e = configNode.toElement();
              if (!e.text().isEmpty())
	              tempDevice->setBroadcast(e.text());
            }
            configNode = configNode.nextSibling();
          }
        }  
      }
      
      //Parse -d list_ifaces interfaces configuration
      if ( nodeName == "addr")
      {
        QDomElement e = node.toElement();
        if (!e.text().isEmpty())
	       tempDevice->setIpAddress(e.text());
      }
      else if ( nodeName == "mask")
      {
        QDomElement e = node.toElement();
        if (!e.text().isEmpty())
          tempDevice->setNetmask(e.text());
      }
      else if ( nodeName == "bdcast")
      {
        QDomElement e = node.toElement();
        if (!e.text().isEmpty())
	        tempDevice->setBroadcast(e.text());
      }
     
       
      //These ones are common for both --get and -d list_ifaces
      else if ( nodeName == "dev" )
      {
        QDomElement e = node.toElement();
        tempDevice->setDeviceName(e.text());
      }
      //we had to add the OR because the xml sintax when listing the interfaces
      //is different than when loading the network info. ie.: enabled->active
      //address->addr, etc...
      else if ( nodeName == "enabled"  || nodeName == "active")
      {
        QDomElement e = node.toElement();
        if (e.text() == "1")        
          tempDevice->setActive(true);
        else
          tempDevice->setActive(false);  
      }
      else if ( nodeName == "hwaddr" )
      {
        QDomElement e = node.toElement();
        if (!e.text().isEmpty())
          tempDevice->setMacAddress(e.text());
      }
    }
    node = node.nextSibling();
  }
  if (type != QString::null)
    tempDevice->setType(type);
  
  QString description;
  KSimpleConfig cfg("knetworkconfrc");
  cfg.setGroup("Interfaces");
  description = cfg.readEntry(tempDevice->getDeviceName());
  if (!description.isEmpty())
    tempDevice->setDescription(description);
  else
  {
    if (tempDevice->getType() == ETHERNET_IFACE_TYPE)
      tempDevice->setDescription(i18n("Ethernet Network Device"));
    else if (tempDevice->getType() == WIRELESS_IFACE_TYPE)
      tempDevice->setDescription(i18n("Wireless Network Device"));
  }

  //Clear IP address settings if boot protocol is dhcp or bootp, in case that they are
  //setted in the config files.
  if ((tempDevice->getBootProto().lower() == "dhcp") || (tempDevice->getBootProto().lower() == "bootp"))
  {
    tempDevice->setIpAddress("");
    tempDevice->setNetmask("");
    tempDevice->setNetwork("");
    tempDevice->setBroadcast("");
  }

  return tempDevice;
}

/** Parses all of the <statichost>...</statichost> entries in the xml configuration file. */
KKnownHostInfo * KNetworkConfigParser::getStaticHostInfo(QDomElement host)
{
  QDomNode node = host.firstChild();
  KKnownHostInfo *tempHost = new KKnownHostInfo();

  while ( !node.isNull() )
  {
    if ( node.isElement() )
    {
      QString nodeName =node.nodeName();
      
      if ( nodeName == "ip")
      {
        QDomElement e = node.toElement();
        if (!e.text().isEmpty())
	       tempHost->setIpAddress(e.text());
      }
      else if ( nodeName == "alias")
      {
        QDomElement e = node.toElement();
        if (!e.text().isEmpty())
          tempHost->addAlias(e.text());
      }
    }  
    node = node.nextSibling();
  }

  return tempHost;
}

/** Reads the xml with the network info. */
void KNetworkConfigParser::readNetworkInfo()
{ 
  QPtrList<KNetworkInfo> profilesList;
  //deviceList.clear();
  //The gst backend puts a \n at the beginning of the xml output, so
  //we have to erase it first before we can parse it.
  xmlOuput = xmlOuput.section('\n',1);
  qDebug("--get XML:\n%s",xmlOuput.latin1());

  //If the platform where knetworkconf is running isn't supported, show the
  //user a dialog with all the supported platforms to choose.
  if (xmlErr.contains("platform_unsup::"))
  {    
      connect( this, SIGNAL(readyLoadingSupportedPlatforms()), this, SLOT(showSupportedPlatformsDialogSlot()) );
      loadSupportedPlatforms();
      emit errorDetectingPlatform();  
  }
  else  //parse the XML file
  {
    QString err;
    int x,y;
    QDomDocument doc( "network");
    if ( !doc.setContent( xmlOuput.utf8(),false,&err,&x,&y  ) )
    {
      KMessageBox::error(0,
                      i18n("Could not parse the XML output from the network configuration backend."),
                      i18n("Error Loading The Network Configuration"));
//        qDebug("error: %s %d,%d",err.latin1(),x,y);
    }
      
    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();
    
    //Load first the network information
    parseNetworkInfo(node, networkInfo, false);
    
    //Then, load the network profiles
    node = root.firstChild();
    while( !node.isNull() )
    {
      if ( node.isElement())
      {
        QString nodeName = node.nodeName();
        
        if ( nodeName == "profiledb" )
        {
          QDomNode profileNode = node.firstChild();
          
          while( !profileNode.isNull() )
          {            
            if ( profileNode.isElement())
            {
              QString profileName = profileNode.nodeName();
              
              if (profileNode.isElement() && profileName == "profile")
              {                
                KNetworkInfo * networkProfile = new KNetworkInfo();  
                QDomNode profileConfigurationNode = profileNode.firstChild();
                parseNetworkInfo(profileConfigurationNode, networkProfile, true);
                profilesList.append(networkProfile);              
              }
            }
            profileNode = profileNode.nextSibling();
          }
        }        
      }
      node = node.nextSibling();                    
    }
    networkInfo->setProfilesList(profilesList);    
  }
}

void KNetworkConfigParser::parseNetworkInfo(QDomNode node, KNetworkInfo *_networkInfo, bool isProfile){
  QPtrList<KNetworkInterface> deviceList;
  KDNSInfo *_dnsInfo = new KDNSInfo();
  KRoutingInfo *_routingInfo = new KRoutingInfo();
  QStringList serverList;
  QPtrList<KNetworkInterface> tempDeviceList;  
  QPtrList<KKnownHostInfo> knownHostsList;   
   
    while( !node.isNull() )
    {
      if ( node.isElement())
      {
        QString nodeName = node.nodeName();
        
        //Get the gatway
        if ( nodeName == "gateway" )
        {
          QDomElement gateway = node.toElement();
          _routingInfo->setGateway(gateway.text());
        }        
        else if ( nodeName == "gatewaydev" )
        {
          QDomElement gatewaydev = node.toElement();
          _routingInfo->setGatewayDevice(gatewaydev.text());
        }
        //The name of the profile, in the case it's a profile ;)
        else if ( nodeName == "name" && isProfile)
        {
          QDomElement profileName = node.toElement();
          _networkInfo->setProfileName(profileName.text());
        }
        
        //Get the network interfaces
        else if ( nodeName == "interface" )
        {
          QDomElement interface = node.toElement();          
        
          QString attr = interface.attribute("type").lower(); 
          //We have hardcoded to load only the supported interface types,
          //for now, ethernet, loopback and wireless, thus loopback interfaces
          //aren't shown. 
          if ( attr == ETHERNET_IFACE_TYPE || attr == LOOPBACK_IFACE_TYPE )
          {
            KNetworkInterface *tempDevice = NULL;
            tempDevice = getInterfaceInfo(interface,attr);
            deviceList.append(tempDevice);
          }          
          else if (attr == WIRELESS_IFACE_TYPE)
          {
            KWirelessInterface *wifiDevice = NULL;
            wifiDevice = getWirelessInterfaceInfo(interface,attr);
            deviceList.append(wifiDevice);
          }
        }
        
        //Get domain and host names
        else if ( nodeName == "hostname" )
        {
          QDomElement hostname = node.toElement();
          _dnsInfo->setMachineName(hostname.text());
        }
        else if ( nodeName == "domain" )
        {
          QDomElement domainname = node.toElement();
          _dnsInfo->setDomainName(domainname.text());
        }        
        
        //Get the nameServers
        else if ( nodeName == "nameserver" )
        {
          QDomElement nameserver = node.toElement();
          serverList.append(nameserver.text());
        }        
        
        //Get the static hosts
        else if ( nodeName == "statichost" )
        {
          QDomElement host = node.toElement();       
          KKnownHostInfo *tempHost = getStaticHostInfo(host);
          knownHostsList.append(tempHost); 
        }
      }
      node = node.nextSibling();
    }
    _dnsInfo->setNameServers(serverList);
    _dnsInfo->setKnownHostsList(knownHostsList);
          
    _networkInfo->setDeviceList(deviceList);
    loadRoutingInfo(_routingInfo);
    _networkInfo->setRoutingInfo(_routingInfo);
    _networkInfo->setDNSInfo(_dnsInfo);
    
    //if we are loading a profile, don't try to find the IP address of DHCP 
    //interfaces and their states    
    if (!isProfile)
    {
      listIfaces(_networkInfo->getPlatformName());
    }
    //return _networkInfo;
}
void KNetworkConfigParser::readFromStdoutReloadScript(){
  QString s = procReloadNetwork->readStdout();
  reloadScriptOutput.append(s);
}
/** emits a signal when the network changes have benn saved. */
void KNetworkConfigParser::sendNetworkInfoSavedSignalSlot(){
  procSaveNetworkInfo->closeStdin();
}

/** Concatenates into a QString the xml output of the network backend. */
void KNetworkConfigParser::concatXMLOutputSlot(){
  xmlOuput.append(procDetect->readStdout());
}

void KNetworkConfigParser::readXMLErrSlot(){
  xmlErr.append(procDetect->readStderr());
}
/** lists all platforms supported by GST. */
void KNetworkConfigParser::loadSupportedPlatforms(){
	procDetect = new QProcess(this);
  procDetect->addArgument( locate("data",BACKEND_PATH) );
  procDetect->addArgument( "-d" );
  procDetect->addArgument( "platforms" );
  connect( procDetect, SIGNAL(processExited()), this, SLOT(readSupportedPlatformsSlot()) );
  xmlOuput = "";
  connect( procDetect, SIGNAL(readyReadStdout()),this, SLOT(concatXMLOutputSlot()));
//  connect( procDetect, SIGNAL(readyReadStderr()),this, SLOT(readXMLErrSlot()));

 	if ( !procDetect->start() )
  {
// error handling
      KMessageBox::error(0,
                        i18n("Could not execute backend script for the network configuration detection. Something is wrong with your installation."),
                        i18n("Could Not Launch Network Configuration Backend Script"));
	}

}
/** Parses the xml ouput generated by GST that has all the supported platforms. */
void KNetworkConfigParser::readSupportedPlatformsSlot(){
  //The gst backend puts a \n at the beginning of the xml output, so
  //we have to erase it first before we parse it.
  xmlOuput = xmlOuput.section('\n',1);
  QDomDocument doc( "platforms" );
  if ( !doc.setContent( xmlOuput.utf8() ) )
  {
    KMessageBox::error(0,
                         i18n("Could not parse the list of supported platforms from the network configuration backend."),
                         i18n("Error Obtaining Supported Platforms List"));
  }
  QDomElement root = doc.documentElement();
  QDomNode node = root.firstChild();
  QString s;
  while( !node.isNull() )
  {
    if ( node.isElement() && node.nodeName() == "platform" )
    {
      QDomElement platform = node.toElement();
      s = getPlatformInfo(platform);
    }
    supportedPlatformsList << s;
    node = node.nextSibling();
  }
  emit readyLoadingSupportedPlatforms();
}
/** Returns the info of a platform in the form of 'key:value' . */
QString KNetworkConfigParser::getPlatformInfo(QDomElement platform){
  QDomNode node = platform.firstChild();
  QString s;
  while ( !node.isNull() )
  {
    if ( node.isElement() )
    {
      if ( node.nodeName() == "key" )
      {
        QDomElement e = node.toElement();
        s += e.text();
        s += ":";
      }
      else if ( node.nodeName() == "name" )
      {
        QDomElement e = node.toElement();
        s += e.text();
      }

    }
    node = node.nextSibling();
  }

  return s;
}
/** Shows the dialog with all the supported platforms by GST. */
void KNetworkConfigParser::showSupportedPlatformsDialogSlot(){
  KSelectDistroDlg* dialog = new KSelectDistroDlg(0, 0);

  for ( QStringList::Iterator it = supportedPlatformsList.begin(); it != supportedPlatformsList.end(); ++it )
  {
    QString key = (*it);
    key = key.section(":",0,0);
    QString name = (*it);
    name = name.section(":",1,1);
    if (key.contains("debian"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/debian.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("mandriva"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/mandriva.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("conectiva"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/conectiva.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }    
    else if (key.contains("pld"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/pld.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("redhat"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/redhat.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("suse"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/suse.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("turbolinux"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/turbolinux.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("fedora"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/fedora.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("openna"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/openna.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("slackware"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/slackware.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("freebsd"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/freebsd.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("gentoo"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/gentoo.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("blackpanther"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/blackpanther.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }
    else if (key.contains("rpath"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/rpath.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }    
    else if (key.contains("vine"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/vine.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }    
    else if (key.contains("ubuntu"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/kubuntu.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }    
    else if (key.contains("yoper"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/yoper.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }    
    else if (key.contains("ark"))
    {
      QPixmap distroImg(locate("data","knetworkconf/pixmaps/ark.png"));
      dialog->klbDistroList->insertItem(distroImg, name);
    }    
  } 
  
  if (!dialog->exec())
    emit setReadOnly(true);
  else
  {
    int i = 0;
    QStringList::Iterator it = supportedPlatformsList.begin();
    while (i < dialog->klbDistroList->currentItem())
    {
      i++;
      ++it;
    }
    QString key = (*it);
    key = key.section(":",0,0); 
    //clean variables and run again the detection script but now don't
    //auto-detect.
    xmlErr = "";
    xmlOuput = "";
    networkInfo->setPlatformName(key);
    runDetectionScript(key);
    if (dialog->cbAskAgain->isChecked())
      saveAskAgain(key,!dialog->cbAskAgain->isChecked());
  }  
}

void KNetworkConfigParser::saveAskAgain(const QString &platform, bool askAgain)
{
  KSimpleConfig cfg("knetworkconfrc");
  cfg.setGroup("General");
  cfg.writeEntry("detectedPlatform",platform);
  cfg.writeEntry("askAgainPlatform",askAgain);
  cfg.sync();
}

bool KNetworkConfigParser::readAskAgain(QString &platform)
{
  KSimpleConfig cfg("knetworkconfrc");
  cfg.setGroup("General");
  platform = cfg.readEntry("detectedPlatform");
  return cfg.readBoolEntry("askAgainPlatform");
}

/** No descriptions */
void KNetworkConfigParser::readFromStdoutSaveNetworkInfo(){
  xmlOuput.append(procSaveNetworkInfo->readStdout());
}
/** Calls runDetectionScript(). */
void KNetworkConfigParser::listIfacesSlot(){
  listIfaces(networkInfo->getPlatformName());
}
/**Returns the info of the network device 'device or NULL if not found.'*/
KNetworkInterface * KNetworkConfigParser::getDeviceInfo(QString device){
  QPtrList<KNetworkInterface> deviceList = networkInfo->getDeviceList();
  QPtrListIterator<KNetworkInterface>  i(deviceList);
  KNetworkInterface *temp;
  while ((temp = i.current()) != 0)
  {
    if (temp->getDeviceName() == device)
    {
      return temp;
    }
    ++i;
  }
  return NULL;
}

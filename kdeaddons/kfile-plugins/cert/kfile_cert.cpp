/***************************************************************************
 *   Copyright (C) 2004 by Leonid Zeitlin                                  *
 *   lz@europe.com                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include <config.h>
#include "kfile_cert.h"

#include <kgenericfactory.h>
#include <ksslcertificate.h>
#include <ksslx509map.h>
#include <kopenssl.h>
//#include <kstandarddirs.h>
//#include <kdebug.h>
//#include <kio/global.h>

#include <qdatetime.h>
#include <qfile.h>
#include <qcstring.h>
//#include <qfileinfo.h>
//#include <qdir.h>

typedef KGenericFactory<CertPlugin> CertFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_cert, CertFactory("kfile-cert"))

CertPlugin::CertPlugin(QObject *parent, const char *name, const QStringList &args)
  : KFilePlugin(parent, name, args)
{
    //add the mimetype here - example:
    //KFileMimeTypeInfo* info = addMimeTypeInfo( "text/html" );
    KFileMimeTypeInfo* info = addMimeTypeInfo( "application/x-x509-ca-cert" );

    // our new group
    KFileMimeTypeInfo::GroupInfo* group = 0L;
    KFileMimeTypeInfo::ItemInfo* item;
    
    group = addGroupInfo(info, "certInfo", i18n("Certificate Information"));
    item = addItemInfo(group, "ValidFrom", i18n("Valid From"), QVariant::DateTime);
    item = addItemInfo(group, "ValidUntil", i18n("Valid Until"), QVariant::DateTime);
    item = addItemInfo(group, "State", i18n("State"), QVariant::String);
    item = addItemInfo(group, "SerialNo", i18n("Serial Number"), QVariant::String);
    
    group = addGroupInfo(info, "certSubjectInfo", i18n("Subject"));
    item = addItemInfo(group, "O", i18n("Organization"), QVariant::String);
    item = addItemInfo(group, "OU", i18n("Organizational Unit"), QVariant::String);
    item = addItemInfo(group, "L", i18n("Locality"), QVariant::String);
    item = addItemInfo(group, "C", i18n("Country"), QVariant::String);
    item = addItemInfo(group, "CN", i18n("Common Name"), QVariant::String);
    item = addItemInfo(group, "E", i18n("Email"), QVariant::String);
    
    group = addGroupInfo(info, "certIssuerInfo", i18n("Issuer"));
    item = addItemInfo(group, "O", i18n("Organization"), QVariant::String);
    item = addItemInfo(group, "OU", i18n("Organizational Unit"), QVariant::String);
    item = addItemInfo(group, "L", i18n("Locality"), QVariant::String);
    item = addItemInfo(group, "C", i18n("Country"), QVariant::String);
    item = addItemInfo(group, "CN", i18n("Common Name"), QVariant::String);
    item = addItemInfo(group, "E", i18n("Email"), QVariant::String);
    
    //setUnit(item, KFileMimeTypeInfo::KiloBytes);

    // strings are possible, too:
    //addItemInfo(group, "Text", i18n("Document Type"), QVariant::String);
}

void CertPlugin::appendDNItems(KFileMetaInfoGroup &group, const QString &DN)
{
  KSSLX509Map map(DN);
  QString value;
  //QString dbg;
  QStringList keys = group.supportedKeys();
  QStringList::ConstIterator end = keys.end();
  for (QStringList::ConstIterator it = keys.begin(); it != end; ++it) {
    value = map.getValue(*it);
    //dbg += *it + " = " + value + "; ";
    if (!value.isNull()) appendItem(group, *it, value);    
    //appendItem(group, "CN", dbg);
  }
}

static KSSLCertificate *readCertFromFile(const QString &path)
{
  KSSLCertificate *ret = NULL;
  
  QFile file(path);
  if (!file.open(IO_ReadOnly)) return NULL;
  QByteArray file_data = file.readAll();
  file.close();
  
  QCString file_string = QCString(file_data.data(), file_data.size());
  // try as is:
  ret = KSSLCertificate::fromString(file_string);
  if (ret) return ret;
  // didn't work. Let's see if begin/end lines are there:
  KOSSL::self()->ERR_clear_error();
  const char *begin_line = "-----BEGIN CERTIFICATE-----\n";
  const char *end_line = "\n-----END CERTIFICATE-----";
  int begin_pos = file_string.find(begin_line);
  if (begin_pos >= 0) {
    begin_pos += strlen(begin_line);
    int end_pos = file_string.find(end_line, begin_pos);
    if (end_pos >= 0) {
      // read the data between begin and end lines
      QCString body = file_string.mid(begin_pos, end_pos - begin_pos);
      ret = KSSLCertificate::fromString(body);
      return ret; // even if it's NULL, we can't help it
    }
  }
  // still didn't work. Assume the file was in DER (binary) encoding
  unsigned char *p = (unsigned char*) file_data.data();
  KOSSL::self()->ERR_clear_error();
  X509 *x = KOSSL::self()->d2i_X509(NULL, &p, file_data.size());
  if (x) {
    ret = KSSLCertificate::fromX509(x);
    KOSSL::self()->X509_free(x);
    return ret;
  }  
  else return NULL;
}

bool CertPlugin::readInfo(KFileMetaInfo& info, uint /*what*/)
{
  KSSLCertificate *cert = readCertFromFile(info.path());
  if (cert) {
    KFileMetaInfoGroup group = appendGroup(info, "certInfo");
    appendItem(group, "ValidFrom", cert->getQDTNotBefore());
    appendItem(group, "ValidUntil", cert->getQDTNotAfter());
    appendItem(group, "State", KSSLCertificate::verifyText(cert->validate()));
    appendItem(group, "SerialNo", cert->getSerialNumber());
    
    group = appendGroup(info, "certSubjectInfo");
    appendDNItems(group, cert->getSubject());
    
    group = appendGroup(info, "certIssuerInfo");
    appendDNItems(group, cert->getIssuer());
  
    delete cert;
    return true;
  }  
  else {
    KOSSL::self()->ERR_clear_error(); // don't leave errors behind
    return false;
  }
}

#include "kfile_cert.moc"

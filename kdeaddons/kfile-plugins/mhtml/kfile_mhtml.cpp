/***************************************************************************
 *   Copyright (C) 2005 by Spiros Georgaras                                *
 *   sngeorgaras@otenet.gr                                                 *
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
#include "kfile_mhtml.h"

#include <kgenericfactory.h>
#include <kmdcodec.h>
#include <qstring.h>
#include <qcstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>

typedef KGenericFactory<mhtmlPlugin> mhtmlFactory;

K_EXPORT_COMPONENT_FACTORY(kfile_mhtml, mhtmlFactory( "kfile_mhtml" ))

mhtmlPlugin::mhtmlPlugin(QObject *parent, const char *name,
                       const QStringList &args)
    : KFilePlugin(parent, name, args)
{
	KFileMimeTypeInfo* info = addMimeTypeInfo( "application/x-mimearchive" );
	KFileMimeTypeInfo::GroupInfo* group = 0L;
	group = addGroupInfo(info, "mhtmlInfo", i18n("Document Information"));
	KFileMimeTypeInfo::ItemInfo* item;
	item = addItemInfo(group, "Subject", i18n("Subject"), QVariant::String);
	item = addItemInfo(group, "Sender", i18n("Sender"), QVariant::String);
	item = addItemInfo(group, "Recipient", i18n("Recipient"), QVariant::String);
	item = addItemInfo(group, "CopyTo", i18n("CC"), QVariant::String);
	item = addItemInfo(group, "BlindCopyTo", i18n("BCC"), QVariant::String);
	item = addItemInfo(group, "Date", i18n("Date"), QVariant::String);
}

bool mhtmlPlugin::readInfo( KFileMetaInfo& info, uint /*what*/)
{
	QString mSender;
	QString mRecipient;
	QString mCopyTo;
	QString mBlindCopyTo;
	QString mSubject;
	QString mDate;
	bool canUnfold;
	if ( info.path().isEmpty() ) // remote file
		return false;

	QFile f(info.path());
	if (!f.open(IO_ReadOnly)) return false;
	QTextStream stream(&f);
	QString l=stream.readLine();
	int nFieldsFound = 0;
	while(!l.isEmpty()){
		if(l.startsWith("From: ")) {
			mSender=l.mid(6);
			nFieldsFound |= 1;
			canUnfold=TRUE;
		} else if(l.startsWith("To: ")) {
			mRecipient=l.mid(4);
			nFieldsFound |= 2;
			canUnfold=TRUE;
		} else if(l.startsWith("Subject: ")) {
			mSubject=l.mid(9);
			nFieldsFound |= 4;
			canUnfold=TRUE;
		} else if(l.startsWith("Cc: ")) {
			mCopyTo=l.mid(4);
			nFieldsFound |= 8;
			canUnfold=TRUE;
		} else if(l.startsWith("Bcc: ")) {
			mBlindCopyTo=l.mid(5);
			nFieldsFound |= 16;
			canUnfold=TRUE;
		} else if(l.startsWith("Date: ")) {
			mDate=l.mid(6);
			nFieldsFound |= 32;
			canUnfold=FALSE;
		}else if(l.startsWith(" ") || l.startsWith("\t")){
			// unfold field
			if(canUnfold){
				QString tmp=l.stripWhiteSpace();
				if(nFieldsFound & 16) mBlindCopyTo=mBlindCopyTo+" "+tmp;
				else if(nFieldsFound & 8) mCopyTo=mCopyTo+" "+tmp;
				else if(nFieldsFound & 4) mSubject=mSubject+" "+tmp;
				else if(nFieldsFound & 2) mRecipient=mRecipient+" "+tmp;
				else if(nFieldsFound & 1) mSender=mSender+" "+tmp;
			}
		}else canUnfold=FALSE;
		// break out of the loop once the six fields have been found
		if ( nFieldsFound == 32+16+8+4+2+1 )
			break;
		l=stream.readLine();
	}
	f.close();
	KFileMetaInfoGroup group = appendGroup(info, "mhtmlInfo");
	appendItem(group, "Subject", decodeRFC2047Phrase(mSubject,FALSE));
	appendItem(group, "Sender", decodeRFC2047Phrase(mSender));
	appendItem(group, "Recipient", decodeRFC2047Phrase(mRecipient));
	appendItem(group, "CopyTo", decodeRFC2047Phrase(mCopyTo));
	appendItem(group, "BlindCopyTo", decodeRFC2047Phrase(mBlindCopyTo));
	appendItem(group, "Date", mDate);
	return true;
}

QString mhtmlPlugin::decodeRFC2047Phrase(const QString &msg, bool removeLessGreater){
	int st=msg.find("=?");
	int en=-1;
	QString msgCopy=msg;
	QString decodedText=msgCopy.left(st);
	QString encodedText=msgCopy.mid(st);
	st=encodedText.find("=?");
	while(st!=-1){
		en=encodedText.find("?=");
		while(encodedText.mid(en+2,1)!=" " && en+2<(int)encodedText.length()) en=encodedText.find("?=",en+1);
		if(en==-1) break;
		decodedText+=encodedText.left(st);
		QString tmp=encodedText.mid(st,en-st+2);
		encodedText=encodedText.mid(en+2);
		decodedText+=decodeRFC2047String(tmp);
		st=encodedText.find("=?",st+1);
	}
	decodedText += encodedText;
	// remove unwanted '<' and '>'
	if(removeLessGreater){
		if(decodedText.stripWhiteSpace().startsWith("<") && decodedText.stripWhiteSpace().endsWith(">")){
			QString tmp=decodedText.stripWhiteSpace();
			tmp=tmp.mid(1,tmp.length()-2);
			decodedText=tmp;
		}else{
			QString dec=decodedText;
			QString tmp;
			
			st=decodedText.find("<");
			while(st!=-1){
				st=dec.find("<",st);
				if(st==0 || (st!=0 && (dec.mid(st-2,2)==", "))){
					en=dec.find(">",st);
					if(en==-1 && dec.find(",",st)<en){
						st++;
						continue;
					}
					dec=dec.left(st)+dec.mid(st+1,en-st-1)+dec.mid(en+1);
				}else if(st!=-1) st++;
			}
			decodedText=dec;
		}
	}
	return decodedText;
}

QString mhtmlPlugin::decodeRFC2047String(const QString &msg){
	QString charset;
	QString encoding;
	QString notEncodedText;
	QString encodedText;
	QString decodedText;
	int encEnd=0;
	if(msg.startsWith("=?") && (encEnd=msg.findRev("?="))!=-1){
		notEncodedText=msg.mid(encEnd+2);
		encodedText=msg.left(encEnd);
		encodedText=encodedText.mid(2,encodedText.length()-2); 
		int questionMark=encodedText.find('?');
		if(questionMark==-1) return msg;
		charset=encodedText.left(questionMark).lower();
		encoding=encodedText.mid(questionMark+1,1).lower();
		if(encoding!="b" && encoding!="q") return msg;
		encodedText=encodedText.mid(questionMark+3);
		if(charset.find(" ")!=-1 && encodedText.find(" ")!=-1) return msg;
		QCString tmpIn;
		QCString tmpOut;
		tmpIn = encodedText.local8Bit();
		if(encoding=="q")tmpOut=KCodecs::quotedPrintableDecode(tmpIn);
		else tmpOut=KCodecs::base64Decode(tmpIn);
		if(charset!="us-ascii"){
			QTextCodec *codec = QTextCodec::codecForName(charset.local8Bit());
			if(!codec) return msg;
			decodedText=codec->toUnicode(tmpOut);
 			decodedText=decodedText.replace("_"," ");
		}else decodedText=tmpOut.replace("_"," ");
		return decodedText + notEncodedText;
	}else return msg;
}
#include "kfile_mhtml.moc"


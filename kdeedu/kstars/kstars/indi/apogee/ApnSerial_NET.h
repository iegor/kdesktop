/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// ApnSerial_NET.h: interface for the CApnSerial_NET class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APNSERIAL_NET_H__F31A372D_2B82_4998_B74C_FFAD8E3EEE86__INCLUDED_)
#define AFX_APNSERIAL_NET_H__F31A372D_2B82_4998_B74C_FFAD8E3EEE86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApnSerial.h"


class CApnSerial_NET : public CApnSerial  
{
public:
	CApnSerial_NET();
	virtual ~CApnSerial_NET();

	bool InitPort( unsigned long	CamIdA,
				   unsigned short	CamIdB,
				   unsigned short	SerialId );

	bool ClosePort();

	bool GetBaudRate( unsigned long *BaudRate );
	bool SetBaudRate( unsigned long BaudRate );

	bool GetFlowControl( Apn_SerialFlowControl *FlowControl );
	bool SetFlowControl( Apn_SerialFlowControl FlowControl );

	bool GetParity( Apn_SerialParity *Parity );
	bool SetParity( Apn_SerialParity Parity );

	bool Read( char			  *ReadBuffer,
			   unsigned short *ReadCount );

	bool Write( char		   *WriteBuffer,
				unsigned short WriteCount );

};

#endif // !defined(AFX_APNSERIAL_NET_H__F31A372D_2B82_4998_B74C_FFAD8E3EEE86__INCLUDED_)

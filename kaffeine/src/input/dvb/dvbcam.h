/*
 * dvbcam.h
 *
 * Copyright (C) 2008 Christophe Thommeret <hftom@free.fr>
 * Copyright (C) 2006 Christoph Pfister <christophpfister@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef DVBCAM_H
#define DVBCAM_H

#include <libdvbapi/dvbca.h>
#include <libdvbapi/dvbdemux.h>
#include <libdvben50221/en50221_app_ai.h>
#include <libdvben50221/en50221_app_ca.h>
#include <libdvben50221/en50221_app_mmi.h>
#include <libdvben50221/en50221_app_rm.h>
#include <libdvben50221/en50221_app_tags.h>
#include <libdvben50221/en50221_session.h>
#include <libdvben50221/en50221_stdcam.h>
#include <libucsi/mpeg/section.h>

#include <qthread.h>
#include <qmutex.h>
#include <qptrlist.h>

#include "channeldesc.h"
#include "camdialog.h"
#include "cammenudialog.h"

#define MMI_STATE_CLOSED 0
#define MMI_STATE_OPEN 1
#define MMI_STATE_ENQ 2
#define MMI_STATE_MENU 3

#define MMI_NO_MENU 0
#define MMI_MENU 1



class CamService : protected QThread
{
public:
	enum PmtState{ NotReady=0, Ready, Added, Remove, Destroy };
	CamService( int adapter, int demux_device, ChannelDesc *chan, int maxService );
	~CamService();
	void setState( PmtState st );
	int getState();
	const ChannelDesc& getChannel() { return channel; }
	void restart();

	unsigned char caPmt[4096];
	int caPmtSize;

protected:
	void run();

private:
	int createSectionFilter( uint16_t pid, uint8_t table_id );
	int processPat( int demux_fd );
	int processPmt( int demux_fd );
	bool setCaPmt( int list_management, int cmd_id );
	void stop();

	int Adapter;
	int DemuxDevice;
	ChannelDesc channel;
	unsigned char pmtBuffer[4096];
	struct mpeg_pmt_section *parsedPmt;
	PmtState state;
	bool isRunning;
	QMutex mutex;
	int CamMaxService;
};



class DvbCam;

class StandardCam
{
public:
	StandardCam( en50221_stdcam *sc, DvbCam *dc ) {
		stdcam=sc;
		dvbcam=dc;
		mmi_state=MMI_STATE_CLOSED;
		menuType=MMI_NO_MENU;
	}

	en50221_stdcam *stdcam;
	DvbCam *dvbcam;
	int mmi_state;
	int mmi_enq_blind;
	int mmi_enq_length;

	int menuType;
	QStringList menuList;

	QMutex mutex;
};



class MCamMenuDialog : public CamMenuDialog
{
	Q_OBJECT
public:
	MCamMenuDialog( StandardCam *sc );
private:
	StandardCam *stdcam;
	QTimer readTimer;
private slots:
	void setMenu();
	void validateClicked();
signals:
	void enteredResponse( QString );
};



class DvbCam : public QObject, public QThread
{
	Q_OBJECT
public:
	DvbCam(int adapter, int ca_device, int demux_device, int ci_type, int maxService);
	~DvbCam();
	void startService( ChannelDesc *chan );
	void stopService( ChannelDesc *chan );
	bool canPlay( ChannelDesc *chan );
	static int probe( int adapter, int ca_device );

	void setAppType( QString s ) { appType=s; }
	void setAppManu( QString s ) { appManu=s; }
	void setManuCode( QString s ) { manuCode=s; }
	void setMenuString( QString s ) { menuString=s; }
	QString getAppType() { return appType; }
	QString getAppManu() { return appManu; }
	QString getManuCode() { return manuCode; }
	QString getMenuString() { return menuString; }
	int getCamMaxService() { return CamMaxService; }

	int showCamDialog();

protected:
	void run();
	void timerEvent( QTimerEvent *e );

private slots:
	void mmiResponse( QString s );
	void showMMI();
	void closeMMI();
	void enterMenu();

private:
	bool init();
	bool sendPmt( unsigned char *pmt_buffer, int size );
	void resendPmts();

	static int infoCallback(void *arg, uint8_t slot_id, uint16_t session_number, uint32_t ca_id_count, uint16_t *ca_ids);
	static int aiCallback(void *arg, uint8_t slot_id, uint16_t session_number, uint8_t application_type,
		uint16_t application_manufacturer, uint16_t manufacturer_code, uint8_t menu_string_length, uint8_t *menu_string);
	static int mmi_close_callback(void *arg, uint8_t slot_id, uint16_t session_number, uint8_t cmd_id, uint8_t delay);
	static int mmi_display_control_callback(void *arg, uint8_t slot_id, uint16_t session_number, uint8_t cmd_id, uint8_t mmi_mode);
	static int mmi_enq_callback(void *arg, uint8_t slot_id, uint16_t session_number, uint8_t blind_answer, uint8_t expected_answer_length,
		uint8_t *text, uint32_t text_size);
	static int mmi_menu_callback(void *arg, uint8_t slot_id, uint16_t session_number, struct en50221_app_mmi_text *title,
		struct en50221_app_mmi_text *sub_title, struct en50221_app_mmi_text *bottom, uint32_t item_count,
		struct en50221_app_mmi_text *items, uint32_t item_raw_length, uint8_t *items_raw);

	int Adapter;
	int CaDevice;
	int DemuxDevice;
	int ciType;
	int CamMaxService;

	bool isRunning;

	QPtrList<CamService> sidList;
	QMutex sidMutex;

	QString appType, appManu, manuCode, menuString;

	StandardCam *stdcam;
	en50221_session_layer *SessionLayer;
	en50221_transport_layer *TransportLayer;

	MCamMenuDialog *menuDialog;
};

#endif /* DVBCAM_H */

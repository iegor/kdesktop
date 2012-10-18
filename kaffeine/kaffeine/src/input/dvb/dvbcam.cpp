/*
 * dvbcam.cpp
 *
 * Copyright (C) 2008 Christophe Thommeret <hftom@free.fr>
 * Copyright (C) 2006 Christoph Pfister <christophpfister@gmail.com>
 *
 * code based on ca_zap (LGPL)
 * Copyright (C) 2004, 2005 Manu Abraham (manu@kromtek.com)
 * Copyright (C) 2006 Andrew de Quincey (adq_dvb@lidskialf.net)
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

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <linux/dvb/ca.h>

#include <qapplication.h>
#include <qthread.h>
#include <qstring.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qtextbrowser.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "dvbcam.h"

#define TIMER_EVENT_MMI_OPEN 500
#define TIMER_EVENT_MMI_CLOSE 501
#define TIMER_EVENT_CAM_READY 502



DvbCam::DvbCam(int adapter, int ca_device, int demux_device, int ci_type, int maxService)
{
	Adapter     = adapter;
	CaDevice    = ca_device;
	DemuxDevice = demux_device;
	ciType = ci_type;
	CamMaxService = maxService;
	fprintf(stderr, "DvbCam: CamMaxService = %d\n", CamMaxService);

	stdcam = NULL;
	menuDialog = NULL;

	sidList.setAutoDelete( true );

	isRunning = true;
	start();
}

DvbCam::~DvbCam()
{
	isRunning = false;
	wait();
	sidMutex.lock();
	sidList.clear();
	sidMutex.unlock();
}

int DvbCam::probe( int adapter, int ca_device )
{
	int fdCa;
	ca_caps_t cap;
	ca_slot_info_t info;
	QString s = QString("/dev/dvb/adapter%1/ca%2").arg( adapter ).arg( ca_device );

	if ( (fdCa = open( s.ascii(), O_RDWR )) < 0) {
		fprintf(stderr, "DvbCam::probe(): %s:", s.ascii());
		perror(" ");
		return -1;
	}

	if ( ioctl(fdCa, CA_GET_CAP, &cap) == 0) {
		if ( cap.slot_num>0 ) {
			info.num = 0;
			if ( ioctl(fdCa, CA_GET_SLOT_INFO, &info) == 0) {
				if ( (info.type & CA_CI_LINK)!=0) {
					fprintf(stderr, "DvbCam::probe(): LLCI slot found on %s\n", s.ascii());
					if ( (info.flags & CA_CI_MODULE_PRESENT)!=0 ) {
						fprintf(stderr, "DvbCam::probe(): CA module present on %s\n", s.ascii());
						close (fdCa);
						return CA_CI_LINK;
					}
					else
						fprintf(stderr, "DvbCam::probe(): no CA module present on %s\n", s.ascii());
				}
				else if ( (info.type & CA_CI)!=0 ) {
					fprintf(stderr, "DvbCam::probe(): HLCI slot found on %s\n", s.ascii());
					if ( (info.flags & CA_CI_MODULE_PRESENT)!=0 ) {
						fprintf(stderr, "DvbCam::probe(): CA module present on %s\n", s.ascii());
						close (fdCa);
						return CA_CI;
					}
					else
						fprintf(stderr, "DvbCam::probe(): no CA module present on %s\n", s.ascii());
				}
				else
					fprintf(stderr, "DvbCam::probe(): unsupported CI interface on %s\n", s.ascii());
			}
			else
				fprintf(stderr, "DvbCam::Probe(): ioctl failed on %s\n", s.ascii());
		}
		else
			fprintf(stderr, "DvbCam::Probe(): no CI slot found on %s\n", s.ascii());
	}
	else
		fprintf(stderr, "DvbCam::Probe(): ioctl failed on %s\n", s.ascii());

	close (fdCa);
	return -1;
}

bool DvbCam::canPlay( ChannelDesc *chan )
{
	int i, n=0;

	QMutexLocker locker( &sidMutex );
	for ( i=0; i<sidList.count(); ++i ) {
		if ( sidList.at(i)->getState()<CamService::Remove )
			n++;
		if ( sidList.at(i)->getChannel().name==chan->name )
			return true;
	}
	return ( n<CamMaxService );
}

void DvbCam::startService( ChannelDesc *chan )
{
	int i;
	QMutexLocker locker( &sidMutex );
	for ( i=0; i<sidList.count(); ++i ) {
		if ( sidList.at(i)->getChannel().name==chan->name ) {
			sidList.at(i)->restart();
			return;
		}
	}
	sidList.append( new CamService( Adapter, DemuxDevice, chan, CamMaxService ) );
}

void DvbCam::stopService( ChannelDesc *chan )
{
	int i;
	QMutexLocker locker( &sidMutex );
	for ( i=0; i<sidList.count(); ++i ) {
		if ( sidList.at(i)->getChannel().name==chan->name ) {
			sidList.at(i)->setState( CamService::Remove );
			return;
		}
	}
}

void DvbCam::resendPmts()
{
	int i;
	QMutexLocker locker( &sidMutex );
	for ( i=0; i<sidList.count(); ++i ) {
		if ( sidList.at(i)->getState()==CamService::Added ) {
			sidList.at(i)->setState( CamService::Ready );
		}
	}
}

void DvbCam::run()
{
	int i, reset_loop, query_loop, state_loop;
	bool cam_ready = false;

	fprintf(stderr, "CamThread: started\n");

	int ca_fd = dvbca_open(Adapter, CaDevice);
	if ( ca_fd<0 ) {
		fprintf( stderr, "CamThread: [error] opening ca device failed\n" );
		return;
	}

	//fprintf(stderr, "CamThread: just using the first cam slot\n");

	reset_loop=0;
	while ( isRunning && reset_loop++<6 && !cam_ready ) {
		if ( ciType!=CA_CI ) { // do not reset HLCI
			if ( dvbca_reset(ca_fd, 0) ) {
				fprintf( stderr, "CamThread: [error] resetting cam slot failed\n" );
				//close( ca_fd );
				//ca_fd = -1;
				//return;
				usleep(1000000);
				continue;
			}
			fprintf( stderr, "CamThread: reset cam slot\n" );
		}

		state_loop=0;
		query_loop=0;
		while ( isRunning && state_loop++<30 ) {
			switch( dvbca_get_cam_state(ca_fd, 0) ) {
				case DVBCA_CAMSTATE_MISSING: {
					//fprintf(stderr, "CamThread: [error] no cam detected\n");
					//close(ca_fd);
					//return;  // FIXME: find a more reliable solution
					break;
				}
				case DVBCA_CAMSTATE_READY: {
					fprintf( stderr, "CamThread: cam 0 is ready\n" );
					cam_ready = true;
					break;
				}
				case DVBCA_CAMSTATE_INITIALISING: {
					fprintf( stderr, "CamThread: cam is initialising\n" );
					if ( ciType==CA_CI ) { // workaround needed for hlci
						fprintf(stderr, "CamThread: cam 0 is ready [hlci workaround]\n");
						cam_ready = true;
					}
					break;
				}
				default: {
					if ( ++query_loop>3 ) {
						fprintf(stderr, "CamThread: [error] querying the cam state failed\n");
						close(ca_fd);
						ca_fd = -1;
						return;
					}
				}
			}
			if(cam_ready) {
				break;
			}
			usleep(100000); // 100 ms
		}
	}

	if ( isRunning ) {
		switch(dvbca_get_interface_type(ca_fd, 0)) {
			case DVBCA_INTERFACE_LINK: {
				fprintf(stderr, "CamThread: LLCI cam slot detected\n");
				break;
			}
			case DVBCA_INTERFACE_HLCI: {
				fprintf(stderr, "CamThread: HLCI cam slot detected\n");
				break;
			}
			default: {
				fprintf(stderr, "CamThread: [error] unknown cam slot type\n");
				close(ca_fd);
				ca_fd = -1;
				return;
			}
		}
	}

	close(ca_fd);
	ca_fd = -1;

	if ( isRunning ) {
		if ( !init() ) {
			fprintf(stderr, "CamThread: [error] cam slot initialisation failed\n");
			return;
		}
	}
	fprintf(stderr, "CamThread: cam slot initialised\n");

	CamService *cs;
	while ( isRunning ) {
		if ( stdcam->stdcam->poll( stdcam->stdcam )!=EN50221_STDCAM_CAM_OK ) {
			usleep( 100000 );
			continue;
		}
		sidMutex.lock();
		for ( i=0; i<sidList.count(); ++i ) {
			cs = sidList.at(i);
			if ( cs->getState()==CamService::Remove ) {
				if ( sendPmt( cs->caPmt, cs->caPmtSize ) ) {
					fprintf( stderr, "CamThread: %s removed from camlist\n", cs->getChannel().name.ascii() );
					sidList.remove( cs );
					--i;
				}
				else
					fprintf( stderr, "CamThread: %s failed removing from camlist\n", cs->getChannel().name.ascii() );
				stdcam->stdcam->poll( stdcam->stdcam );
				usleep(100000);
			}
			else if ( cs->getState()==CamService::Destroy ) {
				fprintf( stderr, "CamThread: %s service deleted\n", cs->getChannel().name.ascii() );
				sidList.remove( cs );
				--i;
			}
		}
		for ( i=0; i<sidList.count(); ++i ) {
			cs = sidList.at(i);
			if ( cs->getState()==CamService::Ready ) {
				if ( sendPmt( cs->caPmt, cs->caPmtSize ) ) {
					cs->setState( CamService::Added );
					fprintf( stderr, "CamThread: %s pmt sent to cam\n", cs->getChannel().name.ascii() );
				}
				else
					fprintf( stderr, "CamThread: %s pmt failed sending to cam\n", cs->getChannel().name.ascii() );
				stdcam->stdcam->poll( stdcam->stdcam );
				usleep(100000);
			}
		}
		sidMutex.unlock();
		usleep( 10000 ); //sleep a bit
	}

	fprintf(stderr, "CamThread: stopping requested\n");

	if ( stdcam ) {
		if (stdcam->stdcam->destroy)
			stdcam->stdcam->destroy(stdcam->stdcam, 1);
		en50221_sl_destroy( SessionLayer );
		en50221_tl_destroy( TransportLayer );
		delete stdcam;
	}

	fprintf(stderr, "CamThread: stopped\n");
	return;
}

bool DvbCam::init()
{
	TransportLayer = en50221_tl_create(1, 16);
	if ( TransportLayer==NULL ) {
		fprintf(stderr, "Failed to create transport layer\n");
		return false;
	}
	SessionLayer = en50221_sl_create(TransportLayer, 16);
	if ( SessionLayer==NULL ) {
		fprintf(stderr, "Failed to create session layer\n");
		en50221_tl_destroy( TransportLayer );
		return false;
	}
	en50221_stdcam *sc = en50221_stdcam_create( Adapter, 0, TransportLayer, SessionLayer );
	if ( sc==NULL ) {
		en50221_sl_destroy( SessionLayer );
		en50221_tl_destroy( TransportLayer );
		fprintf(stderr, "Failed to create stdcam\n");
		return false;
	}

	stdcam = new StandardCam( sc, this );

	// hook up the AI callbacks
	if ( stdcam->stdcam->ai_resource ) {
		en50221_app_ai_register_callback( stdcam->stdcam->ai_resource, aiCallback, stdcam );
	}

	// hook up the CA callbacks
	if ( stdcam->stdcam->ca_resource ) {
		en50221_app_ca_register_info_callback( stdcam->stdcam->ca_resource, infoCallback, stdcam );
	}

	// hook up the MMI callbacks
	if ( stdcam->stdcam->mmi_resource ) {
		en50221_app_mmi_register_close_callback( stdcam->stdcam->mmi_resource, mmi_close_callback, stdcam );
		en50221_app_mmi_register_display_control_callback( stdcam->stdcam->mmi_resource, mmi_display_control_callback, stdcam );
		en50221_app_mmi_register_enq_callback( stdcam->stdcam->mmi_resource, mmi_enq_callback, stdcam );
		en50221_app_mmi_register_menu_callback( stdcam->stdcam->mmi_resource, mmi_menu_callback, stdcam );
		en50221_app_mmi_register_list_callback( stdcam->stdcam->mmi_resource, mmi_menu_callback, stdcam );
	} else {
		fprintf(stderr, "CAM Menus are not supported by this interface hardware\n");
	}

	return true;
}

bool DvbCam::sendPmt( unsigned char *pmt_buffer, int size )
{
	if ( !stdcam )
		return false;
	if ( en50221_app_ca_pmt( stdcam->stdcam->ca_resource, stdcam->stdcam->ca_session_number, pmt_buffer, size) )
		return false;

	return true;
}

int DvbCam::infoCallback(void *arg, uint8_t slot_id, uint16_t session_number, uint32_t ca_id_count, uint16_t *ca_ids)
{
	StandardCam *std =  (StandardCam*)arg;
	(void)slot_id;
	(void)session_number;

	fprintf(stderr, "CAM supports the following ca system ids:\n");
	uint32_t i;
	for ( i=0; i<ca_id_count; i++ ) {
		fprintf( stderr, "  0x%04x\n", ca_ids[i]);
	}

	QApplication::postEvent( std->dvbcam, new QTimerEvent(TIMER_EVENT_CAM_READY ) );

	return 0;
}

int DvbCam::aiCallback(void *arg, uint8_t slot_id, uint16_t session_number, uint8_t application_type, uint16_t application_manufacturer, uint16_t manufacturer_code, uint8_t menu_string_length, uint8_t *menu_string)
{
	StandardCam *std =  (StandardCam*)arg;
	(void)slot_id;
	(void)session_number;

	fprintf(stderr, "CAM Application type: %02x\n", application_type);
	std->dvbcam->setAppType( QString("0x%1").arg(application_type, 0, 16 ) );
	fprintf(stderr, "CAM Application manufacturer: %04x\n", application_manufacturer);
	std->dvbcam->setAppManu( QString("0x%1").arg(application_manufacturer, 0, 16 ) );
	fprintf(stderr, "CAM Manufacturer code: %04x\n", manufacturer_code);
	std->dvbcam->setManuCode( QString("0x%1").arg(manufacturer_code, 0, 16 ) );
	fprintf(stderr, "CAM Menu string: %.*s\n", menu_string_length, menu_string);
	QString s = (const char*)menu_string;
	s.truncate( menu_string_length );
	std->dvbcam->setMenuString( s );
	return 0;
}

int DvbCam::mmi_close_callback(void *arg, uint8_t slot_id, uint16_t session_number, uint8_t cmd_id, uint8_t delay)
{
	fprintf( stderr,"mmi_close_callback, delay=%d\n",delay);
	StandardCam *std = (StandardCam*)arg;
	(void) slot_id;
	(void) session_number;
	(void) cmd_id;
	(void) delay;

	// note: not entirely correct as its supposed to delay if asked
	std->mmi_state = MMI_STATE_CLOSED;
	QApplication::postEvent( std->dvbcam, new QTimerEvent(TIMER_EVENT_MMI_CLOSE ) );
	return 0;
}

int DvbCam::mmi_display_control_callback(void *arg, uint8_t slot_id, uint16_t session_number, uint8_t cmd_id, uint8_t mmi_mode)
{
	fprintf( stderr,"mmi_display_control_callback\n");
	struct en50221_app_mmi_display_reply_details reply;
	StandardCam *std = (StandardCam*)arg;
	(void) slot_id;

	// don't support any commands but set mode
	if ( cmd_id!=MMI_DISPLAY_CONTROL_CMD_ID_SET_MMI_MODE ) {
		en50221_app_mmi_display_reply( std->stdcam->mmi_resource, session_number, MMI_DISPLAY_REPLY_ID_UNKNOWN_CMD_ID, &reply );
		return 0;
	}

	// we only support high level mode
	if ( mmi_mode!=MMI_MODE_HIGH_LEVEL ) {
		en50221_app_mmi_display_reply( std->stdcam->mmi_resource, session_number, MMI_DISPLAY_REPLY_ID_UNKNOWN_MMI_MODE, &reply );
		return 0;
	}

	// ack the high level open
	reply.u.mode_ack.mmi_mode = mmi_mode;
	en50221_app_mmi_display_reply( std->stdcam->mmi_resource, session_number, MMI_DISPLAY_REPLY_ID_MMI_MODE_ACK, &reply );
	std->mmi_state = MMI_STATE_OPEN;
	return 0;
}

int DvbCam::mmi_enq_callback(void *arg, uint8_t slot_id, uint16_t session_number, uint8_t blind_answer, uint8_t expected_answer_length,
		uint8_t *text, uint32_t text_size)
{
	fprintf( stderr,"mmi_enq_callback\n");
	StandardCam *std = (StandardCam*)arg;
	(void) slot_id;
	(void) session_number;

	QString s;

	QMutexLocker locker( &std->mutex );

	std->menuList.clear();
	fprintf(stderr, "%.*s: ", text_size, text);
	s = (const char*)text;
	s.truncate( text_size );
	std->menuList.append( s );
	fflush(stdout);

	std->mmi_enq_blind = blind_answer;
	std->mmi_enq_length = expected_answer_length;
	std->mmi_state = MMI_STATE_ENQ;
	std->menuType = MMI_MENU;

	return 0;
}

int DvbCam::mmi_menu_callback(void *arg, uint8_t slot_id, uint16_t session_number, struct en50221_app_mmi_text *title,
		struct en50221_app_mmi_text *sub_title, struct en50221_app_mmi_text *bottom, uint32_t item_count,
		struct en50221_app_mmi_text *items, uint32_t item_raw_length, uint8_t *items_raw)
{
	fprintf( stderr,"mmi_menu_callback, session=%d\n",session_number);
	StandardCam *std = (StandardCam*)arg;
	(void) slot_id;
	(void) session_number;
	(void) item_raw_length;
	(void) items_raw;

	QString s;

	QMutexLocker locker( &std->mutex );
	std->menuList.clear();

	fprintf(stderr, "------------------------------\n");

	if (title->text_length) {
		fprintf(stderr, "%.*s\n", title->text_length, title->text);
		s = (const char*)title->text;
		s.truncate( title->text_length );
		std->menuList.append( s );
	}
	if (sub_title->text_length) {
		fprintf(stderr, "%.*s\n", sub_title->text_length, sub_title->text);
		s = (const char*)sub_title->text;
		s.truncate( sub_title->text_length );
		std->menuList.append( s );
	}

	uint32_t i;
	fprintf(stderr, "0. Quit menu\n");
	std->menuList.append( "0. Quit menu" );
	for(i=0; i< item_count; i++) {
		fprintf(stderr, "%i. %.*s\n", i+1, items[i].text_length, items[i].text);
		s = (const char*)items[i].text;
		s.truncate( items[i].text_length );
		std->menuList.append( QString("%1. %2").arg(i+1).arg(s) );

	}

	if (bottom->text_length) {
		fprintf(stderr, "%.*s\n", bottom->text_length, bottom->text);
		s = (const char*)bottom->text;
		s.truncate( bottom->text_length );
		std->menuList.append( s );
	}
	fflush(stdout);

	std->mmi_state = MMI_STATE_MENU;
	std->menuType = MMI_MENU;

	QApplication::postEvent( std->dvbcam, new QTimerEvent(TIMER_EVENT_MMI_OPEN ) );

	return 0;
}

void DvbCam::timerEvent( QTimerEvent *e )
{
	switch ( e->timerId() ) {
		case TIMER_EVENT_MMI_OPEN:
			showMMI();
			break;
		case TIMER_EVENT_MMI_CLOSE:
			closeMMI();
			break;
		case TIMER_EVENT_CAM_READY:
			resendPmts();
			break;
	}
}

void DvbCam::showMMI()
{
	if ( !menuDialog && stdcam ) {
		menuDialog = new MCamMenuDialog( stdcam );
		connect( menuDialog, SIGNAL(enteredResponse(QString)), this, SLOT(mmiResponse(QString)) );
		menuDialog->exec();
		closeMMI();
	}
}

void DvbCam::closeMMI()
{
	if ( menuDialog ) {
		delete menuDialog;
		menuDialog = 0;
		if ( stdcam && stdcam->stdcam->mmi_resource ) {
			en50221_app_mmi_close( stdcam->stdcam->mmi_resource, stdcam->stdcam->mmi_session_number, MMI_CLOSE_MMI_CMD_ID_IMMEDIATE, 0 );
			stdcam->mmi_state = MMI_STATE_CLOSED;
		}
	}
}

int DvbCam::showCamDialog()
{

	CamDialog dlg;
	dlg.maxServiceSpin->setValue( getCamMaxService() );
	dlg.appTypeLab->setText( getAppType() );
	dlg.appManuLab->setText( getAppManu() );
	dlg.manuCodeLab->setText( getManuCode() );
	dlg.menuStringLab->setText( getMenuString() );
	connect( dlg.camMenuBtn, SIGNAL(clicked()), this, SLOT(enterMenu()) );
	dlg.exec();
	CamMaxService = dlg.maxServiceSpin->value();
	return CamMaxService;
}

void DvbCam::enterMenu()
{
	if ( stdcam && stdcam->stdcam->ai_resource )
		en50221_app_ai_entermenu( stdcam->stdcam->ai_resource, stdcam->stdcam->ai_session_number );
}

void DvbCam::mmiResponse( QString s )
{
	QString res = s.stripWhiteSpace();

	switch( stdcam->mmi_state ) {
		case MMI_STATE_CLOSED:
		case MMI_STATE_OPEN: {
			en50221_app_ai_entermenu(stdcam->stdcam->ai_resource, stdcam->stdcam->ai_session_number);
			fprintf(stderr,"en50221_app_ai_entermenu 2\n");
			break;
		}
		case MMI_STATE_ENQ: {
			if ( res.isEmpty() ) {
				en50221_app_mmi_answ( stdcam->stdcam->mmi_resource, stdcam->stdcam->mmi_session_number, MMI_ANSW_ID_CANCEL, NULL, 0);
			}
			else {
				en50221_app_mmi_answ( stdcam->stdcam->mmi_resource, stdcam->stdcam->mmi_session_number, MMI_ANSW_ID_ANSWER, (uint8_t*)res.ascii(), res.length() );
			}
			stdcam->mmi_state = MMI_STATE_OPEN;
			break;
		}
		case MMI_STATE_MENU: {
			en50221_app_mmi_menu_answ( stdcam->stdcam->mmi_resource, stdcam->stdcam->mmi_session_number, res.toInt() );
			stdcam->mmi_state = MMI_STATE_OPEN;
			break;
		}
	}
}




MCamMenuDialog::MCamMenuDialog( StandardCam *sc )
{
	stdcam = sc;
	connect( inputLine, SIGNAL(returnPressed()), this, SLOT(validateClicked()) );
	connect( &readTimer, SIGNAL(timeout()), this, SLOT(setMenu()) );
	readTimer.start( 500 );
}

void MCamMenuDialog::setMenu()
{
	if ( !stdcam )
		return;
	QMutexLocker locker( &stdcam->mutex );
	if ( stdcam->menuType==MMI_MENU )
		menuText->setText( stdcam->menuList.join("\n") );
}

void MCamMenuDialog::validateClicked()
{
	emit enteredResponse( inputLine->text() );
	inputLine->clear();
}





// class CamService
CamService::CamService( int adapter, int demux_device, ChannelDesc *chan, int maxService )
{
	Adapter     = adapter;
	DemuxDevice = demux_device;
	CamMaxService = maxService;
	channel = *chan;
	parsedPmt = NULL;
	state = NotReady;
	isRunning = true;
	start();
}

CamService::~CamService()
{
	stop();
}

void CamService::restart()
{
	setState( NotReady );
}

void CamService::stop()
{
	isRunning = false;
	wait();
}

int CamService::createSectionFilter(uint16_t pid, uint8_t table_id)
{
	// open the demuxer
	int demux_fd = dvbdemux_open_demux(Adapter, DemuxDevice, 0);
	if(demux_fd < 0) {
		return -1;
	}

	// create a section filter
	uint8_t filter[18] = {table_id};
	uint8_t mask[18] = {0xff};
	if(dvbdemux_set_section_filter(demux_fd, pid, filter, mask, 1, 1)) { // crc check on
		close(demux_fd);
		return -1;
	}

	return demux_fd;
}

int CamService::processPat(int demux_fd)
{
	// read section
	unsigned char si_buf[4096];
	int size = read( demux_fd, si_buf, sizeof(si_buf) );
	if ( size<0 )
		return -1;

	// parse section
	section *parsed_section = section_codec( si_buf, size );
	if ( parsed_section==NULL )
		return -1;

	// parse section_ext
	section_ext *parsed_section_ext = section_ext_decode( parsed_section, 1 ); // crc check on
	if ( parsed_section_ext==NULL )
		return -1;

	// parse pat
	mpeg_pat_section *parsed_pat = mpeg_pat_section_codec( parsed_section_ext );
	if ( parsed_pat==NULL )
		return -1;

	// try and find the requested program
	mpeg_pat_program *cur_program;
	mpeg_pat_section_programs_for_each( parsed_pat, cur_program ) {
		if ( cur_program->program_number==channel.sid )
			return cur_program->pid;
	}

	fprintf( stderr, "CamService (%s): [warning] channel couldn't be found in PAT\n", channel.name.ascii() );

	return -1;
}

int CamService::processPmt( int demux_fd )
{
	// read section
	memset( pmtBuffer, 0, sizeof(pmtBuffer) );
	int size = read( demux_fd, pmtBuffer, sizeof(pmtBuffer) );
	if ( size<=0 )
		return -1;

	// parse section
	section *parsed_section = section_codec( pmtBuffer, size );
	if ( parsed_section==NULL )
		return -1;

	// parse section_ext
	section_ext *parsed_section_ext = section_ext_decode( parsed_section, 1 ); // crc check on
	if ( parsed_section_ext==NULL )
		return -1;

	// parse pmt
	struct mpeg_pmt_section *pmt = mpeg_pmt_section_codec( parsed_section_ext );
	if ( pmt==NULL )
		return -1;

	parsedPmt = pmt;
	return parsedPmt->head.version_number;
}

void CamService::setState( PmtState st )
{
	QMutexLocker locker( &mutex );
	switch ( st ) {
		case Ready: {
			int listmgnt;
			if ( CamMaxService==1 )
				listmgnt = CA_LIST_MANAGEMENT_ONLY;
			else
				listmgnt = CA_LIST_MANAGEMENT_ADD;
			if ( state>Ready )
				listmgnt = CA_LIST_MANAGEMENT_UPDATE;
			if ( setCaPmt( listmgnt, CA_PMT_CMD_ID_OK_DESCRAMBLING ) )
				state = st;
			break;
		}
		case Remove: {
			if ( state!=Added )
				state = Destroy;
			else if ( setCaPmt( CA_LIST_MANAGEMENT_UPDATE, CA_PMT_CMD_ID_NOT_SELECTED ) )
				state = st;
			break;
		}
		default: {
			state = st;
		}
	}
}

int CamService::getState()
{
	QMutexLocker locker( &mutex );
	return state;
}

bool CamService::setCaPmt( int list_management, int cmd_id )
{
	if ( !parsedPmt )
		return false;

	caPmtSize = en50221_ca_format_pmt( parsedPmt, caPmt, sizeof(caPmt), 0, list_management, cmd_id );
	if ( caPmtSize<=0 )
		return false;

	return true;
}

void CamService::run()
{
	int i, pmtVersion=-1;
	int demux_fd=-1;

	fprintf( stderr, "CamService (%s): started\n", channel.name.ascii() );

	while ( isRunning ) {
loopLabel:
		demux_fd = createSectionFilter( TRANSPORT_PAT_PID, stag_mpeg_program_association );
		if ( demux_fd<0 ) {
			fprintf( stderr, "CamService (%s): [error] opening demux device failed\n", channel.name.ascii() );
			demux_fd = -1;
			usleep( 100000 );
			goto loopLabel;
		}

		pollfd poll_desc;
		poll_desc.fd = demux_fd;
		poll_desc.events = POLLIN | POLLPRI;
		while ( isRunning ) {
			int ret = poll( &poll_desc, 1, 1000 );
			if ( ret<0 ) {
				fprintf( stderr, "CamService (%s): [error] polling demux device failed\n", channel.name.ascii() );
				close( demux_fd );
				demux_fd = -1;
				usleep( 100000 );
				goto loopLabel;
			}
			if ( ret>0 ) {
				int processed_pat = processPat( demux_fd );
				if ( processed_pat>=0 ) {
					close( demux_fd );
					demux_fd = -1;
					demux_fd = createSectionFilter( processed_pat, stag_mpeg_program_map );
					if ( demux_fd<0 ) {
						fprintf( stderr, "CamService (%s): [error] opening demux device failed\n", channel.name.ascii() );
						demux_fd = -1;
						usleep( 100000 );
						goto loopLabel;
					}
					poll_desc.fd = demux_fd;
					break;
				}
				else
					usleep( 10000 );
			}
			else
				usleep( 10000 );
		}

		while ( isRunning ) {
			int ret = poll( &poll_desc, 1, 1000 );
			if ( ret<0 ) {
				fprintf( stderr, "CamService (%s): [error] polling demux device failed\n", channel.name.ascii() );
				close( demux_fd );
				demux_fd = -1;
				usleep( 100000 );
				goto loopLabel;
			}
			if ( ret>0 ) {
				//fprintf( stderr, "CamService (%s): parsing pmt\n", channel.name.ascii() );
				i = processPmt( demux_fd );
				if ( i==-1 )
					usleep( 10000 );
				else {
					if ( i!=pmtVersion || getState()<Ready ) {
						fprintf( stderr, "CamService (%s): new pmt received\n", channel.name.ascii() );
						setState( Ready );
						pmtVersion = i;
					}
					i = 200;
					while ( i-- && isRunning )
						usleep( 10000 );
				}
			}
			else
				usleep( 10000 );
		}
	}

	if ( demux_fd != -1 )
		close( demux_fd );
	fprintf( stderr, "CamService (%s): stopped\n", channel.name.ascii() );
}

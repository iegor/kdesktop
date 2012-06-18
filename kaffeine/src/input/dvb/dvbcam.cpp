/*
 * dvbcam.cpp
 *
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

#include <libdvbapi/dvbca.h>
#include <libdvbapi/dvbdemux.h>
#include <libdvben50221/en50221_app_ai.h>
#include <libdvben50221/en50221_app_ca.h>
#include <libdvben50221/en50221_app_rm.h>
#include <libdvben50221/en50221_app_tags.h>
#include <libdvben50221/en50221_session.h>
#include <libucsi/mpeg/section.h>

#include <qthread.h>
#include <qstring.h>

#include "dvbcam.h"

class DvbCamCamHandler
{
public:
	virtual ~DvbCamCamHandler();

	bool init();

	virtual void poll() = 0;
	virtual bool registerCam(int ca_fd, uint8_t slot) = 0;

	bool sendPmt(char *pmt_buffer, int size);

protected:
	DvbCamCamHandler();

	static int infoCallback(void *arg, uint8_t slot_id, uint16_t session_number, uint32_t ca_id_count, uint16_t *ca_ids);

	virtual bool sub_init() = 0;

	void *AiResource;
	void *CaResource;
	en50221_app_send_functions SendFuncs;

	volatile int SessionNumber;
};

class DvbCamCamThread : protected QThread
{
public:
	DvbCamCamThread(int adapter, int ca_device, int ci_type);
	~DvbCamCamThread();

	void start();
	void stop();
	void wait();

	bool processPmt(int demux_fd);

private:
	void run();

	int Adapter;
	int CaDevice;
	int ciType;

	DvbCamCamHandler *CamHandler;

	char PmtBuffer[4096]; // we imply that processPmt is only once called

	volatile int  PmtSize; // PmtSize <= 0 means PmtBuffer invalid
	volatile bool Stopped;
};

class DvbCamPmtThread : protected QThread
{
public:
	DvbCamPmtThread(DvbCamCamThread *cam_thread, int adapter, int demux_device, int service_id);
	~DvbCamPmtThread();

	void start();
	void stop();
	void wait();

private:
	int  createSectionFilter(uint16_t pid, uint8_t table_id);
	int  processPat(int demux_fd);
	void run();

	DvbCamCamThread *CamThread;
	int Adapter;
	int DemuxDevice;
	int ServiceId;

	volatile bool Stopped;
};

class DvbCamCamHandlerLLCI : public DvbCamCamHandler
{
public:
	DvbCamCamHandlerLLCI();
	~DvbCamCamHandlerLLCI();

private:
	struct SlResource
	{
		en50221_app_public_resource_id resid;
		uint32_t binary_resource_id;
		en50221_sl_resource_callback callback;
		void *arg;
	};

	static int llci_rm_enq_callback(void *arg, uint8_t slot_id, uint16_t session_number);
	static int llci_rm_reply_callback(void *arg, uint8_t slot_id, uint16_t session_number, uint32_t resource_id_count, uint32_t *resource_ids);
	static int llci_rm_changed_callback(void *arg, uint8_t slot_id, uint16_t session_number);
	static int llci_lookup_callback(void *arg, uint8_t slot_id, uint32_t requested_resource_id, en50221_sl_resource_callback *callback_out, void **arg_out, uint32_t *connected_resource_id);
	static int llci_session_callback(void *arg, int reason, uint8_t slot_id, uint16_t session_number, uint32_t resource_id);

	void poll();
	bool registerCam(int ca_fd, uint8_t slot);
	bool sub_init();

	void *RmResource;
	void *SessionLayer;
	void *TransportLayer;

	SlResource Resources[3];
};

class DvbCamCamHandlerHLCI : public DvbCamCamHandler
{
public:
	DvbCamCamHandlerHLCI();
	~DvbCamCamHandlerHLCI();

private:
	static int hlci_send_data(void *arg, uint16_t session_number, uint8_t *data, uint16_t data_length);
	static int hlci_send_datav(void *arg, uint16_t session_number, iovec *vector, int iov_count);

	void poll();
	bool registerCam(int ca_fd, uint8_t slot);
	bool sub_init();
};

// class DvbCam

DvbCam::DvbCam(int adapter, int ca_device, int demux_device, int ci_type)
{
	Adapter     = adapter;
	CaDevice    = ca_device;
	DemuxDevice = demux_device;
	ciType = ci_type;

	isRunning = false;

	CamThread = NULL;
	PmtThread = NULL;

	// at that time, we do reset in cam_thread
	/*if ( ciType!=CA_CI ) { //do not reset HLCI
		int ca_fd = dvbca_open( Adapter, CaDevice );
		if(ca_fd < 0) // should not happen
			fprintf(stderr, "CamThread: [error] opening ca device failed\n");
		else {
			if(dvbca_reset(ca_fd, 0))
				fprintf(stderr, "CamThread: [error] resetting cam slot failed\n");
			close(ca_fd);
		}
	}*/
}

DvbCam::~DvbCam()
{
	stop();
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

void DvbCam::restart(int service_id)
{
	stop();

	isRunning = true;
	sid = service_id;

	CamThread = new DvbCamCamThread(Adapter, CaDevice, ciType);
	CamThread->start();

	PmtThread = new DvbCamPmtThread(CamThread, Adapter, DemuxDevice, service_id);
	PmtThread->start();
}

void DvbCam::stop()
{
	if(PmtThread != NULL) {
		PmtThread->stop();
	}
	if(CamThread != NULL) {
		CamThread->stop();
	}
	if(PmtThread != NULL) {
		PmtThread->wait();
		delete PmtThread;
		PmtThread = NULL;
	}
	if(CamThread != NULL) {
		CamThread->wait();
		delete CamThread; // be careful about deletion: PmtThread uses CamThread internally
		CamThread = NULL;
	}
	isRunning = false;
}

// class DvbCamCamThread

DvbCamCamThread::DvbCamCamThread(int adapter, int ca_device, int ci_type)
{
	Adapter  = adapter;
	CaDevice = ca_device;
	ciType = ci_type;

	CamHandler = NULL;
}

DvbCamCamThread::~DvbCamCamThread()
{
	wait(); // should never be necessary
}

void DvbCamCamThread::start()
{
	PmtSize = 0;
	Stopped = false;
	QThread::start();
}

void DvbCamCamThread::stop()
{
	Stopped = true;
}

void DvbCamCamThread::wait()
{
	QThread::wait();
}

bool DvbCamCamThread::processPmt(int demux_fd)
{
	// read section
	char si_buf[4096];
	int size = read(demux_fd, si_buf, sizeof(si_buf));
	if(size <= 0) {
		return false;
	}

	// parse section
	section *parsed_section = section_codec(reinterpret_cast<unsigned char *> (si_buf), size);
	if(parsed_section == NULL) {
		return false;
	}

	// parse section_ext
	section_ext *parsed_section_ext = section_ext_decode(parsed_section, 1); // crc check on
	if(parsed_section_ext == NULL) {
		return false;
	}

	// parse pmt
	mpeg_pmt_section *parsed_pmt = mpeg_pmt_section_codec(parsed_section_ext);
	if(parsed_pmt == NULL) {
		return false;
	}

	// translate it into a cam pmt
	PmtSize = en50221_ca_format_pmt(parsed_pmt, reinterpret_cast<unsigned char *> (PmtBuffer), sizeof(PmtBuffer), 0, CA_LIST_MANAGEMENT_ONLY, CA_PMT_CMD_ID_OK_DESCRAMBLING);
	if(PmtSize <= 0) {
		return false;
	}

	// the DvbCamCamThread will send it to the cam
	return true;
}

void DvbCamCamThread::run()
{
	fprintf(stderr, "CamThread: started\n");

	int ca_fd = dvbca_open(Adapter, CaDevice);
	if(ca_fd < 0) {
		fprintf(stderr, "CamThread: [error] opening ca device failed\n");
		return;
	}

	fprintf(stderr, "CamThread: just using the first cam slot\n");

	if ( ciType!=CA_CI ) { // do not reset HLCI
		if(dvbca_reset(ca_fd, 0)) {
			fprintf(stderr, "CamThread: [error] resetting cam slot failed\n");
			close(ca_fd);
			return;
		}
	}

	while(!Stopped) {
		bool cam_ready = false;
		switch(dvbca_get_cam_state(ca_fd, 0)) {
			case DVBCA_CAMSTATE_MISSING: {
				/*fprintf(stderr, "CamThread: [error] no cam detected\n");
				close(ca_fd);
				return; */ // FIXME: find a more reliable solution
				break;
			}
			case DVBCA_CAMSTATE_READY: {
				fprintf(stderr, "CamThread: cam 0 is ready\n");
				cam_ready = true;
				break;
			}
			case DVBCA_CAMSTATE_INITIALISING: {
				if ( ciType==CA_CI ) { // workaround needed for hlci
					fprintf(stderr, "CamThread: cam 0 is ready [hlci workaround]\n");
					cam_ready = true;
				}
				break;
			}
			default: {
				fprintf(stderr, "CamThread: [error] querying the cam state failed\n");
				close(ca_fd);
				return;
			}
		}
		if(cam_ready) {
			break;
		}
		usleep(100000); // 100 ms
	}

	if(!Stopped) {
		switch(dvbca_get_interface_type(ca_fd, 0)) {
			case DVBCA_INTERFACE_LINK: {
				fprintf(stderr, "CamThread: LLCI cam slot detected\n");
				CamHandler = new DvbCamCamHandlerLLCI();
				break;
			}
			case DVBCA_INTERFACE_HLCI: {
				fprintf(stderr, "CamThread: HLCI cam slot detected\n");
				CamHandler = new DvbCamCamHandlerHLCI();
				break;
			}
			default: {
				fprintf(stderr, "CamThread: [error] unknown cam slot type\n");
				close(ca_fd);
				return;
			}
		}
	}

	if(!Stopped) {
		if(!CamHandler->init()) {
			fprintf(stderr, "CamThread: [error] cam slot initialization failed\n");
			delete CamHandler;
			CamHandler = NULL;
			close(ca_fd);
			return;
		}
	}

	if(!Stopped) {
		if(!CamHandler->registerCam(ca_fd, 0)) {
			fprintf(stderr, "CamThread: [error] registering cam 0 failed\n");
			delete CamHandler;
			CamHandler = NULL;
			close(ca_fd);
			return;
		}
	}

	while(!Stopped) {
		CamHandler->poll();
		if(PmtSize > 0) {
			if(CamHandler->sendPmt(PmtBuffer, PmtSize)) {
				fprintf(stderr, "CamThread: pmt sent to cam\n");
				PmtSize = 0;
			}
		}
	}

	fprintf(stderr, "CamThread: stopping requested\n");

	delete CamHandler;
	CamHandler = NULL;
	close(ca_fd);

	fprintf(stderr, "CamThread: stopped\n");
	return;
}

// class DvbCamPmtThread

DvbCamPmtThread::DvbCamPmtThread(DvbCamCamThread *cam_thread, int adapter, int demux_device, int service_id)
{
	CamThread   = cam_thread;
	Adapter     = adapter;
	DemuxDevice = demux_device;
	ServiceId   = service_id;
}

DvbCamPmtThread::~DvbCamPmtThread()
{
	wait(); // should never be necessary
}

void DvbCamPmtThread::start()
{
	Stopped = false;
	QThread::start();
}

void DvbCamPmtThread::stop()
{
	Stopped = true;
}

void DvbCamPmtThread::wait()
{
	QThread::wait();
}

int DvbCamPmtThread::createSectionFilter(uint16_t pid, uint8_t table_id)
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

int DvbCamPmtThread::processPat(int demux_fd)
{
	// read section
	char si_buf[4096];
	int size = read(demux_fd, si_buf, sizeof(si_buf));
	if(size < 0) {
		return -1;
	}

	// parse section
	section *parsed_section = section_codec(reinterpret_cast<unsigned char *> (si_buf), size);
	if(parsed_section == NULL) {
		return -1;
	}

	// parse section_ext
	section_ext *parsed_section_ext = section_ext_decode(parsed_section, 1); // crc check on
	if(parsed_section_ext == NULL) {
		return -1;
	}

	// parse pat
	mpeg_pat_section *parsed_pat = mpeg_pat_section_codec(parsed_section_ext);
	if(parsed_pat == NULL) {
		return -1;
	}

	// try and find the requested program
	mpeg_pat_program *cur_program;
	mpeg_pat_section_programs_for_each(parsed_pat, cur_program) {
		if(cur_program->program_number == ServiceId) {
			return cur_program->pid;
		}
	}

	fprintf(stderr, "PmtThread: [warning] the requested service id couldn't be found\n");

	return -1;
}

void DvbCamPmtThread::run()
{
	fprintf(stderr, "PmtThread: started\n");

	int demux_fd = createSectionFilter(TRANSPORT_PAT_PID, stag_mpeg_program_association);
	if(demux_fd < 0) {
		fprintf(stderr, "PmtThread: [error] opening demux device failed\n");
		return;
	}

	pollfd poll_desc;
	poll_desc.fd = demux_fd;
	poll_desc.events = POLLIN | POLLPRI;
	while(!Stopped) {
		int ret = poll(&poll_desc, 1, 100); // 100 ms
		if(ret < 0) {
			fprintf(stderr, "PmtThread: [error] polling demux device failed\n");
			close(demux_fd);
			return;
		}
		if((ret > 0) && (poll_desc.revents != 0) && ((poll_desc.revents & ~(POLLIN | POLLPRI)) == 0)) {
			int processed_pat = processPat(demux_fd);
			if(processed_pat >= 0) {
				close(demux_fd);
				demux_fd = createSectionFilter(processed_pat, stag_mpeg_program_map);
				if(demux_fd < 0) {
					fprintf(stderr, "PmtThread: [error] opening demux device failed\n");
					return;
				}
				poll_desc.fd = demux_fd;
				break;
			}
		}
	}

	while(!Stopped) {
		int ret = poll(&poll_desc, 1, 100); // 100 ms
		if(ret < 0) {
			fprintf(stderr, "PmtThread: [error] polling demux device failed\n");
			close(demux_fd);
			return;
		}
		if((ret > 0) && (poll_desc.revents != 0) && ((poll_desc.revents & ~(POLLIN | POLLPRI)) == 0)) {
			if(CamThread->processPmt(demux_fd)) {
				fprintf(stderr, "PmtThread: new pmt received\n");
				close(demux_fd);
				fprintf(stderr, "PmtThread: stopped\n");
				return;
			}
		}
	}

	fprintf(stderr, "PmtThread: stopping requested\n");

	close(demux_fd);

	fprintf(stderr, "PmtThread: stopped\n");
	return;
}

// class DvbCamCamHandler

DvbCamCamHandler::DvbCamCamHandler()
{
	AiResource = NULL;
	CaResource = NULL;

	SessionNumber = -1;
}

DvbCamCamHandler::~DvbCamCamHandler()
{
	if(CaResource != NULL) {
		en50221_app_ca_destroy(CaResource);
		CaResource = NULL;
	}
	if(AiResource != NULL) {
		en50221_app_ai_destroy(AiResource);
		AiResource = NULL;
	}
}

bool DvbCamCamHandler::init()
{
	AiResource = en50221_app_ai_create(&SendFuncs);
	CaResource = en50221_app_ca_create(&SendFuncs);

	if(CaResource != NULL) {
		en50221_app_ca_register_info_callback(CaResource, infoCallback, this);
	}

	return sub_init();
}

bool DvbCamCamHandler::sendPmt(char *pmt_buffer, int size)
{
	if((CaResource != NULL) && (SessionNumber >= 0)) {
		if(!en50221_app_ca_pmt(CaResource, SessionNumber, reinterpret_cast<unsigned char *> (pmt_buffer), size)) {
			return true;
		}
	}

	return false;
}

int DvbCamCamHandler::infoCallback(void *arg, uint8_t /*slot_id*/, uint16_t session_number, uint32_t /*ca_id_count*/, uint16_t */*ca_ids*/)
{
	(static_cast<DvbCamCamHandler *> (arg))->SessionNumber = session_number;
	return 0;
}

// class DvbCamCamHandlerLLCI

#define MAX_CARDS 1
#define MAX_TC 16
#define MAX_SESSIONS 16

DvbCamCamHandlerLLCI::DvbCamCamHandlerLLCI()
{
	RmResource     = NULL;
	SessionLayer   = NULL;
	TransportLayer = NULL;
}

DvbCamCamHandlerLLCI::~DvbCamCamHandlerLLCI()
{
	if(SessionLayer != NULL) {
		en50221_sl_destroy(SessionLayer);
		SessionLayer = NULL;
	}
	if(TransportLayer != NULL) {
		en50221_tl_destroy(TransportLayer);
		TransportLayer = NULL;
	}
	if(RmResource != NULL) {
		en50221_app_rm_destroy(RmResource);
		RmResource = NULL;
	}
}

int DvbCamCamHandlerLLCI::llci_rm_enq_callback(void *arg, uint8_t /*slot_id*/, uint16_t session_number)
{
	uint32_t resource_ids[] = {EN50221_APP_RM_RESOURCEID, EN50221_APP_AI_RESOURCEID, EN50221_APP_CA_RESOURCEID};
	en50221_app_rm_reply(arg, session_number, sizeof(resource_ids) / 4, resource_ids);
	return 0;
}

int DvbCamCamHandlerLLCI::llci_rm_reply_callback(void *arg, uint8_t /*slot_id*/, uint16_t session_number, uint32_t /*resource_id_count*/, uint32_t */*resource_ids*/)
{
	en50221_app_rm_changed(arg, session_number);
	return 0;
}

int DvbCamCamHandlerLLCI::llci_rm_changed_callback(void *arg, uint8_t /*slot_id*/, uint16_t session_number)
{
	en50221_app_rm_enq(arg, session_number);
	return 0;
}

int DvbCamCamHandlerLLCI::llci_lookup_callback(void *arg, uint8_t /*slot_id*/, uint32_t requested_resource_id, en50221_sl_resource_callback *callback_out, void **arg_out, uint32_t *connected_resource_id)
{
	// decode the resource id
	en50221_app_public_resource_id resid;
	if(!en50221_app_decode_public_resource_id(&resid, requested_resource_id)) {
		return -1;
	}

	// try and find an instance of the resource
	const SlResource *Resources = (static_cast<DvbCamCamHandlerLLCI *> (arg))->Resources;
	int i;
	for(i = 0; i < 3; ++i) {
		if((resid.resource_class == Resources[i].resid.resource_class) && (resid.resource_type == Resources[i].resid.resource_type)) {
			*callback_out = Resources[i].callback;
			*arg_out = Resources[i].arg;
			*connected_resource_id = Resources[i].binary_resource_id;
			return 0;
		}
	}
	return -1;
}

int DvbCamCamHandlerLLCI::llci_session_callback(void *arg, int reason, uint8_t /*slot_id*/, uint16_t session_number, uint32_t resource_id)
{
	if(reason == S_SCALLBACK_REASON_CAMCONNECTED) {
		if(resource_id == EN50221_APP_RM_RESOURCEID) {
			void *RmResource = (static_cast<DvbCamCamHandlerLLCI *> (arg))->Resources[0].arg;
			en50221_app_rm_enq(RmResource, session_number);
		}
		else if(resource_id == EN50221_APP_AI_RESOURCEID) {
			void *AiResource = (static_cast<DvbCamCamHandlerLLCI *> (arg))->Resources[1].arg;
			en50221_app_ai_enquiry(AiResource, session_number);
		}
		else if(resource_id == EN50221_APP_CA_RESOURCEID) {
			void *CaResource = (static_cast<DvbCamCamHandlerLLCI *> (arg))->Resources[2].arg;
			en50221_app_ca_info_enq(CaResource, session_number);
		}
	}
	return 0;
}

void DvbCamCamHandlerLLCI::poll()
{
	if(en50221_tl_poll(TransportLayer)) {
		fprintf(stderr, "CamThread: [warning] polling the stack failed\n");
		usleep(10000); // wait 10 ms to not block
	}
}

bool DvbCamCamHandlerLLCI::registerCam(int ca_fd, uint8_t slot)
{
	// register the slot
	int slot_id = en50221_tl_register_slot(TransportLayer, ca_fd, slot, 1000, 100);
	if(slot_id < 0) {
		return false;
	}

	// create a new connection on the slot
	if(en50221_tl_new_tc(TransportLayer, slot_id) < 0) {
		return false;
	}

	return true;
}

bool DvbCamCamHandlerLLCI::sub_init()
{
	// create transport layer
	TransportLayer = en50221_tl_create(MAX_CARDS, MAX_TC);
	if(TransportLayer == NULL) {
		return false;
	}

	// create session layer
	SessionLayer = en50221_sl_create(TransportLayer, MAX_SESSIONS);
	if(SessionLayer == NULL) {
		en50221_tl_destroy(TransportLayer);
		TransportLayer = NULL;
		return false;
	}

	// create sendfuncs
	SendFuncs.arg        = SessionLayer;
	SendFuncs.send_data  = en50221_sl_send_data;
	SendFuncs.send_datav = en50221_sl_send_datav;

	// create the resource manager resource
	RmResource = en50221_app_rm_create(&SendFuncs);
	en50221_app_decode_public_resource_id(&Resources[0].resid, EN50221_APP_RM_RESOURCEID);
	Resources[0].binary_resource_id = EN50221_APP_RM_RESOURCEID;
	Resources[0].callback = en50221_app_rm_message;
	Resources[0].arg = RmResource;
	en50221_app_rm_register_enq_callback(RmResource, llci_rm_enq_callback, RmResource);
	en50221_app_rm_register_reply_callback(RmResource, llci_rm_reply_callback, RmResource);
	en50221_app_rm_register_changed_callback(RmResource, llci_rm_changed_callback, RmResource);

	// integrate the application information resource
	en50221_app_decode_public_resource_id(&Resources[1].resid, EN50221_APP_AI_RESOURCEID);
	Resources[1].binary_resource_id = EN50221_APP_AI_RESOURCEID;
	Resources[1].callback = en50221_app_ai_message;
	Resources[1].arg = AiResource;

	// integrate the ca resource
	en50221_app_decode_public_resource_id(&Resources[2].resid, EN50221_APP_CA_RESOURCEID);
	Resources[2].binary_resource_id = EN50221_APP_CA_RESOURCEID;
	Resources[2].callback = en50221_app_ca_message;
	Resources[2].arg = CaResource;

	// register session layer callbacks
	en50221_sl_register_lookup_callback(SessionLayer, llci_lookup_callback, this);
	en50221_sl_register_session_callback(SessionLayer, llci_session_callback, this);

	return true;
}

// class DvbCamCamHandlerHLCI

DvbCamCamHandlerHLCI::DvbCamCamHandlerHLCI()
{
}

DvbCamCamHandlerHLCI::~DvbCamCamHandlerHLCI()
{
}

int DvbCamCamHandlerHLCI::hlci_send_data(void *arg, uint16_t /*session_number*/, uint8_t *data, uint16_t data_length)
{
	return dvbca_hlci_write(static_cast<int> (reinterpret_cast<intptr_t> (arg)), data, data_length);
}

int DvbCamCamHandlerHLCI::hlci_send_datav(void *arg, uint16_t /*session_number*/, iovec *vector, int iov_count)
{
	// calculate the total length of the data to send
	uint32_t data_size = 0;
	for(int i = 0; i < iov_count; ++i) {
		data_size += vector[i].iov_len;
	}

	// allocate memory for it
	uint8_t *buf = new uint8_t[data_size];

	// merge the iovecs
	uint8_t *pos = buf;
	for(int i = 0; i < iov_count; ++i) {
		memcpy(pos, vector[i].iov_base, vector[i].iov_len);
		pos += vector[i].iov_len;
	}

	// send it
	int status = dvbca_hlci_write(static_cast<int> (reinterpret_cast<intptr_t> (arg)), buf, data_size);
	delete buf;
	return status;
}

void DvbCamCamHandlerHLCI::poll()
{
	// we do nothing here for the moment
	usleep(100000); // 100 ms
}

bool DvbCamCamHandlerHLCI::registerCam(int ca_fd, uint8_t /*slot*/)
{
	SendFuncs.arg = reinterpret_cast<void *> (ca_fd);

	// get application information
	if(en50221_app_ai_enquiry(AiResource, 0)) {
		fprintf(stderr, "CamThread: [DEBUG #1]\n");
		return false;
	}

	uint8_t buf[256];
	int size = dvbca_hlci_read(ca_fd, TAG_APP_INFO, buf, sizeof(buf));
	if(size <= 0) {
		fprintf(stderr, "CamThread: [DEBUG #2]\n");
		return false;
	}

	if(en50221_app_ai_message(AiResource, 0, 0, EN50221_APP_AI_RESOURCEID, buf, size)) {
		fprintf(stderr, "CamThread: [DEBUG #3]\n");
		return false;
	}

	// FIXME: try to change this soon
	buf[0] = TAG_CA_INFO >> 16;
	buf[1] = TAG_CA_INFO >> 8;
	buf[2] = TAG_CA_INFO;
	buf[3] = 0;
	if(en50221_app_ca_message(CaResource, 0, 0, EN50221_APP_CA_RESOURCEID, buf, 4)) {
		fprintf(stderr, "CamThread: [DEBUG #4]\n");
		return false;
	}

	fprintf(stderr, "CamThread: [DEBUG #5]\n");

	return true;
}

bool DvbCamCamHandlerHLCI::sub_init()
{
	// create sendfuncs
	SendFuncs.arg        = NULL;
	SendFuncs.send_data  = hlci_send_data;
	SendFuncs.send_datav = hlci_send_datav;

	return true;
}

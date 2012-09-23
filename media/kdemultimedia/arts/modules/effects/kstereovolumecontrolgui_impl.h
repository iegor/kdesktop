/*

    Copyright ( C ) 2003 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#ifndef ARTS_STEREOVOLUMECONTROL_GUI_H
#define ARTS_STEREOVOLUMECONTROL_GUI_H

#include <artsmoduleseffects.h>

#include <klayoutbox_impl.h>

#include <kdebug.h>

class KStereoVolumeControlGui_EventMapper;

namespace Arts { // namespace Arts

class KStereoVolumeControlGui_impl : virtual public Arts::StereoVolumeControlGui_skel,
                                     virtual public Arts::KLayoutBox_impl
{
protected:
	Arts::StereoVolumeControl _svc;
	Arts::LevelMeter _left, _right;
	Arts::Tickmarks _tickmarks;
	Arts::Tickmarks _fadertickmarks;
	Arts::VolumeFader _volumefader;
	Arts::Label _label;
	KStereoVolumeControlGui_EventMapper* _mapper;
	float _dbmin, _dbmax;
	Arts::Direction _dir;
public:
	KStereoVolumeControlGui_impl( QFrame* =0 );

	void constructor( Arts::StereoVolumeControl );

	Arts::Direction direction();
	void direction( Arts::Direction );
	std::string title();
	void title( const std::string& );
	float dbmin();
	void dbmin( float );
	float dbmax();
	void dbmax( float );

	Arts::LevelMeter left() { return _left; }
	Arts::LevelMeter right() { return _right; }
	Arts::VolumeFader fader() { return _volumefader; }
	Arts::Tickmarks levelmetertickmarks() { return _tickmarks; }
	Arts::Tickmarks volumefadertickmarks() { return _fadertickmarks; }
	Arts::Label label() { return _label; }

	void couple( bool ) {}
	bool couple() { return true; }

	void updateValues();
private:
	void allWidgets( Arts::Direction );
}; // class StereoVolumeControlGui

} // namespace Arts

#include <qobject.h>
#include <qtimer.h>

class KStereoVolumeControlGui_EventMapper : public QObject {
   Q_OBJECT
public:
	QTimer* _timer;
	Arts::KStereoVolumeControlGui_impl* _impl;
public:
	KStereoVolumeControlGui_EventMapper( Arts::KStereoVolumeControlGui_impl* impl, QObject* parent, const char* name=0 ) : QObject( parent,name ), _impl( impl ) {
		_timer = new QTimer( this );
		connect( _timer, SIGNAL( timeout() ), this, SLOT( slotTimerSignal() ) );
	}
public slots:
	void slotTimerSignal() { _impl->updateValues(); }
};

#endif
// vim: sw=4 ts=4

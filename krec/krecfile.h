/***************************************************************************
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#ifndef KRECFILE_H
#define KRECFILE_H

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <arts/common.h>

class KRecBuffer;
class KTempDir;
class KSimpleConfig;
class QFile;
class KRecFileViewWidget;

class KRecFile : virtual public QObject {
   Q_OBJECT
	friend class KRecFileWidget;
public:
	KRecFile( QObject*, const char* =0 );
	KRecFile( const QString &, QObject*, const char* =0 );
	~KRecFile();

	QString filename();
	void filename( const QString &);

	bool saved() const { return _saved; }
	int samplerate() const { return _samplerate; }
	int channels() const { return _channels; }
	int bits() const { return _bits; }

	QIODevice::Offset offsetSize() const { return samplesToOffset( _size ); }
	QIODevice::Offset offsetPosition() const { return samplesToOffset( _pos ); }

	int size() const { return _size; }
	int position() const { return _pos; }

	/// Calculates an offset into a frames-value depending on the files settings (channels and bits)
	int offsetToSamples( QIODevice::Offset ) const;
	QIODevice::Offset samplesToOffset( int ) const;
public slots:
	/// Creates a new empty buffer and marks it for recording.
	void newBuffer();

	/// Deletes the buffer from the file.
	void deleteBuffer( KRecBuffer* =0 );

	void writeData( Arts::mcopbyte* data, uint length );
	void writeData( QByteArray* );
	void writeData( QByteArray& );

	void save( const QString & );
	void exportwave( const QString & );

	/// Fills a QByteArray with values from the specified Offset.
	QByteArray* getData( int );

	void getData( QByteArray& );

	void newPos( int );

private slots:
	void newPos( KRecBuffer*, QIODevice::Offset );
	void newSize( KRecBuffer*, QIODevice::Offset );
signals:
	/// Position signals
	void posChanged( int );
	void sizeChanged( int );
	/// The end of the file is reached, you should stop reading.
	void endReached();

	/// GUI-Signals
	void sNewBuffer( KRecBuffer* );
	void sDeleteBuffer( KRecBuffer* );

	void filenameChanged( const QString & );
private:
	void saveProps();
	void loadProps();
	/**
		Creates a new buffer for a given file
		@param filename Name of the file
	*/
	void newBuffer( const QString &filename );
	/**
		Adds buffer to the buffers.
		@param buffer The to be added buffer
	*/
	void newBuffer( KRecBuffer* buffer );

	/// Get the top-most-buffer at the specified position.
	int getTopBuffer_int( int );
	KRecBuffer* getTopBuffer_buffer( int );

	bool _saved;
	QString _filename;
	int _samplerate, _channels, _bits;
	int _currentBuffer;
	QValueList<KRecBuffer*> _buffers;
	KTempDir *_dir;
	KSimpleConfig *_config;
	int _pos, _size;

	void init();
};

class QFile;
class QDir;
class QFileInfo;
class KConfig;
class QDataStream;

class KRecBuffer : virtual public QObject {
   Q_OBJECT
public:
	KRecBuffer( const QString &, int, bool, KRecFile*, const char* =0 );
	~KRecBuffer();

	// After how many samples in the File this Buffer starts...
	int startpos() const;

	QIODevice::Offset size() const;
	int sizeInSamples() const { return _krecfile->offsetToSamples( size() ); }

	/**
		Writes out its Config
		It doesn't set the group.
		@param config the KConfig where the data gets written
	*/
	void writeConfig( KConfig* config );

	/**
		restores a KRecBuffer from Config
		The group has to be set the right way.
		@param config The configuration thats read
		@param dir The directory where the file is saved
	*/
	static KRecBuffer* fromConfig( KConfig* config, QDir* dir, KRecFile* p, const char* n=0 );

	QString filename() const;

	bool active() const;

	QString title() const;
	QString comment() const;
public slots:
	/// writes the data into the buffer
	void writeData( Arts::mcopbyte* data, uint length );
	void writeData( QByteArray* );
	void writeData( QByteArray& );
	/// gets the data from the stream
	void getData( QByteArray& );

	void setPos( QIODevice::Offset );

	void setActive( bool );

	void deleteBuffer();

	void setTitle( const QString & );
	void setComment( const QString & );

	/// Returns the sample at the specified position and channel.
	float getSample( int pos, int channel );
	float* getsamples( int start, int end, int channel );
signals:
	void posChanged( KRecBuffer*, QIODevice::Offset );
	void sizeChanged( KRecBuffer*, QIODevice::Offset );
	void activeChanged( bool );

	/// Is emitted when something has changed.
	void somethingChanged();

	void deleteSelf( KRecBuffer* );
private:
	KRecFile* _krecfile;
	QFile* _file;
	QDataStream* _stream;
	QFileInfo* _fileinfo;
	bool _open, _active;
	QIODevice::Offset _pos;
	int _start;
	QString _title, _comment;
};

#endif

// vim:sw=4:ts=4

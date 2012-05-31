
#ifndef _LIRC_H_
#define _LIRC_H_

#include <qobject.h>
#include <qstringlist.h>
#include <qmap.h>

class QSocket;

typedef QMap<QString, QStringList> Remotes;

class Lirc : public QObject
{
Q_OBJECT
public:
	/**
	 * Constructor
	 */
	Lirc(QObject *parent);
	/**
	 * Destructor
	 */
	virtual ~Lirc();

	/**
	 * Returns true if the connection to lircd is operational
	 */
	bool isConnected() const { return m_socket; }
	/**
	 * The names of the remote configured controls
	 */
	const QStringList remotes() const;
	/**
	 * The names of the buttons for the specified
	 * remote control
	 */
	const QStringList buttons(const QString &remote) const
	{
			return m_remotes[remote];
	}

signals:
	/**
	 * Emitted when the list of controls / buttons was cmpletely read
	 */
	void remotesRead();
	/**
	 * Emitted when a IR command was received
	 * 
	 * The arguments are the name of the remote control used,
	 * the name of the button pressed and the repeat counter.
	 *
	 * The signal is emitted repeatedly as long as the button
	 * on the remote control remains pressed.
	 * The repeat counter starts with 0 and increases
	 * every time this signal is emitted.
	 */
	void commandReceived(const QString &, const QString &, int);
	
private slots:
	void slotRead();
	
private:
	void update();
	const QString readLine();
	void sendCommand(const QString &);
	
private:
	QSocket *m_socket;
	Remotes m_remotes;
};

#endif


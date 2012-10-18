
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>

#include <qsocket.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "lirc.h"

Lirc::Lirc(QObject *parent)
	: QObject(parent),
	  m_socket(0)
{
	int sock = ::socket(PF_UNIX, SOCK_STREAM, 0);
	if (sock == -1)
	{
		KMessageBox::sorry(0, i18n("Could not create a socket to receive infrared signals. The error is:\n") + strerror(errno));
		return;
	}
	sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "/dev/lircd");
	if (::connect(sock, (struct sockaddr *)(&addr), sizeof(addr)) == -1)
	{
		KMessageBox::sorry(0, i18n("Could not establish a connection to receive infrared signals. The error is:\n") + strerror(errno));
		::close(sock);
		return;
	}
	
	m_socket = new QSocket;
	m_socket->setSocket(sock);
	connect(m_socket, SIGNAL(readyRead()), SLOT(slotRead()));
	update();
}

Lirc::~Lirc()
{
	delete m_socket;
}

const QStringList Lirc::remotes() const
{
	QStringList result;
	for (Remotes::ConstIterator it = m_remotes.begin(); it != m_remotes.end(); ++it)
		result.append(it.key());
	result.sort();
	return result;
}

void Lirc::slotRead()
{
	while (m_socket->bytesAvailable())
	{
		QString line = readLine();
		if (line == "BEGIN")
		{
			// BEGIN
			// <command>
			// [SUCCESS|ERROR]
			// [DATA
			// n
			// n lines of data]
			// END
			line = readLine();
			if (line == "SIGHUP")
			{
				// Configuration changed
				do line = readLine();
				while (!line.isEmpty() && line != "END");
				update();
				return;
			}
			else if (line == "LIST")
			{
				// remote control list
				if (readLine() != "SUCCESS" || readLine() != "DATA")
				{
					do line = readLine();
					while (!line.isEmpty() && line != "END");
					return;
				}
				QStringList remotes;
				int count = readLine().toInt();
				for (int i = 0; i < count; ++i)
					remotes.append(readLine());
				do line = readLine();
				while (!line.isEmpty() && line != "END");
				if (line.isEmpty())
					return; // abort on corrupt data
				for (QStringList::ConstIterator it = remotes.begin(); it != remotes.end(); ++it)
					sendCommand("LIST " + *it);
				return;
			}
			else if (line.left(4) == "LIST")
			{
				// button list
				if (readLine() != "SUCCESS" || readLine() != "DATA")
				{
					do line = readLine();
					while (!line.isEmpty() && line != "END");
					return;
				}
				QString remote = line.mid(5);
				QStringList buttons;
				int count = readLine().toInt();
				for (int i = 0; i < count; ++i)
				{
					// <code> <name>
					QString btn = readLine();
					buttons.append(btn.mid(17));
				}
				m_remotes.insert(remote, buttons);
			}
			do line = readLine();
			while (!line.isEmpty() && line != "END");
			emit remotesRead();
		}
		else
		{
			// <code> <repeat> <button name> <remote control name>
			line.remove(0, 17); // strip code
			int pos = line.find(' ');
			if (pos < 0)
				return;
			bool ok;
			int repeat = line.left(pos).toInt(&ok, 16);
			if (!ok)
				return;
			line.remove(0, pos + 1);
			
			pos = line.find(' ');
			if (pos < 0)
				return;
			QString btn = line.left(pos);
			line.remove(0, pos + 1);

			emit commandReceived(line, btn, repeat);
		}
	}
}

void Lirc::update()
{
	m_remotes.clear();
	sendCommand("LIST");
}

const QString Lirc::readLine()
{
	if (!m_socket->bytesAvailable())
		return QString::null;

	QString line = m_socket->readLine();
	if (line.isEmpty())
		return QString::null;

	line.remove(line.length() - 1, 1);
	return line;
}

void Lirc::sendCommand(const QString &command)
{
	QString cmd = command + "\n";
	m_socket->writeBlock(cmd.latin1(), cmd.length());
}

#include "lirc.moc"


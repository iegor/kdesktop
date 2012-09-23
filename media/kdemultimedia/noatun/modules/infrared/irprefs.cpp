
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <noatun/app.h>

#include <kdialog.h>
#include <klocale.h>
#include <kconfig.h>
#include <klistview.h>
#include <kcombobox.h>
#include <knuminput.h>

#include "irprefs.h"
#include "lirc.h"

class CommandItem : public QListViewItem
{
public:
	CommandItem(QListViewItem *remote, const QString &name,
	            IRPrefs::Action action, int interval)
		: QListViewItem(remote, name, IRPrefs::actionName(action),
		                interval ? QString().setNum(interval) : QString::null),
		  m_name(remote->text(0) + "::" + name),
		  m_action(action),
		  m_interval(interval)
	{
	}

	const QString &name() const { return m_name; }
	IRPrefs::Action action() const { return m_action; }
	int interval() const { return m_interval; }
	void setAction(IRPrefs::Action action)
	{
		setText(1, IRPrefs::actionName(action));
		m_action = action;
	}
	void setInterval(int interval)
	{
		setText(2, interval ? QString().setNum(interval) : QString::null);
		m_interval = interval;
	}

private:
	QString m_name;
	IRPrefs::Action m_action;
	int m_interval;
};

Lirc *IRPrefs::s_lirc = 0;
bool IRPrefs::s_configRead = false;
QMap<QString, IRPrefs::Command> IRPrefs::s_commands;

IRPrefs::IRPrefs(QObject *parent)
	: CModule(i18n("Infrared Control"), i18n("Configure Infrared Commands"), "remote", parent)
{
	QGridLayout *layout = new QGridLayout(this, 3, 5, KDialog::marginHint(), KDialog::spacingHint());
	layout->setColStretch(1, 1);

	QLabel *label = new QLabel(i18n("Remote control &commands:"), this);
	layout->addMultiCellWidget(label, 0, 0, 0, 4);

	label->setBuddy(m_commands = new KListView(this));
	layout->addMultiCellWidget(m_commands, 1, 1, 0, 4);

	label = new QLabel(i18n("&Action:"), this);
	layout->addWidget(label, 2, 0);

	label->setBuddy(m_action = new KComboBox(this));
	m_action->setEnabled(false);
	layout->addWidget(m_action, 2, 1);

	m_repeat = new QCheckBox(i18n("&Repeat"), this);
	m_repeat->setEnabled(false);
	layout->addWidget(m_repeat, 2, 2);

	label = new QLabel(i18n("&Interval:"), this);
	layout->addWidget(label, 2, 3);

	label->setBuddy(m_interval = new KIntSpinBox(this));
	m_interval->setMinValue(1);
	m_interval->setMaxValue(0xff);
	m_interval->setValue(10);
	m_interval->setEnabled(false);
	layout->addWidget(m_interval, 2, 4);

	connect(s_lirc, SIGNAL(remotesRead()), SLOT(reopen()));
	connect(m_commands,
	        SIGNAL(selectionChanged(QListViewItem *)),
			SLOT(slotCommandSelected(QListViewItem *)));
	connect(m_action,
	        SIGNAL(activated(int)),
			SLOT(slotActionActivated(int)));
	connect(m_repeat,
	        SIGNAL(toggled(bool)),
			SLOT(slotRepeatToggled(bool)));
	connect(m_interval,
	        SIGNAL(valueChanged(int)),
			SLOT(slotIntervalChanged(int)));

	reopen();
}

void IRPrefs::save()
{
	KConfig *c = kapp->config();
	KConfigGroupSaver groupSaver(c, "Infrared");
	c->writeEntry("CommandCount", s_commands.count());
	int i = 1;
	for (QMap<QString, Command>::ConstIterator it = s_commands.begin(); it != s_commands.end(); ++it)
	{
		c->writePathEntry(QString("Command_%1").arg(i), it.key());
		c->writeEntry(QString("Action_%1").arg(i), (int)((*it).action));
		c->writeEntry(QString("Interval_%1").arg(i), (*it).interval);
		++i;
	}
}

void IRPrefs::reopen()
{
	readConfig();

	QStringList remotes = s_lirc->remotes();
	m_commands->clear();
	while (m_commands->columns()) m_commands->removeColumn(0);
	if (!remotes.count())
	{
		m_commands->addColumn(i18n("Sorry"));
		m_commands->setSorting(-1);
		if (s_lirc->isConnected())
		{
			new QListViewItem(m_commands, i18n("You do not have any remote control configured."));
			new QListViewItem(m_commands, i18n("Please make sure lirc is setup correctly."));
		}
		else
		{
			new QListViewItem(m_commands, i18n("Connection could not be established."));
			new QListViewItem(m_commands, i18n("Please make sure lirc is setup correctly and lircd is running."));
		}
		m_commands->setEnabled(false);
		return;
	}
	m_commands->setEnabled(true);
	m_commands->addColumn(i18n("Button"));
	m_commands->addColumn(i18n("Action"));
	m_commands->addColumn(i18n("Interval"));
	m_commands->setSorting(0);
	for (QStringList::ConstIterator it = remotes.begin(); it != remotes.end(); ++it)
	{
		QListViewItem *remote = new QListViewItem(m_commands, *it);
		const QStringList &buttons = s_lirc->buttons(*it);
		for (QStringList::ConstIterator btn = buttons.begin(); btn != buttons.end(); ++btn)
		{
			QString key = *it + "::" + *btn;
			if (s_commands.contains(key))
				new CommandItem(remote, *btn, s_commands[key].action, s_commands[key].interval);
			else
				new CommandItem(remote, *btn, None, 0);
		}
		remote->setOpen(true);
	}

	m_action->clear();
	for (int i = 0; ; ++i)
	{
		QString action = actionName((Action)i);
		if (action.isNull())
			break;
		if (action.isEmpty())
			m_action->insertItem(i18n("None"));
		else
			m_action->insertItem(action);
	}


}

void IRPrefs::slotCommandSelected(QListViewItem *item)
{
	CommandItem *cmd = dynamic_cast<CommandItem *>(item);
	if (cmd)
	{
		m_action->setCurrentItem((int)(cmd->action()));
		m_repeat->setChecked(cmd->interval());
		if (cmd->interval())
			m_interval->setValue(cmd->interval());
		else
		{
			m_interval->setValue(10);
			cmd->setInterval(0); // HACKHACKHACK
		}
		m_action->setEnabled(true);
		m_repeat->setEnabled(cmd->action() != None);
		m_interval->setEnabled(cmd->interval());
	}
	else
	{
		m_action->setEnabled(false);
		m_repeat->setEnabled(false);
		m_interval->setEnabled(false);
	}
}

void IRPrefs::slotActionActivated(int action)
{
	CommandItem *cmd = dynamic_cast<CommandItem *>(m_commands->currentItem());
	if (!cmd)
		return; // Shouldn't happen
	cmd->setAction((Action)action);
	if (cmd->action() == None)
	{
		cmd->setInterval(0);
		m_repeat->setChecked(false);
		m_repeat->setEnabled(false);
		m_interval->setEnabled(false);
	}
	else
	{
		m_repeat->setEnabled(true);
		m_interval->setEnabled(cmd->interval());
	}
	s_commands[cmd->name()].action = cmd->action();
	s_commands[cmd->name()].interval = 0;
}

void IRPrefs::slotRepeatToggled(bool value)
{
	CommandItem *cmd = dynamic_cast<CommandItem *>(m_commands->currentItem());
	if (!cmd)
		return; // Shouldn't happen
	cmd->setInterval(value ? 10 : 0);
	s_commands[cmd->name()].interval = cmd->interval();
	m_interval->setEnabled(value);
}

void IRPrefs::slotIntervalChanged(int value)
{
	CommandItem *cmd = dynamic_cast<CommandItem *>(m_commands->currentItem());
	if (!cmd)
		return; // Shouldn't happen
	cmd->setInterval(value);
	s_commands[cmd->name()].interval = cmd->interval();
}

const QString IRPrefs::actionName(Action action)
{
	switch (action)
	{
		case None:
			return QString("");
		case Play:
			return i18n("Play");
		case Stop:
			return i18n("Stop");
		case Previous:
			return i18n("Back");
		case Next:
			return i18n("Next");
		case VolumeDown:
			return i18n("Volume Down");
		case VolumeUp:
			return i18n("Volume Up");
		case Mute:
			return i18n("Mute");
		case Pause:
			return i18n("Pause");
		case SeekBackward:
			return i18n("Seek Backward");
		case SeekForward:
			return i18n("Seek Forward");
		case ShowPlaylist:
			return i18n("Show Playlist");
		case NextSection:
			return i18n("Next Section");
		case PreviousSection:
			return i18n("Previous Section");
	}
	return QString::null;
}

void IRPrefs::readConfig()
{
	if (s_configRead)
		return;
	KConfig *c = kapp->config();
	KConfigGroupSaver groupSaver(c, "Infrared");
	int count = c->readNumEntry("CommandCount");
	for (int i = 1; i <= count; ++i)
	{
		Command cmd;
		cmd.action = (Action)(c->readNumEntry(QString("Action_%1").arg(i)));
		cmd.interval = c->readNumEntry(QString("Interval_%1").arg(i));
		s_commands.insert(c->readPathEntry(QString("Command_%1").arg(i)), cmd);
	}
	s_configRead = true;
}

IRPrefs::Action IRPrefs::actionFor(const QString &remote, const QString &button, int repeat)
{
	readConfig();
	Command cmd = s_commands[remote + "::" + button];
	if ((cmd.interval == 0 && repeat == 0)
		|| (cmd.interval != 0 && repeat % cmd.interval == 0))
		return cmd.action;
	else
		return None;
}

#include "irprefs.moc"



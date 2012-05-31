#include "pref.h"

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <qlayout.h>
//#include <qlabel.h>
#include "cmodule.h"

NoatunPreferences::NoatunPreferences(QWidget *parent)
    : KDialogBase(TreeList, i18n("Preferences - Noatun"),
                  Ok|Apply|Cancel|Help, Ok, parent, "NoatunPreferences", false, true)
{
	resize(640, 480); // KDE is required to support 800x600 min.
	setShowIconsInTreeList(true);
	setRootIsDecorated(false);
}

void NoatunPreferences::slotOk()
{
	slotApply();
	hide();
}

void NoatunPreferences::show()
{
	for (CModule *i=mModules.first(); i != 0; i=mModules.next())
		i->reopen();
	KDialogBase::show();
}

void NoatunPreferences::show(CModule *page)
{
	int index = pageIndex( static_cast<QWidget *>(page->parent()) );
	if (index != -1)
		showPage(index);
	show();
}

void NoatunPreferences::slotApply()
{
	for (CModule *i=mModules.first(); i != 0; i=mModules.next())
		i->save();
}

void NoatunPreferences::add(CModule *page)
{
	mModules.append(page);
}

void NoatunPreferences::remove(CModule *page)
{
	mModules.removeRef(page);
}

CModule::CModule(const QString &name, const QString &description, const QString &icon, QObject *owner)
	: QWidget(napp->preferencesBox()->addPage(name, description, KGlobal::iconLoader()->loadIcon(
		icon, KIcon::Small,0, KIcon::DefaultState,0, true)))
{
	if (owner)
		connect(owner, SIGNAL(destroyed()), SLOT(ownerDeleted()));

	//kdDebug(66666) << k_funcinfo << "name = " << name << endl;

	napp->preferencesBox()->add(this);

	QFrame *page=static_cast<QFrame*>(parent());
	(new QHBoxLayout(page))->addWidget(this);
}

CModule::~CModule()
{
	//kdDebug(66666) << k_funcinfo << endl;
#if QT_VERSION < 0x030102 && KDE_VERSION < KDE_MAKE_VERSION( 3, 1, 90 )
	// Due to a bug in Qt 3.1 and 3.1.1 no close events are sent to hidden
	// widgets, causing the KJanusWidget to crash. This workaround is
	// rather intrusive and should be used only in the affected versions
	// to avoid hard to track bugs in the future. KDE HEAD (to become 3.2)
	// has a workaround for this problem, and additionally it's fixed in
	// Qt 3.1.2.
 	napp->sendPostedEvents();
#endif

	napp->preferencesBox()->remove(this);
}

void CModule::ownerDeleted()
{
	QObject *p=parent();
	delete this;
	p->deleteLater();
}

#include "pref.moc"

#include "effects.h"
#include "engine.h"
#include <common.h>
#include <dynamicrequest.h>
#include <artsflow.h>
#include <app.h>
#include <player.h>
#include <soundserver.h>
#include <noatunarts.h>
#include <qlayout.h>

#include <config.h>

#define HAS_ARTSVERSION_H

#ifdef HAS_ARTSVERSION_H
#include <artsgui.h>
#include <kartswidget.h>
#endif

#define engine napp->player()->engine()
#define server (*(engine->server()))
#define stack (*engine->effectStack())

using namespace std;
using namespace Arts;

class EffectConfigWidget : public QWidget
{
public:
	EffectConfigWidget(Effect *e, QWidget *parent=0)
		: QWidget(parent), mEf(e)
	{}

	virtual ~EffectConfigWidget()
	{
		mEf->mConfig=0;
	}

private:
	Effect *mEf;
};


Effect::Effect(const char *name)
	: mId(0), mName(name), mConfig(0)
{
	mEffect=new StereoEffect;
	*mEffect=DynamicCast(server.createObject(std::string(name)));
	napp->effects()->mItems.append(this);
}

long Effect::id() const
{
	return mId;
}

StereoEffect *Effect::effect() const
{
	return mEffect;
}

Effect *Effect::after() const
{
	QPtrList<Effect> effects=napp->effects()->effects();
	QPtrListIterator<Effect> i(effects);
	for(; i.current(); ++i)
		if ((*i)->id()==mId)
		{
			++i;
			if (*i)
				return *i;
		}

	return 0;
}

Effect *Effect::before() const
{
	QPtrList<Effect> effects=napp->effects()->effects();
	QPtrListIterator<Effect> i(effects);
	for(; i.current(); ++i)
		if ((*i)->id()==mId)
		{
			--i;
			if (*i)
				return *i;
		}

	return 0;
}

QCString Effect::name() const
{
	return mName;
}

QString Effect::title() const
{
	return clean(mName);
}

QString Effect::clean(const QCString &name)
{
	int pos=name.findRev("::");
	if (pos>0)
		return name.right(name.length()-pos-2);
	return name;
}

bool Effect::isNull() const
{
	return effect()->isNull();
}

QWidget *Effect::configure(bool /*friendly*/)
{
#ifdef HAS_ARTSVERSION_H
	if (mConfig) return mConfig;
	if (!configurable()) return 0;

	GenericGuiFactory factory;
	Widget gui = factory.createGui(*mEffect);

	if(!gui.isNull())
	{
		mConfig=new EffectConfigWidget(this);
		mConfig->setCaption(title());

		QBoxLayout *l=new QHBoxLayout(mConfig);
		l->add(new KArtsWidget(gui, mConfig));
		l->freeze();
	}

	return mConfig;
#else
	return 0;
#endif
}

bool Effect::configurable() const
{
#ifdef HAS_ARTSVERSION_H
	TraderQuery query;
	query.supports("Interface","Arts::GuiFactory");
	query.supports("CanCreate", mEffect->_interfaceName());

	vector<TraderOffer> *queryResults = query.query();
	bool yes= queryResults->size();
	delete queryResults;

	return yes;
#else
	return 0;
#endif
}

Effect::~Effect()
{
	delete mConfig;
	napp->effects()->remove(this, false);
	emit napp->effects()->deleting(this);
	delete mEffect;
	napp->effects()->mItems.removeRef(this);
}


Effects::Effects()
{
	mItems.setAutoDelete(false);
}

bool Effects::insert(const Effect *after, Effect *item)
{
	if (!item) return false;
	if (item->id()) return false;
	if (item->isNull()) return false;
	long i;
	item->effect()->start();

	if (!after)
		i=stack.insertTop(*item->effect(), (const char*)item->name());
	else
		i=stack.insertAfter(after->id(), *item->effect(), (const char*)item->name());
	if (!i)
	{
		item->effect()->stop();
		return false;
	}

	item->mId=i;
	emit added(item);
	return true;
}

bool Effects::append(Effect *item)
{
	if (!item) return false;
	if (item->id()) return false;
	if (item->isNull()) return false;

	item->effect()->start();
	item->mId=stack.insertBottom(*item->effect(), (const char*)item->name());
	if (!item->mId)
	{
		item->effect()->stop();
		return false;
	}
	emit added(item);
	return true;
}

void Effects::move(const Effect *after, Effect *item)
{
	if (!item) return;
	if (!item->id()) return;
	long id=after ? after->id() : 0;
	stack.move(id, item->id());
	emit moved(item);
}

void Effects::remove(Effect *item, bool del)
{
	if (!item) return;
	if (!item->id()) return;

	stack.remove(item->id());
	item->effect()->stop();
	item->mId=0;
	removed(item);

	if (del)
		delete item;
}

void Effects::removeAll(bool del)
{
	for (QPtrListIterator<Effect> i(mItems); i.current(); ++i)
		if ((*i)->id())
			remove(*i, del);
}

QStrList Effects::available() const
{
	QStrList val;
	Arts::TraderQuery query;
	query.supports("Interface", "Arts::StereoEffect");
	query.supports("Interface", "Arts::SynthModule");
	query.supports("Use", "directly");
	vector<Arts::TraderOffer> *offers = query.query();
	for (vector<Arts::TraderOffer>::iterator i=offers->begin(); i!=offers->end(); i++)
	{
		Arts::TraderOffer &offer=*i;
		QCString name = offer.interfaceName().c_str();
		val.append(name);
	}
	delete offers;
	return val;
}

Effect *Effects::findId(long id) const
{
	for (QPtrListIterator<Effect> i(mItems); i.current(); ++i)
		if ((*i)->id()==id)
			return *i;
	return 0;
}

QPtrList<Effect> Effects::effects() const
{
	vector<long> *items=stack.effectList();
	QPtrList<Effect> effects;
	for (vector<long>::iterator i=items->begin();i!=items->end();i++)
		if (Effect *e=findId(*i))
			effects.append(e);

	delete items;
	return effects;
}

#undef server
#undef stack
#undef engine

#include "effects.moc"

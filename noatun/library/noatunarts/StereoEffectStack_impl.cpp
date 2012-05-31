#include "noatunarts.h"

#include <artsflow.h>
#include <flowsystem.h>
#include <stdsynthmodule.h>
#include <debug.h>

using namespace std;
using namespace Arts;

namespace Noatun
{
class StereoEffectStack_impl : public StereoEffectStack_skel, public StdSynthModule
{
	public:
		long nextID;

		struct EffectEntry
		{
			StereoEffect effect;
			string name;
			long id;
		};
		list<EffectEntry *> fx;

		void xconnect(bool connect, Object from, string fromP, Object to, string toP)
		{
			if(connect)
				from._node()->connect(fromP,to._node(),toP);
			else
				from._node()->disconnect(fromP,to._node(),toP);
		}
		
		void xvirtualize(bool connect, string myPort, Object impl, string implPort)
		{
			if(connect)
				_node()->virtualize(myPort,impl._node(),implPort);
			else
				_node()->devirtualize(myPort,impl._node(),implPort);
		}
		
		void internalconnect(bool c)
		{
			if(fx.empty())
			{
				/* no effects - forward input through to output */
				xvirtualize(c,"outleft",Object::_from_base(this->_copy()),"inleft");
				xvirtualize(c,"outright",Object::_from_base(this->_copy()),"inright");
			}
			else
			{
				list<EffectEntry *>::iterator ei;
				EffectEntry *laste = 0;

				long count = 0;
				for(ei = fx.begin(); ei != fx.end(); ei++, count++)
				{
					EffectEntry *e = *ei;
					if(count == 0)		/* top of chain? virtualize to effect */
					{
						xvirtualize(c,"inleft",e->effect,"inleft");
						xvirtualize(c,"inright",e->effect,"inright");
					}
					else				/* not top? connect last effect to current effect */
					{
						xconnect(c,laste->effect,"outleft",e->effect,"inleft");
						xconnect(c,laste->effect,"outright",e->effect,"inright");
					}
					laste = e;
				}
				/* end: virtualize effect output to our output */
				xvirtualize(c,"outleft",laste->effect,"outleft");
				xvirtualize(c,"outright",laste->effect,"outright");
			}
		}
		void disconnect() { internalconnect(false); }
		void reconnect()  { internalconnect(true); }


		long insertAfter(long after, StereoEffect effect, const string &name)
		{
			arts_return_val_if_fail(!effect.isNull(),0);
			disconnect();

			list<EffectEntry*>::iterator i = fx.begin();

			bool found=false;
			// seek through until we find 'after'
			while(i != fx.end())
				if((*i)->id == after)
				{
					found = true;
					break;
				}
				else
					i++;

			long newId=0;
			if (found)
			{
				i++;
				EffectEntry *e = new EffectEntry;
				e->effect=effect;
				e->name=name;
				e->id=nextID++;
				fx.insert(i, e);
				newId=e->id;
			}
			else
				arts_warning("StereoEffectStack::insertAfter failed. "
						"id %d not found?", after);

			reconnect();
			return newId;

		}

		void move(long after, long item)
		{
			arts_return_if_fail(item != 0);
			disconnect();

			list<EffectEntry*>::iterator iAfter=fx.begin();
			bool found=false;
			if (after)
				while(iAfter != fx.end())
					if((*iAfter)->id == after)
					{
						found = true;
						iAfter++;
						break;
					}
					else
						iAfter++;
			else
				found=true;

			list<EffectEntry*>::iterator iItem=fx.begin();
			while (iItem != fx.end())
				if((*iItem)->id == item)
				{
					found &= true;
					break;
				}
				else
					iItem++;
			if (!found)
				arts_warning("StereoEffectStack::move couldn't find items");
			else
			{
				fx.insert(iAfter, *iItem);
				fx.erase(iItem);
			}

			reconnect();

		}

		vector<long> *effectList()
		{
			vector<long> *items=new vector<long>;
			for (list<EffectEntry*>::iterator i=fx.begin(); i!=fx.end();i++)
				items->push_back((*i)->id);
			return items;
		}

		// as stolen from stereoeffectstack_impl.cc
		StereoEffectStack_impl() : nextID(1)
		{
			reconnect();
		}
		
		~StereoEffectStack_impl()
		{
			// disconnect remaining effects
			EffectEntry *laste = 0;
			list<EffectEntry *>::iterator ei;

			for(ei = fx.begin(); ei != fx.end(); ei++)
			{
				EffectEntry *e = *ei;
				if(laste)
				{
					xconnect(false,laste->effect,"outleft",e->effect,"inleft");
					xconnect(false,laste->effect,"outright",e->effect,"inright");
				}
				laste = e;
			}
			// delete remaining effect entries
			for(ei = fx.begin(); ei != fx.end(); ei++)
				delete *ei;
			fx.clear();
		}
		long insertTop(StereoEffect effect, const string& name)
		{
			arts_return_val_if_fail(!effect.isNull(),0);

			disconnect();
			EffectEntry *e = new EffectEntry();
			e->effect = effect;
			e->name = name;
			e->id = nextID++;
			fx.push_front(e);
			reconnect();
			return e->id;
		}
		long insertBottom(StereoEffect effect, const string& name)
		{
			arts_return_val_if_fail(!effect.isNull(),0);

			disconnect();
			EffectEntry *e = new EffectEntry();
			e->effect = effect;
			e->name = name;
			e->id = nextID++;
			fx.push_back(e);
			reconnect();
			return e->id;
		}

		void remove(long ID)
		{
			arts_return_if_fail(ID != 0);

			bool found = false;
			disconnect();
			list<EffectEntry *>::iterator ei = fx.begin();

			while(ei != fx.end())
			{
				if((*ei)->id == ID) {
					found = true;
					delete (*ei);
					fx.erase(ei);
					ei = fx.begin();
				}
				else ei++;
			}
			if(!found) {
				arts_warning("StereoEffectStack::remove failed. id %d not found?",
						ID);
			}
			reconnect();
		}

		AutoSuspendState autoSuspend() { return asSuspend; }

};

REGISTER_IMPLEMENTATION(StereoEffectStack_impl);

}


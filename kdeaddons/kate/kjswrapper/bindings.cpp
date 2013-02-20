#include "bindings.h"
#include "bindings.moc"
#include "plugin_katekjswrapper.h"
#include <qstringlist.h>
#include <kdebug.h>
#include <kate/documentmanager.h>
#include <kate/document.h>
#include <kjsembed/kjsembedpart.h>
#include <kjsembed/jsfactory.h>
#include <kjsembed/jsobjectproxy_imp.h>
#include <klocale.h>
#include <kjs/value.h>
#include <kdockwidget.h>
#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kjsembed/jsconsolewidget.h>


using namespace Kate::JS;
Bindings::Bindings(QObject *parent): KJSEmbed::Bindings::JSBindingPlugin(parent,"KateAppBindings",QStringList()) {
}

Bindings::~Bindings() {
}

KJS::Object Bindings::createBinding(KJSEmbed::KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const {
/*
        kdDebug() << "Loading a custom object" << endl;
        DocumentManager *obj = new DocumentManager();
        JSOpaqueProxy *prx = new JSOpaqueProxy(  (void *) obj, "Kate::JS::DocumentManager" );

        KJS::Object proxyObj(prx);
        DocumentManagerImp::addBindings( exec, proxyObj );
        return proxyObj;
*/
	return KJS::Object();
}


void Bindings::addBindings(KJS::ExecState *exec, KJS::Object &target) const {
	kdDebug()<<"Kate::JS::Bindings:: ADDING CUSTOM BINDINGS"<<endl;

	KJSEmbed::JSObjectProxy *proxy = KJSEmbed::JSProxy::toObjectProxy( target.imp() );
	if ( !proxy )
		return;
	
	Kate::DocumentManager *dm=dynamic_cast<Kate::DocumentManager*>(proxy->object());
	if (dm) {
		DocumentManager::addBindings(exec,proxy,target);
	}  else {
		Kate::Application *app=dynamic_cast<Kate::Application*>(proxy->object());
		if (app) {
			Application::addBindings(exec,proxy,target);
		} else {
			Kate::MainWindow *win=dynamic_cast<Kate::MainWindow*>(proxy->object());
			if (win) {
				MainWindow::addBindings(exec,proxy,target);
			}
		}
	}
	General::addBindings(exec,proxy,target);
}

void DocumentManager::addBindings(KJS::ExecState *exec, KJSEmbed::JSObjectProxy *proxy,KJS::Object &object){
	RefCountedObjectDict *dict=new RefCountedObjectDict(100);
        object.put(exec, "document", KJS::Object(new DocumentManager( exec, Document, proxy,dict )));
        object.put(exec, "activeDocument", KJS::Object(new DocumentManager( exec, ActiveDocument, proxy,dict )));
        object.put(exec, "documentForID", KJS::Object(new DocumentManager( exec, DocumentWithID, proxy,dict )));
        object.put(exec, "documentForURL", KJS::Object(new DocumentManager( exec, FindDocument, proxy,dict )));
        object.put(exec, "openURL", KJS::Object(new DocumentManager( exec, OpenURL, proxy,dict )));
        object.put(exec, "isOpen", KJS::Object(new DocumentManager( exec, IsOpen, proxy,dict )));
        object.put(exec, "documentCount", KJS::Object(new DocumentManager( exec, Documents, proxy,dict )));
        object.put(exec, "closeDocument", KJS::Object(new DocumentManager( exec, CloseDocument, proxy,dict )));
        object.put(exec, "closeAllDocument", KJS::Object(new DocumentManager( exec, CloseAllDocuments, proxy,dict )));
}

DocumentManager::DocumentManager( KJS::ExecState *exec, int id, KJSEmbed::JSObjectProxy *parent, RefCountedObjectDict *dict ):KJSEmbed::JSProxyImp(exec) {
	m_dict=dict;
	m_dict->incRef();
	m_id=id;
	m_proxy=parent;
}

DocumentManager::~DocumentManager() {
	m_dict->decRef();
}

KJS::Value DocumentManager::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
	QObject *o=m_proxy->object();
	Kate::DocumentManager *dm=dynamic_cast<Kate::DocumentManager*>(o);
	if (!dm) {
		kdWarning()<<"Object died"<<endl;
	        QString msg = i18n("Call of DocumentManager member on destroyed object");
        	KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
	        exec->setException( err );
		return KJS::Undefined();
	}
	QString mdesc;
	switch (m_id) {
		case Document: {
			mdesc="document(int)";
			if (args.size()!=1) break;
			uint index=args[0].toUInt32(exec);
			if (exec->hadException()) break;
				QObject *doc=dynamic_cast<QObject*>(dm->document(index));
				if (!doc) return KJS::Null();
				return m_dict->jsObject(exec,doc,m_proxy); }
		break;
		case ActiveDocument: {
			mdesc="activeDocument()";
			if (args.size()!=0) break;
			QObject *doc=dynamic_cast<QObject*>(dm->activeDocument());
			if (!doc) return KJS::Null();
			return m_dict->jsObject(exec,doc,m_proxy); }
		break;
		case DocumentWithID: {
			mdesc="documentForID(int)";
			if (args.size()!=1) break;
			uint id=args[0].toUInt32(exec);
			if (exec->hadException()) break;
				QObject *doc=dynamic_cast<QObject*>(dm->documentWithID(id));
				if (!doc) return KJS::Null();
				return m_dict->jsObject(exec,doc,m_proxy); }
		break;
		case FindDocument: {
			mdesc="documentForURL(KURL)";
			if (args.size()!=1) break;
			KURL url = QString( args[0].toString(exec).qstring() );
			if (exec->hadException()) break;
				return KJS::Number(dm->findDocument(url)); }
		break;
		case IsOpen: {
			mdesc="isOpen(KURL)";
			if (args.size()!=0) break;
			KURL url = QString( args[0].toString(exec).qstring() );
			if (exec->hadException()) break;
				return KJS::Boolean(dm->isOpen(url));}

		break;
		case OpenURL: {
			mdesc="openURL(KURL[,String encoding])";
				uint docID;
				if (args.size()==1) {
					KURL url = QString( args[0].toString(exec).qstring() );
					if (exec->hadException()) break;
					(void)dm->openURL(url,QString::null,&docID);
					return KJS::Number(docID);
				} else if (args.size()==2) {
					KURL url = QString( args[0].toString(exec).qstring() );
					if (exec->hadException()) break;
					QString encoding=QString( args[1].toString(exec).qstring() );
					(void)dm->openURL(url,encoding,&docID);
					return KJS::Number(docID);
				}
			}
		break;
		case Documents: {
			mdesc="documentCount()";
			if (args.size()!=0) break;
			return KJS::Number(dm->documents()); }
		break;
		case CloseDocument: {
			mdesc="closeDocument(Kate::Document)";
			if (args.size()!=1) break;
			KJSEmbed::JSObjectProxy *proxy = KJSEmbed::JSProxy::toObjectProxy( args[0].imp() );
			if (!proxy) break;
			QObject *tmp=proxy->object();
			Kate::Document *tmpdoc=dynamic_cast<Kate::Document*>(tmp);
			if (!tmpdoc) break;
			return KJS::Boolean(dm->closeDocument(tmpdoc)); }
		break;
		case CloseAllDocuments: {
			mdesc="closeAllDocuments()";
			if (args.size()!=0) break;
			return KJS::Boolean(dm->closeAllDocuments()); }
		break;
		default:
		kdDebug()<<"calling illegal method of DocumentManager"<<endl;
		return KJS::Null(); 
	}
            QString msg = i18n("Method %1 called with wrong signature").arg(mdesc);
            KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
            exec->setException( err );
	    return KJS::Undefined();
	
}

Kate::JS::Management::Management(KJS::ExecState *exec, int id, PluginKateKJSWrapper *kateplug):KJSEmbed::JSProxyImp(exec) {
	m_id=id;
	m_wrapper=kateplug;
}


KJS::Value Kate::JS::Management::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
	if (m_id==AddConfigPage) {
		if (args.size()!=1) {
        	    QString msg = i18n("One parameter expected");
	            KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
        	    exec->setException( err );
		    return KJS::Undefined();
		}
		KJS::Value v=args[0];
//		m_wrapper->m_configPageFactories.append(v);
#warning implement me
	} else if (m_id==SetConfigPages) {
		if (args.size()>1) {
			QString msg=i18n("One or no parameter expected");
		        KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
        	        exec->setException( err );
		        return KJS::Undefined();
		}
		m_wrapper->m_configPageFactories=(args.size()>0)?args[0]:KJS::Value();
	} else if (m_id==SetWindowConfiguration) {
		if (args.size()>3) {
        	    QString msg = i18n("A maximum of three parameters expected");
	            KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
        	    exec->setException( err );
		    return KJS::Undefined();
		}
		kdDebug()<<"***********************************************************************************"<<endl<<
			"Kate::JS::Management::call: Object type for m_toolViewConstructors (1):"<<args[0].type()<<endl;
		m_wrapper->m_toolViewConstructors=(args.size()>0)?args[0]:KJS::Value();
		kdDebug()<<"Kate::JS::Management::call: Object type for m_toolViewConstructors (2):"<<m_wrapper->m_toolViewConstructors.type()<<endl;
	        m_wrapper->m_newWindowHandler=(args.size()>1)?args[1]:KJS::Value();
        	m_wrapper->m_removeWindowHandler=(args.size()>2)?args[2]:KJS::Value();
	} else if (m_id==KJSConsole) {
		m_wrapper->m_part->view()->show();
	} else
		kdDebug()<<"Remove not implemented yet"<<endl;
	return KJS::Boolean(true);
}

void Kate::JS::Application::addBindings(KJS::ExecState *exec, KJSEmbed::JSObjectProxy *proxy,KJS::Object &object){
	PluginKateKJSWrapper *wrap=static_cast<PluginKateKJSWrapper*>(proxy->part()->parent());
        KJS::Object ToolView(new Application( exec, ToolView, proxy ,wrap));
	ToolView.put(exec,KJS::Identifier("Left"),KJS::Number(KDockWidget::DockLeft) ,KJS::ReadOnly | KJS::DontDelete);
	ToolView.put(exec,KJS::Identifier("Top"),KJS::Number(KDockWidget::DockTop) ,KJS::ReadOnly | KJS::DontDelete);
	ToolView.put(exec,KJS::Identifier("Right"),KJS::Number(KDockWidget::DockRight) ,KJS::ReadOnly | KJS::DontDelete);
	ToolView.put(exec,KJS::Identifier("Bottom"),KJS::Number(KDockWidget::DockBottom), KJS::ReadOnly | KJS::DontDelete);	
	General::addBindings(exec,proxy,ToolView);
	object.put(exec, "ToolView",ToolView);
    
	object.put(exec, KJS::Identifier("DocumentManager"),proxy->part()->bind(::Kate::documentManager()),KJS::ReadOnly | KJS::DontDelete);

	object.put(exec, "windowCount", KJS::Object(new Application( exec, WindowCount, proxy,wrap)));
	object.put(exec, "activeWindow", KJS::Object(new Application( exec, ActiveWindow, proxy,wrap)));
	object.put(exec, "window", KJS::Object(new Application( exec, Window, proxy,wrap )));

//	object.put(exec, "ProjectManager",proxy->part()->bind(::Kate::projectManager());

/*	obbject.put(exec, KJS::Identifier("WindowManager"),proxy->part
	KJS::Object*/
/*    
    Kate::PluginManager *pluginManager ();
    
    Kate::InitPluginManager *initPluginManager ();
    
    Kate::MainWindow *activeMainWindow ();*/
    
 //   uint mainWindows ();
 //   Kate::MainWindow *mainWindow (uint n = 0);
	
}

Kate::JS::Application::Application( KJS::ExecState *exec, int id, KJSEmbed::JSObjectProxy *parent,PluginKateKJSWrapper *plugin):KJSEmbed::JSProxyImp(exec) {
	kdDebug()<<"Kate::JS::Application::Application"<<endl;
	m_id=id;
	m_proxy=parent;
	m_plugin=plugin;
}

Kate::JS::Application::~Application() {
}

KJS::Value Kate::JS::Application::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
        QObject *o=m_proxy->object();
        Kate::Application *ka=dynamic_cast<Kate::Application*>(o);
        if (!ka) {
                kdWarning()<<"Object died"<<endl;
                QString msg = i18n("Call of KATE  member on destroyed object");
                KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
                exec->setException( err );
                return KJS::Undefined();
        }
	QString mdesc;
	switch (m_id) {
                case WindowCount: {
                        mdesc="windowCount()";
                        if (args.size()!=0) break;
                        return KJS::Number(ka->mainWindows()); }
                break;
                case Window: {
                        mdesc="window(int)";
                        if (args.size()!=1) break;
                        uint index=args[0].toUInt32(exec);
                        if (exec->hadException()) break;
                                Kate::MainWindow *mw=ka->mainWindow(index);
                                if (!mw) return KJS::Null();
				return m_plugin->getViewObject(mw)->winObj;
			}
                break;
                case ActiveWindow: {
                        mdesc="activeWindow()";
                        if (args.size()!=0) break;
                        Kate::MainWindow *mw=ka->activeMainWindow();
                        if (!mw) return KJS::Null();
			return m_plugin->getViewObject(mw)->winObj;
			}
                break;


	}
            QString msg = i18n("Method %1 called with wrong signature").arg(mdesc);
            KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
            exec->setException( err );
            return KJS::Undefined();

}






void Kate::JS::General::addBindings(KJS::ExecState *exec, KJSEmbed::JSObjectProxy *proxy,KJS::Object &object){
#warning "try to find a way to make the function implementations static, right now this doesn't work because of the need to retrieve the interpreter"
	KJS::Object methods= KJS::Object(new General( exec,proxy->interpreter(),MethodMethods));
	KJS::Object fields= KJS::Object(new General( exec,proxy->interpreter(),MethodFields));
	object.put(exec, "methods", methods);
	object.put(exec, "fields", fields);
}

Kate::JS::General::General( KJS::ExecState *exec, KJS::Interpreter *interpreter, int id):KJSEmbed::JSProxyImp(exec) {
	m_id=id;
	m_interpreter=interpreter;
}


KJS::Value Kate::JS::General::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
	switch (m_id) {
		case MethodMethods:
			return methodz(exec,self,args);
		case MethodFields:
			return fieldz(exec,self,args);
		default:
			return KJS::Null();
	}
}




KJS::Value Kate::JS::General::methodz(KJS::ExecState *exec, KJS::Object &obj, const KJS::List &)
{
        KJS::List items;

        KJS::ReferenceList list=obj.propList(exec, /*bool recursive*/ false);
        for (KJS::ReferenceListIterator it=list.begin();it!=list.end();it++) {
                if (it->getValue(exec).toObject(exec).implementsCall()) items.append(KJS::String(it->getPropertyName(exec).qstring()));
        }
        return KJS::Object(m_interpreter->builtinArray().construct(exec,items) );
}

KJS::Value Kate::JS::General::fieldz(KJS::ExecState *exec, KJS::Object &obj, const KJS::List &)
{
        KJS::List items;

        KJS::ReferenceList list=obj.propList(exec, /*bool recursive*/ false);
        for (KJS::ReferenceListIterator it=list.begin();it!=list.end();it++) {
                if (!(it->getValue(exec).toObject(exec).implementsCall())) items.append(KJS::String(it->getPropertyName(exec).qstring()));
        }
        return KJS::Object(m_interpreter->builtinArray().construct(exec,items) );
}


Kate::JS::RefCountedObjectDict::RefCountedObjectDict(int size): QObject(), QPtrDict<ObjectEntry>(size) {
	m_usageCount=0;
	setAutoDelete(true);
}

void Kate::JS::RefCountedObjectDict::incRef() {
	m_usageCount++;
}

void Kate::JS::RefCountedObjectDict::decRef() {
	kdDebug()<<"Kate::JS:RefCountedObjectDict::decCount()"<<endl;
	m_usageCount--;
	if (m_usageCount<1) deleteLater();
	
}

KJS::Object  Kate::JS::RefCountedObjectDict::jsObject(KJS::ExecState *exec, QObject *obj, KJSEmbed::JSObjectProxy *proxy) {
	ObjectEntry *oe=find(obj);
	if (oe==0) {
		oe=new ObjectEntry;
		oe->obj=proxy->part()->factory()->createProxy(exec,obj,proxy);
		connect(obj,SIGNAL(destroyed()),this,SLOT(removeSender()));
		insert(obj,oe);
		return oe->obj;
	} else return oe->obj;
	
}

void Kate::JS::RefCountedObjectDict::removeSender() {
	kdDebug()<<"Trying to remove object from dict"<<sender()<<endl;
	remove((void*)sender());	
}


void Kate::JS::MainWindow::addBindings(KJS::ExecState *exec, KJSEmbed::JSObjectProxy *proxy,KJS::Object &object){
	kdDebug()<<"Kate::JS::MainWindow::addBindings - 1"<<endl;
	if (!proxy)
		return;
	kdDebug()<<"Kate::JS::MainWindow::addBindings - 2"<<endl;

	Kate::MainWindow *mw=dynamic_cast<Kate::MainWindow*>(proxy->object());
	if (!mw) return;
	kdDebug()<<"Kate::JS::MainWindow::addBindings - 3"<<endl;

	PluginKateKJSWrapper *wrap=static_cast<PluginKateKJSWrapper*>(proxy->part()->parent());
	if (!wrap) return;
	kdDebug()<<"Kate::JS::MainWindow::addBindings - 4"<<endl;

	object.put(exec, "actionCollection", KJS::Object(new MainWindow( exec, ActionCollection, proxy,wrap)));
	
}



Kate::JS::MainWindow::MainWindow( KJS::ExecState *exec, int id, KJSEmbed::JSObjectProxy *parent,PluginKateKJSWrapper *plugin):KJSEmbed::JSProxyImp(exec) {
	kdDebug()<<"Kate::JS::MainWindow::MainWindow"<<endl;
	m_id=id;
	m_proxy=parent;
	m_plugin=plugin;
}

Kate::JS::MainWindow::~MainWindow() {
}

KJS::Value Kate::JS::MainWindow::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
        QObject *o=m_proxy->object();
        Kate::MainWindow *mw=dynamic_cast<Kate::MainWindow*>(o);
        if (!mw) {
                kdWarning()<<"Object died"<<endl;
                QString msg = i18n("Call of MainWindow  member on destroyed object");
                KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
                exec->setException( err );
                return KJS::Undefined();
        }
	QString mdesc;
	switch (m_id) {
                case ActionCollection: {
                        mdesc="actionCollection()";
                        if (args.size()!=0) break;
			return m_plugin->getViewObject(mw)->actionCollectionObj;
		}
                break;
		default:
		return KJS::Undefined();
	}
            QString msg = i18n("Method %1 called with wrong signature").arg(mdesc);
            KJS::Object err = KJS::Error::create( exec, KJS::GeneralError, msg.utf8() );
            exec->setException( err );
            return KJS::Undefined();
}


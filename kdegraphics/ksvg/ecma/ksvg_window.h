/* This file is part of the KDE project
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2, as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSVG_WINDOW_H
#define KSVG_WINDOW_H

#include <kjs/object.h>
#include <qguardedptr.h>

namespace KSVG {

class SVGDocumentImpl;
class WindowQObject;

// This is currently a fork of khtml's Window object, simplified.
// However in the long run it could become a base class for it.
// Author: David Faure <faure@kde.org>
class Window : public KJS::ObjectImp {
    friend class WindowFunc;
    friend class WindowQObject;
    friend class ScheduledAction;
public:
    Window(KSVG::SVGDocumentImpl *p);
    ~Window();

    virtual KJS::Value get(KJS::ExecState *exec, const KJS::Identifier &propertyName) const;
    virtual void put(KJS::ExecState *exec, const KJS::Identifier &propertyName, const KJS::Value &value, int attr = KJS::None);
    virtual bool hasProperty(KJS::ExecState * /*exec*/, const KJS::Identifier &/*p*/) const;

    /**
     * Returns and registers a window object. In case there's already a Window
     * for the specified part p this will be returned in order to have unique
     * bindings.
     */
    //static KJS::Value retrieve(KSVG::SVGDocumentImpl *p);
    /**
     * Returns the Window object for a given part
     */
    //static Window *retrieveWindow(KSVG::SVGDocumentImpl *p);
    /**
     * returns a pointer to the Window object this javascript interpreting instance
     * was called from.
     */
    static Window *retrieveActive(KJS::ExecState *exec);

    QGuardedPtr<KSVG::SVGDocumentImpl> doc() const { return m_doc; }

	int installTimeout(const KJS::UString &handler, int t, bool singleShot);
	void clearTimeout(int timerId);

    bool isSafeScript(KJS::ExecState *exec) const;
    void clear( KJS::ExecState *exec );

    enum {
        // Properties
        _Closed, _Window, _Document, _Evt, _InnerWidth, _InnerHeight,
		_SVGTransform, _SVGLength, _SVGAngle, _SVGColor, _SVGPreserveAspectRatio, _SVGGradientElement,
		_SVGPathSeg, _SVGTextContentElement, _SVGPaint, _SVGZoomAndPan, _SVGMarkerElement, _SVGTextPathElement,
		_SetInterval, _ClearInterval, _SetTimeout, _ClearTimeout, _Navigator, _PrintNode,
        // Functions
        _Alert, _Confirm, _Debug, _Success, _GetSVGViewerVersion, _GetURL, _PostURL, _ParseXML, _Prompt
    };

    virtual const KJS::ClassInfo* classInfo() const { return &s_classInfo; }
    static const KJS::ClassInfo s_classInfo;
    static const struct KJS::HashTable s_hashTable;

private:
	WindowQObject *winq;
    QGuardedPtr<KSVG::SVGDocumentImpl> m_doc;
};

class ScheduledAction {
public:
	ScheduledAction(KJS::Object _func, KJS::List _args, bool _singleShot);
	ScheduledAction(QString _code, bool _singleShot);
	~ScheduledAction();
	void execute(Window *window);
	KJS::Object func;
	KJS::List args;
	QString code;
	bool isFunction;
	bool singleShot;
};

class WindowQObject : public QObject {
	Q_OBJECT
public:
	WindowQObject(Window *w);
	~WindowQObject();
	int installTimeout(const KJS::UString &handler, int t, bool singleShot);
	int installTimeout(const KJS::Value &func, KJS::List args, int t, bool singleShot);
	void clearTimeout(int timerId, bool delAction = true);
public slots:
	void timeoutClose();
protected slots:
	void parentDestroyed();
protected:
	void timerEvent(QTimerEvent *e);
private:
	Window *parent;
	//KHTMLPart *part;        // not guarded, may be dangling
	QMap<int, ScheduledAction*> scheduledActions;
};

}
#endif // KSVG_WINDOW_H

/*
    Copyright (C) 2001-2003 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef SVGWindow_H
#define SVGWindow_H

#include <dom/dom_string.h>
#include <dom/dom_node.h>
#include <dom/dom2_events.h>

namespace KSVG
{

class SVGDocument;
class SVGWindowImpl;

class SVGWindow 
{ 
public:
	SVGWindow();
	SVGWindow(const SVGWindow &other);
	SVGWindow &operator=(const SVGWindow &other);
	SVGWindow(SVGWindowImpl *other);
	virtual ~SVGWindow();

	//StyleSheet defaultStyleSheet() const;
	SVGDocument document() const;
	DOM::Event evt() const;

	/**
	 * @return Viewport height in pixels.
	 */
	long innerHeight() const;

	/**
	 * @return Viewport width in pixels.
	 */
	long innerWidth() const;

	void setSrc(const DOM::DOMString &src);

	/**
	 * @return The URI of the current document.
	 */
	DOM::DOMString src() const;

	void clearInterval(long interval);
	void clearTimeout(long timeout);

	/**
	 * Get data from the given URL using the HTTP GET method. Notify the callback when done.
	 *
	 * @param uri The URI reference for the data to be loaded.
	 * @param callback The method to be invoked when the data is available.
	 */
	void getURL(const DOM::DOMString &uri, const DOM::EventListener &callback);
	//DOM::DocumentFragment parseXML(const DOM::DOMString &source, const DOM::Document &document);
	void postURL(const DOM::DOMString &uri, const DOM::DOMString &data, const DOM::EventListener &callback, const DOM::DOMString &mimeType, const DOM::DOMString &contentEncoding);

	/**
	 * Converts a Node into a DOMString. The string is an XML representation of the Node.
	 *
	 * @param node The Node to be converted.
	 * @return Converts a Node into a DOMString. The string is an XML representation of the Node.
	 */
	DOM::DOMString printNode(const DOM::Node &node);
	long setInterval(const DOM::DOMString &code, const long &delay);
	long setTimeout(const DOM::DOMString &code, const long &delay);

	/**
	 * Displays a modal synchronous message to the user if possible in the current user environment.
	 * Commonly, this message takes the form of a pop-up window with a single dispose button.
	 *
	 * @param message The message to be displayed.
	 */
	void alert(const DOM::DOMString &message);

	/**
	 * Displays a modal synchronous message to the user if possible in the current user environment.
	 * The user is able to accept or reject the message. Commonly, this message takes the form of
	 * a pop-up window with either a Yes/No or OK/Cancel button pair.
	 *
	 * @param message The message to be displayed.
	 * @return A boolean indicating the user's response. True for accept, False for reject.
	 */
	bool confirm(const DOM::DOMString &message);

	/**
	 * Displays a modal synchronous message to the user if possible in the current user environment.
	 * The user is able to enter a response to the message. Commonly, this message takes the form of
	 * a pop-up window with a single text entry field.
	 *
	 * @param message The message to be displayed.
	 * @param _default The default response to suggest to the user. This can be displayed in the text field and be modified by the user.
	 * @return A boolean indicating the user's response. True for accept, False for reject.
	 */
	DOM::DOMString prompt(const DOM::DOMString &message, const DOM::DOMString &_default);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGWindowImpl *handle() const { return impl; }

private:
	SVGWindowImpl *impl;
};

}

#endif

// vim:ts=4:noet

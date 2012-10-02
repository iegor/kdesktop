#ifndef INC_CharStreamException_hpp__
#define INC_CharStreamException_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: CharStreamException.hpp 626096 2007-01-22 06:35:06Z okellogg $
 */

#include <antlr/config.hpp>
#include <antlr/ANTLRException.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class ANTLR_API CharStreamException : public ANTLRException {
public:
	CharStreamException(const ANTLR_USE_NAMESPACE(std)string& s)
		: ANTLRException(s) {}
	~CharStreamException() throw() {}
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_CharStreamException_hpp__

#ifndef INC_TokenStreamRetryException_hpp__
#define INC_TokenStreamRetryException_hpp__

/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: TokenStreamRetryException.hpp 626096 2007-01-22 06:35:06Z okellogg $
 */

#include <antlr/config.hpp>
#include <antlr/TokenStreamException.hpp>

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

class TokenStreamRetryException : public TokenStreamException {
public:
	TokenStreamRetryException() {}
	~TokenStreamRetryException() throw() {}
};

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif

#endif //INC_TokenStreamRetryException_hpp__

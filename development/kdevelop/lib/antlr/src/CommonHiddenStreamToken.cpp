/* ANTLR Translator Generator
 * Project led by Terence Parr at http://www.jGuru.com
 * Software rights: http://www.antlr.org/license.html
 *
 * $Id: CommonHiddenStreamToken.cpp 626096 2007-01-22 06:35:06Z okellogg $
 */
#include "antlr/CommonHiddenStreamToken.hpp"

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
namespace antlr {
#endif

CommonHiddenStreamToken::CommonHiddenStreamToken()
: CommonToken()
{
}

CommonHiddenStreamToken::CommonHiddenStreamToken(int t, const ANTLR_USE_NAMESPACE(std)string& txt)
: CommonToken(t,txt)
{
}

CommonHiddenStreamToken::CommonHiddenStreamToken(const ANTLR_USE_NAMESPACE(std)string& s)
: CommonToken(s)
{
}

RefToken CommonHiddenStreamToken::getHiddenAfter()
{
	return hiddenAfter;
}

RefToken CommonHiddenStreamToken::getHiddenBefore()
{
	return hiddenBefore;
}

RefToken CommonHiddenStreamToken::factory()
{
	return RefToken(new CommonHiddenStreamToken);
}

void CommonHiddenStreamToken::setHiddenAfter(RefToken t)
{
	hiddenAfter = t;
}

void CommonHiddenStreamToken::setHiddenBefore(RefToken t)
{
	hiddenBefore = t;
}

#ifdef ANTLR_CXX_SUPPORTS_NAMESPACE
}
#endif



# Make #include <antlr/xxx> work..
INCLUDES=-I$(srcdir)/..
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

noinst_LTLIBRARIES = libantlr.la

libantlr_la_LDFLAGS = -no-undefined

libantlr_la_SOURCES = \
	ANTLRException.cpp \
	ASTFactory.cpp \
	ASTRefCount.cpp \
	BaseAST.cpp \
	BitSet.cpp \
	CharBuffer.cpp \
	CharScanner.cpp \
	CommonAST.cpp \
	CommonASTWithHiddenTokens.cpp \
	CommonHiddenStreamToken.cpp \
	CommonToken.cpp \
	InputBuffer.cpp \
	LLkParser.cpp \
	LexerSharedInputState.cpp \
	MismatchedCharException.cpp \
	MismatchedTokenException.cpp \
	NoViableAltException.cpp \
	NoViableAltForCharException.cpp \
	Parser.cpp \
	ParserSharedInputState.cpp \
	RecognitionException.cpp \
	String.cpp \
	Token.cpp \
	TokenBuffer.cpp \
	TokenStreamBasicFilter.cpp \
	TokenStreamHiddenTokenFilter.cpp \
	TokenStreamSelector.cpp \
	TreeParser.cpp \
	TreeParserSharedInputState.cpp

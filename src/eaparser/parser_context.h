#ifndef _ADRENO_EA_PARSER_CONTEXT_H
#define _ADRENO_EA_PARSER_CONTEXT_H

#include <iostream>

class ParserContext {
public:
	ParserContext( std::istream& in = std::cin );

private:
	class Internal;
	Internal* ctx;
};


#endif


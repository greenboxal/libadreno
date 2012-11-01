#include "adreno/ea/parser_context.h"

using namespace std;

class ParserContext::Internal {
public:
	Internal( istream& in_ )
		: in( in_ ) {}

	istream& Input() { return in; }

private:
	istream& in;
};


ParserContext::ParserContext( istream& in )
	: ctx( new ParserContext::Internal( in ) )
{
}

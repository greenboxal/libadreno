#include "eacompiler/compiler.h"
#include "eacompiler/parser_context.h"

using namespace std;

bool
AdrenoEACompiler::Compile( istream& in, ostream& out )
{
	ParserContext ctx( in );
	if( !EAParser_parse( &ctx ) ) {
		return false;
	}

	out << "HOORAY";

	return true;
}

#include "eacompiler/compiler.h"
#include "eacompiler/parser_context.h"

using namespace std;

bool
AdrenoEACompiler::Compile( istream& in, ostream& out )
{
	ParserContext ctx( in );
	if( 0 != EAParser_parse( &ctx ) ) {
		return false;
	}

	ctx.GetRoot()->CodeGen( out );

	return true;
}

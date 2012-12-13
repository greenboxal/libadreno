#include <UnitTest++.h>
#include <fstream>


#include "eacompiler/compiler.h"

using namespace std;

SUITE( AdrenoEATests )
{

TEST( CompileSimpleFunctionCall )
{
	istringstream input( "close;" );
	ostringstream output;

	AdrenoEACompiler compiler;
	CHECK_EQUAL( true, compiler.Compile( input, output ) );

	/*
	string result = output.str();
	CHECK_EQUAL( "enter\n"
	             "ldglob \"close\"\n"
	             "call 0\n"
	             "ldnull\n"
	             "ret\n", result );
	*/
}

}

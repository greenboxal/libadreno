#include "eacompiler/parser_context.h"
#include <string>

using namespace std;

#define CG( x ) void EA ## x :: CodeGen( ParserOutputStream& out )
#define TODO( x ) CG( x ) { cout << "TODO: " << #x << endl; }

CG( ExpressionStatement )
{
	expr->CodeGen( out );
}

CG( Label )
{
	name->CodeGen( out );
	out << ":" << endl;
}

CG( SwitchCase )
{
	out << "TODO: Switch Case" << endl;
}

CG( Block )
{
	NodeList::iterator it;
	for( it = nodes.begin(); it != nodes.end(); ++it ) {
		( *it )->CodeGen( out );
	}
}

CG( Identifier )
{
	cout << "ident: \"" << name << "\"" << endl;
}

CG( Integer )
{
	cout << "int: " << val << endl;
}

TODO( ExprGT )
TODO( ExprLT )
TODO( ExprEQU )
TODO( ExprNEQ )
TODO( ExprAND )
TODO( ExprOR )

TODO( OpADD )
TODO( OpSUB )
TODO( OpMUL )
TODO( OpDIV )

TODO( IfStatement )
TODO( FunctionCall )

TODO( Null )
TODO( Menu )
TODO( MenuOptions )
TODO( ForLoop )
TODO( WhileLoop )
TODO( DoWhileLoop )
TODO( Switch )
TODO( SwitchCases )


#ifndef _ADRENO_EA_PARSER_CONTEXT_H
#define _ADRENO_EA_PARSER_CONTEXT_H

#include <iostream>
#include <vector>

class EANode;
class EABlock;
class EAExpression;
class EAString;
class EAIdentifier;
class EAMenu;
class EAMenuOptions;
class EASwitch;
class EASwitchCases;
class EASwitchCase;

typedef std::istream ParserInputStream;
typedef std::ostream ParserOutputStream;
typedef std::vector<EAExpression*> ExpressionList;
typedef std::vector<EANode*> NodeList;

class ParserContext {
public:
	ParserContext( ParserInputStream& in );
	~ParserContext();

	ParserInputStream& Input()     { return in; }
	void* Scanner()                { return scanner; }

	EABlock* GetRoot()             { return root; }
	void SetRoot( EABlock* block );

private:
	ParserInputStream& in;
	void* scanner;
	EABlock* root;
};

int EAParser_parse( ParserContext* ctx );

class EANode {
public:
	virtual void CodeGen( ParserOutputStream& out ) = 0;
};

class EAExpression : public EANode {

};

class EAStatement : public EANode {

};

class EAExpressionStatement : public EAStatement {
public:
	EAExpressionStatement( EAExpression* expr_ )
		: expr( expr_ ) {}

	virtual void CodeGen( ParserOutputStream& out );
private:
	EAExpression* expr;
};

class EABlock : public EANode {
public:
	virtual void CodeGen( ParserOutputStream& out );
	void Add( EANode* node ) { nodes.push_back( node ); }
private:
	NodeList nodes;
};

class EALabel : public EANode {
public:
	EALabel( EAString* label_name )
		: name( label_name ) {}

	virtual void CodeGen( ParserOutputStream& out );
private:
	EAString* name;
};

class EAMenuOptions : public EANode {
public:
	void Add( EAExpression* expr, EAExpression* jump_target )
	{
		opts.push_back( expr );
		jumps.push_back( jump_target );
	}

	virtual void CodeGen( ParserOutputStream& out );

private:
	ExpressionList opts;
	ExpressionList jumps;
};

class EASwitchCases : public EANode {
public:
	void Add( EASwitchCase* switch_case )
	{
		cases.push_back( switch_case );
	}

	virtual void CodeGen( ParserOutputStream& out );
private:
	std::vector<EASwitchCase*> cases;
};

class EASwitchCase : public EANode {
public:
	EASwitchCase( ExpressionList* labels_, EABlock* code_ )
		: labels( labels_ ), code( code_ ) {}

	virtual void CodeGen( ParserOutputStream& out );
private:
	ExpressionList* labels;
	EABlock* code;
};

class EAIdentifier : public EAExpression {
public:
	EAIdentifier( const std::string& ident_name )
		: name( ident_name ) {}

	virtual void CodeGen( ParserOutputStream& out );
private:
	std::string name;
};

class EAString : public EAExpression {
public:
	EAString( const std::string& str )
		: s( str ) {}

	virtual void CodeGen( ParserOutputStream& out )
	{
		out << "str: \"" << out << "\"" << std::endl;
	}

private:
	std::string s;
};

class EAInteger : public EAExpression {
public:
	EAInteger( intptr_t value )
		: val( value ) {}

	virtual void CodeGen( ParserOutputStream& out );
private:
	intptr_t val;
};

class EAExprOperation : public EAExpression {
public:
	EAExprOperation( EAExpression* lhs_, EAExpression* rhs_ )
		: lhs( lhs_ ), rhs( rhs_ ) {}

protected:
	EAExpression* lhs;
	EAExpression* rhs;
};

class EAExprGT : public EAExprOperation {
public:
	EAExprGT( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAExprLT : public EAExprOperation {
public:
	EAExprLT( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAExprEQU : public EAExprOperation {
public:
	EAExprEQU( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAExprNEQ : public EAExprOperation {
public:
	EAExprNEQ( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAExprAND : public EAExprOperation {
public:
	EAExprAND( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAExprOR : public EAExprOperation {
public:
	EAExprOR( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAOpADD : public EAExprOperation {
public:
	EAOpADD( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAOpSUB : public EAExprOperation {
public:
	EAOpSUB( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAOpMUL : public EAExprOperation {
public:
	EAOpMUL( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAOpDIV : public EAExprOperation {
public:
	EAOpDIV( EAExpression* lhs, EAExpression* rhs )
		: EAExprOperation( lhs, rhs ) {}

	virtual void CodeGen( ParserOutputStream& out );
};

class EAFunctionCall : public EAExpression {
public:
	EAFunctionCall( EAIdentifier* function_name, ExpressionList* arguments = NULL )
		: func_name( function_name ), args( arguments ) {}

	virtual void CodeGen( ParserOutputStream& out );
private:
	EAIdentifier* func_name;
	ExpressionList* args;
};

class EAIfStatement : public EAStatement {
public:
	EAIfStatement( EAExpression* expression, EABlock* code )
		: expr( expression ), if_true( code ), if_false( NULL ) {}

	// Sets the code for the 'else' part of the if statement
	void Else( EABlock* code ) { if_false = code; }

	virtual void CodeGen( ParserOutputStream& out );
private:
	EAExpression* expr;
	EABlock* if_true;
	EABlock* if_false;
};

class EAMenu : public EANode {
public:
	EAMenu( EAMenuOptions* options )
		: opts( options ) {}

	virtual void CodeGen( ParserOutputStream& out );

private:
	EAMenuOptions* opts;
};

class EANull : public EAExpression {
public:
	EANull() {}
	virtual void CodeGen( ParserOutputStream& out );
};

class EAForLoop: public EAStatement {
public:
	EAForLoop( EAStatement* initial_statement,
	           EAExpression* end_condition,
	           EAStatement* loop_statement,
	           EABlock* code_ )
		: initial( initial_statement ),
		  cond( end_condition ),
		  loop( loop_statement ),
		  code( code_ ) {}

	virtual void CodeGen( ParserOutputStream& out );
private:
	EAStatement* initial;
	EAExpression* cond;
	EAStatement* loop;
	EABlock* code;
};

class EASwitch : public EAStatement {
public:
	EASwitch( EAExpression* expression, EASwitchCases* cases_ )
		: expr( expression ), cases( cases_ ) {}

	virtual void CodeGen( ParserOutputStream& out );
private:
	EAExpression* expr;
	EASwitchCases* cases;
};

class EAWhileLoop : public EAStatement {
public:
	EAWhileLoop( EAExpression* expr_, EABlock* code_ )
		: expr( expr_ ), code( code_ ) {}

	virtual void CodeGen( ParserOutputStream& out );
protected:
	EAExpression* expr;
	EABlock* code;
};

class EADoWhileLoop : public EAWhileLoop {
public:
	EADoWhileLoop( EABlock* code, EAExpression* expr )
		: EAWhileLoop( expr, code ) {}

	virtual void CodeGen( ParserOutputStream& out );
};


#endif


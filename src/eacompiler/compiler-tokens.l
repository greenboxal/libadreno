%option reentrant
%option prefix="EAParser_"
%option bison-bridge
%option bison-locations
%option noyywrap
%option yylineno
%option never-interactive

%{
   #include "eacompiler/parser_context.h"
   #include "compiler-parser.tab.hpp"
   #include <assert.h>

   #define YY_EXTRA_TYPE ParserContext*
   #define YY_USER_ACTION yylloc->first_line = yylineno;
   #define YY_INPUT(buf, result, max_size) {                \
	if ( yyextra->Input().eof() )							\
	   result = YY_NULL;									\
	else													\
	   result = yyextra->Input().readsome( buf, max_size );	\
   }

   #define TOKEN( x ) return ( yylval->ival = ( x ) )
%}

D [0-9]
L [A-Za-z_]
H [A-Fa-f0-9]

%s IN_COMMENT

%%

<INITIAL>{
"/*"                 BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"                 BEGIN(INITIAL);
[^*\n]+
"*"
}

","                  { TOKEN( ',' ); }
";"                  { TOKEN( ';' ); }
":"                  { TOKEN( ':' ); }
"("                  { TOKEN( '(' ); }
")"                  { TOKEN( ')' ); }
"{"                  { TOKEN( '{' ); }
"}"                  { TOKEN( '}' ); }
"+"                  { TOKEN( '+' ); }
"-"                  { TOKEN( '-' ); }
"*"                  { TOKEN( '*' ); }
"/"                  { TOKEN( '/' ); }
"<"                  { TOKEN( '<' ); }
">"                  { TOKEN( '>' ); }
"=="                 { TOKEN( T_EQU ); }
"!="                 { TOKEN( T_NEQ ); }
"&&"                 { TOKEN( T_AND ); }
"||"                 { TOKEN( T_OR ); }

"if"                 { TOKEN( T_IF ); }
"else"               { TOKEN( T_ELSE ); }
"for"                { TOKEN( T_FOR ); }
"switch"             { TOKEN( T_SWITCH ); }
"case"               { TOKEN( T_CASE ); }
"default"            { TOKEN( T_DEFAULT ); }
"while"              { TOKEN( T_WHILE ); }
"do"                 { TOKEN( T_DO ); }
"menu"               { TOKEN( T_MENU ); }

0[Xx]{H}+            { yylval->ival = strtol( yytext, NULL, 16 ); return T_INT; }
{L}+({L}|{D})*\$?    { yylval->sval = strdup( yytext ); return T_IDENT; }
\$@{L}+({L}|{D})*\$? { yylval->sval = strdup( yytext ); return T_IDENT; }
\.@{L}+({L}|{D})*\$? { yylval->sval = strdup( yytext ); return T_IDENT; }
@{L}+({L}|{D})*\$?   { yylval->sval = strdup( yytext ); return T_IDENT; }
\${L}+({L}|{D})*\$?  { yylval->sval = strdup( yytext ); return T_IDENT; }
\"[^"]*\"            { yylval->sval = strdup( yytext ); return T_STR; }
{D}+                 { yylval->ival = strtol( yytext, NULL, 10 ); return T_INT; }
\/\/.*$              ; // Ignore comments
(\t\ )               ; // Ignore whitespace
\r?\n?               ; // Ignore newlines
.                    ;

%%

ParserContext::ParserContext( std::istream& in_ )
	: in( in_ ),
	  root( 0 )
{
	yylex_init( &scanner );
	yyset_extra( this, scanner );
}

ParserContext::~ParserContext()
{
	yylex_destroy( scanner );
}

void
ParserContext::SetRoot( EABlock* block )
{
	assert( !root );
	root = block;
}

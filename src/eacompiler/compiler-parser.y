%pure-parser
%name-prefix="EAParser_"
%error-verbose
%locations
%defines
%parse-param { ParserContext* context }
%lex-param { void* scanner }

%{
   #include <stdio.h>
   #include <stdlib.h>
   #include <iostream>
   #include <vector>
   #include "eacompiler/parser_context.h"
   using namespace std;
%}

%union {
   int ival;
   char* sval;

   EANode* node;
   EABlock* block;
   EAStatement* stmt;
   EAExpression* expr;
   EALabel* label;
   EAMenu* menu;
   EAMenuOptions* menu_opts;
   EASwitchCases* switch_cases;
   EASwitchCase* switch_case;
   EAIfStatement* if_stmt;
   std::vector<EAExpression*>* expr_list;

}

%{
   int EAParser_lex( YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner );
   void EAParser_error( YYLTYPE* locp, ParserContext* context, const char* err ) {
      cerr << locp->first_line << ":" << err << endl;
   }

   #define scanner context->Scanner()

%}

%token <ival> T_INT T_EQU T_NEQ T_AND T_OR
%token <sval> T_STR T_IDENT

%token T_IF T_ELSE T_FOR T_SWITCH T_CASE T_DEFAULT T_WHILE T_DO T_MENU
%token T_GLOBAL_TMP
%token T_EOL T_EOF 0

%type <block> eascript code_block code_fragment
%type <node> code
%type <stmt> statement for_loop while_loop do_while_loop switch_block
%type <expr> expression constant_expression function_call operation comparison nested_expression case_label menu_label
%type <label> label
%type <menu_opts> menu_options
%type <expr_list> expression_list case_labels
%type <switch_cases> switch_cases
%type <switch_case> switch_case
%type <if_stmt> if_stmt
%type <menu> menu_stmt

%start eascript

%%

eascript : code_block { $$ = $1; }
         ;

code_block : '{' code_fragment '}' { $$ = $2; }
           | code                  { $$ = new EABlock; $$->Add( $1 ); }
           ;

code_fragment : code               { $$ = new EABlock; $$->Add( $1 ); }
              | code_fragment code { $1->Add( $2 ); }
              ;

code : ';'            {}
     | statement ';'  { $$ = $1; }
     | label ':'      { $$ = $1; }
     | if_stmt        { $$ = $1; }
     | menu_stmt ';'  { $$ = $1; }
     | for_loop       { $$ = $1; }
     | while_loop     { $$ = $1; }
     | do_while_loop  { $$ = $1; }
     | switch_block   { $$ = $1; }
     ;

statement : expression { $$ = new EAExpressionStatement( $1 ); }
          ;

label : T_IDENT { $$ = new EALabel( new EAString( $1 ) ); }
      ;

expression_list : expression                     { $$ = new ExpressionList; $$->push_back( $1 ); }
                | expression_list ',' expression { $1->push_back( $3 ); }
                ;

expression : constant_expression
           | function_call
           | operation
           | comparison
           | nested_expression
           | T_IDENT { $$ = new EAIdentifier( $1 ); }
           ;

nested_expression : '(' expression ')' { $$ = $2; }
                  ;

comparison : expression '>' expression   { $$ = new EAExprGT( $1, $3 ); }
           | expression '<' expression   { $$ = new EAExprLT( $1, $3 ); }
           | expression T_EQU expression { $$ = new EAExprEQU( $1, $3 ); }
           | expression T_NEQ expression { $$ = new EAExprNEQ( $1, $3 ); }
           | expression T_AND expression { $$ = new EAExprAND( $1, $3 ); }
           | expression T_OR expression  { $$ = new EAExprOR( $1, $3 ); }
           ;

operation : expression '+' expression    { $$ = new EAOpADD( $1, $3 ); }
          | expression '-' expression    { $$ = new EAOpSUB( $1, $3 ); }
          | '-' expression               { $$ = new EAOpSUB( 0, $2 ); }
          | expression '*' expression    { $$ = new EAOpMUL( $1, $3 ); }
          | expression '/' expression    { $$ = new EAOpDIV( $1, $3 ); }
          ;

constant_expression : T_STR { $$ = new EAString( $1 ); }
                    | T_INT { $$ = new EAInteger( $1 ); }
                    ;

function_call : T_IDENT '(' ')'                 { $$ = new EAFunctionCall( new EAIdentifier( $1 ) ); }
              | T_IDENT '(' expression_list ')' { $$ = new EAFunctionCall( new EAIdentifier( $1 ), $3 ); }
              | T_IDENT expression_list         { $$ = new EAFunctionCall( new EAIdentifier( $1 ), $2 ); }
              ;

if_stmt : T_IF '(' expression ')' code_block { $$ = new EAIfStatement( $3, $5 ); }
        | if_stmt T_ELSE code_block          { $1->Else( $3 ); }
        ;

menu_stmt : T_MENU menu_options { $$ = new EAMenu( $2 ); }
          ;

menu_options : T_STR ',' menu_label                  { $$ = new EAMenuOptions; $$->Add( new EAString( $1 ), $3 ); }
             | menu_options ',' T_STR ',' menu_label { $1->Add( new EAString( $3 ), $5 ); }
             ;

menu_label : T_IDENT { $$ = new EAString( $1 ); }
           | '-'     { $$ = new EANull; }
           ;

for_loop : T_FOR '(' statement ';' expression ';' statement ')' code_block { $$ = new EAForLoop( $3, $5, $7, $9 ); }
         ;

switch_block : T_SWITCH '(' expression ')' '{' switch_cases '}' { $$ = new EASwitch( $3, $6 ); }
             ;

switch_cases : switch_case              { $$ = new EASwitchCases; $$->Add( $1 ); }
             | switch_cases switch_case { $1->Add( $2 ); }
             ;

switch_case : case_labels code_fragment { $$ = new EASwitchCase( $1, $2 ); }
            | case_labels code_block    { $$ = new EASwitchCase( $1, $2 ); }
            ;

case_labels : case_label                { $$ = new ExpressionList; $$->push_back( $1 ); }
            | case_labels case_label    { $1->push_back( $2 ); }
            ;

case_label : T_CASE T_INT ':'           { $$ = new EAInteger( $2 ); }
           | T_DEFAULT ':'              { $$ = new EANull; }
           ;

while_loop : T_WHILE '(' expression ')' code_block { $$ = new EAWhileLoop( $3, $5 ); }
           ;

do_while_loop : T_DO '{' code_fragment '}' T_WHILE '(' expression ')' { $$ = new EADoWhileLoop( $3, $7 ); }
              ;

%%


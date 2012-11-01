%error-verbose
%locations

%{
   #include <stdio.h>
   #include <stdlib.h>
   #include <iostream>
   using namespace std;

   int yylex();
   extern int yyparse();
   extern void yyerror( const char* s );
   extern FILE* yyin;
   extern int yylineno;
%}

%union {
   int ival;
   char* sval;
}

%token <ival> T_INT T_EQU T_NEQ T_AND T_OR
%token <sval> T_STR T_IDENT

%token T_IF T_ELSE T_FOR T_SWITCH T_CASE T_DEFAULT T_WHILE T_DO T_MENU
%token T_GLOBAL_TMP
%token T_EOL T_EOF 0

%start eascript

%%

eascript : code_block
         ;

code_block : '{' code_fragment '}'
           | code
           ;

code_fragment : code
              | code_fragment code
              ;

code : ';'
     | statement ';'
     | label ':'
     | if_stmt
     | menu_stmt ';'
     | for_loop
     | while_loop
     | do_while_loop
     | switch_block
     ;

statement : expression
          ;

label : T_IDENT
      ;

expression_list : expression
                | expression_list ',' expression
                ;

expression : constant_expression
           | function_call
           | operation
           | comparison
           | nested_expression
           | T_IDENT
           ;

nested_expression : '(' expression ')'
                  ;

comparison : expression '>' expression
           | expression '<' expression
           | expression T_EQU expression
           | expression T_NEQ expression
           | expression T_AND expression
           | expression T_OR expression
           ;

operation : expression '+' expression
          | expression '-' expression
          | '-' expression
          | expression '*' expression
          | expression '/' expression
          ;

constant_expression : T_STR
                    | T_INT
                    ;

function_call : T_IDENT '(' ')'
              | T_IDENT '(' expression_list ')'
              | T_IDENT expression_list
              ;

if_stmt : T_IF '(' expression ')' code_block
        | if_stmt T_ELSE code_block
        ;

menu_stmt : T_MENU menu_options
          ;

menu_options : T_STR ',' menu_label
             | menu_options ',' T_STR ',' menu_label
             ;

menu_label : T_IDENT
           | '-'
           ;

for_loop : T_FOR '(' statement ';' expression ';' statement ')' code_block
         ;

switch_block : T_SWITCH '(' expression ')' '{' switch_cases '}'
             ;

switch_cases : switch_case
             | switch_cases switch_case
             ;

switch_case : case_labels code_fragment
            | case_labels code_block
            ;

case_labels : case_label
            | case_labels case_label
            ;

case_label : T_CASE T_INT ':'
           | T_DEFAULT ':'
           ;

while_loop : T_WHILE '(' expression ')' code_block
           ;

do_while_loop : T_DO '{' code_fragment '}' T_WHILE '(' expression ')'
              ;

%%

void
usage( const char* cmd )
{
   printf( "Usage: %s <filename>\n", cmd );
   exit( -1 );
}

int
main( int argc, char* argv[] )
{
   if( argc != 2 ) {
      usage( argv[ 0 ] );
   }

   FILE* f = fopen( argv[ 1 ], "r" );
   if( !f ) {
      perror( "fopen" );
      exit( -1 );
   }

   yyin = f;
   do {
      yyparse();
   } while( !feof( yyin ) );

   return 0;
}

void yyerror( const char* s ) {
   printf( "autobot.bs:%d: %s\n", yylineno, s );
   exit( -1 );
}


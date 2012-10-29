#include "testrunner.h"

#include <assert.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_test_result {
   const char* file;
   const char* test_name;
   int passes;
   int failures;
} TestResult;


typedef struct s_test {
   struct s_test* next;
   char* name;
   TestFunc func;
} Test;


Test *
test_create( const char* filename, TestFunc func )
{
   Test* t = malloc( sizeof( Test ) );
   t->next = 0;
   t->name = strdup( filename );
   t->func = func;
   return t;
}


void
test_delete( Test* test )
{
   free( test->name );
   free( test );
}


struct s_test_runner {
   TestResult current_test;
   Test* tests;
   int tests_run;
   int tests_passed;
   int checks_passed;
   int checks_failed;
};


static Test* test_list = NULL;

void
testrunner_register( const char* test_name, TestFunc func )
{
   Test** slot = &test_list;
   while( *slot ) {
      slot = &( *slot )->next;
   }

   *slot = test_create( test_name, func );
}


TestRunner *
testrunner_new()
{
   TestRunner* runner = calloc( 1, sizeof( TestRunner ) );
   runner->tests = test_list;
   test_list = NULL;

   return runner;
}


void
testrunner_delete( TestRunner* runner )
{
   Test* test;
   for( test = runner->tests; test; ) {
      Test* next = test->next;
      test_delete( test );
      test = next;
   }

   free( runner );
}


void
testrunner_check_passed( TestRunner* runner )
{
   runner->current_test.passes++;
}


void
testrunner_check_failed( TestRunner* runner,
                         const int line,
                         const char* format,
                         ... )
{
   char buf[ 2048 ];
   va_list args;

   if( !runner->current_test.failures++ ) {
      printf( "failed\n" );
   }

   va_start( args, format );
   vsnprintf( buf, sizeof( buf ), format, args );
   va_end( args );

   fprintf( stderr, "    line %d: %s\n", line, buf );
}


void
testrunner_start_test( TestRunner* runner,
                       const char* file,
                       const char* test_name )
{
   if( !runner->current_test.file
      || strcmp( file, runner->current_test.file )
   ) {
      runner->current_test.file = file;
      printf( "%s:\n", file );
   }

   runner->current_test.test_name = test_name;
   runner->current_test.passes    = 0;
   runner->current_test.failures  = 0;
   runner->current_test.test_name = 0;

   printf( "  %s ", test_name );
}


void
testrunner_end_test( TestRunner* runner )
{
   runner->tests_run++;
   if( !runner->current_test.failures ) {
      printf( "passed\n" );
      runner->tests_passed++;
   }

   runner->checks_passed += runner->current_test.passes;
   runner->checks_failed += runner->current_test.failures;
}


void
testrunner_add_test( TestRunner* runner,
                     const char* test_name,
                     TestFunc func )
{
}


int
testrunner_run( TestRunner* runner )
{
   Test* test;
   for( test = runner->tests; test; test = test->next ) {
      test->func( runner );
   }
   return runner->tests_run - runner->tests_passed;
}


void
testrunner_print_report( TestRunner* runner )
{
   if( runner->tests_run == runner->tests_passed ) {
      printf( "SUCCESS: All %d tests passed! (%d checks)\n",
              runner->tests_run,
              runner->checks_passed );
   } else {
      printf( "FAILURE: %d/%d tests failed! (%d/%d checks)\n",
              runner->tests_passed,
              runner->tests_run,
              runner->checks_passed,
              runner->checks_failed + runner->checks_passed );
   }
}


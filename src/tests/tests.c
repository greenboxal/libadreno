#include "testrunner.h"

int
main( int argc, char* argv[] )
{
   int result;
   TestRunner* runner;

   runner = testrunner_new();
   result = testrunner_run( runner );
   testrunner_print_report( runner );
   testrunner_delete( runner );

   return result;
}

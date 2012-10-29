/* Pinched from Cloister project - TODO: license attribution */

#ifndef _CLOISTER_TEST_FRAMEWORK_H
#define _CLOISTER_TEST_FRAMEWORK_H


/******************************************************************************/
/* Initializer/finalizer sample for MSVC and GCC.
   2010 Joe Lowe. Released into the public domain.
   */
#ifdef _MSC_VER

#define CCALL __cdecl
#pragma section(".CRT$XCU",read)
#define INITIALIZER(f) \
   static void __cdecl f(void); \
   __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
   static void __cdecl f(void)

#elif defined(__GNUC__)

#define CCALL
#define INITIALIZER(f) \
   static void f() __attribute__((constructor)); \
   static void f()

#endif
/******************************************************************************/


/* Public */
struct s_test_runner;
typedef struct s_test_runner TestRunner;

typedef void ( *TestFunc )( TestRunner* );

TestRunner* testrunner_new();
void testrunner_delete( TestRunner* runner );

int testrunner_run( TestRunner* runner );

void testrunner_print_report( TestRunner* runner );

#define check( __expression, ... ) \
   if( ( __expression ) ) { \
      testrunner_check_passed( __runner__ ); \
   } else { \
      testrunner_check_failed( __runner__, __LINE__, \
         "Assertion `" #__expression "` failed " __VA_ARGS__, 0 ); \
   }

#define check_equal( __x, __y, ... ) \
   if( ( __x == __y ) ) { \
      testrunner_check_passed( __runner__ ); \
   } else { \
      testrunner_check_failed( __runner__, __LINE__, \
         "Assertion `" #__x " == " #__y "` failed " __VA_ARGS__, 0 ); \
   }

#define TEST( test_name ) \
   static void test_ ## test_name ## _impl( TestRunner* ); \
   void test_ ## test_name( TestRunner* r ) { \
      testrunner_start_test( r, __FILE__, #test_name ); \
      test_ ## test_name  ## _impl( r ); \
      testrunner_end_test( r ); \
   } \
   INITIALIZER( testrunner_register_ ## test_name ) { \
      testrunner_register( #test_name, test_ ## test_name ); \
   } \
   static void test_ ## test_name ## _impl( TestRunner* __runner__ ) \

/* Private */
void testrunner_register( const char* test_name,
                          TestFunc func );

void testrunner_start_test( TestRunner* runner,
                            const char* file,
                            const char* test_name );

void testrunner_end_test( TestRunner* runner );
void testrunner_check_passed( TestRunner* runner );

void testrunner_check_failed( TestRunner* runner,
                              const int line,
                              const char* format,
                              ... );

#endif


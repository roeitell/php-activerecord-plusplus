#ifndef PHP_ACTIVERECORD_H
#define PHP_ACTIVERECORD_H

extern zend_module_entry activerecord_module_entry;
#define phpext_activerecord_ptr &activerecord_module_entry

#ifdef PHP_WIN32
#	define PHP_ACTIVERECORD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ACTIVERECORD_API __attribute__ ((visibility("default")))
#else
#	define PHP_ACTIVERECORD_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "php_activerecord_ce.h"
#include "php_activerecord_fe.h"

PHP_MINIT_FUNCTION(activerecord);
PHP_MSHUTDOWN_FUNCTION(activerecord);
PHP_RINIT_FUNCTION(activerecord);
PHP_RSHUTDOWN_FUNCTION(activerecord);
PHP_MINFO_FUNCTION(activerecord);

PHP_METHOD(ActiveRecordUtils, is_blank);
PHP_METHOD(ActiveRecordUtils, extract_options);
PHP_METHOD(ActiveRecordUtils, is_odd);
PHP_METHOD(ActiveRecordUtils, is_a);
PHP_METHOD(ActiveRecordUtils, pluralize);
PHP_METHOD(ActiveRecordUtils, singularize);
PHP_METHOD(ActiveRecordUtils, pluralize_if);
PHP_METHOD(ActiveRecordUtils, squeeze);

PHP_METHOD(ActiveRecordConfig, initialize);
PHP_METHOD(ActiveRecordConfig, instance);
PHP_METHOD(ActiveRecordConfig, __construct);
PHP_METHOD(ActiveRecordConfig, set_connections);
PHP_METHOD(ActiveRecordConfig, get_default_connection_string);
PHP_METHOD(ActiveRecordConfig, get_default_connection);
PHP_METHOD(ActiveRecordConfig, set_default_connection);
PHP_METHOD(ActiveRecordConfig, get_connection);
PHP_METHOD(ActiveRecordConfig, set_model_directory);
PHP_METHOD(ActiveRecordConfig, get_model_directory);
PHP_METHOD(ActiveRecordConfig, set_logging);
PHP_METHOD(ActiveRecordConfig, get_logging);
PHP_METHOD(ActiveRecordConfig, set_logger);
PHP_METHOD(ActiveRecordConfig, get_logger);
PHP_METHOD(ActiveRecordConfig, set_date_format);
PHP_METHOD(ActiveRecordConfig, get_date_format);
PHP_METHOD(ActiveRecordConfig, get_connections);

/*
ZEND_BEGIN_MODULE_GLOBALS(activerecord)
ZEND_END_MODULE_GLOBALS(activerecord)
 */

/* In every utility function you add that needs to use variables 
   in php_activerecord_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as ACTIVERECORD_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/


#ifdef ZTS
#define ACTIVERECORD_G(v) TSRMG(activerecord_globals_id, zend_activerecord_globals *, v)
#else
#define ACTIVERECORD_G(v) (activerecord_globals.v)
#endif

#endif
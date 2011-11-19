#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_activerecord.h"

ZEND_DECLARE_MODULE_GLOBALS(activerecord)

static int le_activerecord;
zend_class_entry * activerecord_config_ce;
zend_class_entry * activerecord_utils_ce;

const zend_function_entry activerecord_functions[] = {
	PHP_FE_END
};

zend_module_entry activerecord_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"activerecord",
	activerecord_functions,
	PHP_MINIT(activerecord),
	PHP_MSHUTDOWN(activerecord),
	PHP_RINIT(activerecord),
	PHP_RSHUTDOWN(activerecord),
	PHP_MINFO(activerecord),
#if ZEND_MODULE_API_NO >= 20010901
	"5.3",
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ACTIVERECORD
ZEND_GET_MODULE(activerecord)
#endif

PHP_MINIT_FUNCTION(activerecord)
{
	zend_class_entry ce;
	    
    INIT_CLASS_ENTRY(ce, "ActiveRecordUtils", activerecord_utils_methods);
    activerecord_utils_ce = zend_register_internal_class(&ce TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "ActiveRecordConfig", activerecord_config_methods);
	activerecord_config_ce = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null( activerecord_config_ce, "_instance", 9, ZEND_ACC_PRIVATE | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_string( activerecord_config_ce, "default_connection", 18, "development", ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "connections", 11, ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_string( activerecord_config_ce, "model_directory", 15, "", ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_bool( activerecord_config_ce, "logging", 7, 0, ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "logger", 6, ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_string( activerecord_config_ce, "date_format", 11, "Y-m-d\\TH:i:sO", ZEND_ACC_PRIVATE TSRMLS_CC );

    INIT_CLASS_ENTRY(ce, "ActiveRecordUtils", activerecord_model_methods);
    activerecord_model_ce = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null( activerecord_config_ce, "errors", 6, ZEND_ACC_PUBLIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "attributes", 10, ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "__dirty", 7, ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_bool( activerecord_config_ce, "__readonly", 10, 0, ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "__relationships", 15, ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_bool( activerecord_config_ce, "__new_record", 12, 1, ZEND_ACC_PRIVATE TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "connection", 10, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "db", 2, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "table_name", 10, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "primary_key", 11, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "sequence", 8, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "alias_attribute", 15, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "attr_accessible", 15, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "attr_protected", 14, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "delegate", 8, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "setters", 7, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );
	zend_declare_property_null( activerecord_config_ce, "getters", 7, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC );

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(activerecord)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(activerecord)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(activerecord)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(activerecord)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "activerecord support", "enabled");
	php_info_print_table_end();
}

/**
 * Access private property (mangle && fetch)
 *
 */
zval * activerecord_private_property( zval * classObj, const char * propName )
{
	char *privPropName;
	int privPropNameLen;
	zval **retVal;
	TSRMLS_FETCH();
	
		// deduces the name of the private property, within the classObj context
	zend_mangle_property_name( 
		&privPropName, &privPropNameLen, 
		Z_OBJCE_P(classObj)->name, Z_OBJCE_P(classObj)->name_length, 
		propName, strlen(propName), 1 
	);

		// fetches the private property into the zval pointer
	zend_hash_find( 
		Z_OBJPROP_P(classObj), 
		privPropName, privPropNameLen+1, 
		(void**)&retVal 
	);

		// returns the proeprty's string value
	return *retVal;
}

/**
 * Translate userland arguments into a single zval array.
 *	Similar to func_get_args().
 *
 */
void activerecord_pack_args( zval *args, zend_bool init )
{
	zend_execute_data *ex = EG(current_execute_data)->prev_execute_data;
	void **p = ex->function_state.arguments;
	int arg_count = (int)(zend_uintptr_t) *p, i;

	if( init )
	{
		MAKE_STD_ZVAL( args );
		array_init_size( args, arg_count );
	}
	
	for (i=0; i<arg_count; i++) {
		zval *element;
		ALLOC_ZVAL(element);
		*element = **((zval **) (p-(arg_count-i)));
		zval_copy_ctor(element);
		INIT_PZVAL(element);
		zend_hash_next_index_insert( args->value.ht, &element, sizeof(zval *), NULL);
	}
}

/**
 * Call a method by a callable string, passing a zval array containing all the packed arguments.
 *	Similar to call_user_func_array() with a string callable.
 */
zval * activerecord_call_function( char *function_name, zval *args )
{
	zval *callable, *retval, *retval_copy;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	MAKE_STD_ZVAL( callable );
	ZVAL_STRING( callable, function_name, 0 );
	
	zend_fcall_info_init( callable, &fci, &fci_cache TSRMLS_CC );
	zend_fcall_info_args( &fci, args TSRMLS_CC );
	fci.retval_ptr_ptr = &retval;

	if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr) {
		COPY_PZVAL_TO_ZVAL(*retval_copy, *fci.retval_ptr_ptr);
	}
	
	zend_fcall_info_args_clear(&fci, 1);

	return retval_copy;
}
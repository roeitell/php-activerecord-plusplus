#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_activerecord.h"

/*
 * Configuration details are stored in a request-global struct, instead of as
 * 	singleton properties. This is done to save the necessary method-calls when
 * 	from the ActiveRecord libraries.
 *
 */
const zend_function_entry activerecord_config_methods[] = {
	PHP_ME(ActiveRecordConfig, initialize, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordConfig, instance, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordConfig, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, set_connections, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, get_default_connection_string, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, get_default_connection, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, set_default_connection, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, get_connection, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, set_model_directory, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, get_model_directory, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, set_logging, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, get_logging, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, set_logger, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, get_logger, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, set_date_format, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, get_date_format, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(ActiveRecordConfig, get_connections, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

zval * activerecord_config_instance( TSRMLS_D )
{
	zval * instance;
	instance = zend_read_static_property( activerecord_config_ce, "_instance", 9, 0 TSRMLS_CC );
	if( Z_TYPE_P(instance) == IS_NULL )
	{
		MAKE_STD_ZVAL( instance );
		object_init_ex( instance, activerecord_config_ce );
		zend_update_static_property( activerecord_config_ce, "_instance", 9, instance TSRMLS_CC );
	}
	return instance;
}

zval * activerecord_find_connection_zval( char * conn_key TSRMLS_DC )
{
	zval ** result,
		 * conns = zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "connections", 11, 0 TSRMLS_CC );

	if( zend_hash_find( Z_ARRVAL_P(conns), conn_key, strlen(conn_key)+1, (void**)&result ) == FAILURE )
		return NULL;

	return *result;
}

char * activerecord_find_connection( char * conn_key TSRMLS_DC )
{
	return Z_STRVAL_P( activerecord_find_connection_zval( conn_key TSRMLS_CC ) );
}

char * activerecord_default_connection( TSRMLS_D )
{
	return activerecord_find_connection( 
		Z_STRVAL_P(
			zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "default_connection", 18, 0 TSRMLS_CC )
		) 
		TSRMLS_CC 
	);
}

PHP_METHOD(ActiveRecordConfig, __construct)
{
	zval * conns;
	
	MAKE_STD_ZVAL( conns );
	array_init( conns );
	
	zend_update_property( activerecord_config_ce, this_ptr, "connections", 11, conns TSRMLS_CC );
}

PHP_METHOD(ActiveRecordConfig, instance)
{
	zval * res = activerecord_config_instance( TSRMLS_C );
	RETURN_ZVAL( res, 1, 0 );
}

PHP_METHOD(ActiveRecordConfig, initialize)
{
	zval * result, *** params = emalloc( sizeof(zval**) ), * instance;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f", &fci, &fci_cache) == FAILURE )
		return;

	if (ZEND_FCI_INITIALIZED(fci)) {
		instance = activerecord_config_instance( TSRMLS_C );
		params[0] = &instance;
	
		fci.retval_ptr_ptr = &result;
		fci.param_count = 1;
		fci.params = params;
		fci.no_separation = 0;
		zend_call_function(&fci, &fci_cache TSRMLS_CC);
	}
}

PHP_METHOD(ActiveRecordConfig, set_connections)
{
	zval * conns, * defaultConn = NULL;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &conns, &defaultConn) == FAILURE )
		return;
	
	if( conns->type == IS_ARRAY )
		zend_update_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "connections", 11, conns TSRMLS_CC );

	if( defaultConn != NULL && defaultConn->type == IS_STRING )
		zend_update_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "default_connection", 18, defaultConn TSRMLS_CC );
}

PHP_METHOD(ActiveRecordConfig, get_default_connection_string)
{
	zval * conn = activerecord_find_connection_zval( 
		Z_STRVAL_P(
			zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "default_connection", 18, 0 TSRMLS_CC )
		) 
		TSRMLS_CC 
	);
	RETURN_ZVAL( conn, 1, 0 );
}

PHP_METHOD(ActiveRecordConfig, get_default_connection)
{
	zval * conn = zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "default_connection", 18, 0 TSRMLS_CC );
	RETURN_ZVAL( conn, 1, 0 );
}

PHP_METHOD(ActiveRecordConfig, set_default_connection)
{
	char * conn;
	int conn_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &conn, &conn_len) == FAILURE )
		return;

	zend_update_property_string( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "default_connection", 18, conn TSRMLS_CC );
}

PHP_METHOD(ActiveRecordConfig, set_model_directory)
{
	char * dir;
	int dir_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dir, &dir_len) == FAILURE )
		return;

	zend_update_property_string( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "model_directory", 15, dir TSRMLS_CC );
}

PHP_METHOD(ActiveRecordConfig, get_model_directory)
{
	zval * dir = zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "model_directory", 15, 0 TSRMLS_CC );
	RETURN_ZVAL( dir, 1, 0 );
}

PHP_METHOD(ActiveRecordConfig, get_connection)
{
	char * conn_key;
	int conn_len;
	zval * conn;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &conn_key, &conn_len) == FAILURE )
		return;

	conn = zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), conn_key, conn_len, 0 TSRMLS_CC );
	RETURN_ZVAL( conn, 1, 0 );
}

PHP_METHOD(ActiveRecordConfig, set_logging)
{
	zend_bool logging;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &logging) == FAILURE )
		return;

	zend_update_property_bool( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "logging", 7, logging TSRMLS_CC );
}

PHP_METHOD(ActiveRecordConfig, get_logging)
{
	zval * logging = zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "logging", 7, 0 TSRMLS_CC );
	RETURN_ZVAL( logging, 1, 0 );
}

PHP_METHOD(ActiveRecordConfig, set_date_format)
{
	char * date_format;
	int df_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &date_format, &df_len) == FAILURE )
		return;

	zend_update_property_string( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "logging", 7, date_format TSRMLS_CC );
}

PHP_METHOD(ActiveRecordConfig, get_date_format)
{
	zval * df = zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "date_format", 11, 0 TSRMLS_CC );
	RETURN_ZVAL( df, 1, 0 );
}

PHP_METHOD(ActiveRecordConfig, get_logger)
{
	zval * logger = zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "logger", 6, 0 TSRMLS_CC );
	RETURN_ZVAL( logger, 1, 0 );
}

PHP_METHOD(ActiveRecordConfig, set_logger)
{
	zval * logger;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &logger) == FAILURE )
		return;

	zend_update_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "logger", 6, logger TSRMLS_CC );
}

PHP_METHOD(ActiveRecordConfig, get_connections)
{
	zval * conns = zend_read_property( activerecord_config_ce, activerecord_config_instance( TSRMLS_C ), "connections", 11, 0 TSRMLS_CC );
	RETURN_ZVAL( conns, 1, 0 );
}
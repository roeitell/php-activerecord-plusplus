#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_activerecord.h"

/*
 * TODO:
 *	1. add activerecord_table_get_relationship( table, name, name_len ), null if not found
 */
const zend_function_entry activerecord_table_methods[] = {
	PHP_ME(ActiveRecordTable, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

zval * activerecord_table_load( char * name, int name_len )
{
	zval ** table,
		 * cache = zend_read_static_property( activerecord_table_ce, "cache", 5, 0 TSRMLS_CC );
	
	if( zend_hash_find( Z_ARRVAL_P(cache), name, name_len, (void**)&table ) == FAILURE )
	{
		MAKE_STD_ZVAL( *table );
		object_init_ex( *table, &activerecord_table_ce );
		//zend_call_method_with_1_params( model, *ce, NULL, "__construct", ... );
		//set_associations
		zend_hash_update( Z_ARRVAL_P(cache), name, name_len, *table, sizeof(zval*), NULL );
	}

	return * table;
}

PHP_METHOD(ActiveRecordTable, load)
{
	char * name;
	int name_len;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE )
		return;

	RETURN_ZVAL( activerecord_table_load(name, name_len), 1, 0 );
}

PHP_METHOD(ActiveRecordTable, clear_cache)
{
	zval * cache = zend_read_static_property( activerecord_table_ce, "cache", 5, 0 TSRMLS_CC );
	char * name;
	int name_len = 0;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE )
		return;
	
	if( name_len > 0 )
	{
		zend_hash_del_key_or_index( Z_ARRVAL_P(cache), name, name_len, 0, HASH_DEL_KEY );
	}
	else
	{
		ZVAL_NULL( cache );
	}
}

PHP_METHOD(ActiveRecordTable, __construct)
{
	char * name;
	int name_len;
	zval * conn, * table_name, * pk, * tmp;
	zend_class_entry **ce;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE )
		return;
	
	if( !zend_hash_find( EG(class_table), name, name_len+1, (void **) &ce ) )
		return;

	conn = zend_read_static_property( *ce, "connection", 10, 0 TSRMLS_CC );
	// conn = ConnectionManager::get_connection( conn );
	zend_update_property( activerecord_table_ce, this_ptr, "connection", 10, conn, TSRMLS_CC );
	
	table_name = zend_read_static_property( *ce, "table_name", 10, 0 TSRMLS_CC );
	// if is_null table_name, table_name = Inflector::instance()->tableize($this->class->getName());
	subst_table_name = strrchr( Z_STRVAL_P(table_name), '\\' );
	ZVAL_STRINGL( table_name, subst_table_name, strlen(subst_table_name), 1 );
	zend_update_property( activerecord_table_ce, this_ptr, "table", 5, table_name, TSRMLS_CC );
	
	zend_update_property( activerecord_table_ce, this_ptr, "db_name", 7, zend_read_static_property( *ce, "db", 2, 0 TSRMLS_CC ) TSRMLS_CC );
	//$this->columns = $this->conn->columns($this->get_fully_qualified_table_name($quote_name));
	
	pk = zend_read_static_property( *ce, "pk", 2, 0 TSRMLS_CC );
	if( Z_TYPE_P(pk) == IS_STRING )
	{
		ZVAL_STRINGL( tmp, Z_STRVAL_P(pk), Z_STRLEN_P(pk), 1 );
		array_init( pk );
		add_next_index_string( pk, tmp, 1 );
	}
	else if( Z_TYPE_P(pk) != IS_ARRAY )
	{
		MAKE_STD_ZVAL( pk );
		array_init( pk );
		//			foreach ($this->columns as $c)
		//	{
		//		if ($c->pk)
		//			$this->pk[] = $c->inflected_name;
		//	}
	}
	
	
}
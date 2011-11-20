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

char * activerecord_table_create_joins( zval *table, zval *joins )
{
	char *ret = (char*)emalloc( 1 /* a lot */ );
	HashPosition pos;
	zval **value, 
		 *relationships = zend_read_property(activerecord_table_ce, model, "relationships", 13, 0 TSRMLS_CC);

	if( Z_TYPE_P(joins) != IS_ARRAY )
		return Z_TYPE_P(joins) == IS_STRING? joins : "";

	for( 
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(joins), &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(joins), (void **)&value, &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(joins), &pos)
	)
	{
		if( Z_TYPE_PP(value) != IS_STRING )
			continue;
		if( !strstr( Z_STRVAL_PP(value),"JOIN ") )
		{
			if( zend_hash_find( Z_ARRVAL_P(relationships), Z_STRVAL_PP(value), Z_STRLEN_PP(value), NULL ) == SUCCESS )
			{
				/*
									$rel = $this->get_relationship($value);

					// if there is more than 1 join for a given table we need to alias the table names
					if (array_key_exists($rel->class_name, $existing_tables))
					{
						$alias = $value;
						$existing_tables[$rel->class_name]++;
					}
					else
					{
						$existing_tables[$rel->class_name] = true;
						$alias = null;
					}

					$ret .= $rel->construct_inner_join_sql($this, false, $alias);
				*/
			}
			else
				/* throw exception */;
		}
		else
		{
			strcat( ret, Z_STRVAL_PP(value) );
		}
	}

	return ret;
}

activerecord_sql * options_to_sql( zval *table, zval *options )
{
	activerecord_sql *res = activerecord_sql_new();
	zval **tmp;

	if( Z_TYPE_P(options) != IS_ARRAY )
		/* throw exception */;
	
	if( zend_hash_find( Z_ARRVAL_P(options), "from", 4, (void**)&tmp ) == SUCCESS )
	{
		res->table = *tmp;
	}
	else
	{
		/*res->table = activerecord_table_fully_qualified_name( table );*/
	}
	
	if( zend_hash_find( Z_ARRVAL_P(options), "joins", 4, (void**)&tmp ) == SUCCESS )
	{
		res->joins = activerecord_table_create_joins( table, *tmp );

		if( zend_hash_find( Z_ARRVAL_P(options), "select", 6, NULL ) == FAILURE )
			/* zend_hash_add( Z_ARRVAL_P(options), "select", 6, activerecord_table_fully_qualified_name(table), sizeof(zval*), NULL ); */
	}
	
	if( zend_hash_find( Z_ARRVAL_P(options), "select", 4, (void**)&tmp ) == SUCCESS )
	{
		res->select = *tmp;
	}

	if( zend_hash_find( Z_ARRVAL_P(options), "conditions", 10, (void**)&tmp ) == SUCCESS )
	{
		if( !activerecord_is_hash(*tmp) && Z_TYPE_PP(tmp) == IS_STRING )
		{
				zval *new_conditions;
				MAKE_STD_ZVAL( new_conditions );
				array_init_size( new_conditions, 1 );
				zend_hash_next_index_insert( Z_ARRVAL_P(new_conditions), &tmp, sizeof(zval *), NULL );
				zend_hash_add( Z_ARRVAL_P(options), "conditions", 10, new_conditions, sizeof(zval*), NULL );
				tmp = new_conditions;
		}
		else
		{
			zval **mapped_names;
			zend_hash_find( Z_ARRVAL_P(options), "mapped_names", 12, (void**)&mapped_names );
			if( Z_TYPE_PP(mapped_names) == IS_ARRAY )
				/*zend_hash_add( Z_ARRVAL_P(options), "conditions", 10, activerecord_map_names(tmp, mapped_names), sizeof(zval*), NULL );*/
		}
		activerecord_sql_create_where_zval( res, tmp ); /* see activerecord_sql_create_where_zval, many cases to consider */
	}
	
	if( zend_hash_find( Z_ARRVAL_P(options), "order", 5, (void**)&tmp ) == SUCCESS )
	{
		res->order = *tmp;
	}
	if( zend_hash_find( Z_ARRVAL_P(options), "limit", 5, (void**)&tmp ) == SUCCESS )
	{
		res->limit = *tmp;
	}
	if( zend_hash_find( Z_ARRVAL_P(options), "offset", 6, (void**)&tmp ) == SUCCESS )
	{
		res->offset = *tmp;
	}
	if( zend_hash_find( Z_ARRVAL_P(options), "group", 5, (void**)&tmp ) == SUCCESS )
	{
		res->group = *tmp;
	}
	if( zend_hash_find( Z_ARRVAL_P(options), "having", 6, (void**)&tmp ) == SUCCESS )
	{
		res->having = *tmp;
	}

	return res;
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
	zval * conn, * table_name, * pk;
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
		zval *tmp;
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


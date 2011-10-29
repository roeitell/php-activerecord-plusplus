#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_activerecord.h"

/*
 *
 */
const zend_function_entry activerecord_model_methods[] = {
	PHP_ME(ActiveRecordModel, __construct, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

zend_bool activerecord_model_validate( zval * model )
{
	// TODO: Complete
}

void activerecord_model_invoke_callback( char * method_name, zend_bool foo )
{
	// TODO: Complete
}

void activerecord_model_flag_dirty( zval * model, char * attr, int attr_len )
{
	zval * dirty = zend_read_property(activerecord_model_ce, model, "__dirty", 7, 0 TSRMLS_CC);
	zval * trueval;
	if( Z_TYPE_P(dirty) == IS_NULL )
		array_init( dirty );
		
	ZVAL_BOOL(trueval, 1);
	
	zend_hash_add(
		Z_ARRVAL_P(dirty), attr, attr_len, trueval, sizeof(zval*), NULL
	);
}

zval * activerecord_model_read_attribute( zval * model, char * name, int name_len )
{
	zval ** alias, ** retval = NULL, ** tmp;
	zval * table = activerecord_table_load( EG(scope)->name, EG(scope)->name_length );
	
		// alias?
	if( zend_hash_find( Z_ARRVAL_P(zend_read_static_property(activerecord_model_ce, "alias_attribute", 15, 0 TSRMLS_CC)), name, name_len, (void**)&alias ) == SUCCESS )
	{
		efree( name );
		emalloc( name, sizeof( Z_STRVAL_PP(alias) ) );
		strncpy( name, Z_STRVAL_PP(alias), Z_STRLEN_PP(alias) );
		name_len = Z_STRLEN_PP(alias);
	}
	
		// straight-forward attribute?
	if( zend_hash_find( Z_ARRVAL_P(zend_read_property(activerecord_model_ce, model, "attributes", 10, 0 TSRMLS_CC)), name, name_len, (void**)&retval) == SUCCESS )
	{
	}
	
		// relationship?
	else if( zend_hash_find( Z_ARRVAL_P(zend_read_property(activerecord_model_ce, model, "__relationships", 15, 0 TSRMLS_CC)), name, name_len, (void**)&retval) == SUCCESS )
	{
	}
	else if( NULL != (*tmp = activerecord_table_get_relationship( table, name, name_len )) )
	{
		zend_hash_add( Z_ARRVAL_P(zend_read_property(activerecord_model_ce, model, "__relationships", 15, 0 TSRMLS_CC)),
			name, name_len, *tmp, sizeof(zval*), NULL
		);
		retval = *tmp;
	}
	
		// pk?
	else if( strcmp(name, "id") == 0 )
	{
		zend_hash_index_find(
			Z_ARRVAL_P( zend_read_property( activerecord_model_table, table, "pk", 2, 0 TSRMLS_CC ) ),
			0, (void**)&tmp
		);
		zend_hash_find( Z_ARRVAL_P(zend_read_property(activerecord_model_ce, model, "attributes", 10, 0 TSRMLS_CC)), Z_STRVAL_PP( tmp ), Z_STRLEN_PP( tmp ), (void**)&retval);
	}
	
	if( retval == NULL );
		// throw something
	
	efree( name );
	RETURN_ZVAL( retval, 0, 0 );
}

zval * activerecord_model_assign_attribute( zval * model, char * key, int key_len, zval * value )
{
	zval * table = activerecord_table_load( EG(called_scope)->name, EG(called_scope)->name_length );
	/*
			
		if (array_key_exists($name,$table->columns) && !is_object($value))
			$value = $table->columns[$name]->cast($value,static::connection());

		// convert php's \DateTime to ours
		if ($value instanceof \DateTime)
			$value = new DateTime($value->format('Y-m-d H:i:s T'));

		// make sure DateTime values know what model they belong to so
		// dirty stuff works when calling set methods on the DateTime object
		if ($value instanceof DateTime)
			$value->attribute_of($this,$name);
	*/
	zend_hash_add( 
		Z_ARRVAL_P(zend_read_property(activerecord_model_ce,model,"attributes",10,0 TSRMLS_CC)),
		key, key_len, value, sizeof(zval*), NULL
	);
	activerecord_model_flag_dirty( model, key, key_len );
	
	return value;
}

void activerecord_model_assign_attributes( zval * model, zval * attributes, zend_bool guard )
{
	zval * accessible 	= zend_read_static_property(activerecord_model_ce, "attr_accessible", 15, 0 TSRMLS_CC);
	zval * protected	= zend_read_static_property(activerecord_model_ce, "attr_protected", 14, 0 TSRMLS_CC);
	zval ** attribute;
	int acc_count = zend_hash_num_elements(Z_ARRVAL_P(accessible));
	int prt_count = zend_hash_num_elements(Z_ARRVAL_P(protected));
	HashPosition pos;
	int index, key_len;
	char * key_name;
	
	for( zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(attributes), &pos); 
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(attributes), (void **) &attribute, &pos) == SUCCESS; 
		 zend_hash_move_forward_ex(Z_ARRVAL_P(attributes), &pos) )
	{
		/*
					if (array_key_exists($name,$table->columns))
			{
				$value = $table->columns[$name]->cast($value,$connection);
				$name = $table->columns[$name]->inflected_name;
			}
		*/
		zend_hash_get_current_key_ex( Z_ARRVAL_P(attributes), &key_name, &key_len, &index, 0, &pos);
		if( guard )
		{
			if( acc_count > 0 && !activerecord_model_array_search( accessible, key_name ) )
				continue;

			if( prt_count > 0 && activerecord_model_array_search( protected, key_name ) )
				continue;

			//$this->$name = $value; ( __set() )
			//add_property_zval_ex( model, key_name, key_len, *attribute, 0 TSRMLS_CC );
		}
		else
		{
			
		}
	}
}

zend_bool activerecord_model_array_search( zval * arr, char * value )
{
	zval  **entry;
	HashPosition pos;
	zend_bool res = 0;
	
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);
	while( zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **)&entry, &pos) == SUCCESS ) 
	{
		if( strcmp(Z_STRVAL_PP(entry), value) == 0 )
		{
			res = 1;
			break;
		}
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos);
	}

	if( !res )
	{
		RETURN_FALSE;
	}
	else
	{
		RETURN_TRUE;
	}
}

void activerecord_model_verify_readonly( zval * model )
{
	zval * ro = zend_read_property(activerecord_model_ce, model, "__readonly", 10, 0 TSRMLS_CC);
	if( Z_BVAL_P(ro) == 0 );
		//zend_throw_exception(zend_class_entry *exception_ce, char *message, long code TSRMLS_DC) 
}}

void activerecord_model_insert( zval * model, zend_bool validate )
{
	zval * table = activerecord_table_load( EG(scope)->name, EG(scope)->name_length );
	zval * dirty = zend_read_property(activerecord_model_ce, this_ptr, "__dirty", 7, 0 TSRMLS_CC);
	zval * attributes, ** pk, * tmp1, * tmp2;

	activerecord_model_verify_readonly( model );
	if( validate )
	{
		if( !activerecord_model_validate( model ) || !activerecord_model_invoke_callback("before_create", 0) )
		{
			RETURN_FALSE;
		}
	}

	attributes = ( Z_TYPE_P(dirty) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(dirty)) > 0 ) ?
		dirty : zend_read_property( activerecord_model_ce, this_ptr, "attributes", 10, 0 TSRMLS_CC );
	
	zend_hash_find( Z_ARRVAL_P(table), "pk", 2, (void**)&pk );
	
	zend_call_method_with_1_params( table, Z_OBJ_CE(table), NULL, "insert", NULL, attributes );
	
	if( zend_hash_num_elements(Z_ARRVAL_P(pk)) == 1 )
	{
		//	$column = $table->get_column_by_inflected_name($pk[0]);
		//	if ($column->auto_increment || $use_sequence)
		//		$this->attributes[$pk[0]] = $table->conn->insert_id($table->sequence);
	}
	
	ZVAL_STRING( tmp1, "invoke_callback" );
	ZVAL_BOOL( tmp2, 0 );
	zend_call_method_with_2_params( this_ptr, activerecord_model_ce, NULL, tmp1, tmp2 );
	zend_update_property_bool( activerecord_model_ce, this_ptr, "__new_record", 12, 0 TSRMLS_CC );
	RETURN_TRUE;
}

void activerecord_model_update( zval * model, zend_bool validate )
{
	zval * table = activerecord_table_load( EG(scope)->name, EG(scope)->name_length );
	zval * dirty = zend_read_property(activerecord_model_ce, this_ptr, "__dirty", 7, 0 TSRMLS_CC);
	zval * attributes, ** pk, * tmp1, * tmp2;

	activerecord_model_verify_readonly( model );
	if( validate && !activerecord_model_validate( model ) )
	{
		RETURN_FALSE;
	}

	if( Z_TYPE_P(dirty) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(dirty)) > 0 )
	{
	}
		dirty : zend_read_property( activerecord_model_ce, this_ptr, "attributes", 10, 0 TSRMLS_CC );
	
	zend_hash_find( Z_ARRVAL_P(table), "pk", 2, (void**)&pk );
	
	zend_call_method_with_1_params( table, Z_OBJ_CE(table), NULL, "insert", NULL, attributes );
	
	if( zend_hash_num_elements(Z_ARRVAL_P(pk)) == 1 )
	{
		//	$column = $table->get_column_by_inflected_name($pk[0]);
		//	if ($column->auto_increment || $use_sequence)
		//		$this->attributes[$pk[0]] = $table->conn->insert_id($table->sequence);
	}
	
	ZVAL_STRING( tmp1, "invoke_callback" );
	ZVAL_BOOL( tmp2, 0 );
	zend_call_method_with_2_params( this_ptr, activerecord_model_ce, NULL, tmp1, tmp2 );
	zend_update_property_bool( activerecord_model_ce, this_ptr, "__new_record", 12, 0 TSRMLS_CC );
	RETURN_TRUE;
}

void activerecord_model_delete( zval * model )
{
	activerecord_model_verify_readonly( model );
	// TODO: Complete
}

zval * activerecord_model_magic_set( zval * model, char * name, int name_len, zval * value )
{
	zval ** alias, * retval = NULL;
	char * method_name;
	
	if( zend_hash_find( Z_ARRVAL_P(zend_read_static_property(activerecord_model_ce, "alias_attribute", 15, 0 TSRMLS_CC)), name, name_len, (void**)&alias ) == SUCCESS )
	{
		efree( name );
		emalloc( name, sizeof( Z_STRVAL_PP(alias) ) );
		strncpy( name, Z_STRVAL_PP(alias), Z_STRLEN_PP(alias) );
		name_len = Z_STRLEN_PP(alias);
	}
	else 
	{
		emalloc( method_name, sizeof(name)+4 );
		strcpy( method_name, "set_" );
		strcat( method_name, name );
		if( activerecord_model_array_search( zend_read_property( activerecord_model_ce, this_ptr, "setters", 7, 0 TSRMLS_CC ), method_name) )
		{
			zend_call_method( &this_ptr, activerecord_model_ce, NULL, method_name, sizeof(method_name), &retval, 0, NULL, NULL TSRMLS_CC );
			efree( name );
			RETURN_ZVAL( retval, 0, 0 );
		}
	}

	if( zend_hash_find( Z_ARRVAL_P(zend_read_property(activerecord_model_ce, model, "attributes", 10, 0 TSRMLS_CC)), name, name_len ) == SUCCESS )
	{
		retval = activerecord_model_assign_attribute( name, name_len, value );
	}

	efree( name );
	if( retval == NULL );
		// throw new exception

	RETURN_ZVAL( retval, 0, 0 );
}

zval * activerecord_model_magic_get( zval * model, char * name, int name_len )
{
	char * method_name = emalloc( 4 + sizeof(name) );
	zval * retval;

	strcpy( method_name, "get_" );
	strcat( method_name, name );
	if( activerecord_model_array_search( zend_read_property( activerecord_model_ce, this_ptr, "getters", 7, 0 TSRMLS_CC ), method_name) )
		zend_call_method( &this_ptr, activerecord_model_ce, NULL, method_name, sizeof(method_name), &retval, 0, NULL, NULL TSRMLS_CC );
	else
		retval = activerecord_model_read_attribute( model, name, name_len );

	efree( method_name );
	return retval;
}

zval * activerecord_model_values_for( zval * model, zval * attribute_names )
{
	zval * values_for, ** name;
	HashPosition pos;
	
	MAKE_STD_ZVAL( values_for );
	array_init( values_for );
	
	
	int index, key_len;
	char * key_name;
	
	for( zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(attribute_names), &pos); 
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(attribute_names), (void **) &name, &pos) == SUCCESS; 
		 zend_hash_move_forward_ex(Z_ARRVAL_P(attribute_names), &pos) )
	{
		zend_hash_add(
			Z_ARRVAL_P(values_for), name, sizeof(name), activerecord_model_magic_get( model, name, name_len ), sizeof(zval*), NULL
		);
	}

	return values_for;
}

zend_bool activerecord_model_isset( zval * model, char * name, int name_len )
{
	return 
		zend_hash_find(Z_ARRVAL_P(zend_read_property(activerecord_model_ce, this_ptr, "attributes", 10, 0 TSRMLS_CC)), name, name_len, NULL) == SUCCESS 
			||
		zend_hash_find(Z_ARRVAL_P(zend_read_static_property(activerecord_model_ce, "alias_attribute", 15, 0 TSRMLS_CC)), name, name_len, NULL) == SUCCESS; 
}

PHP_METHOD(ActiveRecordModel, __construct)
{
	zval * attrs;
	zend_bool guard = 1, via_find = 0, new_record = 1;
	
	array_init( zend_read_property(activerecord_model_ce, this_ptr, "attributes", 10, 0 TSRMLS_CC) );
	array_init( zend_read_property(activerecord_model_ce, this_ptr, "__relationships", 15, 0 TSRMLS_CC) );
	array_init( zend_read_property(activerecord_model_ce, this_ptr, "alias_attribute", 15, 0 TSRMLS_CC) );
	array_init( zend_read_property(activerecord_model_ce, this_ptr, "attr_accessible", 15, 0 TSRMLS_CC) );
	array_init( zend_read_property(activerecord_model_ce, this_ptr, "attr_protected", 14, 0 TSRMLS_CC) );
	array_init( zend_read_property(activerecord_model_ce, this_ptr, "delegate", 8, 0 TSRMLS_CC) );
	array_init( zend_read_property(activerecord_model_ce, this_ptr, "setters", 7, 0 TSRMLS_CC) );
	array_init( zend_read_property(activerecord_model_ce, this_ptr, "getters", 7, 0 TSRMLS_CC) );
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|zbbb", &attrs, &guard, &via_find, &new_record) == FAILURE )
		return;

	if( !via_find )
	{
		// TODO: Depends on Table
	}
	MAKE_STD_ZVAL( attrs );
	array_init( attrs );
	
	// TODO: Complete
}

PHP_METHOD(ActiveRecordModel, __get)
{
	char * name;
	int name_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name &name_len) == FAILURE )
		return;
		
	RETURN_ZVAL( activerecord_model_magic_get( this_ptr, name, name_len ), 0, 0 );
}

PHP_METHOD(ActiveRecordModel, __isset)
{
	char * name;
	int name_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name &name_len) == FAILURE )
		return;

	RETURN_BOOL(
		activerecord_model_isset( this_ptr, name, name_len )
	);
}

PHP_METHOD(ActiveRecordModel, __set)
{
	char * name;
	int name_len;
	zval * value;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name &name_len, &value) == FAILURE )
		return;
	
	RETURN_ZVAL( activerecord_model_magic_set( this_ptr, name, name_len, value ), 1, 0 );
}

PHP_METHOD(ActiveRecordModel, __wakeup)
{
	activerecord_table_load( EG(called_scope)->name, EG(called_scope)->name_length );
}

PHP_METHOD(ActiveRecordModel, assign_attribute)
{
	char * key;
	int key_len;
	zval * value;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &key_len, &value) == FAILURE )
		return;

	RETURN_ZVAL(
		activerecord_model_assign_attribute( this_ptr, key, key_len, value ), 1, 0
	);
}

PHP_METHOD(ActiveRecordModel, read_attribute)
{
	char * key;
	int key_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE )
		return;

	RETURN_ZVAL(
		activerecord_model_read_attribute( this_ptr, key, key_len ), 0, 0
	);
}

PHP_METHOD(ActiveRecordModel, flag_dirty)
{
	char * key;
	int key_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE )
		return;

	activerecord_model_flag_dirty( this_ptr, key, key_len );
}

PHP_METHOD(ActiveRecordModel, dirty_attributes)
{
	zval * dirty = zend_read_property(activerecord_model_ce, this_ptr, "__dirty", 7, 0 TSRMLS_CC);
	zval * attributes = zend_read_property(activerecord_model_ce, this_ptr, "attributes", 10, 0 TSRMLS_CC);
	zval **data;
	Bucket * p;

	if( Z_TYPE_P(dirty) == IS_NULL )
	{
		RETURN_NULL();
	}

	array_init(return_value);
	for( p = Z_ARRVAL_PP(dirty)->pListHead; p != NULL; p = p->pListNext )
	{
		if( p->nKeyLength > 0 )
		{
			if( zend_hash_quick_find(Z_ARRVAL_PP(attributes), p->arKey, p->nKeyLength, p->h, (void**)&data ) != FAILURE )
			{
				Z_ADDREF_PP((zval**)p->pData);
				zend_hash_quick_update(Z_ARRVAL_P(return_value), p->arKey, p->nKeyLength, p->h, p->pData, sizeof(zval*), NULL);
			}
		}
	}
}

PHP_METHOD(ActiveRecordModel, attributes)
{
	RETURN_ZVAL(
		zend_read_property(activerecord_model_ce, this_ptr, "attributes", 10, 0 TSRMLS_CC), 1, 0
	);
}

PHP_METHOD(ActiveRecordModel, get_primary_key)
{
	zval * table = activerecord_table_load( EG(scope)->name, EG(scope)->name_length );
	zval ** pk;
	zend_hash_find( Z_ARRVAL_P(table), "pk", 2, (void**)&pk );
	RETURN_ZVAL( *pk, 1, 0 );
}

PHP_METHOD(ActiveRecordModel, get_real_attribute_name)
{
	zval ** result;
	zval * attributes = zend_read_property(activerecord_model_ce, model, "attributes", 10, 0 TSRMLS_CC);
	zval * aliases = zend_read_static_property(activerecord_model_ce, "alias_attribute", 15, 0 TSRMLS_CC);
	char * name;
	int name_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE )
		return;

	if( zend_hash_find( Z_ARRVAL_P(attributes), name, name_len, (void**)&result ) != FAILURE )
	{
		RETURN_STRING( name, 1 );
	}
	else if ( zend_hash_find( Z_ARRVAL_P(aliases), name, name_len, (void**)&result ) != FAILURE )
	{
		RETURN_ZVAL( *result, 0, 0 );
	}
}

PHP_METHOD(ActiveRecordModel, get_validation_rules)
{
	// TODO: depends on Validations
}

PHP_METHOD(ActiveRecordModel, get_values_for)
{
	zval * keys, ** name, ** value;
	zval * attributes = zend_read_property(activerecord_model_ce, model, "attributes", 10, 0 TSRMLS_CC);
	HashPosition pos;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &keys) == FAILURE )
		return;
	
	array_init( return_value );
	for( zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(keys), &pos); 
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(keys), (void **) &name, &pos) == SUCCESS; 
		 zend_hash_move_forward_ex(Z_ARRVAL_P(keys), &pos) )
	{
		if( zend_hash_find(Z_ARRVAL_P(attributes), name, strlen(name)+1, (void**)&value) == SUCCESS )
			zend_hash_update( Z_ARRVAL_P(return_value), name, strlen(name)+1, *value, sizeof(zval*), NULL );
	}
}

PHP_METHOD(ActiveRecordModel, table_name)
{
	zval * table = activerecord_table_load( EG(called_scope)->name, EG(called_scope)->name_length );
	zval ** name;
	zend_hash_find( Z_ARRVAL_P(table), "table", 5, (void**)&name );
	RETURN_ZVAL( *name, 1, 0 );
}

PHP_METHOD(ActiveRecordModel, is_readonly)
{
	RETURN_ZVAL(
		zend_read_property(activerecord_model_ce, this_ptr, "__readonly", 10, 0 TSRMLS_CC), 1, 0
	);
}

PHP_METHOD(ActiveRecordModel, is_new_record)
{
	RETURN_ZVAL(
		zend_read_property(activerecord_model_ce, this_ptr, "__new_record", 12, 0 TSRMLS_CC), 1, 0
	);
}

PHP_METHOD(ActiveRecordModel, readonly)
{
	zval * readonly;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &readonly) == FAILURE )
		return;

	if( Z_TYPE_P(readonly) == IS_BOOL )
		zend_update_property(activerecord_model_ce, this_ptr, "__readonly", 10, readonly TSRMLS_CC);
}

PHP_METHOD(ActiveRecordModel, connection)
{
	zval * table = activerecord_table_load(EG(called_scope)->name, EG(called_scope)->name_length );
	zval ** conn;
	zend_hash_find( Z_ARRVAL_P(table), "conn", 4, (void**)&conn );
	RETURN_ZVAL( *conn, 1, 0 );
}

PHP_METHOD(ActiveRecordModel, table)
{
	RETURN_ZVAL( 
		activerecord_table_load( EG(called_scope)->name, EG(called_scope)->name_length ), 1, 0
	);
}

PHP_METHOD(ActiveRecordModel, create)
{
	char * class_name = EG(called_scope)->name;
	int class_name_len = EG(called_scope)->name_length;
	zend_bool validate = 1;
	zval * attributes, * model;
	zend_class_entry ** ce;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &attributes, &validate) == FAILURE )
		return;

	if( !zend_hash_find( EG(class_table), class_name, class_name_len, (void **) &ce ) )
		return;

	object_init_ex( model, *ce );
	zend_call_method_with_1_params( model, *ce, NULL, "__construct", NULL, attributes );
	zend_call_method_with_1_params( model, *ce, NULL, "save", NULL, validate );
	
	RETURN_ZVAL( model, 1, 0 );
}

PHP_METHOD(ActiveRecordModel, save)
{
	zend_bool validate = 1;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &validate) == FAILURE )
		return;

	if( Z_BVAL_P(zend_read_property(activerecord_model_ce, this_ptr, "__new_record", 12, 0 TSRMLS_CC)) == 1 )
		activerecord_model_insert( this_ptr, validate );
	else
		activerecord_model_update( this_ptr, validate );
}

PHP_METHOD(ActiveRecordModel, delete)
{
	 // TODO: Complete...
	activerecord_model_delete( this_ptr );
}

PHP_METHOD(ActiveRecordModel, values_for_pk)
{
	RETURN_ZVAL(
		activerecord_model_values_for(zend_hash_find(
			activerecord_table_ce, 
			activerecord_table_load( EG(scope)->name, EG(scope)->name_length ), 
			"pk", 2, 0 TSRMLS_CC 
		))
	);
}

PHP_METHOD(ActiveRecordModel, values_for)
{
	zval * attribute_names;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &attribute_names) == FAILURE )
		return;
	
	RETURN_ZVAL(
		activerecord_model_values_for( this_ptr, attribute_names ), 1, 0
	);
}

PHP_METHOD(ActiveRecordModel, is_dirty)
{
	RETURN_BOOL(
		zend_hash_num_elements( Z_ARRVAL_P(zend_read_property(activerecord_model_ce, this_ptr, "__dirty", 7, 0 TSRMLS_CC)) ) > 0
	);
}

PHP_METHOD(ActiveRecordModel, is_valid)
{
	RETURN_BOOL(
		activerecord_model_validate( this_ptr )
	);
}

PHP_METHOD(ActiveRecordModel, is_invalid)
{
	RETURN_BOOL(
		!activerecord_model_validate( this_ptr )
	);
}

PHP_METHOD(ActiveRecordModel, set_timestamps)
{
	zval * date;
	ZVAL_STRING( date, php_format_date("Y-m-d H:i:s", 11, time(NULL), 1 TSRMLS_CC)

	if( activerecord_model_isset( this_ptr, "updated_at", 10 ) )
		activerecord_model_assign_attribute( this_ptr, "updated_at", 10, date );
	if( activerecord_model_isset( this_ptr, "created_at", 10 ) )
		activerecord_model_assign_attribute( this_ptr, "created_at", 10, date );
	
}

PHP_METHOD(ActiveRecordModel, update_attributes)
{
	zval * attrs, * retval;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &attrs) == FAILURE )
		return;
	
	zend_call_method_with_1_params( activerecord_model_ce, this_ptr, NULL, "activerecord_model_assign_attributes", NULL, attrs );
	zend_call_method_with_0_params( activerecord_model_ce, this_ptr, NULL, "save", &retval );
	
	RETURN_ZVAL( retval, 1, 0 );
}

PHP_METHOD(ActiveRecordModel, update_attribute)
{
	char * name;
	int name_len;
	zval * value, * retval, * falseval;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &name_len, &value) == FAILURE )
		return;

	activerecord_model_magic_set( this_ptr, name, name_len, value );
	
	ZVAL_BOOL( falseval, 0 );
	zend_call_method_with_1_params( activerecord_model_ce, this_ptr, NULL, "update", &retval, falseval );
	
	RETURN_ZVAL( retval, 1, 0 );
}

PHP_METHOD(ActiveRecordModel, set_attributes)
{
	zval * attrs;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &attrs) == FAILURE )
		return;
		
	activerecord_model_assign_attributes( this_ptr, attrs, 1 );
}

PHP_METHOD(ActiveRecordModel, set_relationship_from_eager_load)
{
	zval * model, * rel,
		 * table = activerecord_table_load( EG(scope)->name, EG(scope)->name_length );
	char * name;
	int name_len;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs", &model, &name, &name_len) == FAILURE )
		return;

	rel = activerecord_table_get_relationship( table, name, name_len );
/*	
				if ($rel->is_poly())
			{
				// if the related model is null and it is a poly then we should have an empty array
				if (is_null($model))
					return $this->__relationships[$name] = array();
				else
					return $this->__relationships[$name][] = $model;
			}
			else
				return $this->__relationships[$name] = $model;
		throw new RelationshipException("Relationship named $name has not been declared for class: {$table->class->getName()}");
*/
}

PHP_METHOD(ActiveRecordModel, reload)
{
	zval * empty_arr, * null_zval, * attrs, * pks, ** pk, * pk_values, ** pk_value, * record;
	zval * attributes = zend_read_property(activerecord_model_ce, model, "attributes", 10, 0 TSRMLS_CC);
	HashPosition pos;
	
	MAKE_STD_ZVAL( empty_arr );
	array_init( empty_arr );
	zval_add_ref( empty_arr );
	
	zend_update_property( activerecord_model_ce, this_ptr, "__relationships", 15, empty_arr TSRMLS_CC );
	zend_call_method( &this_ptr, activerecord_model_ce, NULL, "get_primary_key", 15, &pks, 0, NULL, NULL TSRMLS_CC );

	array_init( pk_values );
	for( zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(pks), &pos); 
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(pks), (void **) &pk, &pos) == SUCCESS; 
		 zend_hash_move_forward_ex(Z_ARRVAL_P(pks), &pos) )
	{
		if( zend_hash_find(Z_ARRVAL_P(attributes), pk, sizeof(pk), (void**)&pk_value) == SUCCESS )
		{
			zval_add_ref( pk_value );
			zend_hash_next_index_insert(Z_ARRVAL_P(pk_values), pk_value, sizeof(zval *), NULL);
		}
	}

	zend_call_method_with_1_params( this_ptr, activerecord_model_ce, NULL, "find", &record, pk_values );
	activerecord_model_assign_attributes( this_ptr, pk_values, 1 );

	MAKE_STD_ZVAL( null_zval );
	ZVAL_NULL( null_zval );
	zval_add_ref( null_zval );
	zend_update_property( activerecord_model_ce, this_ptr, "__dirty", 7, null_zval TSRMLS_CC );

	RETURN_ZVAL( this_ptr, 0, 0 );
}

PHP_METHOD(ActiveRecordModel, __clone)
{
	zval * empty_arr, * null_zval;
		
	MAKE_STD_ZVAL( empty_arr );
	array_init( empty_arr );
	zval_add_ref( empty_arr );
	zend_update_property( activerecord_model_ce, this_ptr, "__relationships", 15, empty_arr TSRMLS_CC );
	
	MAKE_STD_ZVAL( null_zval );
	ZVAL_NULL( null_zval );
	zval_add_ref( null_zval );
	zend_update_property( activerecord_model_ce, this_ptr, "__dirty", 7, null_zval TSRMLS_CC );

	RETURN_ZVAL( this_ptr, 0, 0 );
}

PHP_METHOD(ActiveRecordModel, reset_dirty)
{
	zval * null_zval;

	MAKE_STD_ZVAL( null_zval );
	ZVAL_NULL( null_zval );
	zval_add_ref( null_zval );
	zend_update_property( activerecord_model_ce, this_ptr, "__dirty", 7, null_zval TSRMLS_CC );
}

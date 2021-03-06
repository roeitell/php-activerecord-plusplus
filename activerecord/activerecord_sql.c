#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_activerecord.h"

typedef struct {
	char * table;
	char * select;
	char * joins;
	char * order;
	char * limit;
	char * offset;
	char * group;
	char * having;
	
	char * where;
	zval ** where_values;
	
	int data_len;
	int data_size;
	char ** data_keys;
	char ** data_values;
} activerecord_sql;

activerecord_sql * activerecord_sql_new()
{
	activerecord_sql * result = emalloc( sizeof(activerecord_sql) );
	result->table = NULL;
	result->select = NULL;
	result->joins = NULL;
	result->order = NULL;
	result->limit = NULL;
	result->offset = NULL;
	result->group = NULL;
	result->having = NULL;
	result->where = NULL;
	result->where_values = NULL;
	result->data_len = 0;
	result->data_size = 0;
	result->data_keys = NULL;
	result->data_values = NULL;
	
	return result;
}

char * activerecord_sql_quote_name( char * val )
{
	int val_len = strlen(val);

	if( val[0] != '`' || val[val_len-1] != '`' )
	{
		char *quoted = emalloc( val_len+2 );
		quoted[0] = '`';
		strncpy( quoted+1, val, val_len );
		quoted[val_len+1] = '`';
		return quoted;
	}
	return val;
}

char * activerecord_sql_build_update( activerecord_sql * sql )
{
	int i, param_len = 0;
	char *query, ** param;
	
	param = emalloc( sql->data_len * sizeof(char*) );
	
		// pre-prepare quoted parameters to avoid multiple emallocs
	for( i = 0; i < sql->data_len; i++ )
	{
		param[i] = activerecord_sql_quote_name( sql->data_keys[i] );
		param_len += strlen( param[i] );
	}
	
		// build query
	query = emalloc( strlen(sql->table) + param_len + sql->data_len * 4 + 11 );
	strcat( query, "UPDATE " );
	strcat( query, sql->table );
	strcat( query, " SET " );

	for( i = 0; i < sql->data_len; i++ )
	{
		strcat( query, param[i] );
		strcat( query, i == sql->data_len - 1 ? "=? " : "=?, " );
	}

	for( i = 0; i < sql->data_len; i++ )
		efree( param[i] );
	efree( param );

	return query;
}

char * activerecord_sql_build_delete( activerecord_sql * sql )
{
	char * query;
	int query_size, param_sizes[4], i;
	
	param_sizes[0] = 12 + strlen( sql->table );
	param_sizes[1] = sql->where != NULL ? 7 + strlen( sql->where ) : 0;
	param_sizes[2] = sql->order != NULL ? 10 + strlen( sql->where ) : 0;
	param_sizes[3] = sql->limit != NULL ? 7 + strlen( sql->where ) : 0;
	
	query_size = 0;
	for( i = 0; i < 4; i++ )
		query_size += param_sizes[i];

	query = emalloc( query_size );
	strcat( query, "DELETE FROM " );
	strcat( query, sql->table );
	i = param_sizes[0];
	
	if( sql->where )
	{
		strcat( query, "WHERE " );
		strcat( query, sql->where );
	}
	if( sql->order )
	{
		strcat( query, "ORDER BY " );
		strcat( query, sql->order );
	}
	if( sql->limit )
	{
		strcat( query, " LIMIT " );
		strcat( query, sql->limit );
	}

	return query;
}

char * activerecord_sql_build_select( activerecord_sql * sql )
{
	char * query;
	int query_size, param_sizes[6], i;
	
	if( sql->select == NULL )
	{
		query = "";
		return query;
	}

	param_sizes[0] = 13 + strlen( sql->table ) + strlen( sql->select );
	param_sizes[1] = sql->joins != NULL ? 1 + strlen( sql->joins ) : 0;
	param_sizes[2] = sql->where != NULL ? 7 + strlen( sql->where ) : 0;
	param_sizes[3] = sql->group != NULL ? 10 + strlen( sql->group ) : 0;
	param_sizes[4] = sql->having != NULL ? 8 + strlen( sql->having ) : 0;
	param_sizes[5] = sql->order != NULL ? 10 + strlen( sql->order ) : 0;
	if( sql->limit != NULL || sql->offset != NULL )
	{
		param_sizes[6] = 8 + strlen( sql->limit ) + strlen( sql->offset );
	}
	
	query_size = 0;
	for( i = 0; i < 7; i++ )
		query_size += param_sizes[i];

	query = emalloc( query_size );
	strcat( query, "SELECT " );
	strcat( query, sql->select );
	strcat( query, " FROM " );
	strcat( query, sql->table );

	if( param_sizes[1] > 0 )
	{
		strcat( query, " " );
		strcat( query, sql->joins );
	}
	if( param_sizes[2] > 0 )
	{
		strcat( query, " WHERE " );
		strcat( query, sql->where );
	}
	if( param_sizes[3] > 0 )
	{
		strcat( query, " GROUP BY " );
		strcat( query, sql->group );
	}
	if( param_sizes[4] > 0 )
	{
		strcat( query, " HAVING " );
		strcat( query, sql->having );
	}
	if( param_sizes[5] > 0 )
	{
		strcat( query, " ORDER BY " );
		strcat( query, sql->order );
	}
	if( param_sizes[6] > 0 )
	{
		strcat( query, " LIMIT " );
		strcat( query, sql->offset );
		strcat( query, "," );
		strcat( query, sql->limit );
	}
	
	return query;
}

char * activerecord_sql_build_insert( activerecord_sql * sql )
{
	int i, param_len = 0;
	char * query, ** param;
		
	param = emalloc( sql->data_len * sizeof(char*) );
	
		// pre-prepare quoted parameters to avoid multiple emallocs
	for( i = 0; i < sql->data_len; i++ )
	{
		param[i] = activerecord_sql_quote_name( sql->data_keys[i] );
		param_len += strlen( param[i] );
	}
	
		// build base query
	query = emalloc( strlen(sql->table) + param_len + sql->data_len * 3 + 22 );
	strcat( query, "INSERT INTO " );
	strcat( query, sql->table );
	strcat( query, "(" );

		// build (`a`, `b`, `c`...) keys section
	for( i = 0; i < sql->data_len; i++ )
	{
		strcat( query, param[i] );
		if( i != sql->data_len - 1 )
			strcat( query, "," );
	}

		// build VALUES(?,?,?...) section
	strcat( query, ") VALUES(" );
	for( i = 0; i < sql->data_len; i++ )
		strcat( query, i == sql->data_len - 1 ? "?" : "?," );
	
		// close query
	strcat( query, ")" );
	for( i = 0; i < sql->data_len; i++ )
		efree( param[i] );
	efree( param );
	return query;
}

void activerecord_sql_parameter( activerecord_sql * sql, char * k, char * v )
{
	sql->data_len++;
	if( sql->data_len > sql->data_size )
	{
		char **new_data_container;
		if( sql->data_size == 0 ); // TODO: Handle
		new_data_container = emalloc( sql->data_size*2*sizeof(char*) );
		memcpy( new_data_container, sql->data_keys, sql->data_size*sizeof(char*) );
		sql->data_keys = new_data_container;
		
		new_data_container = emalloc( sql->data_size*2*sizeof(char*) );
		memcpy( new_data_container, sql->data_values, sql->data_size*sizeof(char*) );
		sql->data_values = new_data_container;

		sql->data_size *= 2;
	}
	
	sql->data_keys[sql->data_len-1] = k;
	sql->data_values[sql->data_len-1] = v;
}

/* complete -
	should deal either with:
			hash keys = keys, hash values = values
			hash = array( keys, values )
			hash = array( value )
*/
void activerecord_sql_create_where_zval( activerecord_sql *sql, zval *hash )
{
	int len, i = 0;
	zval **value;
	char **keys, **values;
	HashPosition pos;

	if( Z_TYPE_P(hash) != IS_ARRAY )
		/* throw exception */;

	len = zend_hash_num_elements( Z_ARRVAL_P(hash) );
	keys = (char**)emalloc( sizeof(char*)*len );
	values = (char**)emalloc( sizeof(char*)*len );

	for( 
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(joins), &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(joins), (void **)&value, &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(joins), &pos)
	)
	{
		zval **key;
		int key_len;

		if( HASH_KEY_IS_STRING == zend_hash_get_current_key_ex(Z_ARRVAL_P(arr), &key, &key_len, &j, 0, &pos) )
		{
			keys[i] = key;
			values[i] = Z_STRVAL_PP(value);
		}
	}

	activerecord_sql_create_where( sql, keys, values, len );
}

void activerecord_sql_create_where( activerecord_sql * sql, char ** keys, char ** values, int len )
{
	// output: "`moshe`=? AND `haim` IN(?,?,?,?)"
}

zval * activerecord_create_conditions_from_underscored_string( char * str, int str_len, zval * values, zval * map )
{
	int i, valcount, strsize, curstrsize;
	char * conditions;
	zend_bool use_map, unfinished = 1;
	zval * retval, * condsval;

	if( !name_len || Z_TYPE_P(values) != IS_ARRAY )
		return NULL;

	MAKE_STD_ZVAL( retval );
	array_init( retval );
	add_index_string( retval, 0, "", 1);
	
	use_map = ( map != NULL && Z_TYPE_P(map) == IS_ARRAY );
	valcount = zend_hash_num_elements(Z_ARRVAL_P(values));
	conditions = (char*)emalloc( strsize = valcount*20 );

	for( i = 0; strstr( str, "_and_" ) || strstr( str, "_or_" ) || unfinished; i++ )
	{
		char *attribute, *tmp;
		zend_bool and_condition;
		zval **alias;

		if( tmp = strstr(str,"_and_") )
			and_condition = 1;
		else
		{
			tmp = strstr(str,"_or_");
			and_condition = 0;
		}
		if( tmp )
		{
			attribute = (char*)emalloc( tmp - str );
			strncpy( attribute, str, tmp - str );
			str = tmp + (and_condition? 5 : 4);
		}
		else
		{
			unfinished = 0;
			attribute = str;
		}
		if( i > 0 )
		{
			if( use_map && zend_hash_find( Z_ARRVAL_P(map), attribute, sizeof(attribute), (void**)&alias ) == SUCCESS )
				attribute = Z_STRVAL_PP(alias);

			if( curstrsize + 14 + strlen(attribute) > strsize )
			{
				tmp = (char*)emalloc( strsize*2 );
				strncpy( tmp, conditions, curstrsize );
				efree( conditions );
				conditions = tmp;
				strsize *= 2;
			}

			strsize += (and_condition? 5 : 4) + strlen(attribute);
			strcat( conditions, and_condition? " AND " : " OR " );
			strcat( conditions, activerecord_sql_quote_name(attribute) );

			if( i < valcount )
			{
				zval **value;

				zend_hash_index_find( Z_ARRVAL_P(values), i, (void**)&value );
				if( Z_TYPE_PP(value) != IS_NULL )
				{
					strcat( conditions, Z_TYPE_PP(value) == IS_ARRAY? " IN(?)" : "=?";
					curstrsize += Z_TYPE_PP(value) == IS_ARRAY? 6 : 2;
					add_next_index_zval( retval, *value );
				}
				else
				{
					strcat( conditions, " IS NULL " );
					curstrsize += 9;
				}
			}
			else
			{
				strcat( conditions, " IS NULL" );
				curstrsize += 9;
			}
		}
		efree( attribute );
	}

	MAKE_STD_ZVAL( condsval );
	ZVAL_STRING( condsval, conditions, 1 );
	zend_hash_index_update( Z_ARRVAL_P(retval), 0, &condsval, sizeof(zval *), NULL );
	efree( conditions );
	
	return retval;
}

zval * activerecord_create_hash_from_underscored_string( char * str, int str_len, zval * values, zval * map )
{
	zval * hash;
	int i;
	zend_bool use_map, unfinished = 1;

	MAKE_STD_ZVAL( hash );
	array_init( hash );
	use_map = ( map != NULL && Z_TYPE_P(map) == IS_ARRAY );

	for( i = 0; strstr( str, "_and_" ) || strstr( str, "_or_" ) || unfinished; i++ )
	{
		char **alias, *tmp, *attribute, **value;

		if( tmp = strstr(str,"_and_") )
			and_condition = 1;
		else
		{
			tmp = strstr(str,"_or_");
			and_condition = 0;
		}
		if( tmp )
		{
			attribute = (char*)emalloc( tmp - str );
			strncpy( attribute, str, tmp - str );
			str = tmp + (and_condition? 5 : 4);
		}
		else
		{
			unfinished = 0;
			attribute = str;
		}
			
		if( use_map && zend_hash_find( Z_ARRVAL_P(map), attribute, sizeof(attribute), (void**)&alias ) == SUCCESS )
			attribute = Z_STRVAL_PP(alias);
		zend_hash_index_find( Z_ARRVAL_P(values), i, (void**)&value );
		add_assoc_zval( hash, attribute, *value );
	}

	return hash;
}

zval * activerecord_sql_reverse_order( zval *order_str_zval )
{
	zend_bool empty = 1;
	int i;
	char *pch, *tmp, *reversed;
	zval *retval;

	for( pch = Z_STRVAL_P( order_str_zval ); pch != '\0'; pch++ )
	{
		if( pch != ' ' ) {
			empty = 0;
			break;
		}
	}
	if( empty )
		/* throw exception */;

	reversed = (char*)emalloc( strlen(empty)*2 );	// wasteful, quicker

		// swap ASC/DESC (or add DESC if implicit ASC)
	for( pch = strtok(Z_STRVAL_P( order_str_zval ),","); pch != NULL; pch = strtok(NULL,",") )
	{
		for( i = 0; pch[i]; pch++ )
			pch[i] = tolower(pch[i]);

		if( tmp = strstr(pch, " asc") )
		{
			strncat( reversed, pch, pch - tmp );
			strncat( reversed, " DESC", 5 );
		}
		else if( tmp = strstr(pch, " desc") )
		{
			strncat( reversed, pch, pch - tmp );
			strncat( reversed, " ASC", 4 );
		}
		else
		{
			strcat( reversed, pch );
			strncat( reversed, " DESC", 5 );
		}
		strncat( reversed, "," );
	}
	
		// make zval, free string
	MAKE_STD_ZVAL( retval );
	ZVAL_STRINGL( retval, reversed, strlen(reversed)-1, 1 );
	efree( reversed );
	
	return retval;
}

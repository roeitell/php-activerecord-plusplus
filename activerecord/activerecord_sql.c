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
	char * quoted;
	int val_len = strlen(val);

	if( val[0] != '`' || val[val_len-1] != '`' )
	{
		quoted = emalloc( val_len+2 );
		quoted[0] = '"';
		strcpy( quoted+1, val );
		quoted[val_len+1] = '`';
		return quoted;
	}
	return val;
}

char * activerecord_sql_build_update( activerecord_sql * sql )
{
	int i, param_len = 0;
	char* query, ** param;
	
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
	return query;
}

void activerecord_sql_parameter( activerecord_sql * sql, char * k, char * v )
{
	char ** new_data_container;
	sql->data_len++;
	if( sql->data_len > sql->data_size )
	{
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

void activerecord_sql_create_where( activerecord_sql * sql, char ** keys, char ** values, int len )
{
	// output: "`moshe`=? AND `haim` IN(?,?,?,?)"
}
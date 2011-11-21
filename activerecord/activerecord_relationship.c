#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_activerecord.h"

#define AR_REL_HASMANY 1
#define AR_REL_HASONE 2
#define AR_REL_BELONGSTO 3
#define AR_REL_HASANDBELONGSTOMANY 4

static const char **valid_association_options = { 
	"class_name", "class", "foreign_key", "conditions", "select", "readonly" 
};
static const char **has_many_valid_association_options = {
	"class_name", "class", "foreign_key", "conditions", "select", "readonly",
	"primary_key", "order", "group", "having", "limit", "offset", "through", "source" 
};
typedef struct {
	char * attribute_name;
	char * class_name;	// fci_info?
	int class_name_len;
	int type;
	zval * foreign_key;
	zval * primary_key;
	zval * options;
	zend_bool poly_relationship;
} activerecord_relationship;

zval * activerecord_merge_association_options( activerecord_relationship *rel, zval *options )
{
	zval *valid_options;
	char ** allowed_options = rel->poly_relationship? has_many_valid_association_options : valid_association_options;
	int i, allowed_options_len = sizeof(allowed_options) / sizeof(char*);

	MAKE_STD_ZVAL( valid_options );
	array_init( valid_options );
	for( i = 0; i < allowed_options_len; i++ )
	{
		zval **tmp;
		if( zend_hash_find(Z_ARRVAL_P(options), allowed_options[i], sizeof(allowed_options[i]), (void**)&tmp) == SUCCESS )
			zend_hash_add( valid_options, allowed_options[i], sizeof(allowed_options[i]), *tmp );
	}

	return valid_options;
}

activerecord_relationship * activerecord_relationship_new( zval *options, int type )
{
	activerecord_relationship *rel = emalloc( sizeof(activerecord_relationship) );
	zval **tmp;
	int i;

	zend_hash_index_find( Z_ARRVAL_P(options), 0, (void**)&tmp );
	rel->attribute_name = Z_STRVAL_PP(tmp);
	activerecord_variablize( rel->attribute_name );

	rel->poly_relationship = (type == AR_REL_HASMANY || type == AR_REL_HASANDBELONGSTOMANY? 1 : 0);
	rel->options = activerecord_merge_association_options( rel, options );

	if( zend_hash_find( Z_ARRVAL_P(options), "conditions", 10, (void**)&tmp ) == SUCCESS )
	{
		if( Z_TYPE_PP(tmp) != IS_ARRAY )
		{
			zval *arr;
			MAKE_STD_ZVAL( arr );
			array_init_size( arr, 1 );
			zend_hash_next_index_insert( Z_ARRVAL_P(arr), tmp, sizeof(zval*), NULL);
			zend_hash_add( Z_ARRVAL_P(options), "conditions", 10, arr, sizeof(zval*), NULL );
		}
	}
	
	if( 
		zend_hash_find( Z_ARRVAL_P(options), "class", 5, (void**)&tmp ) == SUCCESS ||
		zend_hash_find( Z_ARRVAL_P(options), "class_name", 10, (void**)&tmp ) == SUCCESS 
	)
	{
		rel->class_name = Z_STRVAL_PP( tmp );
		rel->class_name_len = Z_STRLEN_PP( tmp );
	}
	
	if( zend_hash_find( Z_ARRVAL_P(options), "foreign_key", 11, (void**)&tmp ) == SUCCESS )
	{
		if( Z_TYPE_PP(tmp) == IS_ARRAY )
		{
			rel->foreign_key = *tmp;
		}
		else
		{
			zval *arr;
			MAKE_STD_ZVAL( arr );
			array_init_size( arr, 1 );
			zend_hash_next_index_insert( Z_ARRVAL_P(arr), tmp, sizeof(zval*), NULL);
			rel->foreign_key = arr;
		}
	}
}

char * construct_inner_join_sql( activerecord_relationship *rel, zval *table, zend_bool through, char *alias )
{
	char *res, *join_table_name, *from_table_name, *foreign_key, *join_pk, *aliased_join_table_name;
	
	join_table_name = activerecord_table_fully_qualified_name(
		activerecord_table_load( rel->class_name, rel->class_name_len )
	);
	from_table_name = activerecord_table_fully_qualified_name( table );

	if( through )
	{
		char *tmp = join_table_name;
		join_table_name = from_table_name;
		from_table_name = tmp;
	}
	
	if( rel->type == AR_REL_HASMANY || rel->type == AR_REL_HASONE )
	{
		zval **tmp;
		/* $this->set_keys($from_table->class->getName()); */
		zend_hash_find( Z_ARRVAL_P(rel->foreign_key), 0, (void**)&tmp );
		join_pk = Z_STRVAL_PP(tmp);
		zend_hash_find( Z_ARRVAL_P(rel->primary_key), 0, (void**)&tmp );
		foreign_key = Z_STRVAL_PP(tmp);
		if( through )
		{
			char *tmp = join_pk;
			join_pk = foreign_key;
			foreign_key = tmp;
		}
	}
	else
	{
		zval **tmp;
		zend_hash_index_find( Z_ARRVAL_P(rel->foreign_key), 0, (void**)&tmp );
		foreign_key = Z_STRVAL_PP(tmp);
		zend_hash_index_find( Z_ARRVAL_P(rel->primary_key), 0, (void**)&tmp );
		join_pk = Z_STRVAL_PP(tmp);
	}
	
	if( alias != NULL )
	{
		aliased_join_table_name = activerecord_sql_quote_name( alias );
	}
	else
	{
		aliased_join_table_name = join_table_name;
	}
	
	res = (char*)emalloc( 
		22 + 
		strlen(join_table_name) + (strlen_aliased_join_table_name*(aliased_join_table_name != join_table_name? 2 : 1)) + strlen(from_table_name) 
		+ strlen(foreign_key) + strlen(join_pk) 
	);
	strcpy( res, "INNER JOIN " );
	strcat( res, join_table_name );
	strcat( res, " " );
	if( aliased_join_table_name != join_table_name )
	{
		strcat( res, aliased_join_table_name );
		strcat( res, " " );
	}
	strcat( res, "ON(" );
	strcat( res, from_table_name );
	strcat( res, "." );
	strcat( res, foreign_key );
	strcat( res, " = " );
	strcat( res, aliased_join_table_name );
	strcat( res, "." );
	strcat( res, join_pk );
	strcat( res, ")" );

	return res;
}
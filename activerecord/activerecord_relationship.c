#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_activerecord.h"

static const char **valid_association_options = { "class_name", "class", "foreign_key", "conditions", "select", "readonly" };
typedef struct {
	char * attribute_name;
	char * class_name;	// fci_info?
	zval * foreign_key;
	zval * options;
	zend_bool poly_relationship;
} activerecord_relationship;

activerecord_relationship * activerecord_relationship_new( zval *options )
{
	activerecord_relationship *rel = emalloc( sizeof(activerecord_relationship) );
	zval **tmp;
	char *rel_name;
	int i;

	zend_hash_index_find( Z_ARRVAL_P(options), 0, (void**)&tmp );
	rel->attribute_name = Z_STRVAL_PP(tmp);
	activerecord_variablize( rel->attribute_name );
	
	rel->options = options; /* $this->merge_association_options($options); */
	
	rel_name = emalloc( EG(called_scope)->name_length +1 );
	strcpy( rel_name, activerecord_denamespace( EG(called_scope)->name ) );
	for( i = 0; i < strlen(rel_name); i++ )
		rel_name[i] = tolower( rel_name[i] );

	rel->poly = (strcmp(rel_name, "hasmany") && strcmp(rel_name, "hasandbelongstomany") )? 0 : 1;

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
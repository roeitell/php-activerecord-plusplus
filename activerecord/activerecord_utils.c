#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_activerecord.h"
#include "activerecord_utils_grammar.c"

/*
 * Most of the ActiveRecord\Utils class has been left as-is. Many people use it for
 *	various tasks. (pluralizing, for example)
 *
 */
const zend_function_entry activerecord_utils_methods[] = {
	PHP_ME(ActiveRecordUtils, is_blank, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordUtils, extract_options, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordUtils, is_odd, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordUtils, is_a, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordUtils, pluralize, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordUtils, singularize, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordUtils, squeeze, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(ActiveRecordUtils, pluralize_if, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_FE_END
};

zend_bool activerecord_is_hash( zval * hash )
{
	char * key;
	int key_len, j;

	if( Z_TYPE_P(hash) != IS_ARRAY )
		return 0;

	zend_hash_internal_pointer_reset( hash );
	if( zend_hash_get_current_key_ex(Z_ARRVAL_P(arr), &key, &key_len, &j, 0, NULL) != HASH_KEY_IS_STRING )
		return 0;

	return 1;
}

PHP_METHOD(ActiveRecordUtils, is_blank)
{
	zval * pVal;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &pVal) == FAILURE )
		return;
	
	if( pVal->type != IS_STRING )
	{
		RETURN_BOOL( 0 );
	}
	else
	{
		RETURN_BOOL( pVal->value.str.len == 0 );
	}
}

PHP_METHOD(ActiveRecordUtils, extract_options)
{
	zval * pVal;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &pVal) == FAILURE )
		return;
	
	if( pVal->type == IS_ARRAY )
	{
		zval **retVal;
		HashTable *array = pVal->value.ht;
		zend_hash_internal_pointer_end(array);
		zend_hash_get_current_data( array, (void**)&retVal );
		RETURN_ZVAL( *retVal, 1, 0 );
	}
	else
	{
		array_init(return_value);
	}
}

PHP_METHOD(ActiveRecordUtils, is_odd)
{
	long num;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &num) == FAILURE )
		return;
	
	RETURN_BOOL( num % 2 != 0 );
}

PHP_METHOD(ActiveRecordUtils, singularize)
{
	char * str, * lower;
	int str_len, i;
	zval * singular_replacement;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE )
		return;

		// match against lowercase
	lower = ecalloc( strlen(str), sizeof(char) );
	for( i = 0; i < str_len; i++ )
		lower[i] = tolower(str[i]);

		// uncountable form
	for( i = 0; i < 9; i++ )
	{
		if( strcmp( lower, activerecord_uncountable[i] ) == 0 )
		{
			RETURN_STRING(str, 1);
			efree( lower );
			return;
		}
	}

		// irregular form?
	for( i = 0; i < 8; i++ )
	{
		char *index = strstr( lower, activerecord_irregular_plurals[i] );
		if( index != NULL )
		{
			char *res;
			int dist = index - lower;
			res = (char*) ecalloc( strlen(str) + strlen(activerecord_irregulars[i]) - strlen(activerecord_irregular_plurals[i]) + 1, sizeof(char) );
			strncat( res, str, dist );
			strcat( res, activerecord_irregulars[i] );
			strcat( res, str + dist + strlen(activerecord_irregular_plurals[i]) );
			RETURN_STRING( res, 1 );	/* RETVAL_STRING(...) */
			efree( lower );
			efree( res );
			return;
		}
	}
	efree( lower );

		// regular forms - regexps
	ALLOC_INIT_ZVAL(singular_replacement);
    Z_TYPE_P(singular_replacement) = IS_STRING;
	for( i = 0; i < 28; i++ )
	{
		int rep_count, rep_len;
		Z_STRVAL_P(singular_replacement) = ecalloc( strlen(activerecord_singular_replacements[i]), sizeof(char) );
		strcpy( Z_STRVAL_P(singular_replacement), activerecord_singular_replacements[i] );
		Z_STRLEN_P(singular_replacement) = strlen(activerecord_singular_replacements[i]);
		res = php_pcre_replace( 
			activerecord_singular_matches[i], strlen(activerecord_singular_matches[i]), str, strlen(str), 
			singular_replacement, 0, &rep_len, -1, &rep_count TSRMLS_CC
		);
		efree( Z_STRVAL_P(singular_replacement) );
		if( strlen(res) != strlen(str) || strncmp( res, str, strlen(res) ) != 0 )
		{
			RETURN_STRING( res, 1 );
			return;
		}
	}
	
	RETURN_STRING( str, 1 );
}

char * activerecord_pluralize( char * str, int str_len TSRMLS_DC )
{
	char * index, * res, * lower;
	int i, rep_len, rep_count, dist;
	zval * plural_replacement;
	
		// prepare lowercase form
	lower = ecalloc( strlen(str), sizeof(char) );
	for( i = 0; i < str_len; i++ )
		lower[i] = tolower(str[i]);

		// uncountable form 
	for( i = 0; i < 9; i++ )
	{
		if( strcmp( lower, activerecord_uncountable[i] ) == 0 )
		{
			efree( lower );
			return str;
		}
	}

		// irregular form?
	for( i = 0; i < 8; i++ )
	{
		index = strstr( lower, activerecord_irregulars[i] );
		if( index != NULL )
		{
			dist = index - lower;
			res = (char*) ecalloc( strlen(str) + strlen(activerecord_irregular_plurals[i]) - strlen(activerecord_irregulars[i]) + 1, sizeof(char) );
			strncat( res, str, dist );
			strcat( res, activerecord_irregular_plurals[i] );
			strcat( res, str + dist + strlen(activerecord_irregulars[i]) );
			efree( lower );
			//efree( res );
			return res;
		}
	}
	efree( lower );

		// regular forms - regexps
	ALLOC_INIT_ZVAL(plural_replacement);
    Z_TYPE_P(plural_replacement) = IS_STRING;
	for( i = 0; i < 19; i++ )
	{
		Z_STRVAL_P(plural_replacement) = ecalloc( strlen(activerecord_plural_replacements[i]), sizeof(char) );
		strcpy( Z_STRVAL_P(plural_replacement), activerecord_plural_replacements[i] );
		Z_STRLEN_P(plural_replacement) = strlen(activerecord_plural_replacements[i]);
		res = php_pcre_replace( 
			activerecord_plural_matches[i], strlen(activerecord_plural_matches[i]), str, strlen(str), 
			plural_replacement, 0, &rep_len, -1, &rep_count TSRMLS_CC
		);
		efree( Z_STRVAL_P(plural_replacement) );
		if( strlen(res) != strlen(str) || strncmp( res, str, strlen(res) ) != 0 )
			return res;
	}
	
	return str;
}

PHP_METHOD(ActiveRecordUtils, pluralize)
{
	char * str;
	int str_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE )
		return;

	RETURN_STRING( activerecord_pluralize(str, str_len TSRMLS_CC), 1 );
}

PHP_METHOD(ActiveRecordUtils, pluralize_if)
{
	int count, str_len;
	char * str;
	
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &count, &str, &str_len) == FAILURE )
		return;

	if( count == 1 )
	{
		RETURN_STRING( str, 1 );
	}
	else
	{
		RETURN_STRING( activerecord_pluralize( str, str_len TSRMLS_CC ), 1 );
	}
}

PHP_METHOD(ActiveRecordUtils, squeeze)
{
	char * ch, * str, * res, * match;
	int str_len, ch_len, rep_len, rep_count;
	zval * replacement;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &ch, &ch_len, &str, &str_len) == FAILURE )
		return;

	match = ecalloc( 3 + strlen(ch), sizeof(char) );
	strcat( match, "/" );
	strcat( match, ch );
	strcat( match, "+/" );
	
	ALLOC_INIT_ZVAL(replacement);
    Z_TYPE_P(replacement) = IS_STRING;
	Z_STRVAL_P(replacement) = ch;
	Z_STRLEN_P(replacement) = strlen(ch);
	res = php_pcre_replace( 
		match, strlen(ch)+3, str, strlen(str), 
		replacement, 0, &rep_len, -1, &rep_count TSRMLS_CC
	);
	
	efree( match );

	RETURN_STRING( res, 1 );
}

PHP_METHOD(ActiveRecordUtils, is_a)
{
	char * type;
	int type_len;
	zval * pVal;
	zval ** first, ** second;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &type, &type_len, &pVal) == FAILURE )
		return;

	if( strncmp(type, "range", type_len) == 0 )
	{
		if( pVal->type == IS_ARRAY && zend_hash_index_exists(pVal->value.ht, 0) && zend_hash_index_exists(pVal->value.ht, 1) )
		{
			zend_hash_index_find( pVal->value.ht, 0, (void**)&first );
			zend_hash_index_find( pVal->value.ht, 1, (void**)&second );
			convert_to_double_ex( first );
			convert_to_double_ex( second );
			if( (*first)->value.dval < (*second)->value.dval )
				RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
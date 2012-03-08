#include "symtab.h"
#include <stdio.h>

static hash_t **scopes;
static symbol_t **values;
static char **strings;
static int32_t scopes_size = 16, scopes_index = -1;
static int32_t values_size = 16, values_index = -1;
static int32_t strings_size = 16, strings_index = -1;


void
symtab_init ( void )
{
	/* initialize scopes */
	scopes = malloc(sizeof(hash_t*)*scopes_size);
//	for(int i = 0; i < scopes_size; i++) {
//		scopes[i] = ght_create(10); /* (N is # of hash buckets), 
//									 * don't know what this is supposed to be */
//	}

	/* init values */
	values = malloc(sizeof(symbol_t*)*values_size);
//	for(int i = 0; i < values_size; i++) {
//		values[i] = malloc(sizeof(symbol_t));
//	}

	/* init strings */
	strings = malloc(sizeof(char*)*strings_size);
}


void
symtab_finalize ( void )
{
    for(int i = 0; i < scopes_size; i++) {
        free( scopes[i] );
    }
    free(scopes);

    for(int i = 0; i < values_size; i++) {
        free( values[i] );
    }
    free(values);

    for(int i = 0; i < strings_size; i++) {
        free( strings[i] );
    }
    free(strings);
}


int32_t
strings_add ( char *str )
{
	strings_index++;
	if(strings_index == strings_size) {
        strings = realloc(strings, sizeof(char*)*strings_size*2);
		strings_size = strings_size*2;
	}
	strings[strings_index] = str;
	
    return strings_index;
}


void
strings_output ( FILE *stream )
{
	fprintf( stream, "\n.data\n.INTEGER:\t.string \"\%d \"\n" );
	for(int i = 0; i < strings_size; i++) {
		fprintf( stream, ".STRING%d:\t.string \"%s\"\n", i, strings[i] );
	}
	fprintf( stream, "\n.globl main\n" );
}


void
scope_add ( void )
{
	scopes_index++;
	if(scopes_index == scopes_size) {
		scopes = realloc(scopes, sizeof(hash_t*)*2*scopes_size);
		scopes_size = scopes_size*2;
	}
    if(scopes != NULL)
        scopes[scopes_index] = ght_create(HASH_BUCKETS);
}


void
scope_remove ( void )
{
	if(scopes_index < 0)
		return;
	// remove uppermost scope
    hash_t* p_ht = scopes[scopes_index];
/*
	ght_iterator_t iterator;
	void *p_key;   
	void *p_e;

	for(p = ght_first(p_ht, &iterator); p; p = ght_next(p, &iterator)) {      
		free(p);
	}
*/
	ght_finalize(p_ht);
	
	free( scopes[scopes_index] );
	scopes_index--;
}


void
symbol_insert ( char *key, symbol_t *value )
{
    hash_t* tab = scopes[scopes_index];
    unsigned int ret = ght_insert(tab, value, sizeof(char)*strlen(key)+1, key);

    values_index++;
    if(values_index == values_size) {
        values = realloc(values, sizeof(symbol_t*)*2*values_size);
        values_size = values_size*2;
    }
    values[values_index] = value;

#ifdef DUMP_SYMTAB
fprintf ( stderr, "Inserting (%s,%d)\n", key, value->stack_offset );
#endif
}


void
symbol_get ( symbol_t **value, char *key )
{
    symbol_t* result = NULL;
	int keysz = strlen(key)+1;
	for( int i = scopes_size; i >= 0 && result == NULL; i-- ) {
    	result = ght_get(scopes[i], keysz, key);
    	*value = result;
	}

#ifdef DUMP_SYMTAB
    if ( result != NULL )
        fprintf ( stderr, "Retrieving (%s,%d)\n", key, result->stack_offset );
    else
        fprintf ( stderr, "Retrieving (%s) was NULL\n", key);
#endif
}

/* vim: set ts=4 sw=4: */


#include "tree.h"


#ifdef DUMP_TREES
void node_print ( FILE *output, node_t *root, uint32_t nesting )
{
    if ( root != NULL )
    {
        fprintf ( output, "%*c%s", nesting, ' ', root->type.text );
        if ( root->type.index == INTEGER )
            fprintf ( output, "(%d)", *((int32_t *)root->data) );
        if ( root->type.index == VARIABLE || root->type.index == EXPRESSION )
        {
            if ( root->data != NULL )
                fprintf ( output, "(\"%s\")", (char *)root->data );
            else
                fprintf ( output, "%p", root->data );
        }
        fputc ( '\n', output );
        for ( int32_t i=0; i<root->n_children; i++ )
            node_print ( output, root->children[i], nesting+1 );
    }
    else {
	//fprintf ( stderr, "root was null");
        fprintf ( output, "%*c%p\n", nesting, ' ', root );
    }
}
#endif


void node_init ( node_t *nd, nodetype_t type, void *data, uint32_t n_children, ... )
{

	//nd = malloc( sizeof(node_t) );
	nd->type = type;
	if(type.index == INTEGER) {
		uint32_t* temp = malloc(sizeof(uint32_t));
		*temp = atoi((char*)data);
		nd->data = temp;
	} else {
		nd->data = data;
	}
	nd->n_children = n_children;
	nd->children = (node_t **) malloc( sizeof(node_t*) * n_children );
	// this is dog...
	va_list args;
	// assign all the children from an array...
	va_start(args, n_children); // initialize args to store all argument after the argument nargs
	for (int i = 0; i < n_children; ++i)
		nd->children[i] = va_arg(args, node_t*);
	va_end(args); //clean up

}


void node_finalize ( node_t *discard )
{
	//destroy_subtree( discard );
	free( discard->data );
	free( discard->children );
	//free( discard->entry );
	//free( discard->n_children ); // not sure if... is not a pointer...
	free( discard ); // maybe not do this here, but let caller do it?
}


void destroy_subtree ( node_t *discard )
{
	// this is dog...
	if(discard->n_children > 0) {
		for(int i = 0; i < discard->n_children; ++i) {
			if(discard->children[i] != NULL)
				destroy_subtree( discard->children[i] );
		}
	}
	node_finalize(discard);
}



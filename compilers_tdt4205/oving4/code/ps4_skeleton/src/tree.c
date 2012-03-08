#include "tree.h"
#include "symtab.h"

static int dybde = 0;

#ifdef DUMP_TREES
void
node_print ( FILE *output, node_t *root, uint32_t nesting )
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
    else
        fprintf ( output, "%*c%p\n", nesting, ' ', root );
}
#endif


void
node_init ( node_t *nd, nodetype_t type, void *data, uint32_t n_children, ... )
{
    va_list child_list;
    *nd = (node_t) { type, data, NULL, n_children,
        (node_t **) malloc ( n_children * sizeof(node_t *) )
    };
    va_start ( child_list, n_children );
    for ( uint32_t i=0; i<n_children; i++ )
        nd->children[i] = va_arg ( child_list, node_t * );
    va_end ( child_list );
}


void
node_finalize ( node_t *discard )
{
    if ( discard != NULL )
    {
        free ( discard->data ), free ( discard->children );
        free ( discard );
    }
}


void
destroy_subtree ( node_t *discard )
{
    if ( discard != NULL )
    {
        for ( uint32_t i=0; i<discard->n_children; i++ )
            destroy_subtree ( discard->children[i] );
        node_finalize ( discard );
    }
}


void
simplify_tree ( node_t **simplified, node_t *root )
{
    node_t *result = root;

    /*
     * First, we ensure that we have a node to look at. This has the
     * convenient side-effect that optional elements in the syntax
     * remain marked by a NULL placeholder in the tree, keeping it simple
     * to recognize the structures imposed by the grammar.
     */
    if ( root != NULL )
    {
        /* Recur before treating any single node: depth-first traversal */
        for ( uint32_t i=0; i<root->n_children; i++ )
            simplify_tree ( &root->children[i], root->children[i] );

        /* Here is where we do something to the lowest nodes in the tree */
        switch ( root->type.index )
        {
            /*
             * These types have only syntactic value, so we can throw
             * them out now:
             * STATEMENT always has one child, which can identify itself
             * PARAMETER_LIST only serves to make variable lists optional
             *                in function declarations
             * ARGUMENT_LIST does the same thing for function calls
             */
            case STATEMENT: case PRINT_ITEM:
            case PARAMETER_LIST: case ARGUMENT_LIST: 
                result = root->children[0];
                node_finalize ( root );
                break;

            /*
             * Print statements always have exactly one PRINT_LIST child.
             * Since we are done with the recursive list definition,
             * its descendants may instead be children of the print statement
             * itself now. (Quick hack - it's easier to rename the list node
             * and eliminate the old statement than to copy/move all children.)
             */
            case PRINT_STATEMENT:
                result = root->children[0];
                result->type = root->type;
                node_finalize ( root );
                break;

            /*
             * DECLARATION_LIST can be NULL altogether, but we preserved
             * that at the beginning of the function. This has a somewhat
             * nasty side-effect in the grammar, however, as it gets a
             * different structure from the other lists when it DOES exist
             * (such as here). Other lists have a single-element list with
             * the last list item at the bottom of the tree, whereas
             * declaration lists have a 2-element list with NULL and the
             * last item. The code that follows molds the bottom of a
             * declaration list into the same form as the other lists, so
             * the rest can be handled by the otherwise standard
             * list-flattening code.
             */
            case DECLARATION_LIST:
                if ( root->children[0] == NULL )
                {
                    root->children[0] = root->children[1];
                    root->n_children--;
                    root->children = realloc (
                        root->children, root->n_children * sizeof(node_t *)
                    );
                }
                /* NB! There is no 'break' here on purpose - since the
                 * declaration list is now in the standard form, we WANT
                 * control to fall through to the standard list-handling
                 * code below.
                 */

            /*
             * All these lists have the same structure, so general flattening
             * is quite simple: extend the left child's list with the right
             * child, and substitute the parent.
             */
            case FUNCTION_LIST: case STATEMENT_LIST: case PRINT_LIST:
            case EXPRESSION_LIST: case VARIABLE_LIST:
                if ( root->n_children >= 2 )
                {
                    result = root->children[0];
                    uint32_t n = (result->n_children += 1);
                    result->children =
                        realloc ( result->children, n * sizeof(node_t *) );
                    result->children[n-1] = root->children[1];
                    node_finalize ( root );
                }
                break;

            case EXPRESSION:
                switch ( root->n_children )
                {
                    case 1:
                        if ( root->children[0]->type.index == INTEGER )
                        {   /* Single integers */
                            result = root->children[0];
                            if ( root->data != NULL )   /* Negative constants */
                                *((int32_t *)result->data) *= -1;
                            node_finalize ( root );
                        }
                        else if ( root->data == NULL )
                        {   /* Single variables, parentheses, etc. */
                            result = root->children[0];
                            node_finalize ( root );
                        }
                        break;
                    case 2:     /* Constant binary expressions */
                        if ( root->children[0]->type.index == INTEGER &&
                             root->children[1]->type.index == INTEGER &&
                             root->data != NULL )
                        {
                            result = root->children[0];
                            int32_t
                                *a = result->data,
                                *b = root->children[1]->data;
                            switch ( *((char *)root->data) )
                            {
                                case '+': *a += *b; break;
                                case '-': *a -= *b; break;
                                case '*': *a *= *b; break;
                                case '/': *a /= *b; break;
                                case '^': 
                                          if (*b < 0 && *a != 1)
                                              *a = 0;
                                          else {
                                              *a = 1;
                                              for (int32_t x=*a, c=*b; c > 0; c--) 
                                                  *a *= x; 
                                          }
                                          break;
                            }
                            node_finalize ( root->children[1] );
                            node_finalize ( root );
                        }
                        break;
                }
                break;
        }
    }
    *simplified = result;
}


void bind_names ( node_t* root )
{
    /* TODO: bind tree nodes to symtab entries */
	symbol_t** temp = NULL;
	node_t* child;
	if(root != NULL) {
		for(int i = 0; i < root->n_children; i++) {
			child = root->children[i];
			switch(child->type.index) {
				case FUNCTION:
					/* add scope, reset depth, add arguments */
					scope_add();
					node_t* varlist = child->children[1];
					// add function symbol
					node_t* var = child->children[0];
					symbol_t* func = malloc(sizeof(symbol_t));
					func->stack_offset = 0;
					func->depth = 0;
					func->n_args = varlist->n_children;
					func->label = var->data;


					// add vars in the var list under (arguments)
					int counter = 8;
					for( int = 0; i < varlist->n_children; i++ ) {
						char* key = varlist->children[i]->data;

						symbol_t* s = malloc(sizeof(symbol_t));
						s->stack_offset = counter;
						counter += 4;
						s->n_args = 0;
						s->depth = dybde;
						s->label = key;

						symbol_insert( key, s );
					}

					dybde = 0;
					bind_names(child);
					scope_remove();
				/* push scope */
				case BLOCK:
					scope_add();
					dybde++;	
					bind_names( child );
					dybde--;
					scope_remove();
					break;
				case DECLARATION_LIST:
					int counter = 0; // counter for calculating offset
					for( int dec = 0; dec < child->n_children; dec++) {
						node_t* declaration = child->children[dec];
						// add vars in the var list under
						node_t* varlist = declaration->children[0];
						for( int = 0; i < varlist->n_children; i++ ) {
							char* key = varlist->children[i]->data;

							symbol_t* s = malloc(sizeof(symbol_t));
							s->stack_offset = 4*--counter;
							s->n_args = 0;
							s->depth = dybde;
							s->label = key;

							symbol_insert( key, s );
						}
					}
					return; // safe to return now ?
					break;
				case VARIABLE:
					char* key = child->data;
					*temp = NULL;
					symbol_get( temp, key );
					if( *temp == NULL ) { // symbol not yet declared, error ?
					} else
						child->entry = *value;
					bind_names( child );
					break;
				case PARAMETER:
				case TEXT:
					int index = strings_add(child->data);
				default: 
					bind_names( child );


			}
		}
	}	
}

/* vim: set ts=4 sw=4: */

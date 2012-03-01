#include "tree.h"


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

void prune_print_list(node_t* node, node_t* parent) {
    if(node->type.index == print_list_n.index) {
        parent->n_children = node->n_children;
        parent->children = node->children;
        //node_finalize(node);
    }
}

void dfs( node_t* current, node_t* parent )
{
    if(current == NULL)
        return;

    flatten_list(current);
    prune_print_list(current, parent);
    for(int i = 0; i < current->n_children; i++) {
        dfs(current->children[i], current);
    }
    if(current->type.index == expression_n.index)
        calculate_expr(current, parent);

}

void
simplify_tree ( node_t **simplified, node_t *root )
{
    /* TODO: implement the simplifications of the tree here */
    *simplified = root;
    // prune all redundant nodes first
    prune_node(root, simplified);
    *simplified = root;
    // go through the whole tree, doing our work on each node
    dfs(root, root);
}

// node is prunable if has 1 child and contains no data
// and is of the mentioned types
bool is_prunable( node_t* node ) {
    if( node->n_children == 1 && node->data == NULL ) {
        switch( node->type.index ) {
            case STATEMENT: return true; break;
            case PARAMETER_LIST: return true; break;
            case PRINT_ITEM: return true; break;
            case ARGUMENT_LIST: return true; break;
            case EXPRESSION: return true; break;
        }
    }
    return false;
}

void prune_node(node_t* node, node_t** simpler) {
    if(node == NULL) {
        // update simpler so that if we are pruning,
        // the pointer pointing to this node, will point to this node instead
        *simpler = node;
        return;
    }

    if(is_prunable(node)) {
        // update the pointer pointing to the node we want to point to instead
        prune_node(node->children[0], simpler);
        // clean up node we are removing
        node_finalize(node);
    } else {
        // recurse down the children and update pointers
        for (int i = 0; i < node->n_children; ++i) {
            node_t* child = node->children[i];
            prune_node(child, simpler);
            // update the child pointer we are exploring, in case it was prunable
            node->children[i] = *simpler;
        }
        *simpler = node;
    }

}

void doOp( node_t* node, node_t* parent) {
    // DEBUG
    fprintf(stdout, "doOp(%s)", node->data);

    uint32_t* result = malloc(sizeof(uint32_t));
    uint32_t* tall1 = (uint32_t*)node->children[0]->data;
    uint32_t* tall2 = (uint32_t*)node->children[1]->data;
    char* c = (char*)node->data;
    if(*c == '+') {
        *result = *tall1 + *tall2;
    }
    if(*c == '-') {
        *result = *tall1 - *tall2;
    }
    if(*c == '*') {
        *result = *tall1 * *tall2;
    }
    if(*c == '/') {
        *result = *tall1 / *tall2;
    }
    if(*c == '^') {
        //*result = *tall1 ** *tall2;
    }
    // DEBUG
    fprintf(stdout, "%d %c %d = %d\n", *tall1, *c, *tall2, *result);

    free(node->data);
    free(node->children);
    free(tall1);
    free(tall2);
    node->n_children = 0;
    node->data = result;
    node->type = integer_n;
    // finalize the children?

}
void unary_minus(node_t* node) {
    uint32_t* tall = malloc(sizeof(uint32_t));
    uint32_t* tall2 = node->children[0]->data;
    *tall = -1* *tall2;

    free(node->data);
    free(node->children);
    node->n_children = 0;
    node->data = tall;
    node->type = integer_n;
}

void calculate_expr( node_t* node, node_t* parent) {
    // recurse to bottom, and evaluate bottom-up

    if( node == NULL )
        return;
    if( node->type.index != expression_n.index )
        return;

    if(node->n_children > 0)
        for(int i = 0; i < node->n_children; i++)
            calculate_expr(node->children[i], node);

    if(node->n_children == 2 &&
       node->children[0]->type.index == INTEGER &&
       node->children[1]->type.index == INTEGER) {
            doOp(node, parent);
    }

    char* c;
    if(node->data != NULL)
        c = (char*)node->data;
    if(node->n_children==1 && *c == '-' &&
            node->children[0]->type.index == INTEGER) {
        unary_minus(node);
    }
}


void recurse_flatten( node_t* node, int* count, int depth, node_t* parent, nt_number search) {
    if(node == NULL)
        return;

    // only recurse to first expression_n, if those are what we are looking for
    if(node->type.index == expression_n.index && expression_n.index == search) {
        fprintf(stdout, "Found what we are looking for.. returning...\n");
        return;
    }

    if(node->type.index == search) {
        // add the nodes that are not the list node we are trying to remove:
        for(int i = 0; i < node->n_children; i++){
            node_t* temp = node->children[i];

            if(temp == NULL)
                fprintf( stdout, "\tA NULL CHILD!!!!\n");
            else{
                if(temp->type.index != search) {
                    fprintf( stdout, "FOUND A CHILD NUMBER: %d of type: %s\n", *count, temp->type.text);
                    if(temp == NULL)
                        fprintf( stdout, "\tA NULL CHILD!!!!\n");
                    if( *count >= 0 ) {
                        parent->children[*count] = temp;
                        *count = *count-1;
                    }
                }
            }
        }
    }
    if(node->n_children > 0) {
        for(int i = 0; i < node->n_children; i++) {
            recurse_flatten( node->children[i], count, depth, parent, search );
        }
    }



}

node_t* construct_flatten_list( node_t* node, node_t* parent, nt_number search ) {
    if(node == NULL)
        fprintf( stdout, "NULLNULLNULLNULLL\n");

    // what are we flattening? search
    //fprintf( stdout, "FOUND %s IN FLATTEN_LIST\n", node->type.text);

    // how many nodes in the subtree we are flattening (children of parent node)
    int depth = subtree_depth(node, search);

    // debug
    //fprintf( stdout, "Count: %d of list type: %s\n", depth, node->type.text);

    int* count;

    // if this is first run, make a parent node to store new children in
    if(parent == NULL) {
        parent = malloc(sizeof(node_t));
        parent->n_children = depth;
        parent->children = malloc(sizeof(node_t*)*depth);
        count = malloc(sizeof(int));
        *count = depth-1;
    }

    if(node != NULL && node->n_children != 0) {
        recurse_flatten( node, count, depth, parent, search );
    }
    return parent;
}

/* find depth of subtree (of a list) */
int subtree_depth( node_t* node, nt_number search ) {
    if(node == NULL)
        return 0;

    int c = 0;
    // only recurse to first statement node after the list
    // if not, we will add all statements under a statementlist to the upper statement_list node,
    // all the way from the bottom
    if(search == STATEMENT_LIST && node->type.index != search)
        return 0;

    if(node->type.index == search) {
        c=1;//node->n_children;
    }

    // really needed?
    // only recurse to first expression_n, if those are what we are looking for
    if(node->type.index == expression_n.index && expression_n.index == search) {
        //fprintf(stdout, "Found what we are looking for.. returning...\n");
        return c;
    }

    if(node->n_children > 0) {
        for(int i = 0; i < node->n_children; i++)
            c += subtree_depth(node->children[i], search);
    }

    return c;
}

// this should be done first time we encounter a list type
void flatten_list( node_t* node )
{
    // DEBUG
    //fprintf( stdout, "FLATTEN A LIST OF TYPE %s\n", node->type.text);
    node_t* new_node = NULL;

    if(node->type.index == statement_list_n.index) {
        new_node = construct_flatten_list(node, NULL, STATEMENT_LIST);
        new_node->type = statement_list_n;
    }
    else if(node->type.index == print_list_n.index) {
        new_node = construct_flatten_list(node, NULL, PRINT_LIST);
        new_node->type = print_list_n;
    }
    else if(node->type.index == function_list_n.index) {
        new_node = construct_flatten_list(node, NULL, FUNCTION_LIST);
        new_node->type = function_list_n;
    }
    else if(node->type.index == declaration_list_n.index) {
        new_node = construct_flatten_list(node, NULL, DECLARATION_LIST);
        new_node->type = declaration_list_n;
    }
    else if(node->type.index == expression_list_n.index) {
        new_node = construct_flatten_list(node, NULL, EXPRESSION_LIST);
        new_node->type = expression_list_n;
    }
    else if(node->type.index == variable_list_n.index) {
        new_node = construct_flatten_list(node, NULL, VARIABLE_LIST);
        new_node->type = variable_list_n;
    }

    if(new_node != NULL) {
        node->n_children = new_node->n_children;
        node->children = new_node->children;
        node->type = new_node->type;
    }

    return;

}



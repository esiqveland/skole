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


void recurse_flatten( node_t* node, int* count, int depth, node_t* parent, nodetype_t search) {
    if(node == NULL)
        return;
    if(node->type.index == search.index) {
        if( *count < depth ) {
            parent->children[*count] = node;
            *count = *count+1;
        }
    }
    // only recurse to first expression_n, if those are what we are looking for
    if(node->type.index == expression_n.index && expression_n.index == search.index) {
        //fprintf(stdout, "Found what we are looking for.. returning...\n");
        return;
    }

    for(int i = 0; i < node->n_children; i++) {
        recurse_flatten( node->children[i], count, depth, parent, search );
    }
}

node_t* construct_flatten_list( node_t* node, node_t* parent, nodetype_t search ) {
    // what are we flattening? search
    // how many nodes in the subtree we are flattening (children of parent node)
    int depth = subtree_depth(node, search);
    int* count;

    // if this is first run, make a parent node
    if(parent == NULL) {
        parent = malloc(sizeof(node_t));
        parent->n_children = depth;
        parent->children = malloc(sizeof(node_t*)*depth);
        count = malloc(sizeof(int));
        *count = 0;
    }
    
    if(node != NULL && node->n_children != 0) {
        recurse_flatten( node, count, depth, parent, search );
        //for(int i = 0; i < node->n_children; i++) {
        //recurse_flatten( node->children[i], count, depth, parent, search);
        //}
    }
    return parent;
}

/* find depth of subtree (of a list) */
int subtree_depth( node_t* node, nodetype_t search ) {
    if(node == NULL)
        return 0;

    int c = 0;
    if(node->type.index == search.index) {
        c=1;
    }
    // only recurse to first expression_n, if those are what we are looking for
    if(node->type.index == expression_n.index && expression_n.index == search.index) {
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
    node_t* new_node = NULL;

    if(node->type.index == statement_list_n.index) {
        //fprintf( stdout, "Count: %d of list type: %s\n", depth, statement_n.text);
        new_node = construct_flatten_list(node, NULL, statement_n);
        new_node->type = statement_list_n;
    }
    else if(node->type.index == print_list_n.index) {
        new_node = construct_flatten_list(node, NULL, print_item_n);
        new_node->type = print_list_n;
    }
    else if(node->type.index == function_list_n.index) {
        /* no need? */
        int depth = subtree_depth(node, function_n);
        new_node = construct_flatten_list(node, NULL, function_n);
        new_node->type = function_list_n;
    }
    else if(node->type.index == declaration_list_n.index) {
        new_node = construct_flatten_list(node, NULL, declaration_n);
        new_node->type = declaration_list_n;
    }
    else if(node->type.index == argument_list_n.index) {
        /* not needed? */
        //new_node = construct_flatten_list(node, NULL, expression_list_n);
        //new_node->type = argument_list_n;
    }
    else if(node->type.index == expression_list_n.index) {
        new_node = construct_flatten_list(node, NULL, expression_n);
        new_node->type = expression_list_n;
        //    	fprintf( stdout, "How result looks (depth: %d): \n\n", depth);
        //    	node_print( stdout, new_node, 0 );
        //    	fprintf( stdout, "How original looks (depth: %d): \n\n", depth);
        //    	node_print( stdout, node, 0 );
    }
    else if(node->type.index == variable_list_n.index) {
        //fprintf( stdout, "Count: %d of list type: %s\n", depth, variable_n.text);
        new_node = construct_flatten_list(node, NULL, variable_n);
        new_node->type = variable_list_n;

    }

    if(new_node != NULL) {
        //fprintf( stdout, "Simplified a %s node ASDF\n", node->type.text);
        // exchange node for new node...
        //node = new_node;

#ifdef DUMP_TREES
        //fprintf( stdout, "MADE NEW NODE: %s\n", new_node->type.text);
        //if ( (DUMP_TREES & 2) != 0 )
        //node_print ( stdout, new_node, 0 );
#endif
        node->n_children = new_node->n_children;
        node->children = new_node->children;

        //return new_node;
    }

    //    return node;
    return;
    
}

/*
node_t* redundancy( node_t* parent, node_t* node )
{
    if(node->type == statement_n || node->type == print_item_n
        || node->type == parameter_list_n
        || node->type == argument_list_n
        || node->type == expression_n) {
    // prune if node is redundant,
    // i.e. no data and only one child
    if(node->data == NULL && node->n_children == 1) {
        // find node in parent child list and set this pointer to this
        // node's child
        node_t* temp = node;
        node = node->children[0];

        // clean up this node
        node_finalize( temp );

        //node_t* child = NULL;
        //for(int i = 0; i < parent->n_children; i++) {

    }
    }
    return node;
}
*/
bool is_prunable( node_t* node ) {
    if( node->n_children == 1 ) {
        if( node->type.index == statement_n.index
                || node->type.index == print_item_n.index
                || node->type.index == parameter_list_n.index
                || node->type.index == argument_list_n.index
                || node->type.index == expression_n.index)
            if( node->data == NULL )
                return true;
    }
    return false;
}

void prune_node(node_t* node) {
    if(is_prunable(node)) {
        node_t* temp = node->children[0];
        node->type = temp->type;
        node->data = temp->data;
        node->n_children = temp->n_children;
        node->children = temp->children;
        // free( node );
    }
}

void prune_print_list(node_t* node, node_t* parent) {
    if(node->type.index == print_list_n.index) {

        //node->data = temp->data;
        //node->n_children = temp->n_children;
        parent->n_children = node->n_children;
        parent->children = node->children;
        // free( node );
    }
}

void dfs( node_t* current, node_t* parent ) 
{ 
    if(current != NULL) {
        //current = flatten_list(current);
        prune_node(current);
        flatten_list(current);
        prune_print_list(current, parent);
        for(int i = 0; i < current->n_children; i++) {
            dfs(current->children[i], current);
        }
        if(current->type.index == expression_n.index)
            calculate_expr(current, parent);

    }

}

void simplify_tree ( node_t **simplified, node_t *root )
{
    /* TODO: implement the simplifications of the tree here */
    *simplified = root;
    dfs(root, root);

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
    if(node->data == "**") {
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

//
void calculate_expr( node_t* node, node_t* parent) {
    // recurse to bottom, and evaluate bottom-up

    if( node == NULL )
        return;
    if( node->type.index != expression_n.index )
        return;

    // DEBUG
    //fprintf(stdout, "recurse\n");
    if(node->n_children > 0)
        for(int i = 0; i < node->n_children; i++)
            calculate_expr(node->children[i], node);

    // DEBUG
    //fprintf(stdout, "Bottom!\n");


    if(node->n_children == 2 &&
       node->children[0]->type.index == integer_n.index &&
       node->children[1]->type.index == integer_n.index) {
            //fprintf(stdout, " TWO INT CHILDS\n");
            doOp(node, parent);
    } else {
                //fprintf(stdout, " NOT TWO INT CHILDS\n");
    }
    char* c;
    if(node->data != NULL)
        c = (char*)node->data;
    if(node->n_children==1 && *c == '-') {
        //fprintf(stdout, "UMINUS!\n");
        unary_minus(node);
    } else {
        //fprintf(stdout, "NOT AN OPERATION!\n");
    }
}
/* 
 * Basic data structure for syntax tree nodes.
 * Both the label data and the list of children are consistently allocated
 * in a dynamic fashion, even if data is just a single character, integer,
 * etc., because it simplifies using a recursive traversal of the tree, both
 * for decoration, printing and destruction.
 */
/*
//typedef struct n {
//    nodetype_t type;        * Type of this node */
//    void *data;             * Data label for terminals and expressions */
//    void *entry;            * Pointer to symtab entry */
//    uint32_t n_children;    * Number of children */
//    struct n **children;    * Pointers to child nodes */
//} node_t;
// 


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
    if(node->type.index == search.index)
	parent->children[*count++] = node;

    for(int i = 0; i < node->n_children; i++) {
	recurse_flatten( node->children[i], count, depth, parent, search );
    }
}

node_t* construct_flatten_list( node_t* node, int depth, node_t* parent, nodetype_t search ) {
    // what are we flattening? search
    // call one method for flatten var list, statement list osv.? nope
    // flatten_var_list( node, depth ) ?
    int* count;
    if(parent == NULL) {
    	parent = malloc(sizeof(node_t));
    	parent->n_children = depth;
    	parent->children = malloc(sizeof(node_t*)*depth);
    	count = malloc(sizeof(int));
	*count = 0;
    }
    
    recurse_flatten( node, count, depth, parent, search );
/*  
    for(int i = 0; i < node->n_children; i++) {
	recurse_flatten( node->children[i], count, depth, parent, search);
    }
*/
    return parent;
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

/* find depth of subtree (of a list) */
int subtree_depth( node_t* node, int count, nodetype_t search ) {
    if(node->type.index == search.index)
	return 1;
    if(node->children!=0) {
	for(int i = 0; i < node->n_children; i++)
	    count += subtree_depth(node->children[i], count, search);
    }
    return count;
}


// this should be done first time we encounter a list type
node_t* flatten_list( node_t* node ) 
{
    node_t* new_node = NULL;

    if(node->type.index == statement_list_n.index) {
	// find depth of subtree
	int depth = subtree_depth(node, 0, statement_n);
	new_node = construct_flatten_list(node, depth, NULL, statement_n);
    }
    else if(node->type.index == print_list_n.index) {
	int depth = subtree_depth(node, 0, print_item_n);
	new_node = construct_flatten_list(node, depth, NULL, print_item_n);
    }
    else if(node->type.index == parameter_list_n.index) {
	int depth = subtree_depth(node, 0, variable_n);
	new_node = construct_flatten_list(node, depth, NULL, variable_n);
    }
    else if(node->type.index == declaration_list_n.index) {
	int depth = subtree_depth(node, 0, declaration_n);
	new_node = construct_flatten_list(node, depth, NULL, declaration_n);
    }
    else if(node->type.index == argument_list_n.index) {
	int depth = subtree_depth(node, 0, expression_list_n);
	new_node = construct_flatten_list(node, depth, NULL, expression_list_n);
    }
    else if(node->type.index == expression_list_n.index) {
	int depth = subtree_depth(node, 0, expression_n);
	new_node = construct_flatten_list(node, depth, NULL, expression_n);
    }
    return new_node;
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
void dfs( node_t* current, node_t* root ) 
{ 
    node_t* temp = NULL;
    if(current != NULL) {
	temp=flatten_list(current);
	if(temp != NULL)
	    current = temp;
    	for(int i = 0; i < current->n_children; i++) {
	    dfs(current->children[i], root);
	}
    }

}

void
simplify_tree ( node_t **simplified, node_t *root )
{
    /* TODO: implement the simplifications of the tree here */
    *simplified = root;
    dfs(root, root);

}


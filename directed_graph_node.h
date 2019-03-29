#ifndef DIRECTED_GRAPH_NODE_H
#define DIRECTED_GRAPH_NODE_H

#include "unordered_set.h"
#include <stdbool.h>
#include <string.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define TRUE 1
#define FALSE 0

    typedef struct directed_graph_node {
        int           m_id;
        unordered_set m_parent_node_set;
        unordered_set m_child_node_set;
    } directed_graph_node;

    int directed_graph_nodes_equal_function(void* a, void* b);

    size_t directed_graph_node_hash_function(void* v);

    typedef struct directed_graph_node directed_graph_node;

    typedef struct directed_graph_node_parent_node_generator {
        void* (*expand)(void*);
    } directed_graph_node_parent_node_generator;

    typedef struct directed_graph_node_child_node_generator {
        void* (*expand)(void*);
    } directed_graph_node_child_node_generator;

    /***************************************************************************
    * Allocates a new directed graph node with given ID, initializes it and    *
    * returns it.                                                              *
    ***************************************************************************/
    directed_graph_node* directed_graph_node_alloc(int id);

    /***************************************************************************
    * Initializes a given memory area to a directed graph node.                *
    ***************************************************************************/
    void directed_graph_node_init(directed_graph_node* p_node, int id);

    /***************************************************************************
    * Creates an arc (p_tail, p_head) and returns true if the arc is actually  *
    * created. 'p_tail' is called a "parent" of 'p_head', and 'p_head' is      *
    * called a "child" of 'p_tail'.                                            *
    ***************************************************************************/
    int directed_graph_node_add_arc(directed_graph_node* p_tail,
                                    directed_graph_node* p_head);

    /***************************************************************************
    * Returns true if 'p_node' has a child 'p_child_candidate'.                *
    ***************************************************************************/
    int directed_graph_node_has_arc(directed_graph_node* p_node,
                                    directed_graph_node* p_child_candidate);

    /***************************************************************************
    * Removes the arc (p_tail, p_node) between the two nodes.                  *
    ***************************************************************************/
   int directed_graph_node_remove_arc(directed_graph_node* p_tail,
                                      directed_graph_node* p_head);

    /***************************************************************************
    * Returns the textual representation of the node.                          *
    ***************************************************************************/
    char* directed_graph_node_to_string(directed_graph_node* p_node);

    /***************************************************************************
    * Returns the set containing all the child nodes of the given node.        *
    ***************************************************************************/
    unordered_set* 
        directed_graph_node_children_set(directed_graph_node* p_node);

    /***************************************************************************
    * Returns the set containing all the parent nodes of the given node.       *
    ***************************************************************************/
    unordered_set*
        directed_graph_node_parent_set(directed_graph_node* p_node);

    /***************************************************************************
    * Removes all the arcs involving the input node.                           *
    ***************************************************************************/
    void directed_graph_node_clear(directed_graph_node* p_node);

    /***************************************************************************
    * Deallocates the node.                                                    *
    ***************************************************************************/
    void directed_graph_node_free(directed_graph_node* p_node);

    void directed_graph_node_destruct(directed_graph_node* p_node);


#ifdef  __cplusplus
} 
#endif
#endif  /* DIRECTED_GRAPH_NODE_H */
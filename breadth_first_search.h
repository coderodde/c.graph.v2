#ifndef BREADTH_FIRST_SEARCH_H
#define BREADTH_FIRST_SEARCH_h
#include "list.h"

/*******************************************************************************
* Defines the API before client graph and this BFS implementation.             *
*******************************************************************************/
typedef struct child_node_iterator {

    /********************************************
    * Holds the actual iterator implementation. *
    ********************************************/
    void* state;

    /**********************************************************************
    * Performs whatever initialization are needed on behalf of the state. *
    **********************************************************************/
    void (*child_node_iterator_init)(struct child_node_iterator* me,
                                     void* node);

    /**************************************************************
    * Signals whether there are more child nodes to iterate over. *
    **************************************************************/
    int (*child_node_iterator_has_next) (struct child_node_iterator* me);

    /******************************
    * Return the next child node. *
    ******************************/
    void* (*child_node_iterator_next) (struct child_node_iterator* me);
} 
child_node_iterator;


/*******************************************************************************
* Performs a BFS shortest path search from a source node to a target node.     * 
*******************************************************************************/
list* breadth_first_search(void* source_node,
                           void* target_node,
                           child_node_iterator* child_iterator,
                           size_t (*hash_function)(void*),
                           int(*equals_function)(void*, void*));

#endif

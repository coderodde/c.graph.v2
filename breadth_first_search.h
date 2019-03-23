#ifndef BREADTH_FIRST_SEARCH_H
#define BREADTH_FIRST_SEARCH_h
#include "list.h"
#include "utils.h"

/*******************************************************************************
* Performs a BFS shortest path search from a source node to a target node.     * 
*******************************************************************************/
list* breadth_first_search(void* source_node,
                           void* target_node,
                           child_node_iterator* child_iterator,
                           size_t (*hash_function)(void*),
                           int(*equals_function)(void*, void*));

#endif

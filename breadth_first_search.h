#ifndef BREADTH_FIRST_SEARCH_H
#define BREADTH_FIRST_SEARCH_h
#include "list.h"


list* breadth_first_search(void* source_node,
    void* target_node,
    size_t(*hash_function)(void*),
    int(*equals_function)(void*, void*));

#endif

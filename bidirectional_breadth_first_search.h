#ifndef BIDIRECTIONAL_BREADTH_FIRST_SEARCH_H
#define BIDIRECTIONAL_BREADTH_FIRST_SEARCH_H
#include "list.h"
#include "utils.h"

list* bidirectional_breadth_first_search(void* source_node,
                                         void* target_node,
                                         child_node_iterator* child_iterator,
                                         parent_node_iterator* parent_iterator,
                                         size_t (*hash_function)(void*),
                                         int (*equals_function)(void*, void*));

void bidirectional_breadht_first_search_test();

#endif /* BIDIRECTIONAL_BREADTH_FIRST_SEARCH_H */
#include "breadth_first_search.h"
#include "directed_graph_node.h"
#include "queue.h"
#include "list.h"
#include "unordered_map.h"
#include "unordered_set.h" //! REMOVE

static list* trace_back_path(void* target_node, unordered_map* parents)
{
    list* path = list_alloc(10);
    void* node = target_node;

    while (node)
    {
        list_push_front(path, node);
        node = unordered_map_get(parents, node);
    }

    return path;
}

list* breadth_first_search(void* source_node, 
                           void* target_node,
                           child_node_iterator* child_iterator,
                           size_t (*hash_function)   (void*),
                           int    (*equals_function) (void*, void*))
{
    queue* q = queue_alloc();// TODO!
    unordered_map* parent_map = 
        unordered_map_alloc(10,
                            1.0f, 
                            hash_function, 
                            equals_function);
    void* current_node;
    void* child_node;
    size_t children_set_size;
    int i;

    if (!source_node
        || !target_node
        || !child_iterator
        || !hash_function
        || !equals_function)
    {
        return NULL;
    }

    queue_push_back(q, source_node);
    unordered_map_put(parent_map, source_node, NULL);

    while (queue_size(q) > 0)
    {
        current_node = queue_pop_front(q);

        if (equals_function(current_node, target_node))
        {
            return trace_back_path(target_node, parent_map);
        }

        child_iterator->child_node_iterator_init(child_iterator, current_node);

        while (child_iterator->child_node_iterator_has_next(child_iterator))
        {
            child_node = child_iterator->
                         child_node_iterator_next(child_iterator);

            if (!unordered_map_contains_key(parent_map, child_node))
            {
                unordered_map_put(parent_map, child_node, current_node);
                queue_push_back(q, child_node);
            }
        }

        child_iterator->child_node_iterator_free(child_iterator);
    }

    return NULL;
}

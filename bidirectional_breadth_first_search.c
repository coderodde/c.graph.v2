#include "bidirectional_breadth_first_search.h"
#include "list.h"
#include "queue.h"
#include "utils.h"
<<<<<<< HEAD
#include <limits.h>
=======

>>>>>>> 762273c60dc5d5029d89da43112e43c70a82b704

#define NEIGHBOR_NODE_ITERATOR_HAS_NEXT child_node_iterator_has_next
#define NEIGHBOR_NODE_ITERATOR_NEXT     child_node_iterator_next

typedef child_node_iterator neighbor_node_iterator;

list* bidirectional_breadth_first_search(void* source_node,
                                         void* target_node,
                                         child_node_iterator* child_iterator,
                                         parent_node_iterator* parent_iterator,
                                         size_t (*hash_function)(void*),
                                         int (*equals_function)(void*, void*))
{
    queue* queue_a;
    queue* queue_b;
    unordered_map* parents_a;
    unordered_map* parents_b;
    unordered_map* distance_a;
    unordered_map* distance_b;
    size_t dist_a;
    size_t dist_b;
    size_t best_cost;
    void* touch_node;
    void* current_node;
    void* child_node;
    void* parent_node;
    
    if (!source_node
        || !target_node
        || !child_iterator
        || !parent_iterator
        || !hash_function
        || !equals_function)
    {
        return NULL;
    }

    queue_a = queue_alloc();
    queue_b = queue_alloc();

    parents_a = unordered_map_alloc(10,
                                    1.0f,
                                    hash_function,
                                    equals_function);

    parents_b = unordered_map_alloc(10,
                                    1.0f,
                                    hash_function, 
                                    equals_function);

    distance_a = unordered_map_alloc(10,
                                     1.0f,
                                     hash_function,
                                     equals_function);

    distance_b = unordered_map_alloc(10,
                                     1.0f,
                                     hash_function,
                                     equals_function);

    queue_push_back(queue_a, source_node);
    queue_push_back(queue_b, target_node);
    unordered_map_put(parents_a, source_node, NULL);
    unordered_map_put(parents_b, target_node, NULL);
    unordered_map_put(distance_a, source_node, 0);
    unordered_map_put(distance_b, target_node, 0);

    best_cost = UINT_MAX;
    touch_node = NULL;

    while (queue_size(queue_a) > 0 && queue_size(queue_b) > 0)
    {
        dist_a = (size_t) unordered_map_get(distance_a, queue_front(queue_a));
        dist_b = (size_t) unordered_map_get(distance_a, queue_front(queue_b));

        if (touch_node && best_cost <= dist_a + dist_b)
        {
            return trace_back_path_bidirectional(touch_node,
                                                 parents_a,
                                                 parents_b);
        }
<<<<<<< HEAD
         
        current_node = queue_pop_front(queue_a);
        dist_a = (size_t) unordered_map_get(parents_a, current_node);
        dist_b = (size_t) unordered_map_get(parents_b, current_node);

        if (unordered_map_contains_key(parents_b, current_node)
            &&
            best_cost > dist_a + dist_b)
        {
            best_cost = dist_a + dist_b;
            touch_node = current_node;
        }

        child_iterator->child_node_iterator_init(child_iterator, 
                                                 current_node);

        while (child_iterator->child_node_iterator_has_next(child_iterator))
        {
            child_node = child_iterator->
                         child_node_iterator_next(child_iterator);

            if (!unordered_map_contains_key(parents_a, child_node))
            {
                unordered_map_put(parents_a, child_node, current_node);
                queue_push_back(queue_a, child_node);
            }
        }

        child_iterator->child_node_iterator_free(child_iterator);
        current_node = queue_pop_front(queue_b);
        dist_a = (size_t) unordered_map_get(distance_a, current_node);
        dist_b = (size_t) unordered_map_get(distance_b, current_node);

        if (unordered_map_contains_key(parents_a, current_node)
            &&
            best_cost > dist_a + dist_b)
        {
            best_cost = dist_a + dist_b;
            touch_node = current_node;
        }

        parent_iterator->parent_node_iterator_init(parent_iterator,
                                                   current_node);

        while (parent_iterator->
               parent_node_iterator_has_next(parent_iterator))
        {
            parent_node = parent_iterator->
                          parent_node_iterator_next(parent_iterator);

            if (!unordered_map_contains_key(parents_b, parent_node))
            {
                unordered_map_put(parents_b, parent_node, current_node);
                queue_push_back(queue_b, parent_node);
            }
        }

        parent_iterator->parent_node_iterator_free(parent_iterator);
=======
        /*
        if (unordered_map_size(distance_a) < unordered_map_size(distance_b))
        {*/
            current_node = queue_pop_front(queue_a);
            dist_a = (size_t) unordered_map_get(parents_a, current_node);
            dist_b = (size_t) unordered_map_get(parents_b, current_node);

            if (unordered_map_contains_key(parents_b, current_node)
                &&
                best_cost > dist_a + dist_b)
            {
                best_cost = dist_a + dist_b;
                touch_node = current_node;
            }

            child_iterator->child_node_iterator_init(child_iterator, 
                                                     current_node);

            while (child_iterator->child_node_iterator_has_next(child_iterator))
            {
                child_node = child_iterator->
                             child_node_iterator_next(child_iterator);

                if (!unordered_map_contains_key(parents_a, child_node))
                {
                    unordered_map_put(parents_a, child_node, current_node);
                    queue_push_back(queue_a, child_node);
                }
            }

            child_iterator->child_node_iterator_free(child_iterator);
        /*}
        else
        {*/
            current_node = queue_pop_front(queue_b);
            dist_a = (size_t) unordered_map_get(distance_a, current_node);
            dist_b = (size_t) unordered_map_get(distance_b, current_node);

            if (unordered_map_contains_key(parents_a, current_node)
                &&
                best_cost > dist_a + dist_b)
            {
                best_cost = dist_a + dist_b;
                touch_node = current_node;
            }

            parent_iterator->parent_node_iterator_init(parent_iterator,
                                                       current_node);

            while (parent_iterator->
                   parent_node_iterator_has_next(parent_iterator))
            {
                parent_node = parent_iterator->
                              parent_node_iterator_next(parent_iterator);

                if (!unordered_map_contains_key(parents_b, parent_node))
                {
                    unordered_map_put(parents_b, parent_node, current_node);
                    queue_push_back(queue_b, parent_node);
                }
            }

            parent_iterator->parent_node_iterator_free(parent_iterator);
        //}
>>>>>>> 762273c60dc5d5029d89da43112e43c70a82b704
    }

    return NULL;
}
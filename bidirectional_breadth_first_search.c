#include "bidirectional_breadth_first_search.h"
#include "directed_graph_node.h"
#include "list.h"
#include "my_assert.h"
#include "queue.h"
#include "utils.h"
#include <limits.h>

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
        dist_b = (size_t) unordered_map_get(distance_b, queue_front(queue_b));

        if (touch_node && best_cost <= dist_a + dist_b)
        {
            return trace_back_path_bidirectional(touch_node,
                                                 parents_a,
                                                 parents_b);
        }
         
        current_node = queue_pop_front(queue_a);

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
                unordered_map_put(
                    distance_a,
                    child_node, 
                        (void*)(intptr_t)
                        ((size_t)(unordered_map_get(distance_a, 
                                                    current_node)) + 1));

                queue_push_back(queue_a, child_node);
            }
        }

        child_iterator->child_node_iterator_free(child_iterator);

        current_node = queue_pop_front(queue_b);

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
                unordered_map_put(
                    distance_b,
                    parent_node,
                    (void*)(intptr_t)
                    ((size_t)(unordered_map_get(distance_b,
                                                current_node)) + 1));

                queue_push_back(queue_b, parent_node);
            }
        }

        parent_iterator->parent_node_iterator_free(parent_iterator);
    }

    return NULL;
}

/* Initaliaize the child node generator. */
static void directed_graph_children_iterator_init(
    child_node_iterator* cni,
    void* node)
{
    unordered_set* p_node_children_set = directed_graph_node_children_set(node);
    cni->state = unordered_set_iterator_alloc(p_node_children_set);
}

static int directed_graph_children_iterator_has_next(
    child_node_iterator* cni)
{
    return unordered_set_iterator_has_next(cni->state);
}

static void* directed_graph_children_iterator_next(child_node_iterator* cni) {
    void* value;

    unordered_set_iterator_next((unordered_set_iterator*)cni->state, &value);
    return value;
}

static void directed_graph_children_iterator_free(child_node_iterator* cni)
{
    unordered_set_iterator_free((unordered_set_iterator*)cni->state);
}

/* Initialize the parent node generator. */
static void directed_graph_parents_iterator_init(parent_node_iterator* pni,
    void* node)
{
    unordered_set* p_node_parent_set = directed_graph_node_parent_set(node);
    pni->state = unordered_set_iterator_alloc(p_node_parent_set);
}

static int directed_graph_parents_iterator_has_next(parent_node_iterator* pni)
{
    return unordered_set_iterator_has_next(pni->state);
}

static void* directed_graph_parents_iterator_next(parent_node_iterator* pni)
{
    void* value;
    unordered_set_iterator_next(pni->state, &value);
    return value;
}

static void directed_graph_parents_iterator_free(parent_node_iterator* pni)
{
    unordered_set_iterator_free(pni->state);
}


void bidirectional_breadth_first_search_test()
{
    directed_graph_node* a = directed_graph_node_alloc(1);
    directed_graph_node* b = directed_graph_node_alloc(2);
    directed_graph_node* c = directed_graph_node_alloc(3);
    directed_graph_node* d = directed_graph_node_alloc(4);
    directed_graph_node* e = directed_graph_node_alloc(5);

    child_node_iterator children_iterator;
    parent_node_iterator parents_iterator;

    /* Fill the child node generator interface: */
    children_iterator.child_node_iterator_init =
        directed_graph_children_iterator_init;

    children_iterator.child_node_iterator_has_next =
        directed_graph_children_iterator_has_next;

    children_iterator.child_node_iterator_next =
        directed_graph_children_iterator_next;

    children_iterator.child_node_iterator_free =
        directed_graph_children_iterator_free;

    /* Fill the parent node generator interface: */
    parents_iterator.parent_node_iterator_init =
        directed_graph_parents_iterator_init;

    parents_iterator.parent_node_iterator_has_next =
        directed_graph_parents_iterator_has_next;

    parents_iterator.parent_node_iterator_next =
        directed_graph_parents_iterator_next;

    parents_iterator.parent_node_iterator_free =
        directed_graph_parents_iterator_free;

    puts("    bidirectional_breadth_first_search_test()");

    directed_graph_node_add_arc(a, b);
    directed_graph_node_add_arc(a, c);
    directed_graph_node_add_arc(c, d);
    directed_graph_node_add_arc(b, e);
    directed_graph_node_add_arc(d, e);

    list* path = bidirectional_breadth_first_search(
        a,
        e,
        &children_iterator,
        &parents_iterator,
        directed_graph_node_hash_function,
        directed_graph_nodes_equal_function);

    printf("Path length: %d\n", list_size(path));

    ASSERT(a == list_get(path, 0));
    ASSERT(b == list_get(path, 1));
    ASSERT(e == list_get(path, 2));
}

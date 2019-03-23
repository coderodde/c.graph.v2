#include "breadth_first_search.h"
#include "bidirectional_breadth_first_search.h"
#include "directed_graph_node.h"
#include "my_assert.h"
#include "queue.h"
#include "unordered_map.h"
#include "unordered_set.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>

#define _C_GRAPH_V2_TEST
#define _C_GRAPH_V2_BENCHMARK

#define FALSE 0

static void test_all()
{
    puts("--- Testing ---");
    queue_test();
    list_test();
    unordered_map_test();
    unordered_set_test();
    PRINT_TEST_RESULTS;
    puts("--- Done testing ---");
}

static const size_t NODES = 10000;
static const size_t ARCS = 50000;
static const double MAXX = 1000.0;
static const double MAXY = 1000.0;
static const double MAXZ = 1000.0;

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

    unordered_set_iterator_next((unordered_set_iterator*) cni->state, &value);
    return value;
}

static void directed_graph_children_iterator_free(child_node_iterator* cni)
{
    unordered_set_iterator_free((unordered_set_iterator*) cni->state);
}

/* Initialize the parent node generator. */
static void directed_graph_parents_iterator_init(parent_node_iterator* pni,
                                                 void* node)
{
    unordered_set* p_node_parent_set = directed_graph_node_parent_set(node);
    pni->state = unordered_set_iterator_alloc(p_node_parent_set);
}

static void directed_graph_parents_iterator_has_next(parent_node_iterator* pni)
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

static void benchmark_unweighted_general_graph()
{
    child_node_iterator children_iterator;
    parent_node_iterator parents_iterator;
    int i;

    unweighted_graph_data* gd = create_unweighted_random_graph(
        NODES,
        ARCS,
        MAXX,
        MAXY,
        MAXZ,
        directed_graph_node_hash_function,
        directed_graph_node_equals_function);

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
    
    /* Randomize the terminal nodes: */
    directed_graph_node* source_node = choose(gd->p_node_array, NODES);
    directed_graph_node* target_node = choose(gd->p_node_array, NODES);

    printf("Source: %s\n", directed_graph_node_to_string(source_node));
    printf("Target: %s\n", directed_graph_node_to_string(target_node));

    /* BFS path: */
    list* path1;
    list* path2;
    double time_a;
    double time_b;

    time_a = get_time();
    path1 = breadth_first_search(source_node,
                                 target_node,
                                 &children_iterator,
                                 directed_graph_node_hash_function,
                                 directed_graph_node_equals_function);
    time_b = get_time();

    puts("Shortest path from source to target using BFS:");
    printf("Duration: %d milliseconds.\n", (int)(time_b - time_a));

    for (i = 0; i < list_size(path1); i++)
    {
        printf("[%s]\n",
               directed_graph_node_to_string(
                   list_get(path1, i)));
    }

    printf("Valid path: %s\n", is_valid_path(path1) ? "true" : "false");

    /* Bidirectional BFS path: */
    time_a = get_time();
    path2 = bidirectional_breadth_first_search(source_node,
                                               target_node,
                                               &children_iterator,
                                               &parents_iterator,
                                               directed_graph_node_hash_function,
                                               directed_graph_node_equals_function);
    time_b = get_time();

    puts("Shortest path from source to target using bidirectional BFS:");
    printf("Duration: %d milliseconds.\n", (int)(time_b - time_a));

    for (i = 0; i < list_size(path2); i++)
    {
        printf("[%s]\n",
               directed_graph_node_to_string(list_get(path2, i)));
    }

    printf("Valid path: %s\n", is_valid_path(path2) ? "true" : "false");
    return 0;
}

static void benchmark_all()
{
    benchmark_unweighted_general_graph();
}

int main(int argc, char* argv[])
{
#ifdef _C_GRAPH_V2_TEST
    test_all();
#endif

#ifdef _C_GRAPH_V2_BENCHMARK
    benchmark_all();
#endif

    return 0;
}
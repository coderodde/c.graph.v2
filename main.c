#include "breadth_first_search.h"
#include "directed_graph_node.h"
#include "my_assert.h"
#include "queue.h"
#include "unordered_map.h"
#include "unordered_set.h"
#include "utils.h"
#include <stdio.h>
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

static void directed_graph_children_iterator_init(
    child_node_iterator* cni, 
    void* node)
{
    directed_graph_node* p_node = (directed_graph_node*) node;
    unordered_set* p_node_children_set =
        directed_graph_node_children_set(p_node);

    cni->state = unordered_set_iterator_alloc(p_node_children_set);
}

static int directed_graph_children_iterator_has_next(
    child_node_iterator* cni)
{
    return unordered_set_iterator_has_next(
        (unordered_set_iterator*) cni->state);
}

static void* directed_graph_children_iterator_next(child_node_iterator* cni) {
    void* value;

    unordered_set_iterator_next((unordered_set_iterator*) cni->state, &value);
    return value;
}

static void benchmark_unweighted_general_graph()
{
    child_node_iterator children_iterator;
    int i;

    unweighted_graph_data* gd = create_unweighted_random_graph(
        NODES,
        ARCS,
        MAXX,
        MAXY,
        MAXZ,
        directed_graph_node_hash_function,
        directed_graph_node_equals_function);
    /*
    children_iterator.state = 
        unordered_set_alloc(
            10,
            1.0f, 
            directed_graph_node_hash_function, 
            directed_graph_node_equals_function);*/

    children_iterator.child_node_iterator_init = 
        directed_graph_children_iterator_init;

    children_iterator.child_node_iterator_has_next =
        directed_graph_children_iterator_has_next;

    children_iterator.child_node_iterator_next =
        directed_graph_children_iterator_next;

    directed_graph_node* source_node = choose(gd->p_node_array, NODES);
    directed_graph_node* target_node = choose(gd->p_node_array, NODES);

    printf("Source: %s\n", directed_graph_node_to_string(source_node));
    printf("Target: %s\n", directed_graph_node_to_string(target_node));

    list* path;


    path = breadth_first_search(source_node,
                                target_node,
                                &children_iterator,
                                directed_graph_node_hash_function,
                                directed_graph_node_equals_function);

    puts("Shortest path from source to target using BFS:");

    for (i = 0; i < list_size(path); i++)
    {
        printf("[%s]\n",
               directed_graph_node_to_string(
                   list_get(path, i)));
    }

    printf("Valid path: %s\n", is_valid_path(path) ? "true" : "false");
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
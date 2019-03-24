#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

#include "directed_graph_node.h"
#include "directed_graph_weight_function.h"
#include "unordered_map.h"
#include "list.h"

#ifdef  __cplusplus
extern "C" {
#endif


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
        void(*child_node_iterator_init)(struct child_node_iterator* me,
            void* node);

        /**************************************************************
        * Signals whether there are more child nodes to iterate over. *
        **************************************************************/
        int(*child_node_iterator_has_next)(struct child_node_iterator* me);

        /******************************
        * Return the next child node. *
        ******************************/
        void* (*child_node_iterator_next)(struct child_node_iterator* me);

        /********************************************
        * Frees the resources held by the iterator. *
        ********************************************/
        void (*child_node_iterator_free)(struct child_node_iterator* me);
    }
    child_node_iterator;



    /*******************************************************************************
    * Defines the API before client graph and this BFS implementation.             *
    *******************************************************************************/
    typedef struct parent_node_iterator {

        /********************************************
        * Holds the actual iterator implementation. *
        ********************************************/
        void* state;

        /**********************************************************************
        * Performs whatever initialization are needed on behalf of the state. *
        **********************************************************************/
        void (*parent_node_iterator_init)(struct parent_node_iterator* me,
                                          void* node);

        /**************************************************************
        * Signals whether there are more child nodes to iterate over. *
        **************************************************************/
        int (*parent_node_iterator_has_next)(struct parent_node_iterator* me);

        /******************************
        * Return the next child node. *
        ******************************/
        void* (*parent_node_iterator_next)(struct parent_node_iterator* me);

        /********************************************
        * Frees the resources held by the iterator. *
        ********************************************/
        void (*parent_node_iterator_free)(struct parent_node_iterator* me);
    }
    parent_node_iterator;

    /*************************************************************************
    * Constructs a shortest path from the data structures of a bidirectional *
    * path finders.                                                          *
    *************************************************************************/
    list*
        trace_back_path_bidirectional(void* touch_node,
                                      unordered_map* parents_forward,
                                      unordered_map* parents_backward);

    typedef struct point_3d {
        double x;
        double y;
        double z;
    } point_3d;

    typedef struct unweighted_graph_data {
        directed_graph_node**           p_node_array;
        unordered_map*                  p_point_map;
    } unweighted_graph_data;

    typedef struct weighted_graph_data {
        directed_graph_node**           p_node_array;
        directed_graph_weight_function* p_weight_function;
        unordered_map*                  p_point_map;
    } weighted_graph_data;

    point_3d* random_point(double maxx, double maxy, double maxz);

    double point_3d_distance(point_3d* p_a, point_3d* p_b);

    directed_graph_node* choose(directed_graph_node** p_table,
        const size_t size);

    unweighted_graph_data* create_unweighted_random_graph(
        const size_t nodes,
        size_t edges,
        const double maxx,
        const double maxy,
        const double maxz,
        size_t (*hash_function)(void*),
<<<<<<< HEAD
        int (*equals_function)(void*, void*));
=======
        int (*equals_function)(void*));
>>>>>>> 762273c60dc5d5029d89da43112e43c70a82b704

    list* traceback_path(directed_graph_node* p_target,
        unordered_map* p_parent_map);

    int directed_graph_node_equals_function(void*, void*);
    size_t directed_graph_node_hash_function(void*);

    int is_valid_path(list* p_path);
    double get_time();

    /*
    double compute_path_cost(
        list* p_path, directed_graph_weight_function* p_weight_function);*/

#ifdef  __cplusplus
}
#endif

#endif  /* GRAPH_UTILS_H */
#define _CRT_SECURE_NO_WARNINGS
#include "directed_graph_node.h"
#include "my_assert.h"
#include "unordered_set.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

static const int INITIAL_CAPACITY = 16;
static const int MAXIMUM_NAME_STRING_LEN = 80;
static const float MINIMUM_LOAD_FACTOR = 0.2f;

size_t directed_graph_node_hash_function(void* v)
{
    return (size_t)((directed_graph_node*) v)->m_id;
}

int directed_graph_nodes_equal_function(void* a, void* b)
{
    return ((directed_graph_node*) a)->m_id ==
           ((directed_graph_node*) b)->m_id;
}

static size_t fix_initial_capacity(size_t initial_capacity)
{
    size_t result_capacity = 1;
    while (result_capacity <=  initial_capacity)
    {
        result_capacity <<= 1;
    }
    return result_capacity;
}

static float fix_load_factor(float requested_load_factor)
{
    return requested_load_factor < MINIMUM_LOAD_FACTOR ?
           MINIMUM_LOAD_FACTOR :
           requested_load_factor;
}

void directed_graph_node_init(directed_graph_node* p_node, 
                              int id,
                              size_t initial_capacity,                          
                              float load_factor,
                              size_t (*p_hash_function)(void*),
                              int (*p_equals_function)(void*, void*))
{
    p_node->m_id = id;
    
    unordered_set_init(&p_node->m_child_node_set,
                       initial_capacity,
                       load_factor,
                       p_hash_function,
                       p_equals_function);

    unordered_set_init(&p_node->m_parent_node_set,
                       initial_capacity,
                       load_factor,
                       p_hash_function,
                       p_equals_function);
}

directed_graph_node* directed_graph_node_alloc(int id)
{
    directed_graph_node* p_node = malloc(sizeof(*p_node));

    if (!p_node)
    {
        return NULL;
    }

    directed_graph_node_init(p_node,
                             id,
                             5,
                             1.0f,
                             directed_graph_node_hash_function,
                             directed_graph_nodes_equal_function);

    return p_node;
}

int directed_graph_node_add_arc(directed_graph_node* p_tail,
                                directed_graph_node* p_head)
{
    if (!p_tail || !p_head) return FALSE;


    /* Attempt to add p_head to the child list of p_tail: */
    if (!unordered_set_add(&p_tail->m_child_node_set, p_head))
    {
        return FALSE;
    }

    /* Attempt to add p_tail to the parent list of p_head: */
    if (!unordered_set_add(&p_head->m_parent_node_set, p_tail))
    {
        unordered_set_remove(&p_tail->m_child_node_set, p_head);
        return FALSE;
    }

    return TRUE;
}

int directed_graph_node_has_arc(directed_graph_node* p_node,
    directed_graph_node* p_child_candidate)
{
    if (!p_node || !p_child_candidate) return FALSE;

    return unordered_set_contains(&p_node->m_child_node_set,
                                  p_child_candidate);
}

int directed_graph_node_remove_arc(directed_graph_node* p_tail,
                                   directed_graph_node* p_head)
{
    if (!p_tail || !p_head) return FALSE;

    unordered_set_remove(&p_tail->m_child_node_set, p_head);
    unordered_set_remove(&p_head->m_parent_node_set, p_tail);
    return TRUE;
}

char* directed_graph_node_to_string(directed_graph_node* p_node)
{
    char* str = calloc(11, sizeof(char));
    sprintf(str, "%d", p_node->m_id);
    return str;
}

unordered_set*
    directed_graph_node_children_set(directed_graph_node* p_node)
{
    return p_node ? &p_node->m_child_node_set : NULL;
}

unordered_set*
    directed_graph_node_parent_set(directed_graph_node* p_node)
{
    return p_node ? &p_node->m_parent_node_set : NULL;
}

void directed_graph_node_clear(directed_graph_node* p_node)
{
    unordered_set_iterator* p_iterator;
    directed_graph_node*    p_tmp_node;

    if (!p_node) return;

    /* use here the iterator */
    p_iterator = unordered_set_iterator_alloc(&p_node->m_child_node_set);

    /* Proceed to removing this node (p_node) from the parent/child lists. */
    while (unordered_set_iterator_has_next(p_iterator))
    {
        unordered_set_iterator_next(p_iterator, (void*) &p_tmp_node);

        if (p_node->m_id != p_tmp_node->m_id)
        {
            unordered_set_remove(&p_tmp_node->m_parent_node_set, p_node);
        }
    }

    p_iterator = unordered_set_iterator_alloc(&p_node->m_parent_node_set);

    while (unordered_set_iterator_has_next(p_iterator))
    {
        unordered_set_iterator_next(p_iterator, (void**) &p_tmp_node);

        if (p_node->m_id != p_tmp_node->m_id != 0)
        {
            unordered_set_remove(&p_tmp_node->m_child_node_set, p_node);
        }
    }

    unordered_set_clear(&p_node->m_parent_node_set);
    unordered_set_clear(&p_node->m_child_node_set);
}

void directed_graph_node_destruct(directed_graph_node* p_node)
{
    if (p_node)
    {
        unordered_set_destroy(&p_node->m_child_node_set);
        unordered_set_destroy(&p_node->m_parent_node_set);
    }
}

void directed_graph_node_free(directed_graph_node* p_node)
{
    directed_graph_node_destruct(p_node);
    free(p_node);
}

static void directed_graph_node_test_add_arc()
{
    directed_graph_node* node_a;
    directed_graph_node* node_b;
    directed_graph_node* node_c;

    puts("        directed_graph_node_test_add_arc()");

    node_a = directed_graph_node_alloc(1);
    node_b = directed_graph_node_alloc(2);
    node_c = directed_graph_node_alloc(3);

    ASSERT(directed_graph_node_add_arc(node_a, node_b));
    ASSERT(directed_graph_node_add_arc(node_b, node_c));

    ASSERT(directed_graph_node_has_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_b, node_c));
    ASSERT(directed_graph_node_has_arc(node_c, node_a) == FALSE);

    ASSERT(directed_graph_node_add_arc(node_c, node_c));
    ASSERT(directed_graph_node_has_arc(node_c, node_c));
    ASSERT(directed_graph_node_remove_arc(node_c, node_c));
    ASSERT(directed_graph_node_has_arc(node_c, node_c) == FALSE);
}

static void directed_graph_node_test_remove_arc()
{
    directed_graph_node* node_a;
    directed_graph_node* node_b;
    directed_graph_node* node_c;

    node_a = directed_graph_node_alloc(1);
    node_b = directed_graph_node_alloc(2);
    node_c = directed_graph_node_alloc(3);

    ASSERT(directed_graph_node_add_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_b, node_a) == FALSE);
    ASSERT(directed_graph_node_remove_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_a, node_b) == FALSE);
    ASSERT(directed_graph_node_remove_arc(node_a, node_b) == FALSE);
    ASSERT(directed_graph_node_has_arc(node_b, node_c) == FALSE);
}

static void directed_graph_node_test_clear()
{
    directed_graph_node* node_a;
    directed_graph_node* node_b;
    directed_graph_node* node_c;
    unordered_set* set;

    node_a = directed_graph_node_alloc(1);
    node_b = directed_graph_node_alloc(2);
    node_c = directed_graph_node_alloc(3);
    
    ASSERT(directed_graph_node_add_arc(node_a, node_b));
    ASSERT(directed_graph_node_add_arc(node_b, node_c));

    ASSERT(directed_graph_node_has_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_b, node_c));

    set = directed_graph_node_children_set(node_a);
    ASSERT(unordered_set_size(set) == 1);
    ASSERT(unordered_set_contains(set, node_b));

    set = directed_graph_node_children_set(node_b);
    ASSERT(unordered_set_size(set) == 1);
    ASSERT(unordered_set_contains(set, node_c));

    set = directed_graph_node_children_set(node_c);
    ASSERT(unordered_set_size(set) == 0);

    /*********************************************/
    set = directed_graph_node_parent_set(node_a);
    ASSERT(unordered_set_size(set) == 0);

    set = directed_graph_node_parent_set(node_b);
    ASSERT(unordered_set_size(set) == 1);
    ASSERT(unordered_set_contains(set, node_a));

    set = directed_graph_node_parent_set(node_c);
    ASSERT(unordered_set_size(set) == 1);
    ASSERT(unordered_set_contains(set, node_b));
}

void directed_graph_node_test()
{
    puts("    directed_graph_node_test()");

    directed_graph_node_test_add_arc();
    directed_graph_node_test_remove_arc();
    directed_graph_node_test_clear();
}
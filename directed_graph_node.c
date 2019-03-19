#include "directed_graph_node.h"
#include "my_assert.h"
#include "unordered_set.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifndef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

typedef struct directed_graph_node_state {
    char* p_name;
    struct unordered_set* p_parent_node_set;
    struct unordered_set* p_child_node_set;
} directed_graph_node_state;

static const int INITIAL_CAPACITY = 16;
static const int MAXIMUM_NAME_STRING_LEN = 80;
static const float LOAD_FACTOR = 1.0f;

int equals_function(void* a, void* b)
{
    if (!a || !b) return false;

    return strcmp(((directed_graph_node*)a)->state->p_name,
        ((directed_graph_node*)b)->state->p_name) == 0;
}

/* TODO: Come up with a better hash function. */
size_t hash_function(void* v)
{
    size_t ret;
    size_t i;
    char* pc;

    if (!v) return 0;

    ret = 0;
    i = 1;
    pc = ((directed_graph_node*)v)->state->p_name;

    while (*pc)
    {
        ret += *pc * i;
        ++i;
        ++pc;
    }

    return ret;
}

static const size_t MAXIMUM_NAME_LENGTH = 80;

void mycpy(char* dest, const char* src)
{
    size_t len = strlen(src);
    int i;

    dest[len] = 0;

    for (i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
}

directed_graph_node* directed_graph_node_alloc(char* name)
{
    directed_graph_node* p_node = malloc(sizeof(*p_node));
    size_t input_name_length;

    if (!p_node)
    {
        return NULL;
    }

    p_node->state = malloc(sizeof(*p_node->state));

    if (!p_node->state)
    {
        free(p_node);
        return NULL;
    }

    input_name_length = strlen(name);
    p_node->state->p_name = calloc(input_name_length,
        sizeof(char));
    mycpy(p_node->state->p_name, name);
    p_node->state->p_child_node_set = unordered_set_alloc(INITIAL_CAPACITY,
        LOAD_FACTOR,
        hash_function,
        equals_function);

    if (!p_node->state->p_child_node_set)
    {
        free(p_node->state->p_name);
        free(p_node->state);
        free(p_node);
        return NULL;
    }

    p_node->state->p_parent_node_set =
        unordered_set_alloc(INITIAL_CAPACITY,
            LOAD_FACTOR,
            hash_function,
            equals_function);

    if (!p_node->state->p_parent_node_set)
    {
        unordered_set_free(p_node->state->p_child_node_set);
        free(p_node->state->p_name);
        free(p_node->state);
        free(p_node);
        return NULL;
    }

    return p_node;
}

int directed_graph_node_add_arc(directed_graph_node* p_tail,
    directed_graph_node* p_head)
{
    if (!p_tail || !p_head) return false;

    if (!unordered_set_add(p_tail->state->p_child_node_set, p_head))
    {
        return false;
    }

    if (!unordered_set_add(p_head->state->p_parent_node_set, p_tail))
    {
        unordered_set_remove(p_tail->state->p_child_node_set, p_head);
        return false;
    }

    return true;
}

int directed_graph_node_has_arc(directed_graph_node* p_node,
    directed_graph_node* p_child_candidate)
{
    if (!p_node || !p_child_candidate) return false;

    return unordered_set_contains(p_node->state->p_child_node_set,
        p_child_candidate);
}

int directed_graph_node_remove_arc(directed_graph_node* p_tail,
    directed_graph_node* p_head)
{
    if (!p_tail || !p_head) return false;

    unordered_set_remove(p_tail->state->p_child_node_set, p_head);
    unordered_set_remove(p_head->state->p_parent_node_set, p_tail);
    return true;
}

char* directed_graph_node_to_string(directed_graph_node* p_node)
{
    char* text;

    if (!p_node) return "NULL node";

    text = calloc(MAXIMUM_NAME_STRING_LEN, 1);

    sprintf_s(text,
        MAXIMUM_NAME_STRING_LEN,
        "[directed_graph_node: id = %s]",
        p_node->state->p_name);

    return text;
}

unordered_set*
    directed_graph_node_children_set(directed_graph_node* p_node)
{
    return p_node ? p_node->state->p_child_node_set : NULL;
}

unordered_set*
    directed_graph_node_parent_set(directed_graph_node* p_node)
{
    return p_node ? p_node->state->p_parent_node_set : NULL;
}

void directed_graph_node_clear(directed_graph_node* p_node)
{
    unordered_set_iterator* p_iterator;
    directed_graph_node*    p_tmp_node;

    if (!p_node) return;

    p_iterator = unordered_set_iterator_alloc(p_node->state->p_child_node_set);

    while (unordered_set_iterator_has_next(p_iterator))
    {
        unordered_set_iterator_next(p_iterator, &p_tmp_node);

        if (strcmp(p_node->state->p_name, p_tmp_node->state->p_name) != 0)
        {
            unordered_set_remove(p_tmp_node->state->p_parent_node_set, p_node);
        }
    }

    p_iterator = unordered_set_iterator_alloc(p_node->state->p_parent_node_set);

    while (unordered_set_iterator_has_next(p_iterator))
    {
        unordered_set_iterator_next(p_iterator, &p_tmp_node);

        if (strcmp(p_node->state->p_name, p_tmp_node->state->p_name) != 0)
        {
            unordered_set_remove(p_tmp_node->state->p_child_node_set, p_node);
        }
    }

    unordered_set_clear(p_node->state->p_parent_node_set);
    unordered_set_clear(p_node->state->p_child_node_set);
}

void directed_graph_node_free(directed_graph_node* p_node)
{
    unordered_set_iterator* p_iterator;
    directed_graph_node* p_tmp_node;

    if (!p_node) return;

    directed_graph_node_clear(p_node);
    unordered_set_free(p_node->state->p_child_node_set);
    unordered_set_free(p_node->state->p_parent_node_set);
    free(p_node->state);
    free(p_node);
}

static void directed_graph_node_test_add_arc()
{
    directed_graph_node* node_a;
    directed_graph_node* node_b;
    directed_graph_node* node_c;

    puts("        directed_graph_node_test_add_arc()");

    node_a = directed_graph_node_alloc("a");
    node_b = directed_graph_node_alloc("b");
    node_c = directed_graph_node_alloc("c");

    ASSERT(directed_graph_node_add_arc(node_a, node_b));
    ASSERT(directed_graph_node_add_arc(node_b, node_c));

    ASSERT(directed_graph_node_has_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_b, node_c));
    ASSERT(directed_graph_node_has_arc(node_c, node_a) == false);

    ASSERT(directed_graph_node_add_arc(node_c, node_c));
    ASSERT(directed_graph_node_has_arc(node_c, node_c));
    ASSERT(directed_graph_node_remove_arc(node_c, node_c));
    ASSERT(directed_graph_node_has_arc(node_c, node_c) == false);
}

static void directed_graph_node_test_remove_arc()
{
    directed_graph_node* node_a;
    directed_graph_node* node_b;
    directed_graph_node* node_c;

    node_a = directed_graph_node_alloc("a");
    node_b = directed_graph_node_alloc("b");
    node_c = directed_graph_node_alloc("c");

    ASSERT(directed_graph_node_add_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_a, node_b));
    ASSERT(directed_graph_node_remove_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_a, node_b) == false);
    ASSERT(directed_graph_node_remove_arc(node_a, node_b) == false);
}

static void directed_graph_node_test_clear()
{
    directed_graph_node* node_a;
    directed_graph_node* node_b;
    directed_graph_node* node_c;
    unordered_set* set;

    node_a = directed_graph_node_alloc("a");
    node_b = directed_graph_node_alloc("b");
    node_c = directed_graph_node_alloc("c");
    
    ASSERT(directed_graph_node_add_arc(node_a, node_b));
    ASSERT(directed_graph_node_add_arc(node_b, node_c));

    ASSERT(directed_graph_node_has_arc(node_a, node_b));
    ASSERT(directed_graph_node_has_arc(node_b, node_c));

    set = directed_graph_node_children_set(node_a);
    ASSERT(unordered_set_size(set) == 1);
    ASSERT(unordered_set_contains(set, node_b));

    set = directed_graph_node_children_set(node_b);
    ASSERT(unordered_set_size(set) == 1);
    ASSERT(unrodered_set_contains(set, node_c));

    set = directed_graph_node_children_set(node_c);
    ASSERT(unordered_set_size(set) == 0);

    /*********************************************/
    set = directed_graph_node_parent_set(node_a);
    ASSERT(unordered_set_size(set) == 0);

    set = directed_graph_node_parent_set(node_b);
    ASSERT(unordered_set_size(set) == 1);
    ASSERT(unrodered_set_contains(set, node_a));

    set = directed_graph_node_parent_set(node_c);
    ASSERT(unordered_set_size(set) == 1);
    ASSERT(unordered_set_contains(set, node_b));
}

void directed_graph_node_test()
{
    puts("    directed_graph_node_test()");

    directed_graph_node_test_add_arc();
    directed_graph_node_test_remove_arc();
}
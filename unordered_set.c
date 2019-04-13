#include "my_assert.h"
#include "unordered_set.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

static unordered_set_entry* unordered_set_entry_alloc(void* key)
{
    unordered_set_entry* entry = malloc(sizeof(*entry));

    if (!entry)
    {
        return NULL;
    }

    entry->key = key;
    return entry;
}

static const float MINIMUM_LOAD_FACTOR = 0.3f;
static const int MINIMUM_INITIAL_CAPACITY = 4;

static float maxf(float a, float b)
{
    return a < b ? b : a;
}

static int maxi(int a, int b)
{
    return a < b ? b : a;
}

/*******************************************************************************
* Makes sure that the load factor is no less than a minimum threshold.         *
*******************************************************************************/
static float fix_load_factor(float load_factor)
{
    return maxf(load_factor, MINIMUM_LOAD_FACTOR);
}

/*******************************************************************************
* Makes sure that the initial capacity is no less than a minimum allowed and   *
* is a power of two.                                                           *
*******************************************************************************/
static size_t fix_initial_capacity(size_t initial_capacity)
{
    size_t ret = 1;

    while (ret < initial_capacity)
    {
        ret <<= 1;
    }
    
    return ret;
}

int unordered_set_init(unordered_set* p_memory,
                       size_t initial_capacity,
                       float load_factor,
                       size_t(*p_hash_function)(void*),
                       int(*p_equals_function)(void*, void*))
{
    if (!p_memory || !p_hash_function || !p_equals_function)
    {
        return FALSE;
    }

    initial_capacity           = fix_initial_capacity(initial_capacity);
    load_factor                = fix_load_factor(load_factor);
    p_memory->hash_function    = p_hash_function;
    p_memory->equals_function  = p_equals_function;
    p_memory->table_capacity   = initial_capacity;
    p_memory->load_factor      = load_factor;
    p_memory->head             = NULL;
    p_memory->tail             = NULL;
    p_memory->size             = 0;
    p_memory->mod_count        = 0;
    p_memory->table            = 
        calloc(p_memory->table_capacity,
               sizeof(p_memory->table[0]));

    if (!p_memory->table)
    {
        return FALSE;
    }

    p_memory->mask             = p_memory->table_capacity - 1;
    p_memory->max_allowed_size =
        (size_t)(initial_capacity * load_factor);

    return TRUE;
}

unordered_set* unordered_set_alloc(size_t initial_capacity,
                                   float load_factor,
                                   size_t(*p_hash_function)(void*),
                                   int (*p_equals_function)(void*, void*))
{
    unordered_set* set = malloc(sizeof(*set));
    int initialization_succeeded;

    if (!set || !p_hash_function || !p_equals_function)
    {
        return NULL;
    }

    initialization_succeeded = 
        unordered_set_init(set,
                           initial_capacity,
                           load_factor,
                           p_hash_function,
                           p_equals_function);

    if (!initialization_succeeded)
    {
        free(set);
        return NULL;
    }

    return set;
}

static int ensure_capacity(unordered_set* set)
{
    size_t new_capacity;
    size_t new_mask;
    size_t index;
    unordered_set_entry*  entry;
    unordered_set_entry** new_table;

    if (set->size < set->max_allowed_size)
    {
        return FALSE;
    }

    new_capacity = 2 * set->table_capacity;
    new_mask = new_capacity - 1;
    new_table = calloc(new_capacity, sizeof(unordered_set_entry*));

    if (!new_table)
    {
        return FALSE;
    }

    /* Rehash the entries. */
    for (entry = set->head; entry; entry = entry->next)
    {
        index = entry->hash_value & new_mask;
        entry->chain_next = new_table[index];
        new_table[index] = entry;
    }

    free(set->table);

    set->table          = new_table;
    set->table_capacity = new_capacity;
    set->mask           = new_mask;
    set->max_allowed_size = (size_t)(new_capacity * set->load_factor);

    return TRUE;
}

int unordered_set_add(unordered_set* set, void* key)
{
    size_t index;
    size_t hash_value;
    int recompute_hash_table;
    unordered_set_entry* entry = unordered_set_entry_alloc(key);

    if (!entry)
    {
        return FALSE;
    }

    hash_value = set->hash_function(key);
    index = hash_value & set->mask;

    for (entry = set->table[index]; entry; entry = entry->chain_next)
    {
        /* Since hash values are integers, we can speed up the following if 
           statement a bit by comparing the hash values first: */
        if (entry->hash_value == hash_value 
            && set->equals_function(entry->key, key))
        {
            /* key is already in this set: */
            return FALSE;
        }
    }

    if (recompute_hash_table = ensure_capacity(set))
    {
        /* Recompute the index since it is possibly changed by 
          'ensure_capacity' */
        index = hash_value & set->mask;
    }

    entry->chain_next = set->table[index];
    entry->hash_value = hash_value;
    set->table[index] = entry;

    /* Link the new entry to the tail of the list. */
    if (!set->tail)
    {
        set->head = entry;
        set->tail = entry;
        entry->prev = NULL;
        entry->next = NULL;
    }
    else
    {
        /* Let 'entry' be at the tail: */
        set->tail->next = entry;
        entry->prev = set->tail;
        entry->next = NULL;
        set->tail = entry;
    }

    set->size++;
    set->mod_count++;
    return TRUE; 
}

int unordered_set_contains(unordered_set* set, void* key)
{
    size_t index;
    size_t hash_value;
    unordered_set_entry* p_entry;

    if (!set)
    {
        return FALSE;
    }

    hash_value = set->hash_function(key);
    index = hash_value & set->mask;

    for (p_entry = set->table[index]; p_entry; p_entry = p_entry->chain_next)
    {
        if (p_entry->hash_value == hash_value 
            && set->equals_function(key, p_entry->key))
        {
            return TRUE;
        }
    }

    return FALSE;
}

//! Store the hash in nodes?
/* Unlinks a set entry from its collision chain: */
static void unlink_from_collision_chain(unordered_set* set,
                                        unordered_set_entry* prev_entry,
                                        unordered_set_entry* entry_to_unlink)
{
    size_t index;

    if (prev_entry)
    {
        prev_entry->chain_next = entry_to_unlink->chain_next;
    }
    else
    {
        index = set->hash_function(entry_to_unlink->key) & set->mask;
        set->table[index] = entry_to_unlink->chain_next;
    }
}

/* Unlinks the given entry from its predecessor entry in the linked list: */
static void unlink_from_predecessor_entry(unordered_set* set,
                                          unordered_set_entry* entry)
{
    unordered_set_entry* prev_entry = entry->prev;

    if (prev_entry)
    {
        prev_entry->next = entry->next;

        if (entry->next)
        {
            entry->next->prev = prev_entry;
        }
    } 
    else
    {
        set->head = entry->next;

        if (set->head)
        {
            set->head->prev = NULL;
        }
    }
}

/* Unlinks the given entry from its successor entry in the linked list: */
static void unlink_from_successor_entry(unordered_set* set,
                                        unordered_set_entry* entry)
{
    unordered_set_entry* next_entry = entry->next;

    if (next_entry)
    {
        next_entry->prev = entry->prev;

        if (entry->prev)
        {
            entry->prev->next = next_entry;
        }
    }
    else
    {
        set->tail = entry->prev;

        if (set->tail)
        {
            set->tail->next = NULL;
        }
    }
}

int unordered_set_remove(unordered_set* set, void* key)
{
    size_t index;
    size_t hash_value;
    unordered_set_entry* prev_entry;
    unordered_set_entry* current_entry;

    if (!set)
    {
        return FALSE;
    }

    hash_value = set->hash_function(key);
    index = hash_value & set->mask;

    for (current_entry = set->table[index], prev_entry = NULL;
        current_entry;
        prev_entry = current_entry,
        current_entry = current_entry->chain_next)
    {
        if (hash_value == current_entry->hash_value
            && set->equals_function(key, current_entry->key))
        {
            /* Unlink from the collision chain: */
            unlink_from_collision_chain(set, prev_entry, current_entry);

            /* Unlink from linked list: */
            unlink_from_predecessor_entry(set, current_entry);
            unlink_from_successor_entry  (set, current_entry);
            
            /* Update the set state: */
            set->size--;
            set->mod_count++;
            free(current_entry);
            return TRUE;
        }

        prev_entry = current_entry;
    }

    return FALSE;
}

void unordered_set_clear(unordered_set* set)
{
    unordered_set_entry* entry;
    unordered_set_entry* next_entry;
    size_t index;

    if (!set)
    {
        return;
    }

    entry = set->head;
    
    while (entry)
    {
        index = entry->hash_value & set->mask;
        next_entry = entry->next;
        free(entry);
        entry = next_entry;
        set->table[index] = NULL;
    }

    set->mod_count++;
    set->size = 0;
    set->head = NULL;
    set->tail = NULL;
}

size_t unordered_set_size(unordered_set* set)
{
    return set ? set->size : 0;
}

int unordered_set_is_healthy(unordered_set* set)
{
    size_t counter;
    size_t i;
    unordered_set_entry* entry;

    if (!set)
    {
        return FALSE;
    }

    counter = 0;
    entry = set->head;

    if (entry && entry->prev)
    {
        return FALSE;
    }

    ASSERT(set->mask == set->table_capacity - 1);
    
    for (; entry; entry = entry->next)
    {
        counter++;
    }

    ASSERT(counter == set->size);

    for (i = 0; i < set->table_capacity; i++)
    {
        entry = set->table[i];

        while (entry)
        {
            ASSERT(entry->hash_value ==
                set->hash_function(entry->key) & set->mask);
            
            entry = entry->chain_next;
        }
    }
}

void unordered_set_destroy(unordered_set* set)
{
    if (!set)
    {
        return;
    }

    unordered_set_clear(set);
    set->head = NULL;
    set->tail = NULL;
    set->size = 0;
    free(set->table);
    set->table = NULL;
}

void unordered_set_free(unordered_set** p_set)
{
    unordered_set_destroy(*p_set);
    free(*p_set);
    *p_set = NULL;
}

void unordered_set_iterator_init(unordered_set* set, 
                                 unordered_set_iterator* iterator)
{
    iterator->set                = set;
    iterator->iterated_count     = 0;
    iterator->next_entry         = set->head;
    iterator->expected_mod_count = set->mod_count;
}

unordered_set_iterator*
unordered_set_iterator_alloc(unordered_set* set)
{
    unordered_set_iterator* iterator =
        (unordered_set_iterator*)malloc(sizeof(*iterator));

    if (!iterator)
    {
        return NULL;
    }

    unordered_set_iterator_init(set, iterator);
    return iterator;
}

size_t unordered_set_iterator_has_next(unordered_set_iterator* iterator)
{
    if (!iterator)
    {
        return 0;
    }

    if (unordered_set_iterator_is_disturbed(iterator))
    {
        return 0;
    }

    return iterator->set->size - iterator->iterated_count;
}

int unordered_set_iterator_next(unordered_set_iterator* iterator,
    void** key_pointer)
{
    if (!iterator)
    {
        return false;
    }

    if (!iterator->next_entry)
    {
        return false;
    }

    if (unordered_set_iterator_is_disturbed(iterator))
    {
        return false;
    }

    *key_pointer = iterator->next_entry->key;
    iterator->iterated_count++;
    iterator->next_entry = iterator->next_entry->next;

    return true;
}

int unordered_set_iterator_is_disturbed(unordered_set_iterator* iterator)
{
    if (!iterator)
    {
        false;
    }

    return iterator->expected_mod_count != iterator->set->mod_count;
}

void unordered_set_iterator_free(unordered_set_iterator* iterator)
{
    if (!iterator)
    {
        return;
    }

    iterator->set = NULL;
    iterator->next_entry = NULL;
    free(iterator);
}

static int int_equals(void* a, void* b)
{
    int ia = (int)(intptr_t) a;
    int ib = (int)(intptr_t) b;
    return ia == ib;
}

static size_t int_hash_function(void* i)
{
    return (size_t) i;
}

static int str_equals(void* a, void* b)
{
    char* ca = (char*) a;
    char* cb = (char*) b;
    return strcmp(ca, cb) == 0;
}

static size_t str_hash_function(void* p)
{
    size_t sum;
    char* str;
    int i;

    sum = 0;
    str = (char*) p;
    i = 1;
    
    while (*str)
    {
        sum += *str * i;
        str++;
    }

    return sum;
}

static void unordered_set_test_debug_clear()
{
    unordered_set* set = unordered_set_alloc(1, 
                                             1.0f, 
                                             int_hash_function, 
                                             int_equals);
    int i;

    puts("        unordered_set_test_debug_clear()");

    for (i = 0; i < 10; i++)
    {
        unordered_set_add(set, (void*)(intptr_t) i);
    }

    unordered_set_remove(set, 0);
    unordered_set_remove(set, 9);
    unordered_set_clear(set);
    unordered_set_free(set);
}

static void unordered_set_test_add()
{
    unordered_set* set = unordered_set_alloc(1,
                                             0.5f, 
                                             int_hash_function, 
                                             int_equals);
    int i;

    puts("        unordered_set_test_add()");

    for (i = 10; i < 20; i++)
    {
        ASSERT(unordered_set_contains(set, (void*)(intptr_t) i) == FALSE); /*!*/
        ASSERT(unordered_set_add(set,      (void*)(intptr_t) i));
        ASSERT(unordered_set_contains(set, (void*)(intptr_t) i));
        ASSERT(unordered_set_is_healthy(set));
    }

    ASSERT(!unordered_set_contains(set, (void*) 9));
    ASSERT( unordered_set_contains(set, (void*) 10));
    ASSERT( unordered_set_contains(set, (void*) 19));
    ASSERT(!unordered_set_contains(set, (void*) 20));
    
    unordered_set_free(set);

    set = unordered_set_alloc(1, 
                              0.45, 
                              str_hash_function,
                              str_equals);

    ASSERT(!unordered_set_contains(set, "hello"));
    ASSERT(!unordered_set_contains(set, "world"));
    ASSERT(unordered_set_add(set, "world"));
    ASSERT(unordered_set_add(set, "hello"));
    ASSERT(unordered_set_contains(set, "hello"));
    ASSERT(unordered_set_contains(set, "world"));
    ASSERT(!unordered_set_contains(set, "bye"));

    ASSERT(unordered_set_is_healthy(set));

    ASSERT(unordered_set_remove(set, "hello"));
    ASSERT(!unordered_set_contains(set, "hello"));
    
    ASSERT(unordered_set_add(set, "repeat"));
    ASSERT(!unordered_set_add(set, "repeat"));
}

static void unordered_set_test_contains()
{
    unordered_set* set = unordered_set_alloc(3,
                                             0.7f,
                                             int_hash_function,
                                             int_equals);
    int i;

    puts("        unordered_set_test_contains()");

    for (i = 0; i < 100; i++)
    {
        ASSERT(unordered_set_add(set, (void*)(intptr_t) i));
        ASSERT(unordered_set_is_healthy(set));
    }

    for (i = 99; i >= 0; i--)
    {
        ASSERT(unordered_set_is_healthy(set));
        ASSERT(unordered_set_contains(set, (void*)(intptr_t)i));
        ASSERT(unordered_set_is_healthy(set));
    }

    for (i = 50; i < 100; i++)
    {
        ASSERT(unordered_set_is_healthy(set));
        ASSERT(unordered_set_contains(set, (void*)(intptr_t) i));

        ASSERT(unordered_set_is_healthy(set));
        ASSERT(unordered_set_remove(set, (void*)(intptr_t) i));
        ASSERT(unordered_set_is_healthy(set));

        ASSERT(!unordered_set_contains(set, (void*)(intptr_t) i));
        ASSERT(unordered_set_is_healthy(set));
    }

    puts("yeah");
    unordered_set_free(&set);
}

static void unordered_set_test_remove()
{
    unordered_set* set = unordered_set_alloc(3,
        0.7f,
        int_hash_function,
        int_equals);

    puts("unordered_set_test_remove()");

    ASSERT(unordered_set_add(set, (void*) 1));
    ASSERT(unordered_set_add(set, (void*) 2));
    ASSERT(unordered_set_add(set, (void*) 3));
    ASSERT(3 == unordered_set_size(set));
    ASSERT(!unordered_set_add(set, (void*) 2));
    ASSERT(3 == unordered_set_size(set));

    ASSERT(unordered_set_remove(set, (void*) 2));
    ASSERT(!unordered_set_contains(set, (void*) 2));

    unordered_set_free(&set);
}

static void unordered_set_test_clear()
{
    unordered_set* set = unordered_set_alloc(3,
        0.7f,
        int_hash_function,
        int_equals);

    int i;

    puts("unordered_set_test_clear()");

    for (i = 0; i < 100; i++)
    {
        ASSERT((int) unordered_set_size(set) == i);
        unordered_set_add(set, (void*)(intptr_t) i);
    }

    unordered_set_clear(set);

    ASSERT(unordered_set_size(set) == 0);

    for (i = -100; i < 200; i++)
    {
        ASSERT(!unordered_set_contains(set, (void*)(intptr_t) i));
    }

    unordered_set_free(set);
}

static void unordered_set_test_iterator()
{
    unordered_set* set = unordered_set_alloc(
        5,
        0.6f,
        int_hash_function,
        int_equals);

    unordered_set_iterator* iterator;
    int i = 0;
    void* p;

    puts("        unordered_set_test_iterator()");

    for (i = 0; i < 100; i++)
    {
        unordered_set_add(set, (void*)(intptr_t) i);
    }

    iterator = unordered_set_iterator_alloc(set);

    for (i = 0; i < 100; i++)
    {
        ASSERT(unordered_set_iterator_has_next(iterator));
        ASSERT(unordered_set_contains(set, (void*)(intptr_t) i));
        ASSERT(unordered_set_iterator_next(iterator, &p));
        ASSERT(i == (intptr_t) p);
    }

    ASSERT(unordered_set_iterator_has_next(iterator) == FALSE);
}

void debug_mask()
{
    unordered_set set;
    unordered_set_init(&set,
        10,
        1.0f,
        int_hash_function,
        int_equals);

    int i = 0;

    for (i = 0; i < 100; i++)
    {
        unordered_set_add(&set, i);
    }
    unordered_set_remove(&set, 55);
    unordered_set_destroy(&set);
    ASSERT(0 == unordered_set_size(&set));
}

void unordered_set_test()
{
    puts("    unordered_set_test()");
    debug_mask();
    unordered_set_test_debug_clear();
    unordered_set_test_add();
    unordered_set_test_contains();
    unordered_set_test_remove();
    unordered_set_test_clear();
    unordered_set_test_iterator();
}
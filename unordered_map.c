#include "my_assert.h"
#include "unordered_map.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct unordered_map_entry {
    void*                       key;
    void*                       value;
    struct unordered_map_entry* chain_next;
    struct unordered_map_entry* prev;
    struct unordered_map_entry* next;
} unordered_map_entry;

typedef struct unordered_map_state {
    unordered_map_entry** table;
    unordered_map_entry*  head;
    unordered_map_entry*  tail;
    size_t (*hash_function)(void*);
    int (*equals_function)(void*, void*);
    size_t                mod_count;
    size_t                table_capacity;
    size_t                size;
    size_t                max_allowed_size;
    size_t                mask;
    float                 load_factor;
} unordered_map_state;

typedef struct unordered_map_iterator_state {
    unordered_map*       map;
    unordered_map_entry* next_entry;
    size_t               iterated_count;
    size_t               expected_mod_count;
} unordered_map_iterator_state;

/*typedef struct unordered_map_iterator {
    unordered_map*       map;
    unordered_map_entry* next_entry;
    size_t               iterated_count;
    size_t               expected_mod_count;
} unordered_map_iterator;*/

static unordered_map_entry* unordered_map_entry_alloc(void* key, void* value)
{
    unordered_map_entry* entry = malloc(sizeof(*entry));

    if (!entry)
    {
        return NULL;
    }

    entry->key = key;
    entry->value = value;
    entry->chain_next = NULL;
    entry->next = NULL;
    entry->prev = NULL;

    return entry;
}

static const float  MINIMUM_LOAD_FACTOR = 0.2f;
static const size_t MINIMUM_INITIAL_CAPACITY = 16;

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
    size_t ret;

    initial_capacity = maxi(initial_capacity, MINIMUM_INITIAL_CAPACITY);
    ret = 1;

    while (ret < initial_capacity)
    {
        ret <<= 1;
    }

    return ret;
}

unordered_map* unordered_map_alloc(size_t initial_capacity,
                                   float load_factor,
                                   size_t (*hash_function)(void*),
                                   int (*equals_function)(void*, void*))
{
    unordered_map* map;

    if (!hash_function || !equals_function)
    {
        return NULL;
    }

    map = malloc(sizeof(*map));

    if (!map)
    {
        return NULL;
    }

    map->state = malloc(sizeof(*map->state));

    load_factor = fix_load_factor(load_factor);
    initial_capacity = fix_initial_capacity(initial_capacity);

    map->state->load_factor = load_factor;
    map->state->table_capacity = initial_capacity;
    map->state->size = 0;
    map->state->mod_count = 0;
    map->state->head = NULL;
    map->state->tail = NULL;
    map->state->table = calloc(initial_capacity,
        sizeof(unordered_map_entry*));

    map->state->hash_function = hash_function;
    map->state->equals_function = equals_function;
    map->state->mask = initial_capacity - 1;
    map->state->max_allowed_size = (size_t)(initial_capacity * load_factor);

    return map;
}

static void ensure_capacity(unordered_map* map)
{
    size_t new_capacity;
    size_t new_mask;
    size_t index;
    unordered_map_entry* entry;
    unordered_map_entry** new_table;

    if (map->state->size < map->state->max_allowed_size)
    {
        return;
    }

    new_capacity = 2 * map->state->table_capacity;
    new_mask = new_capacity - 1;
    new_table = calloc(new_capacity, sizeof(unordered_map_entry*));

    if (!new_table)
    {
        return;
    }

    /* Rehash the entries. */
    for (entry = map->state->head; entry; entry = entry->next)
    {
        index = map->state->hash_function(entry->key) & new_mask;
        entry->chain_next = new_table[index];
        new_table[index] = entry;
    }

    free(map->state->table);

    map->state->table = new_table;
    map->state->table_capacity = new_capacity;
    map->state->mask = new_mask;
    map->state->max_allowed_size = (size_t)(new_capacity * map->state->load_factor);
}

void* unordered_map_put(unordered_map* map, void* key, void* value)
{
    size_t index;
    size_t hash_value;
    void* old_value;
    unordered_map_entry* entry;

    if (!map)
    {
        return NULL;
    }

    hash_value = map->state->hash_function(key);
    index = hash_value & map->state->mask;

    for (entry = map->state->table[index]; entry; entry = entry->chain_next)
    {
        if (map->state->equals_function(entry->key, key))
        {
            old_value = entry->value;
            entry->value = value;
            return old_value;
        }
    }

    ensure_capacity(map);

    /* Recompute the index since it is possibly changed by 'ensure_capacity' */
    index = hash_value & map->state->mask;
    entry = unordered_map_entry_alloc(key, value);
    entry->chain_next = map->state->table[index];
    map->state->table[index] = entry;

    /* Link the new entry to the tail of the list. */
    if (!map->state->tail)
    {
        map->state->head = entry;
        map->state->tail = entry;
    }
    else
    {
        map->state->tail->next = entry;
        entry->prev = map->state->tail;
        map->state->tail = entry;
    }

    map->state->size++;
    map->state->mod_count++;

    return NULL;
}

int unordered_map_contains_key(unordered_map* map, void* key)
{
    size_t index;
    unordered_map_entry* entry;

    if (!map)
    {
        return false;
    }

    index = map->state->hash_function(key) & map->state->mask;

    for (entry = map->state->table[index]; entry; entry = entry->chain_next)
    {
        if (map->state->equals_function(key, entry->key))
        {
            return true;
        }
    }

    return false;
}

void* unordered_map_get(unordered_map* map, void* key)
{
    size_t index;
    unordered_map_entry* p_entry;

    if (!map)
    {
        return NULL;
    }

    index = map->state->hash_function(key) & map->state->mask;

    for (p_entry = map->state->table[index]; p_entry; p_entry = p_entry->chain_next)
    {
        if (map->state->equals_function(key, p_entry->key))
        {
            return p_entry->value;
        }
    }

    return NULL;
}

void* unordered_map_remove(unordered_map* map, void* key)
{
    void*  value;
    size_t index;
    unordered_map_entry* prev_entry;
    unordered_map_entry* current_entry;

    if (!map)
    {
        return NULL;
    }

    index = map->state->hash_function(key) & map->state->mask;

    prev_entry = NULL;

    for (current_entry = map->state->table[index];
        current_entry;
        current_entry = current_entry->chain_next)
    {
        if (map->state->equals_function(key, current_entry->key))
        {
            if (prev_entry)
            {
                /* Omit the 'p_current_entry' in the collision chain. */
                prev_entry->chain_next = current_entry->chain_next;
            }
            else
            {
                map->state->table[index] = current_entry->chain_next;
            }

            /* Unlink from the global iteration chain. */
            if (current_entry->prev)
            {
                current_entry->prev->next = current_entry->next;
            }
            else
            {
                map->state->head = current_entry->next;
            }

            if (current_entry->next)
            {
                current_entry->next->prev = current_entry->prev;
            }
            else
            {
                map->state->tail = current_entry->prev;
            }

            value = current_entry->value;
            map->state->size--;
            map->state->mod_count++;
            free(current_entry);
            return value;
        }

        prev_entry = current_entry;
    }

    return NULL;
}

void unordered_map_clear(unordered_map* map)
{
    unordered_map_entry* entry;
    unordered_map_entry* next_entry;
    size_t index;

    if (!map)
    {
        return;
    }

    entry = map->state->head;

    while (entry)
    {
        index = map->state->hash_function(entry->key) & map->state->mask;
        next_entry = entry->next;
        free(entry);
        entry = next_entry;
        map->state->table[index] = NULL;
    }

    map->state->mod_count += map->state->size;
    map->state->size = 0;
    map->state->head = NULL;
    map->state->tail = NULL;
}

size_t unordered_map_size(unordered_map* map)
{
    return map ? map->state->size : 0;
}

int unordered_map_is_healthy(unordered_map* map)
{
    size_t counter;
    unordered_map_entry* entry;
    
    if (!map)
    {
        return false;
    }

    counter = 0;
    entry = map->state->head;

    if (entry && entry->prev)
    {
        return false;
    }

    for (; entry; entry = entry->next)
    {
        counter++;
    }

    return counter == map->state->size;
}

void unordered_map_free(unordered_map* map)
{
    if (!map)
    {
        return;
    }

    unordered_map_clear(map);
    free(map->state->table);
    free(map);
}

unordered_map_iterator*
unordered_map_iterator_alloc(unordered_map* map)
{
    unordered_map_iterator* p_ret;

    if (!map)
    {
        return NULL;
    }

    p_ret = malloc(sizeof(*p_ret));

    if (!p_ret)
    {
        return NULL;
    }

    p_ret->state = malloc(sizeof(*p_ret->state));

    if (!p_ret->state)
    {
        free(p_ret);
        return NULL;
    }

    p_ret->state->map = map;
    p_ret->state->iterated_count = 0;
    p_ret->state->next_entry = map->state->head;
    p_ret->state->expected_mod_count = map->state->mod_count;

    return p_ret;
}

int unordered_map_iterator_has_next(unordered_map_iterator* iterator)
{
    if (!iterator)
    {
        return 0;
    }

    if (unordered_map_iterator_is_disturbed(iterator))
    {
        return 0;
    }

    return iterator->state->map->state->size - iterator->state->iterated_count;
}

int unordered_map_iterator_next(unordered_map_iterator* iterator,
    void** key_pointer,
    void** value_pointer)
{
    if (!iterator)
    {
        return false;
    }

    if (!iterator->state->next_entry)
    {
        return false;
    }

    if (unordered_map_iterator_is_disturbed(iterator))
    {
        return false;
    }

    *key_pointer = iterator->state->next_entry->key;
    *value_pointer = iterator->state->next_entry->value;
    iterator->state->iterated_count++;
    iterator->state->next_entry = iterator->state->next_entry->next;

    return true;
}

int unordered_map_iterator_is_disturbed(unordered_map_iterator* iterator)
{
    if (!iterator || !iterator->state)
    {
        false;
    }

    return iterator->state->expected_mod_count != iterator->state->map->state->mod_count;
}

void unordered_map_iterator_free(unordered_map_iterator* iterator)
{
    if (!iterator || !iterator->state)
    {
        return;
    }

    iterator->state->map = NULL;
    iterator->state->next_entry = NULL;
    free(iterator);
}

static int int_equals(void* a, void* b)
{
    int ia = (int) a;
    int ib = (int) b;
    return ia == ib;
}

static size_t int_hash_function(void* i)
{
    return (size_t) i;
}

static void unordered_map_test_put()
{
    unordered_map* map = unordered_map_alloc(3,
                                             1.0f,
                                             int_hash_function, 
                                             int_equals);
    int i;
    puts("        unordered_map_test_put()");

    for (i = 0; i < 30; i++)
    {
        ASSERT(unordered_map_is_healthy(map));
        ASSERT(unordered_map_size(map) == i);
        unordered_map_put(map, (void*) i, (void*)(2 * i));
        ASSERT(unordered_map_size(map) == i + 1);
    }

    ASSERT(unordered_map_is_healthy(map));

    for (i = 0; i < 30; i++)
    {
        ASSERT(unordered_map_is_healthy(map));
        ASSERT((int) unordered_map_get(map, (void*) i) == 2 * i);
    }

    ASSERT(unordered_map_is_healthy(map));
    unordered_map_free(map);
}

static void unordered_map_test_get()
{
    unordered_map* map = unordered_map_alloc(5,
                                             0.3f,
                                             int_hash_function,
                                             int_equals);
    int i;
    puts("        unordered_map_test_get()");

    for (i = 0; i < 30; i++)
    {
        ASSERT(unordered_map_is_healthy(map));
        unordered_map_put(map, (void*) i, (void*)(2 * i));
    }

    ASSERT(unordered_map_is_healthy(map));

    for (i = 0; i < 30; i++)
    {
        ASSERT((int) unordered_map_get(map, (void*) i) == i * 2);
        ASSERT(unordered_map_is_healthy(map));
    }

    unordered_map_free(map);
}

static void unordered_map_test_contains_key()
{
    unordered_map* map = unordered_map_alloc(4,
                                             0.7f,
                                             int_hash_function,
                                             int_equals);
    
    puts("        unordered_map_test_contains_key()");

    unordered_map_put(map, (void*) -2, (void*) 11);
    unordered_map_put(map, (void*) -1, (void*) 12);
    unordered_map_put(map, (void*) 0, (void*)  13);
    unordered_map_put(map, (void*) 1, (void*)  14);
    unordered_map_put(map, (void*) 2, (void*)  15);

    ASSERT(unordered_map_is_healthy(map));
    ASSERT(unordered_map_contains_key(map, (void*) -2));
    ASSERT(unordered_map_contains_key(map, (void*) -1));
    ASSERT(unordered_map_contains_key(map, (void*) 0));
    ASSERT(unordered_map_contains_key(map, (void*) 1));
    ASSERT(unordered_map_contains_key(map, (void*) 2));
    ASSERT(unordered_map_size(map) == 5);

    ASSERT(!unordered_map_contains_key(map, (void*) -3));
    ASSERT(!unordered_map_contains_key(map, (void*) 3));

    unordered_map_free(map);
}

static void unordered_map_test_remove()
{
    unordered_map* map = unordered_map_alloc(2,
                                             1.2f,
                                             int_hash_function,
                                             int_equals);
    int i;

    puts("        unordered_map_test_remove()");

    for (i = 0; i < 100; i++)
    {
        unordered_map_put(map, i, 3 * i);
    }

    unordered_map_is_healthy(map);
    ASSERT(unordered_map_size(map) == 100);

    ASSERT((int) unordered_map_remove(map, 8));
    ASSERT((int) unordered_map_remove(map, 26));
    ASSERT((int) unordered_map_remove(map, 29));
    ASSERT(unordered_map_size(map) == 97);
    ASSERT(unordered_map_is_healthy(map));

    unordered_map_free(map);
}

static void unordered_map_test_clear()
{
    unordered_map* map = unordered_map_alloc(4, 0.4f, int_hash_function, int_equals);
    int i;

    puts("        unordered_map_test_clear()");

    for (i = 0; i < 100; i++) {
        ASSERT(unordered_map_size(map) == i);
        ASSERT(unordered_map_put(map, i, i) == NULL);
        ASSERT(unordered_map_is_healthy(map));
    }

    for (i = 0; i < 100; i++)
    {
        ASSERT(unordered_map_contains_key(map, (void*) i));
    }

    unordered_map_clear(map);
    ASSERT(unordered_map_size(map) == 0);
    unordered_map_free(map);
}

static void unordered_map_test_iterator()
{
    unordered_map* map = unordered_map_alloc(4, 0.4f, int_hash_function, int_equals);
    int i;
    int key;
    int value;

    puts("        unordered_map_test_iterator()");

    for (i = 0; i < 10; i++)
    {
        unordered_map_put(map, i, i + 100);
    }

    unordered_map_iterator* iter = unordered_map_iterator_alloc(map);

    ASSERT(unordered_map_iterator_has_next(iter));
    ASSERT(unordered_map_iterator_next(iter, (void*) &key, (void*) &value));
    ASSERT(key == 0);
    ASSERT(value == 100);

    ASSERT(!unordered_map_iterator_is_disturbed(iter));
    
    ASSERT(unordered_map_iterator_has_next(iter));
    ASSERT(unordered_map_iterator_next(iter, (void*) &key, (void*) &value));
    ASSERT(key == 1);
    ASSERT(value = 101);
    ASSERT(unordered_map_is_healthy(map));

    unordered_map_get(map, 0);

    ASSERT(unordered_map_is_healthy(map));
    ASSERT(!unordered_map_iterator_is_disturbed(iter));
    ASSERT(101 == (int) unordered_map_remove(map, 1));
    ASSERT(unordered_map_iterator_is_disturbed(iter));
    ASSERT(unordered_map_is_healthy(map));

    unordered_map_iterator_free(iter);
    unordered_map_free(map);
}

void unordered_map_test()
{
    puts("    unordered_map_test()");
    unordered_map_test_put();
    unordered_map_test_get();
    unordered_map_test_contains_key();
    unordered_map_test_remove();
    unordered_map_test_clear();
    unordered_map_test_iterator();
}
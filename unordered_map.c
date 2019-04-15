#include "my_assert.h"
#include "unordered_map.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

static unordered_map_entry* unordered_map_entry_alloc(void* key, void* value)
{
    unordered_map_entry* entry = malloc(sizeof(*entry));

    if (!entry)
    {
        return NULL;
    }

    entry->key = key;
    entry->value = value;
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

int unordered_map_init(unordered_map* p_memory,
                        size_t initial_capacity,
                        float load_factor,
                        size_t (*p_hash_function)(void*),
                        int (*p_equals_function)(void*, void*))
{
    if (!p_memory || !p_hash_function || !p_equals_function)
    {
        return FALSE;
    }

    initial_capacity = fix_initial_capacity(initial_capacity);
    load_factor = fix_load_factor(load_factor);
    p_memory->hash_function = p_hash_function;
    p_memory->equals_function = p_equals_function;
    p_memory->table_capacity = initial_capacity;
    p_memory->load_factor = load_factor;
    p_memory->head = NULL;
    p_memory->tail = NULL;
    p_memory->size = 0;
    p_memory->mod_count = 0;
    p_memory->table = 
        calloc(p_memory->table_capacity, 
               sizeof(p_memory->table[0]));

    if (!p_memory->table)
    {
        return FALSE;
    }

    p_memory->mask = p_memory->table_capacity - 1;
    p_memory->max_allowed_size = 
        (size_t)(initial_capacity * load_factor);

    return TRUE;
}

unordered_map* unordered_map_alloc(size_t initial_capacity,
                                   float load_factor,
                                   size_t (*p_hash_function)(void*),
                                   int (*p_equals_function)(void*, void*))
{
    unordered_map* map = malloc(sizeof(*map));
    int initialization_succeeded;

    if (!map || !p_hash_function || !p_equals_function)
    {
        return NULL;
    }

    initialization_succeeded = 
        unordered_map_init(map,
                           initial_capacity,
                           load_factor,
                           p_hash_function,
                           p_equals_function);

    if (!initialization_succeeded)
    {
        free(map);
        return NULL;
    }

    return map;
}

static void ensure_capacity(unordered_map* map)
{
    size_t new_capacity;
    size_t new_mask;
    size_t index;
    unordered_map_entry* entry;
    unordered_map_entry** new_table;

    if (map->size < map->max_allowed_size)
    {
        return;
    }

    new_capacity = map->table_capacity << 1;
    new_mask = new_capacity - 1;
    new_table = calloc(new_capacity, sizeof(unordered_map_entry*));

    if (!new_table)
    {
        return;
    }

    /* Rehash the entries. */
    for (entry = map->head; entry; entry = entry->next)
    {
        index = entry->key_hash_value & new_mask;
        entry->chain_next = new_table[index];
        new_table[index] = entry;
    }

    free(map->table);

    map->table          = new_table;
    map->table_capacity = new_capacity;
    map->mask           = new_mask;
    map->max_allowed_size = (size_t)(new_capacity * map->load_factor);
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

    ensure_capacity(map); /* Might update map->mask. */
    hash_value = map->hash_function(key);
    index = hash_value & map->mask;

    for (entry = map->table[index]; entry; entry = entry->chain_next)
    {
        if (entry->key_hash_value == hash_value
            && map->equals_function(entry->key, key))
        {
            old_value = entry->value;
            entry->value = value;
            return old_value;
        }
    }

    entry = unordered_map_entry_alloc(key, value);
    entry->key_hash_value = hash_value;
    entry->chain_next = map->table[index];
    map->table[index] = entry;

    /* Link the new entry to the tail of the list. */
    if (!map->tail)
    {
        map->head = entry;
        map->tail = entry;
        entry->prev = NULL;
    }
    else
    {
        map->tail->next = entry;
        entry->prev = map->tail;
        map->tail = entry;
    }

    entry->next = NULL;
    map->size++;
    map->mod_count++;
    return NULL;
}

int unordered_map_contains_key(unordered_map* map, void* key)
{
    size_t index;
    size_t hash_value;
    unordered_map_entry* p_entry;

    if (!map)
    {
        return FALSE;
    }

    hash_value = map->hash_function(key);
    index = hash_value & map->mask;

    for (p_entry = map->table[index]; p_entry; p_entry = p_entry->chain_next)
    {
        if (p_entry->key_hash_value == hash_value
            && map->equals_function(key, p_entry->key))
        {
            return TRUE;
        }
    }

    return FALSE;
}

void* unordered_map_get(unordered_map* map, void* key)
{
    size_t index;
    size_t hash_value;
    unordered_map_entry* p_entry;

    if (!map)
    {
        return NULL;
    }

    hash_value = map->hash_function(key);
    index = hash_value & map->mask;

    for (p_entry = map->table[index]; p_entry; p_entry = p_entry->chain_next)
    {
        if (p_entry->key_hash_value == hash_value
            && map->equals_function(key, p_entry->key))
        {
            return p_entry->value;
        }
    }

    return NULL;
}

static void unlink_from_collision_chain(unordered_map* map,
    unordered_map_entry* prev_entry,
    unordered_map_entry* entry_to_unlink)
{
    size_t index;

    if (prev_entry)
    {
        prev_entry->chain_next = entry_to_unlink->chain_next;
    }
    else
    {
        index = entry_to_unlink->key_hash_value & map->mask;
        map->table[index] = entry_to_unlink->chain_next;
    }
}

static void unlink_from_predecessor_entry(unordered_map* map,
                                          unordered_map_entry* entry)
{
    unordered_map_entry* prev_entry = entry->prev;

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
        map->head = entry->next;

        if (map->head)
        {
            map->head->prev = NULL;
        }
    }
}

static void unlink_from_successor_entry(unordered_map* map,
                                        unordered_map_entry* entry)
{
    unordered_map_entry* next_entry = entry->next;

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
        map->tail = entry->prev;

        if (map->tail)
        {
            map->tail->next = NULL;
        }
    }
}

void* unordered_map_remove(unordered_map* map, void* key)
{
    void*  value;
    size_t index;
    size_t hash_value;
    unordered_map_entry* prev_entry;
    unordered_map_entry* current_entry;

    if (!map)
    {
        return NULL;
    }

    hash_value = map->hash_function(key);
    index = hash_value & map->mask;

    for (current_entry = map->table[index], prev_entry = NULL;
        current_entry;
        prev_entry = current_entry,
        current_entry = current_entry->chain_next)
    {
        if (hash_value == current_entry->key_hash_value
            && map->equals_function(key, current_entry->key))
        {
            value = current_entry->value;
            unlink_from_collision_chain(map, prev_entry, current_entry);
            unlink_from_predecessor_entry(map, current_entry);
            unlink_from_successor_entry(map, current_entry);
            map->size--;
            map->mod_count++;
            free(current_entry);
            return value;
        }
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

    entry = map->head;

    while (entry)
    {
        index = entry->key_hash_value & map->mask;
        next_entry = entry->next;
        free(entry);
        entry = next_entry;
        map->table[index] = NULL;
    }

    map->mod_count++;
    map->size = 0;
    map->head = NULL;
    map->tail = NULL;
}

size_t unordered_map_size(unordered_map* map)
{
    return map ? map->size : 0;
}

int unordered_map_is_healthy(unordered_map* map)
{
    size_t counter;
    size_t i;
    size_t hash_value_1;
    size_t hash_value_2;
    unordered_map_entry* entry;
    
    if (!map)
    {
        return FALSE;
    }

    counter = 0;
    entry = map->head;

    if (entry && entry->prev)
    {
        return FALSE;
    }

    for (; entry; entry = entry->next)
    {
        counter++;
    }

    if (counter != map->size)
    {
        return FALSE;
    }

    for (i = 0; i < map->table_capacity; i++)
    {
        entry = map->table[i];

        while (entry)
        {
            hash_value_1 = entry->key_hash_value;
            hash_value_2 = map->hash_function(entry->key);

            if (hash_value_1 != hash_value_2)
            {
                return FALSE;
            }

            entry = entry->chain_next;
        }
    }

    return TRUE;
}

void unordered_map_destroy(unordered_map* map)
{
    if (!map)
    {
        return;
    }

    unordered_map_clear(map);
    free(map->table);
    map->table = NULL;
}

void unordered_map_free(unordered_map** p_map)
{
    if (!*p_map)
    {
        return;
    }

    unordered_map_destroy(*p_map);
    free(*p_map);
    *p_map = NULL;
}

void unordered_map_iterator_init(unordered_map* map,
                                 unordered_map_iterator* iterator)
{
    iterator->map                = map;
    iterator->iterated_count     = 0;
    iterator->next_entry         = map->head;
    iterator->expected_mod_count = map->mod_count;
}

unordered_map_iterator*
unordered_map_iterator_alloc(unordered_map* map)
{
    unordered_map_iterator* iterator;

    if (!map)
    {
        return NULL;
    }

    iterator = (unordered_map_iterator*)
                malloc(sizeof(*iterator));

    if (!iterator)
    {
        return NULL;
    }

    unordered_map_iterator_init(map, iterator);
    return iterator;
}

size_t unordered_map_iterator_has_next(unordered_map_iterator* iterator)
{
    if (!iterator)
    {
        return 0;
    }

    if (unordered_map_iterator_is_disturbed(iterator))
    {
        return 0;
    }

    return iterator->map->size - iterator->iterated_count;
}

int unordered_map_iterator_next(unordered_map_iterator* iterator,
    void** key_pointer,
    void** value_pointer)
{
    if (!iterator)
    {
        return FALSE;
    }

    if (!iterator->next_entry)
    {
        return FALSE;
    }

    if (unordered_map_iterator_is_disturbed(iterator))
    {
        return FALSE;
    }

    *key_pointer = iterator->next_entry->key;
    *value_pointer = iterator->next_entry->value;
    iterator->iterated_count++;
    iterator->next_entry = iterator->next_entry->next;

    return TRUE;
}

int unordered_map_iterator_is_disturbed(unordered_map_iterator* iterator)
{
    if (!iterator)
    {
        return FALSE;
    }

    return iterator->expected_mod_count != iterator->map->mod_count;
}

void unordered_map_iterator_free(unordered_map_iterator* iterator)
{
    if (!iterator)
    {
        return;
    }

    iterator->map = NULL;
    iterator->next_entry = NULL;
    free(iterator);
}

static int int_equals(void* a, void* b)
{
    int ia = (intptr_t) a;
    int ib = (intptr_t) b;
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
        unordered_map_put(map, (void*)(intptr_t) i, (void*)(intptr_t)(2 * i));
        ASSERT(unordered_map_size(map) == i + 1);
    }

    ASSERT(unordered_map_is_healthy(map));

    for (i = 0; i < 30; i++)
    {
        ASSERT(unordered_map_is_healthy(map));
        ASSERT((intptr_t) unordered_map_get(map, (void*)(intptr_t) i) == 2 * i);
    }

    ASSERT(unordered_map_is_healthy(map));
    unordered_map_free(&map);
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
        unordered_map_put(map, (void*)(intptr_t) i, (void*)(intptr_t)(2 * i));
    }

    ASSERT(unordered_map_is_healthy(map));

    for (i = 0; i < 30; i++)
    {
        ASSERT((intptr_t) unordered_map_get(map, (void*)(intptr_t) i) == i * 2);
        ASSERT(unordered_map_is_healthy(map));
    }

    unordered_map_free(&map);
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
    ASSERT(unordered_map_contains_key(map, (void*)-1));  
    ASSERT(unordered_map_contains_key(map, (void*) 0));
    ASSERT(unordered_map_contains_key(map, (void*) 1));
    ASSERT(unordered_map_contains_key(map, (void*) 2));
    ASSERT(unordered_map_size(map) == 5);

    ASSERT(!unordered_map_contains_key(map, (void*) -3));
    ASSERT(!unordered_map_contains_key(map, (void*) 3));

    unordered_map_free(&map);
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
        unordered_map_put(map, (void*)(intptr_t) i, (void*)(intptr_t)(3 * i));
    }

    unordered_map_is_healthy(map);
    ASSERT(unordered_map_size(map) == 100);
    ASSERT(unordered_map_remove(map, (void*)(intptr_t) 8)  == (void*)(  8 * 3));
    ASSERT(unordered_map_remove(map, (void*)(intptr_t) 26) == (void*)( 26 * 3));
    ASSERT(unordered_map_remove(map, (void*)(intptr_t) 29) == (void*)( 29 * 3));
    ASSERT(unordered_map_size(map) == 97);
    ASSERT(unordered_map_is_healthy(map));

    unordered_map_free(&map);
}

static void unordered_map_test_clear()
{
    unordered_map* map = unordered_map_alloc(4, 0.4f, int_hash_function, int_equals);
    int i;

    puts("        unordered_map_test_clear()");

    for (i = 0; i < 100; i++) {
        ASSERT(unordered_map_size(map) == i);
        ASSERT(unordered_map_put(map,
                                 (void*)(intptr_t) i, 
                                 (void*)(intptr_t) i) == NULL);
        
        ASSERT(unordered_map_is_healthy(map));
    }

    for (i = 0; i < 100; i++)
    {
        ASSERT(unordered_map_contains_key(map, (void*)(intptr_t) i));
    }

    unordered_map_clear(map);
    ASSERT(unordered_map_size(map) == 0);
    unordered_map_free(&map);
}

static void unordered_map_test_iterator()
{
    unordered_map* map = unordered_map_alloc(4,
                                             0.4f, 
                                             int_hash_function, 
                                             int_equals);
    unordered_map_iterator* iter;
    int i;
    int key;
    int value;

    puts("        unordered_map_test_iterator()");

    for (i = 0; i < 10; i++)
    {
        unordered_map_put(map, (void*)(intptr_t) i, (void*)(intptr_t)(i + 100));
    }

    iter = unordered_map_iterator_alloc(map);

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
    ASSERT(101 == (intptr_t) unordered_map_remove(map, (void*) 1));
    ASSERT(unordered_map_iterator_is_disturbed(iter));
    ASSERT(unordered_map_is_healthy(map));

    unordered_map_iterator_free(iter);
    unordered_map_free(&map);
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
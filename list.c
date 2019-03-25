#include "list.h"
#include "my_assert.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct list_state {
    void** storage;
    size_t size;
    size_t capacity;
    size_t head;
    size_t mask;
} list_state;

static const size_t MINIMUM_CAPACITY = 16;

static size_t sz_max(size_t a, size_t b)
{
    return a > b ? a : b;
}

static int fix_initial_capacity(size_t initial_capacity)
{
    size_t ret = 1;

    initial_capacity = sz_max(initial_capacity, MINIMUM_CAPACITY);

    while (ret < initial_capacity)
    {
        ret <<= 1;
    }

    return ret;
}

list* list_alloc(size_t initial_capacity)
{
    list* my_list = malloc(sizeof(*my_list));
    list_state* my_list_state;

    if (!my_list)
    {
        return NULL;
    }

    my_list_state = malloc(sizeof(*my_list_state));

    if (!my_list_state)
    {
        free(my_list);
        return NULL;
    }

    my_list->state = my_list_state;
    initial_capacity = fix_initial_capacity(initial_capacity);

    my_list->state->storage = malloc(sizeof(void*) * initial_capacity);

    if (!my_list->state->storage)
    {
        free(my_list);
        return NULL;
    }

    my_list->state->capacity = initial_capacity;
    my_list->state->mask = initial_capacity - 1;
    my_list->state->head = 0;
    my_list->state->size = 0;

    return my_list;
}

static int ensure_capacity_before_add(list* my_list)
{
    void** new_table;
    size_t i;
    size_t new_capacity;

    if (my_list->state->size < my_list->state->capacity)
    {
        return TRUE;
    }

    new_capacity = 2 * my_list->state->capacity;
    new_table = malloc(sizeof(void*) * new_capacity);

    if (!new_table)
    {
        return TRUE;
    }

    for (i = 0; i < my_list->state->size; ++i)
    {
        new_table[i] =
            my_list->state->storage[
                (my_list->state->head + i) & my_list->state->mask];
    }

    free(my_list->state->storage);

    my_list->state->storage = new_table;
    my_list->state->capacity = new_capacity;
    my_list->state->mask = new_capacity - 1;
    my_list->state->head = 0;

    return TRUE;
}

int list_push_front(list* my_list, void* element)
{
    if (!my_list)
    {
        return FALSE;
    }

    if (!ensure_capacity_before_add(my_list))
    {
        return FALSE;
    }

    my_list->state->head =
        (my_list->state->head - 1) & my_list->state->mask;

    my_list->state->storage[my_list->state->head] = element;
    my_list->state->size++;

    return TRUE;
}

int list_push_back(list* my_list, void* element)
{
    if (!my_list)
    {
        return FALSE;
    }

    if (!ensure_capacity_before_add(my_list))
    {
        return FALSE;
    }

    my_list->state->storage[(my_list->state->head +
        my_list->state->size) &
        my_list->state->mask] = element;
    my_list->state->size++;
    return TRUE;
}

int list_insert(list* my_list, size_t index, void* element)
{
    size_t elements_before;
    size_t elements_after;
    size_t i;
    size_t head;
    size_t mask;
    size_t size;

    if (!my_list)
    {
        return FALSE;
    }

    if (!ensure_capacity_before_add(my_list))
    {
        return FALSE;
    }

    if (index > my_list->state->size)
    {
        return FALSE;
    }

    elements_before = index;
    elements_after = my_list->state->size - index;
    head = my_list->state->head;
    mask = my_list->state->mask;
    size = my_list->state->size;

    if (elements_before < elements_after)
    {
        /* Move preceding elements one position to the left. */
        for (i = 0; i < elements_before; ++i)
        {
            my_list->state->storage[(head + i - 1) & mask] =
                my_list->state->storage[(head + i) & mask];
        }

        head = (head - 1) & mask;
        my_list->state->storage[(head + index) & mask] = element;
        my_list->state->head = head;
    }
    else
    {
        /* Move the following elements one position to the right. */
        for (i = 0; i < elements_after; ++i)
        {
            my_list->state->storage[(head + size - i) & mask] =
                my_list->state->storage[(head + size - i - 1) & mask];
        }

        my_list->state->storage[(head + index) & mask] = element;
    }

    my_list->state->size++;
    return TRUE;
}

size_t list_size(list* my_list)
{
    return my_list ? my_list->state->size : 0;
}

void* list_get(list* my_list, size_t index)
{
    if (!my_list)
    {
        return NULL;
    }

    if (index >= my_list->state->size)
    {
        return NULL;
    }

    return my_list->state->storage[
          (my_list->state->head + index) &
           my_list->state->mask];
}

void* list_set(list* my_list, size_t index, void* new_value)
{
    void* old_value;

    if (!my_list)
    {
        return NULL;
    }

    if (index >= my_list->state->size)
    {
        return NULL;
    }

    old_value = my_list->state->storage[
        (my_list->state->head + index) &
            my_list->state->mask];

    my_list->state->storage[
        (my_list->state->head + index) &
            my_list->state->mask] = new_value;

    return old_value;
}

void* list_pop_front(list* my_list)
{
    void* front;

    if (!my_list)
    {
        return NULL;
    }

    if (my_list->state->size == 0)
    {
        return NULL;
    }

    front = my_list->state->storage[my_list->state->head];
    my_list->state->head = (my_list->state->head + 1) & my_list->state->mask;
    my_list->state->size--;
    return front;
}

void* list_pop_back(list* my_list)
{
    void* back;

    if (!my_list)
    {
        return NULL;
    }

    if (my_list->state->size == 0)
    {
        return NULL;
    }

    back = my_list->state->storage[
        (my_list->state->head + my_list->state->size - 1) &
            my_list->state->mask];

    my_list->state->size--;
    return back;
}

void* list_remove_at(list* my_list, size_t index)
{
    void* value;
    size_t head;
    size_t mask;
    size_t elements_before;
    size_t elements_after;
    size_t i;
    size_t j;

    if (!my_list)
    {
        return NULL;
    }

    if (index >= my_list->state->size)
    {
        return NULL;
    }

    head = my_list->state->head;
    mask = my_list->state->mask;

    value = my_list->state->storage[(head + index) & mask];

    elements_before = index;
    elements_after = my_list->state->size - index - 1;

    if (elements_before < elements_after)
    {
        /* Move the preceding elements one position to the right. */
        for (j = elements_before; j > 0; --j)
        {
            my_list->state->storage[(head + j) & mask] =
                my_list->state->storage[(head + j - 1) & mask];
        }

        my_list->state->head = (head + 1) & mask;
    }
    else
    {
        /* Move the following elements one position to the left. */
        for (i = 0; i < elements_after; ++i)
        {
            my_list->state->storage[(head + index + i) & mask] =
                my_list->state->storage[(head + index + i + 1) & mask];
        }
    }

    my_list->state->size--;
    return value;
}

int list_contains(list* my_list,
    void* element,
    int (*equals_function)(void*, void*))
{
    size_t i;

    if (!my_list)
    {
        return FALSE;
    }

    if (!equals_function)
    {
        return FALSE;
    }

    for (i = 0; i < my_list->state->size; ++i)
    {
        if (equals_function(element,
            my_list->state->storage[(my_list->state->head + i) &
            my_list->state->mask]))
        {
            return TRUE;
        }
    }

    return FALSE;
}

int list_equals(list* lst_1, list* lst_2, int (*equals)(void*, void*))
{
    size_t i;

    if (lst_1 == lst_2)
    {
        return TRUE;
    }

    if (!lst_1)
    {
        return FALSE;
    }

    if (!lst_2)
    {
        return FALSE;
    }

    if (list_size(lst_1) != list_size(lst_2))
    {
        return FALSE;
    }

    for (i = 0; i != list_size(lst_1); i++)
    {
        if (!equals(list_get(lst_1, i), list_get(lst_2, i)))
        {
            return FALSE;
        }
    }

    return TRUE;
}

void list_clear(list* my_list)
{
    if (!my_list)
    {
        return;
    }

    my_list->state->head = 0;
    my_list->state->size = 0;
}

void list_free(list* my_list)
{
    if (!my_list)
    {
        return;
    }

    free(my_list->state->storage);
    free(my_list->state);
    free(my_list);
}

static int equals(void* a, void* b)
{
    int ia = (intptr_t) a;
    int ib = (intptr_t) b;
    return ia == ib;
}

static void list_test_push_front_pop_front()
{
    list* lst = list_alloc(3);
    int i;

    puts("        list_test_push_front_pop_front()");

    ASSERT(list_size(lst) == 0);

    for (i = 0; i < 100; i++)
    {
        ASSERT(i == list_size(lst));
        ASSERT(list_push_front(lst, (void*)(intptr_t) i));
        ASSERT(i + 1 == list_size(lst));
    }

    for (i = 0; i < 100; i++)
    {
        ASSERT(100 - i == list_size(lst));
        ASSERT(99 - i == (intptr_t) list_pop_front(lst));
        ASSERT(99 - i == list_size(lst));
    }

    ASSERT(0 == list_size(lst));
    list_free(lst);
}

static void list_test_push_back_pop_back()
{
    list* lst = list_alloc(3);
    int i;

    puts("        list_test_push_back_pop_back()");

    for (i = 0; i < 100; i++)
    {
        ASSERT(i == list_size(lst));
        ASSERT(list_push_back(lst, (void*)(intptr_t) i));
        ASSERT(i + 1 == list_size(lst));
    }

    for (i = 0; i < 100; i++)
    {
        ASSERT(99 - i == (intptr_t) list_pop_back(lst));
        ASSERT(99 - i == list_size(lst));
    }

    ASSERT(0 == list_size(lst));
    list_free(lst);
}

static void list_test_insert()
{
    list* lst = list_alloc(3);

    puts("        list_test_insert()");

    ASSERT(list_insert(lst, 0, (void*)(intptr_t) 0));   /* < 0 > */
    ASSERT(list_insert(lst, 0, (void*)(intptr_t) -1));  /* < -1, 0 > */
    ASSERT(list_insert(lst, 2, (void*)(intptr_t) 3));   /* < -1, 0, 3 > */
    ASSERT(list_insert(lst, 1, (void*)(intptr_t) 4));   /* < -1, 4, 0,  3 > */
    ASSERT(list_insert(lst, 2, (void*)(intptr_t) 8));   /* < -1, 4, 8, 0, 3 > */

    ASSERT(-1 == (intptr_t) list_get(lst, 0));
    ASSERT(4  == (intptr_t) list_get(lst, 1));
    ASSERT(8  == (intptr_t) list_get(lst, 2));
    ASSERT(0  == (intptr_t) list_get(lst, 3));
    ASSERT(3  == (intptr_t) list_get(lst, 4));

    ASSERT(list_insert(lst, 0, (void*)(intptr_t) 0));   /* < 0 > */
    ASSERT(list_insert(lst, 0, (void*)(intptr_t) -1));  /* < -1, 0 > */
    ASSERT(list_insert(lst, 2, (void*)(intptr_t) 3));   /* < -1, 0, 3 > */
    ASSERT(list_insert(lst, 1, (void*)(intptr_t) 4));   /* < -1, 4, 0,  3 > */
    ASSERT(list_insert(lst, 2, (void*)(intptr_t) 8));   /* < -1, 4, 8, 0, 3 > */

    ASSERT(-1 == (intptr_t) list_get(lst, 0));
    ASSERT(4  == (intptr_t) list_get(lst, 1));
    ASSERT(8  == (intptr_t) list_get(lst, 2));
    ASSERT(0  == (intptr_t) list_get(lst, 3));
    ASSERT(3  == (intptr_t) list_get(lst, 4));

    list_free(lst);
}

static void list_test_remove_at()
{
    list* lst = list_alloc(3);
    size_t i;

    puts("        list_test_remove_at()");

    for (i = 0; i < 50; i++)
    {
        ASSERT(list_push_back(lst, (void*)(intptr_t) i));
    }

    ASSERT((intptr_t) list_remove_at(lst, 10) == 10);
    ASSERT((intptr_t) list_remove_at(lst, 10) == 11);
    ASSERT((intptr_t) list_remove_at(lst, 10) == 12);

    for (i = 0; i < 10; i++)
    {
        ASSERT(i == (intptr_t) list_get(lst, i));
        ASSERT(list_push_back(lst, (void*) i));
    }

    ASSERT((intptr_t) list_remove_at(lst, 10) == 13);
    ASSERT((intptr_t) list_remove_at(lst, 10) == 14);
    ASSERT((intptr_t) list_remove_at(lst, 10) == 15);

    for (i = 0; i < 10; i++)
    {
        ASSERT(i == (intptr_t) list_get(lst, i));
    }

    for (i = 10; i < list_size(lst) - 10; i++)
    {
        ASSERT(i + 6 == (intptr_t) list_get(lst, i));
    }

    list_free(lst);
}

static void list_test_contains()
{
    list* lst = list_alloc(3);
    
    puts("        list_test_contains()");

    list_push_back(lst, (void*) 1);
    list_push_back(lst, (void*) 2);
    list_push_back(lst, (void*) 3);

    ASSERT(list_contains(lst, (void*) 1, equals));
    ASSERT(list_contains(lst, (void*) 2, equals));
    ASSERT(list_contains(lst, (void*) 3, equals));
    ASSERT(list_contains(lst, (void*) 4, equals) == 0); /* No 4. */ 
    ASSERT(list_contains(lst, (void*) 0, equals) == 0); /* No 5. */

    list_remove_at(lst, 1);

    ASSERT(list_contains(lst, (void*) 2, equals) == 0); /* No 2 anymore. */

    list_push_back(lst, (void*) 1);
    list_push_back(lst, (void*) 2);
    list_push_back(lst, (void*) 3);

    ASSERT(list_contains(lst, (void*) 1, equals));
    ASSERT(list_contains(lst, (void*) 2, equals));
    ASSERT(list_contains(lst, (void*) 3, equals));
    ASSERT(list_contains(lst, (void*) 4, equals) == 0); /* No 4. */
    ASSERT(list_contains(lst, (void*) 0, equals) == 0); /* No 5. */

    list_remove_at(lst, 1);

    ASSERT(list_contains(lst, (void*) 2, equals) == TRUE); /* No 1 anymore. */

    list_free(lst);
}

static void list_test_clear()
{
    list* lst = list_alloc(3);
    int i;

    puts("        list_test_clear()");

    for (i = 0; i < 100; i++)
    {
        list_push_back(lst, (void*)(intptr_t) i);
    }

    for (i = 0; i < 100; i++)
    {
        ASSERT((intptr_t) list_get(lst, i) == i);
        ASSERT(list_contains(lst, (void*)(intptr_t) i, equals));
    }

    ASSERT(100 == list_size(lst));
    list_clear(lst);
    ASSERT(0 == list_size(lst));

    for (i = 0; i < 100; i++)
    {
        ASSERT(list_contains(lst, (void*)(intptr_t) i, equals) == FALSE);
    }

    list_free(lst);
}

static int list_element_equals_1(void* a, void* b)
{
    return a == b;
}

static int list_element_equals_2(void* a, void* b)
{

    size_t sz_1 = *((size_t*) a);
    size_t sz_2 = *((size_t*) b);
    return sz_1 == sz_2;
}

static void list_test_equals()
{
    list* lst_1 = list_alloc(3);
    list* lst_2 = list_alloc(3);
    size_t i;
    size_t* a;
    size_t* b;

    puts("        list_test_equals()");

    for (i = 0; i < 10; i++)
    {
        list_push_back(lst_1, (void*) i);
        list_push_back(lst_2, (void*) i);
        ASSERT(list_equals(lst_1, lst_2, list_element_equals_1));
    }

    list_clear(lst_1);
    list_clear(lst_2);

    a = malloc(sizeof(*a));
    b = malloc(sizeof(*b));

    *a = 10;
    *b = 10;

    list_push_back(lst_1, a);
    list_push_back(lst_1, a);
    list_push_back(lst_2, a);
    list_push_back(lst_2, b);

    ASSERT(list_equals(lst_1, lst_2, list_element_equals_1) == FALSE);
    ASSERT(list_equals(lst_1, lst_2, list_element_equals_2));
}

void list_test()
{
    puts("    list_test()");
    list_test_push_front_pop_front();
    list_test_push_back_pop_back();
    list_test_insert();
    list_test_remove_at();
    list_test_contains();
    list_test_clear();
    list_test_equals();
}
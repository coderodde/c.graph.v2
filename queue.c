#include "my_assert.h"
#include "queue.h"
#include <stddef.h>
#include <stdint.h>

typedef struct queue_node {
    struct queue_node* next;
    void* element;
} queue_node;

typedef struct queue_state {
    queue_node* front;
    queue_node* back;
    size_t size;
} queue_state;

queue* queue_alloc()
{
    queue* q = malloc(sizeof(*q));

    if (!q)
    {
        return NULL;
    }

    q->state = malloc(sizeof(queue_state));

    if (!q->state)
    {
        free(q);
        return NULL;
    }

    q->state->front = NULL;
    q->state->back = NULL;
    q->state->size = 0;
    return q;
}

void queue_push_back(queue* q, void* element)
{
    queue_node* new_queue_node;

    if (!q || !q->state)
    {
        return;
    }

    new_queue_node = malloc(sizeof(queue_node));
    new_queue_node->element = element;
    new_queue_node->next = NULL;

    if (q->state->back)
    {
        q->state->back->next = new_queue_node;
        q->state->back = new_queue_node;
    }
    else
    {
        q->state->front = new_queue_node;
        q->state->back = new_queue_node;
    }

    q->state->size++;
}

void* queue_pop_front(queue* q)
{
    queue_node* front_node;
    void* element;

    if (!q || !q->state)
    {
        return NULL;
    }

    if (q->state->size == 0)
    {
        return NULL;
    }

    front_node = q->state->front;
    element = front_node->element;

    if (q->state->size == 1)
    {
        q->state->front = NULL;
        q->state->back  = NULL;
    }
    else
    {
        q->state->front = q->state->front->next;
    }

    q->state->size--;
    free(front_node);
    return element;
}

void* queue_front(queue* q)
{
    if (!q || !q->state || q->state->size == 0)
    {
        return NULL;
    }

    return q->state->front->element;
}

size_t queue_size(queue* q)
{
    return q->state->size;
}

void queue_free(queue* q)
{
    queue_node* q_node;
    queue_node* q_node_next;

    if (!q || !q->state || !q->state->front) return;

    q_node = q->state->front;
    q_node_next = q_node->next;

    while (q_node)
    {
        q_node_next = q_node->next;
        free(q_node);
        q_node = q_node_next;
    }

    free(q->state);
    free(q);
}

void queue_test()
{
    int i;
    int num;
    queue* q = queue_alloc();

    puts("    queue_test()");

    ASSERT(q != NULL);

    for (i = 0; i < 10; i++) {
        queue_push_back(q, (void*)(intptr_t) i);
    }

    for (i = 0; i < 10; i++) {
        ASSERT(i == (intptr_t) queue_pop_front(q));
    }

    ASSERT(queue_size(q) == 0);
    queue_push_back(q, (void*) 10);
    ASSERT(queue_size(q) == 1);
    queue_push_back(q, (void*) 11);
    ASSERT(queue_size(q) == 2);
    num = (intptr_t) queue_pop_front(q);
    ASSERT(num == 10);
    ASSERT(queue_size(q) == 1);
    num = (intptr_t) queue_pop_front(q);
    ASSERT(num == 11);
    
    for (i = 0; i < 10; i++) {
        ASSERT(queue_pop_front(q) == NULL);
    }

    ASSERT(queue_size(q) == 0);
    queue_push_back(q, (void*) 10);
    ASSERT(queue_size(q) == 1);
    queue_push_back(q, (void*) 11);
    ASSERT(queue_size(q) == 2);
    num = (intptr_t) queue_pop_front(q);
    ASSERT(num == 10);
    ASSERT(queue_size(q) == 1);
    num = (intptr_t) queue_pop_front(q);
    ASSERT(num == 11);
    ASSERT(queue_size(q) == 0);
}
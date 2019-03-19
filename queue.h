#ifndef QUEUE_H
#define QUEUE_H
#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct queue {
        struct queue_state* state;
    } queue;

    /*******************************************************************************
    * Allocates a queue and returns its pointer.                                   *
    *******************************************************************************/
    queue* queue_alloc();

    /*******************************************************************************
    * Appends an element to a queue.                                               *
    *******************************************************************************/
    void   queue_push_back(queue*, void* element);

    /*******************************************************************************
    * Removes the front element from the queue.                                    *
    *******************************************************************************/
    void*  queue_pop_front(queue*);

    /*******************************************************************************
    * Returns the number of elements in the queue.                                 *
    *******************************************************************************/
    size_t queue_size(queue*);

    /*******************************************************************************
    * Remove all the contents of the queue.                                        *
    *******************************************************************************/
    void   queue_free(queue*);

    /* Contains unit tests. */
    void   queue_test();

#ifdef	__cplusplus
}
#endif
#endif /* QUEUE_H */
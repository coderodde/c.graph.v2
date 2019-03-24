#ifndef UNORDERED_SET_H
#define	UNORDERED_SET_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct unordered_set {
        struct unordered_set_state* state;
    } unordered_set;

    typedef struct unordered_set_iterator unordered_set_iterator;

    /***************************************************************************
    * Allocates a new, empty set with given hash function and given equality   *
    * testing function.                                                        *
    ***************************************************************************/
    unordered_set* unordered_set_alloc(size_t initial_capacity,
        float load_factor,
        size_t(*p_hash_function)(void*),
<<<<<<< HEAD
        int (*p_equals_function)(void*, void*));
=======
        bool(*p_equals_function)(void*, void*));
>>>>>>> 762273c60dc5d5029d89da43112e43c70a82b704

    /***************************************************************************
    * Adds 'p_element' to the set if not already there. Returns true if the    *
    * structure of the set changed.                                            *
    ***************************************************************************/
    int  unordered_set_add(unordered_set* p_set, void* p_element);

    /***************************************************************************
    * Returns true if the set contains the element.                            *
    ***************************************************************************/
    int  unordered_set_contains(unordered_set* p_set, void* p_element);

    /***************************************************************************
    * If the element is in the set, removes it and returns true.               *
    ***************************************************************************/
    int  unordered_set_remove(unordered_set* p_set, void* p_element);

    /***************************************************************************
    * Removes all the contents of the set.                                     *
    ***************************************************************************/
    void   unordered_set_clear(unordered_set* p_set);

    /***************************************************************************
    * Returns the size of the set.                                             *
    ***************************************************************************/
    size_t unordered_set_size(unordered_set* p_set);

    /***************************************************************************
    * Checks that the set is in valid state.                                   *
    ***************************************************************************/
    int unordered_set_is_healthy(unordered_set* p_set);

    /***************************************************************************
    * Deallocates the entire set. Only the set and its nodes are deallocated.  *
    * The user is responsible for deallocating the actual data stored in the   *
    * set.                                                                     *
    ***************************************************************************/
    void unordered_set_free(unordered_set* p_set);

    /***************************************************************************
    * Returns the iterator over the set. The nodes are iterated in insertion   *
    * order.                                                                   *
    ***************************************************************************/
    unordered_set_iterator* unordered_set_iterator_alloc(unordered_set* p_set);

    /***************************************************************************
    * Returns the number of elements not yet iterated over.                    *
    ***************************************************************************/
<<<<<<< HEAD
    size_t unordered_set_iterator_has_next(unordered_set_iterator* p_iterator);
=======
    int unordered_set_iterator_has_next(unordered_set_iterator* p_iterator);
>>>>>>> 762273c60dc5d5029d89da43112e43c70a82b704

    /***************************************************************************
    * Loads the next element in the iteration order. Returns true if advanced  *
    * to the next element.                                                     *
    ***************************************************************************/
    int unordered_set_iterator_next(unordered_set_iterator* p_iterator,
        void** pp_element);

    /***************************************************************************
    * Returns true if the set was modified during the iteration.               *
    ***************************************************************************/
    int unordered_set_iterator_is_disturbed(unordered_set_iterator* p_iterator);

    /***************************************************************************
    * Deallocates the set iterator.                                            *
    ***************************************************************************/
    void unordered_set_iterator_free(unordered_set_iterator* p_iterator);

#ifdef	__cplusplus
}
#endif

#endif	/* UNORDERED_SET_H */

#ifndef LIST_H
#define	LIST_H

#include <stdlib.h>
#define TRUE 1
#define FALSE 0

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct list {
        struct list_state* state;
    } list;

    /***************************************************************************
    * Allocates the new, empty list with initial capacity.                     *
    ***************************************************************************/
    list* list_alloc(size_t initial_capacity);

    /***************************************************************************
    * Inserts the element to in front of the head of the list. Returns true if *
    * operation was successful.                                                *
    ***************************************************************************/
    int list_push_front(list* my_list, void* element);

    /***************************************************************************
    * Appends the element to the tail of the list. Returns true if operation   *
    * was successful.                                                          *
    ***************************************************************************/
    int list_push_back(list* my_list, void* element);

    /***************************************************************************
    * Inserts the element into the list before index'th element. Returns TRUE  *
    * if operation was successful.                                             *
    ***************************************************************************/
    int list_insert(list* my_list, size_t index, void* element);

    /***************************************************************************
    * Returns the amount of elements stored in the list.                       *
    ***************************************************************************/
    size_t list_size(list* my_list);

    /***************************************************************************
    * Returns the index'th element of the list. Returns NULL if the index is   *
    * out of range.                                                            *
    ***************************************************************************/
    void* list_get(list* my_list, size_t index);

    /***************************************************************************
    * Sets the index'th element of the list. Returns the old value. If the     *
    * index is out of range, returns NULL.                                     *
    ***************************************************************************/
    void* list_set(list* my_list, size_t index, void* new_value);

    /***************************************************************************
    * Removes and returns the front element of the list. If list is empty,     *
    * returns NULL.                                                            *
    ***************************************************************************/
    void* list_pop_front(list* my_list);

    /***************************************************************************
    * Removes and returns the last element of the list. If list is empty,      *
    * returns NULL.                                                            *
    ***************************************************************************/
    void* list_pop_back(list* my_list);

    /***************************************************************************
    * Removes the element at index 'index' from the list and returns the       *
    * it. If the list is empty or the index is out of range, returns NULL.     *
    ***************************************************************************/
    void* list_remove_at(list* my_list, size_t index);

    /***************************************************************************
    * Returns true if the list contains the specified element using the        *
    * equality function. Returns false otherwise.                              *
    ***************************************************************************/
    int list_contains(list* my_list, 
                      void* element,
                      int (*equals_function)(void*, void*));

    /**************************************************************
    * Returns true if the two input lists contain the same stuff. *
    **************************************************************/
    int list_equals(list* my_list_1, 
                    list* my_list_2,
                    int (*equals)(void*, void*));

    /***************************************************************************
    * Clears this list. The client programmer is responsible for memory-       *
    * managing the contents.                                                   *
    ***************************************************************************/
    void list_clear(list* my_list);

    /***************************************************************************
    * Clears and deallocates the list.                                         *
    ***************************************************************************/
    void list_free(list* my_list);

#ifdef	__cplusplus
}
#endif

#endif	/* LIST_H */

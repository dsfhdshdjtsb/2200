/**
 * Name: Nicholas Suh
 * GTID: 903901919
 */

/*  PART 2: A CS-2200 C implementation of the arraylist data structure.
    Implement an array list.
    The methods that are required are all described in the header file. 
    Description for the methods can be found there.

    Hint 1: Review documentation/ man page for malloc, calloc, and realloc.
    Hint 2: Review how an arraylist works.
    Hint 3: You can use GDB if your implentation causes segmentation faults.
    Hint 4: Remember to check if memory allocations succeed.

    You will submit this file to gradescope.
*/

#include "arraylist.h"

/* Student code goes below this point */

arraylist_t *create_arraylist(uint capacity) {
    if(capacity == 0) return NULL;
    arraylist_t *list = malloc(sizeof(arraylist_t));

    if(list == NULL) return NULL;
    list->capacity  = capacity;
    list->size = 0;
    list->backing_array = malloc(capacity * sizeof(char*));

    return list;
}

void add_at_index(arraylist_t *arraylist, char *data, int index) {
    if(arraylist->size == arraylist->capacity) {
        resize(arraylist);
    }


    for(int i = arraylist->size; i > index; i--) {
        arraylist->backing_array[i] = arraylist->backing_array[i-1];
    }
    arraylist->backing_array[index] = data;
    arraylist->size++;
}

void append(arraylist_t *arraylist, char *data) {
    if(arraylist->capacity == arraylist->size) {
        resize(arraylist);
    }
    arraylist->backing_array[arraylist->size] = data; 
    arraylist->size++;
}

char *remove_from_index(arraylist_t *arraylist, int index) {
    
    char* ret = arraylist->backing_array[index];
    for(int i = index; i < arraylist->size-1; i++) {
        arraylist->backing_array[i] = arraylist->backing_array[i+1];
    }
    arraylist->size--;
    return ret;

}

void resize(arraylist_t *arraylist) {
    char **new_arr = realloc(arraylist->backing_array, arraylist->capacity * 2 * sizeof(char*)); 
    if(new_arr == NULL) return;
    arraylist->backing_array = new_arr;
    arraylist->capacity *=2;
}


void destroy(arraylist_t *arraylist) {
    free(arraylist->backing_array);
    free(arraylist);
}

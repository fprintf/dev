#include <stdlib.h>
#include <string.h>

#include "vector.h"

#define VECTOR_DEFAULT_SIZE 32
struct vector {
    size_t size;  /* Number of elements in vector */
    size_t capacity;
    void ** data; /* Array of data */
};

/******************************************************************
 * Internal functions
 ******************************************************************/

static void * vector_resize(struct vector * vec, double factor)
{
    void ** tmp = realloc(vec->data, vec->capacity * factor * sizeof *vec->data);

    if (tmp) {
        vec->data = tmp;
        /* Now zero out the new data */
        memset(vec->data + vec->capacity, 0, vec->capacity * (factor - 1) * sizeof *vec->data);
        vec->capacity *= factor;
    }

    return tmp;
}

/******************************************************************
 * API functions
 ******************************************************************/
/*
 * Free vector object
 */
void vector_delete(struct vector * vec)
{
    if (!vec) 
        return;
    free(vec->data);
    free(vec);
}

/*
 * Create vector object and return pointer 
 * to it. If initial is set to 0, use
 * default internal size
 *
 * Return NULL on error
 */
struct vector * vector_new(size_t initial)
{
    struct vector * vec = malloc(sizeof *vec);

    if (vec) {
        initial = initial || VECTOR_DEFAULT_SIZE;

        vec->capacity = initial;
        vec->size = 0;
        vec->data = calloc(initial, sizeof *vec->data);
        if (!vec->data) {
            free(vec);
            vec = NULL;
        }
    }

    return vec;
}

/******************************************************************
 * Accessors
 ******************************************************************/
size_t vector_size(struct vector * vec) 
{ 
    return vec->size; 
}
void * vector_index(struct vector * vec, size_t index)
{
    return index >= vec->size ? NULL : vec->data[index];
}

/******************************************************************
 * Manipulation
 ******************************************************************/
void * vector_sindex(struct vector * vec, size_t index, void * data)
{
    void * element = NULL;

    /* Resize array aggressively based on distance of index */
    while (index >= vec->capacity) {
        if (!vector_resize(vec, index/vec->capacity + 1)) 
            break;
    }

    /* Did resize succeed? */
    if (index < vec->capacity) {
        vec->size = index >= vec->size ? index+1 : vec->size; /* Update our size */
        element = vec->data[index] = data;
    }

    return element;
}

/******************************************************************
 * Stack based access
 ******************************************************************/
void * vector_push(struct vector * vec, void * data)
{
    do {
        if (vec->size >= vec->capacity) {
            vector_resize(vec, 2);
            if (vec->size >= vec->capacity) {
                data = NULL;
                break;
            }
        }
        /* Append the element onto the end of the vector */
        vec->data[vec->size++] = data;
    } while(0);

    return data;
}

void * vector_pop(struct vector * vec)
{
    void * data = NULL;

    if (vec->size > 0)
        data = vec->data[--vec->size];

    if (vec->capacity - vec->size > vec->size*2 + 1)
        vector_resize(vec, 0.5);

    return data;
}

/******************************************************************
 * Vector API linkage
 ******************************************************************/
const struct vector_api vector = {
    .new = vector_new,
    .delete = vector_delete,
    .size = vector_size,

    .push = vector_push,
    .pop = vector_pop,

    .index = vector_index,
    .sindex = vector_sindex,
};

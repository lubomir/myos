/*
 * ordered-array.h -- Defines interface for creating, inserting and deleting
 *                    from ordered arrays.
 * Written for JamesM's kernel development tutorials.
 */

#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "common.h"

/*
 * The array is going to be insertion sorted - between calls it will always
 * be sorted between calls.
 *
 * It can store anything that can be cast to void*, so a u32int or any
 * pointer.
 */

typedef void * type_t;

/*
 * A predicate should return integer less than, equal to, or greater than
 * zero if the first item is found, respectively, to be less than, equal to,
 * or greater to the second item.
 *
 * For example strcmp() is a possible predicate.
 */
typedef int (*cmp_predicate_t)(type_t, type_t);

typedef struct {
    type_t *data;
    u32int size;
    u32int max_size;
    cmp_predicate_t cmp;
} ordered_array_t;

/*
 * A standard comparation predicate for numbers.
 */
int standard_cmp_pred(type_t a, type_t b);

/*
 * Create an ordered array.
 */
ordered_array_t oa_create(u32int max_size, cmp_predicate_t pred);
ordered_array_t oa_place(void *addr, u32int max_size, cmp_predicate_t pred);

/*
 * Destroy an ordered array.
 */
void oa_destroy(ordered_array_t *array);

/*
 * Add an item into the array.
 */
void oa_insert(ordered_array_t *array, type_t item);

/*
 * Lookup the item at index i.
 */
type_t oa_lookup(ordered_array_t *array, u32int i);

/*
 * Delete the item at location i from the array.
 */
void oa_remove(ordered_array_t *array, u32int i);

/*
 * Delete item with given value.
 * Return 0 if there is no such item in the array, 1 otherwise.
 */
u8int oa_remove_item(ordered_array_t *array, type_t item);

#endif /* end of include guard: ORDERED_ARRAY_H */

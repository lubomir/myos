/*
 * ordered-array.c -- implementation for creating, inserting and deleting
 *                    from ordered arrays.
 * Written for JamesM's kernel development tutorials.
 */

#include <string.h>

#include "kheap.h"
#include "ordered-array.h"

int standard_cmp_pred(type_t a, type_t b)
{
    if ((u32int) a < (u32int) b)
        return -((u32int) b - (u32int) a);
    return (u32int) a - (u32int) b;
}

ordered_array_t oa_create(u32int max_size, cmp_predicate_t pred)
{
    void *mem = kmalloc(max_size * sizeof(type_t));
    return oa_place(mem, max_size, pred);
}

ordered_array_t oa_place(void *addr, u32int max_size, cmp_predicate_t pred)
{
    ordered_array_t arr;
    arr.data = (type_t *) addr;
    memset(arr.data, 0, max_size * sizeof(type_t));
    arr.size = 0;
    arr.max_size = max_size;
    arr.cmp = pred;
    return arr;
}

void oa_destroy(ordered_array_t *array)
{
    kfree((u32int) array->data);
}

void oa_insert(ordered_array_t *arr, type_t item)
{
    ASSERT(arr->cmp);
    ASSERT(arr->size < arr->max_size);
    u32int iter = 0;

    while (iter < arr->size && arr->cmp(arr->data[iter], item) <= 0) {
        iter++;
    }
    if (iter == arr->size) {  /* just add at the end of the array */
        arr->data[arr->size++] = item;
        return;
    }
    type_t tmp = arr->data[iter];
    arr->data[iter] = item;
    while (iter < arr->size) {
        iter++;
        type_t tmp2 = arr->data[iter];
        arr->data[iter] = tmp;
        tmp = tmp2;
    }
    arr->size++;
}

type_t oa_lookup(ordered_array_t *array, u32int i)
{
    ASSERT(i < array->size);
    return array->data[i];
}

void oa_remove(ordered_array_t *array, u32int i)
{
    while (i < array->size) {
        array->data[i] = array->data[i+1];
        i++;
    }
    array->size--;
}

u8int oa_remove_item(ordered_array_t *array, type_t item)
{
    u32int iter = 0;
    while (iter < array->size && array->data[iter] != item)
        iter++;
    if (iter < array->size) {
        oa_remove(array, iter);
        return 1;
    }
    return 0;
}

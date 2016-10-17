/*

The Horror C preprocessor library. Abuse at your own risk.

Copyright (c) 2016 Sean Leffler

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "munit.h"

#include "horror/macro.h"

#define HP_SCOPE HR_SCOPE_STATIC_INLINE
#define HP_ELEM_TYPE int
#define HP_NAME int
#define HP_CMP(x, y) (*(x) < *(y) ? true : false)
#define HP_STORAGE HR_STORAGE_BORROWED_INDIRECT
#define HP_DEBUG
#define HP_DEBUG_DUMP(x) do { fprintf(stderr, "[%p](%i)", (void*)x, *x); } while (0)
#include "horror/heap.c"


static void* setup(const MunitParameter params[], void* _) {
    hp_int_t* heap = malloc(sizeof(hp_int_t));
    hp_int_init(heap, 8);
    return heap;
}


static void tear_down(void* heap) {
    hp_int_cleanup(heap);
    free(heap);
}


static MunitResult test(const MunitParameter params[], void* heap) {
    int min = INT_MAX;

    int ints[100];
    munit_rand_memory(100 * sizeof(int), (uint8_t*)ints);

    size_t i;
    for (i = 0; i < 100; i++) {
        if (ints[i] < min) {
            min = ints[i];
        }

        hp_int_insert(heap, &ints[i]);
    }

    int* x = hp_int_extract(heap);
    munit_assert_int(min, ==, *x);

    while (hp_int_size(heap) > 0) {
        munit_assert_int(min, <=, *hp_int_peek(heap));
        min = *hp_int_extract(heap);
    }

    return MUNIT_OK;
}


MunitTest hp_int_borrowed_indirect_test = {
    "/heap HP_SCOPE=HR_SCOPE_STATIC_INLINE HP_ELEM_TYPE=int HP_NAME=int HP_STORAGE=HR_STORAGE_BORROWED_INDIRECT",
    test,
    setup,
    tear_down,
    MUNIT_TEST_OPTION_NONE,
    NULL,
};

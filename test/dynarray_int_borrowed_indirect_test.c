/*

The Horror generic C data structure library. Abuse at your own risk.

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

#define DA_SCOPE HR_SCOPE_STATIC_INLINE
#define DA_STORAGE HR_STORAGE_BORROWED_INDIRECT
#define DA_ELEM_TYPE int
#define DA_NAME da_int
#define DA_DEBUG
#define DA_DEBUG_DUMP(x) do { fprintf(stderr, "%i", *x); } while (0)
#include "horror/dynarray.c"


static void* setup(const MunitParameter params[], void* _) {
    da_int_t* da = malloc(sizeof(da_int_t));
    da_int_init(da, 8);
    return da;
}


static void tear_down(void* da) {
    da_int_cleanup(da);
    free(da);
}


static MunitResult test(const MunitParameter params[], void* da) {
    int ints[100];

    for (size_t i = 0; i < 100; i++) {
        ints[i] = munit_rand_int_range(-10, 10);
    }

    munit_assert_size(0, ==, da_int_size(da));

    for (size_t i = 0; i < 100; i++) {
        da_int_push(da, &ints[i]);
    }

    munit_assert_size(100, ==, da_int_size(da));

    for (size_t i = 100; i > 0; i--) {
        munit_assert_ptr(&ints[i-1], ==, da_int_pop(da));
    }

    munit_assert_size(0, ==, da_int_size(da));

    for (size_t i = 0; i < 100; i++) {
        da_int_insert(da, 0, &ints[i]);
    }

    munit_assert_size(100, ==, da_int_size(da));

    for (size_t i = 0; i < 100; i++) {
        munit_assert_int(ints[i], ==, *da_int_addr(da, 99 - i));
    }

    for (size_t i = 100; i > 0; i--) {
        da_int_remove(da, (size_t)munit_rand_int_range(0, i - 1));
        munit_assert_size(i-1, ==, da_int_size(da));
    }

    return MUNIT_OK;
}


MunitTest da_int_borrowed_indirect_test = {
    "/dynarray DA_SCOPE=DA_SCOPE_STATIC_INLINE DA_ELEM_TYPE=int DA_NAME=int DA_STORAGE=HR_STORAGE_BORROWED_INDIRECT",
    test,
    setup,
    tear_down,
    MUNIT_TEST_OPTION_NONE,
    NULL,
};

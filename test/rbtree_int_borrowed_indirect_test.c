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

#include <assert.h>

#include "munit.h"

#include "horror/macro.h"
#include "horror/rbtree.h"

#define RB_SCOPE HR_SCOPE_STATIC_INLINE
#define RB_ELEM_TYPE int
#define RB_NAME rb_int
#define RB_CMP(x, y) (*(x) < *(y) ? -1 : (*(x) > *(y) ? 1 : 0))
#define RB_STORAGE HR_STORAGE_BORROWED_INDIRECT
#define RB_DEBUG
#define RB_DEBUG_DUMP(x) do { fprintf(stderr, "Node: %i.\n", *x); } while (0)
#include "horror/rbtree.c"


static void* setup(const MunitParameter params[], void* _) {
    rb_int_t* tree = malloc(sizeof(rb_int_t));
    rb_int_init(tree);
    return tree;
}


static void tear_down(void* tree) {
    rb_int_cleanup(tree);
    free(tree);
}


static MunitResult test(const MunitParameter params[], void* tree) {
    munit_assert_size(rb_int_size(tree), ==, 0);

    int ints[100];
    for (size_t i = 0; i < 100; i++) {
        ints[i] = munit_rand_int_range(0, 100);
    }

    size_t i;
    for (i = 0; i < 100; i++) {
        fprintf(stderr, "Generated: %i. Looking to see if it's already there...", ints[i]);

        const int* preexisting = rb_int_find(tree, &ints[i]);

        bool already_there = false;
        if (preexisting != NULL) {
            already_there = true;

            munit_assert_int(ints[i], ==, *preexisting);
        }

        size_t old_size = rb_int_size(tree);

        fprintf(stderr, "Inserting %i...\n", ints[i]);
        munit_assert(rb_int_insert(tree, &ints[i]));

        fprintf(stderr, "Asserting tree invariants...\n");
        munit_assert(rb_int_assert(tree));

        fprintf(stderr, "Asserting size change conditions...\n");
        if (already_there) {
            munit_assert_size(old_size, ==, rb_int_size(tree));
        } else {
            preexisting = rb_int_find(tree, &ints[i]);
            munit_assert_not_null(preexisting);
            munit_assert_int(*preexisting, ==, ints[i]);
            munit_assert_size(old_size + 1, ==, rb_int_size(tree));
        }
    }

    for (; i > 0; i--) {
        int x = munit_rand_int_range(0, 100);

        const int* preexisting = rb_int_find(tree, &x);

        bool already_there = false;
        if (preexisting != NULL) {
            already_there = true;

            munit_assert_int(x, ==, *preexisting);
        }

        size_t old_size = rb_int_size(tree);

        fprintf(stderr, "Removing %i...\n", x);
        rb_int_remove(tree, &x);

        fprintf(stderr, "Asserting tree invariants...\n");
        munit_assert(rb_int_assert(tree));

        fprintf(stderr, "Asserting size change conditions...\n");
        if (!already_there) {
            munit_assert_size(old_size, ==, rb_int_size(tree));
        } else {
            preexisting = rb_int_find(tree, &x);
            munit_assert_null(preexisting);
            munit_assert_size(old_size - 1, ==, rb_int_size(tree));
        }
    }

    for (size_t i = 0; i < 100; i++) {
        rb_int_insert(tree, &ints[i]);
    }

    while (rb_int_size(tree) > 0) {
        int min = *rb_int_min(tree);
        int popped = *rb_int_pop_min(tree);
        munit_assert_int(min, ==, popped);
    }

    for (size_t i = 0; i < 100; i++) {
        rb_int_insert(tree, &ints[i]);
    }

    while (rb_int_size(tree) > 0) {
        int max = *rb_int_max(tree);
        int popped = *rb_int_pop_max(tree);
        munit_assert_int(max, ==, popped);
    }

    return MUNIT_OK;
}


MunitTest rb_int_borrowed_indirect_test = {
    "/rbtree RB_SCOPE=HR_SCOPE_STATIC_INLINE RB_ELEM_TYPE=int RB_NAME=int RB_STORAGE=HR_STORAGE_BORROWED_INDIRECT",
    test,
    setup,
    tear_down,
    MUNIT_TEST_OPTION_NONE,
    NULL,
};

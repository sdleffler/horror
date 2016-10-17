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

#include "munit.h"

#include "rbtree_int_test.h"
#include "rbtree_int_owned_indirect_test.h"
#include "rbtree_int_borrowed_indirect_test.h"

#include "heap_int_test.h"
#include "heap_int_owned_indirect_test.h"
#include "heap_int_borrowed_indirect_test.h"

#include "dynarray_int_test.h"
#include "dynarray_int_owned_indirect_test.h"
#include "dynarray_int_borrowed_indirect_test.h"

int main (int argc, char* argv[]) {
    MunitTest tests[] = {
        rb_int_test,
        rb_int_owned_indirect_test,
        rb_int_borrowed_indirect_test,
        hp_int_test,
        hp_int_owned_indirect_test,
        hp_int_borrowed_indirect_test,
        da_int_test,
        da_int_owned_indirect_test,
        da_int_borrowed_indirect_test,
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    };

    MunitSuite suite = {
        "/horror-tests",
        tests,
        NULL,
        1,
        MUNIT_SUITE_OPTION_NONE,
    };

    return munit_suite_main(&suite, NULL, argc, argv);
}

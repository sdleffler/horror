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
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "horror/macro.h"


#if !defined(HP_ELEM_TYPE)
    #error Error: Generic binary heap requires HP_ELEM_TYPE to be defined.
    #define HP_ELEM_TYPE char // For purposes of testing.
#endif

#if !defined(HP_NAME)
    #error Error: Generic binary heap requires HP_NAME to be defined. It \
can not use the type, since it might be a pointer.
    #define HP_NAME hp_char // For purposes of testing.
#endif

#if !defined(HP_CMP)
    #error Error: Generic binary heap requires HP_CMP to be defined. HP_CMP \
must be a function or function-like macro which takes two arguments and \
returns a boolean value (0 or 1) indicating whether or not the first argument \
precedes the other in the heap ordering. For example, one potential definition \
(to form a min-heap) would be `#define HP_CMP(x, y) (x) < (y)`.
    #define HP_CMP(x, y) ((x) < (y)) // For purposes of testing.
#endif

#if HP_STORAGE == HR_STORAGE_OWNED_INDIRECT && (!defined(HP_MALLOC_ELEM) || !defined(HP_FREE_ELEM))
    #if defined(HP_MALLOC_ELEM)
        #error Error: Generic binary heap is given a custom HP_MALLOC_ELEM, but \
not a custom HP_FREE_ELEM! This is dangerous, potentially even more dangerous than \
deciding to use this library in the first place. Please define a custom HP_FREE_ELEM.
    #endif

    #if defined(HP_FREE_ELEM)
        #error Error: Generic binary heap is given a custom HP_FREE_ELEM, but \
not a custom HP_MALLOC_ELEM! This is dangerous, potentially even more dangerous \
than deciding to use this library in the first place. Please define a custom \
HP_MALLOC_ELEM.
    #endif

    #define HP_MALLOC_ELEM (malloc(sizeof(HP_ELEM_TYPE)))
    #define HP_FREE_ELEM(ptr) (free(ptr))
#endif

#if HP_STORAGE == HR_STORAGE_OWNED_INDIRECT && !defined(HP_MEMCPY_ELEM)
    #define HP_MEMCPY_ELEM(dst, src) (memcpy(dst, src, sizeof(HP_ELEM_TYPE)))
#endif

#if !defined(HP_TYPE)
    #define HP_TYPE HR_CONCAT(HP_NAME, _t)
#endif


#if HP_SCOPE == HR_SCOPE_NONE
    #define HP_FUNC
#elif HP_SCOPE == HR_SCOPE_STATIC
    #define HP_FUNC static
#elif HP_SCOPE == HR_SCOPE_STATIC_INLINE
    #define HP_FUNC static inline
#elif HP_SCOPE == HR_SCOPE_EXTERN_INLINE
    #define HP_FUNC extern inline
#else
    #error Error: Generic binary heap requires HP_SCOPE to be defined.
    #define HP_FUNC // For testing purposes.
#endif

#if !defined(HP_STORAGE)
    #error Error: Generic binary heap requires HP_STORAGE to be defined.
    #define HP_STORAGE HR_STORAGE_DIRECT // For testing purposes.
#endif

#define NAME_(n) HR_CONCAT(HP_NAME, n)


typedef struct HP_TYPE HP_TYPE;


HP_FUNC bool NAME_(_init)(HP_TYPE* hp, size_t _size);
HP_FUNC void NAME_(_cleanup)(HP_TYPE* hp);
HP_FUNC bool NAME_(_conditional_resize)(HP_TYPE* hp, size_t req);

#if HP_STORAGE == HR_STORAGE_DIRECT
HP_FUNC bool NAME_(_insert)(HP_TYPE* hp, HP_ELEM_TYPE elem);
HP_FUNC HP_ELEM_TYPE NAME_(_extract)(HP_TYPE* hp);
HP_FUNC HP_ELEM_TYPE* NAME_(_peek)(HP_TYPE* hp);
#elif HP_STORAGE == HR_STORAGE_OWNED_INDIRECT
HP_FUNC bool NAME_(_insert)(HP_TYPE* hp, HP_ELEM_TYPE* elem);
HP_FUNC void NAME_(_extract)(HP_TYPE* hp, HP_ELEM_TYPE* dst);
HP_FUNC HP_ELEM_TYPE* NAME_(_peek)(HP_TYPE* hp);
#elif HP_STORAGE == HR_STORAGE_BORROWED_INDIRECT
HP_FUNC bool NAME_(_insert)(HP_TYPE* hp, HP_ELEM_TYPE* elem);
HP_FUNC HP_ELEM_TYPE* NAME_(_extract)(HP_TYPE* hp);
HP_FUNC HP_ELEM_TYPE* NAME_(_peek)(HP_TYPE* hp);
#endif

HP_FUNC size_t NAME_(_size)(HP_TYPE* hp);

#if HP_SCOPE != HR_SCOPE_HEADER || HP_SCOPE != HR_SCOPE_EXTERN_INLINE

struct HP_TYPE {
    size_t _used;
    size_t _size;
#if HP_STORAGE == HR_STORAGE_DIRECT
    HP_ELEM_TYPE* _data;
#else
    HP_ELEM_TYPE** _data;
#endif
};


#ifdef HP_DEBUG
HP_FUNC void NAME_(_dump)(HP_TYPE* hp) {
    fprintf(stderr, "Heap: { ");
    if (hp->_used > 0) {
        HP_DEBUG_DUMP(hp->_data[0]);
        for (size_t i = 1; i < hp->_used; i++) {
            fprintf(stderr, ", ");
            HP_DEBUG_DUMP(hp->_data[i]);
        }
    }
    fprintf(stderr, " }\n");
}
#endif


HP_FUNC bool NAME_(_init)(HP_TYPE* hp, size_t _size) {
    hp->_size = (_size > 0 ? _size : 1);
#if HP_STORAGE == HR_STORAGE_DIRECT
    hp->_data = (HP_ELEM_TYPE*)malloc(hp->_size * sizeof(HP_ELEM_TYPE));
#else
    hp->_data = (HP_ELEM_TYPE**)malloc(hp->_size * sizeof(HP_ELEM_TYPE*));
#endif

    if (hp->_data == NULL) {
        return false;
    }

    hp->_used = 0;

    return true;
}


HP_FUNC void NAME_(_cleanup)(HP_TYPE* hp) {
#if HP_STORAGE == HR_STORAGE_OWNED_INDIRECT
    for (size_t i = 0; i < hp->_used; i++) {
        HP_FREE_ELEM((hp->_data[i]));
    }
#endif
    free(hp->_data);
}


HP_FUNC bool NAME_(_conditional_resize)(HP_TYPE* hp, size_t req) {
    while (hp->_size < req) {
        hp->_size *= 2;
    }

#if HP_STORAGE == HR_STORAGE_DIRECT
    hp->_data = (HP_ELEM_TYPE*)realloc(hp->_data, hp->_size * sizeof(HP_ELEM_TYPE));
#else
    hp->_data = (HP_ELEM_TYPE**)realloc(hp->_data, hp->_size * sizeof(HP_ELEM_TYPE*));
#endif

    if (hp->_data == NULL) {
        return false;
    }
    return true;
}


#define HP_PARENT(loc) ((loc - 1) / 2)

#if HP_STORAGE == HR_STORAGE_DIRECT && !defined(HP_MEMCPY_ELEM)
HP_FUNC bool NAME_(_insert)(HP_TYPE* hp, HP_ELEM_TYPE elem)
#else
HP_FUNC bool NAME_(_insert)(HP_TYPE* hp, HP_ELEM_TYPE* elem)
#endif
{
    if (!NAME_(_conditional_resize)(hp, hp->_used + 1)) return false;

    size_t loc = hp->_used++;

    for (; loc > 0 && HP_CMP(elem, hp->_data[HP_PARENT(loc)]); loc = HP_PARENT(loc)) {
#if HP_STORAGE == HR_STORAGE_DIRECT && defined(HP_MEMCPY_ELEM)
        HP_MEMCPY_ELEM(&hp->_data[loc], &hp->_data[HP_PARENT(loc)]);
#else
        hp->_data[loc] = hp->_data[HP_PARENT(loc)];
#endif
    }

#if (HP_STORAGE == HR_STORAGE_DIRECT && defined(HP_MEMCPY_ELEM))
    HP_MEMCPY_ELEM(&hp->_data[loc], elem);
#elif HP_STORAGE == HR_STORAGE_OWNED_INDIRECT
    hp->_data[loc] = (HP_ELEM_TYPE*)(HP_MALLOC_ELEM);
    HP_MEMCPY_ELEM(hp->_data[loc], elem);
#else
    hp->_data[loc] = elem;
#endif

    return true;
}


#define HP_LEFT(loc) (2*(loc) + 1)
#define HP_RIGHT(loc) (2*(loc) + 2)

#if HP_STORAGE == HR_STORAGE_DIRECT
HP_FUNC HP_ELEM_TYPE NAME_(_extract)(HP_TYPE* hp)
#elif HP_STORAGE == HR_STORAGE_OWNED_INDIRECT
HP_FUNC void NAME_(_extract)(HP_TYPE* hp, HP_ELEM_TYPE* dst)
#elif HP_STORAGE == HR_STORAGE_BORROWED_INDIRECT
HP_FUNC HP_ELEM_TYPE* NAME_(_extract)(HP_TYPE* hp)
#endif
{
    assert(hp->_used > 0);

#if HP_STORAGE == HR_STORAGE_DIRECT
    HP_ELEM_TYPE root = hp->_data[0];
#elif HP_STORAGE == HR_STORAGE_OWNED_INDIRECT
    HP_MEMCPY_ELEM((dst), (hp->_data[0]));
    HP_FREE_ELEM((hp->_data[0]));
#elif HP_STORAGE == HR_STORAGE_BORROWED_INDIRECT
    HP_ELEM_TYPE* root = hp->_data[0];
#endif

#if HP_STORAGE == HR_STORAGE_DIRECT && !defined(HP_MEMCPY_ELEM)
    HP_ELEM_TYPE tmp = hp->_data[0] = hp->_data[--hp->_used];
#elif HP_STORAGE == HR_STORAGE_DIRECT && defined(HP_MEMCPY_ELEM)
    HP_ELEM_TYPE tmp;
    --hp->_used;
    HP_MEMCPY_ELEM((&tmp), (hp->_data[hp->_used]));
    HP_MEMCPY_ELEM((&hp->_data[0]), (hp->_data[hp->_used]));
#else
    HP_ELEM_TYPE* tmp = hp->_data[0] = hp->_data[--hp->_used];
#endif

    size_t new = 0, old = 0;
    for (;;) {
        old = new;

        if (HP_LEFT(old) < hp->_used && HP_CMP(hp->_data[HP_LEFT(old)], hp->_data[new])) {
            new = HP_LEFT(old);
        }

        if (HP_RIGHT(old) < hp->_used && HP_CMP(hp->_data[HP_RIGHT(old)], hp->_data[new])) {
            new = HP_RIGHT(old);
        }

        if (old == new) {
#if HP_STORAGE == HR_STORAGE_DIRECT && defined(HP_MEMCPY_ELEM)
            HP_MEMCPY_ELEM((&hp->_data[new]), (&tmp));
#else
            hp->_data[new] = tmp;
#endif
            break;
        }

#if HP_STORAGE == HR_STORAGE_DIRECT && defined(HP_MEMCPY_ELEM)
        HP_MEMCPY_ELEM((&tmp), (&hp->_data[old]));
        HP_MEMCPY_ELEM((&hp->_data[old]), (&hp->_data[new]));
        HP_MEMCPY_ELEM((&hp->_data[new]), (&tmp));
#else
        tmp = hp->_data[old];
        hp->_data[old] = hp->_data[new];
        hp->_data[new] = tmp;
#endif
    }

#if HP_STORAGE != HR_STORAGE_OWNED_INDIRECT
    return root;
#endif
}


HP_FUNC HP_ELEM_TYPE* NAME_(_peek)(HP_TYPE* hp) {
    assert(hp->_used > 0);
    #if HP_STORAGE == HR_STORAGE_DIRECT
    return hp->_data;
    #else
    return hp->_data[0];
    #endif
}

HP_FUNC size_t NAME_(_size)(HP_TYPE* hp) {
    return hp->_used;
}

#endif


#undef HP_ELEM_TYPE
#undef HP_NAME
#undef HP_CMP
#undef HP_STORAGE
#undef HP_MALLOC_ELEM
#undef HP_FREE_ELEM
#undef HP_MEMCPY_ELEM
#undef HP_TYPE
#undef HP_SCOPE
#undef HP_FUNC
#undef NAME_
#undef HP_PARENT
#undef HP_LEFT
#undef HP_RIGHT
#undef HP_DEBUG
#undef HP_DEBUG_DUMP

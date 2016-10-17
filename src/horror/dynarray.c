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


#if !defined(DA_ELEM_TYPE)
    #error Error: Generic dynamic array requires DA_ELEM_TYPE to be defined.
    #define DA_ELEM_TYPE char // For purposes of testing.
#endif

#if !defined(DA_NAME)
    #error Error: Generic dynamic array requires DA_NAME to be defined.
    #define DA_NAME da_char // For purposes of testing.
#endif

#if DA_STORAGE == HR_STORAGE_OWNED_INDIRECT && (!defined(DA_MALLOC_ELEM) || !defined(DA_FREE_ELEM))
    #if defined(DA_MALLOC_ELEM)
        #error Error: Generic binary heap is given a custom DA_MALLOC_ELEM, but \
not a custom DA_FREE_ELEM! This is dangerous, potentially even more dangerous than \
deciding to use this library in the first place. Please define a custom DA_FREE_ELEM.
    #endif

    #if defined(DA_FREE_ELEM)
        #error Error: Generic binary heap is given a custom DA_FREE_ELEM, but \
not a custom DA_MALLOC_ELEM! This is dangerous, potentially even more dangerous \
than deciding to use this library in the first place. Please define a custom \
DA_MALLOC_ELEM.
    #endif

    #define DA_MALLOC_ELEM (malloc(sizeof(DA_ELEM_TYPE)))
    #define DA_FREE_ELEM(ptr) (free(ptr))
#endif

#if DA_STORAGE == HR_STORAGE_DIRECT && defined(DA_MEMCPY_ELEM) && !defined(DA_MEMMOVE_ELEM)
    #error Error: Generic dynamic array is given a custom DA_MEMCPY_ELEM, but \
not a custom DA_MEMMOVE_ELEM! This is dangerous, potentially even more dangerous than \
deciding to use this library in the first place. Please define a custom DA_MEMMOVE_ELEM.
#elif DA_STORAGE == HR_STORAGE_OWNED_INDIRECT && !defined(DA_MEMCPY_ELEM)
    #define DA_MEMCPY_ELEM(dst, src) (memcpy((dst), (src), sizeof(DA_ELEM_TYPE)))
#endif

#if !defined(DA_MEMMOVE_ELEM)
    #if DA_STORAGE == HR_STORAGE_DIRECT
        #define DA_MEMMOVE_ELEM(dst, src, len) (memmove((dst), (src), (len) * sizeof(DA_ELEM_TYPE)))
    #else
        #define DA_MEMMOVE_ELEM(dst, src, len) (memmove((dst), (src), (len) * sizeof(DA_ELEM_TYPE*)))
    #endif
#endif

#if !defined(DA_TYPE)
    #define DA_TYPE HR_CONCAT(DA_NAME, _t)
#endif


#if DA_SCOPE == HR_SCOPE_NONE
    #define DA_FUNC
#elif DA_SCOPE == HR_SCOPE_STATIC
    #define DA_FUNC static
#elif DA_SCOPE == HR_SCOPE_STATIC_INLINE
    #define DA_FUNC static inline
#elif DA_SCOPE == HR_SCOPE_EXTERN_INLINE
    #define DA_FUNC extern inline
#else
    #error Error: Generic dynamic array requires DA_SCOPE to be defined.
    #define DA_FUNC // For testing purposes.
#endif

#if !defined(DA_STORAGE)
    #error Error: Generic dynamic array requires DA_STORAGE to be defined.
    #define DA_STORAGE HR_STORAGE_DIRECT // For testing purposes.
#endif

#define NAME_(n) HR_CONCAT(DA_NAME, n)


typedef struct DA_TYPE DA_TYPE;


DA_FUNC bool NAME_(_init)(DA_TYPE* da, size_t _size);
DA_FUNC void NAME_(_cleanup)(DA_TYPE* da);

#if DA_STORAGE == HR_STORAGE_DIRECT && !defined(DA_MEMCPY_ELEM)
DA_FUNC bool NAME_(_push)(DA_TYPE* da, DA_ELEM_TYPE elem);
DA_FUNC DA_ELEM_TYPE NAME_(_pop)(DA_TYPE* da);
DA_FUNC bool NAME_(_insert)(DA_TYPE* da, size_t pos, DA_ELEM_TYPE elem);
#elif (DA_STORAGE == HR_STORAGE_DIRECT && defined(DA_MEMCPY_ELEM)) || DA_STORAGE == HR_STORAGE_OWNED_INDIRECT
DA_FUNC bool NAME_(_push)(DA_TYPE* da, DA_ELEM_TYPE* elem);
DA_FUNC void NAME_(_pop)(DA_TYPE* da, DA_ELEM_TYPE* dst);
DA_FUNC bool NAME_(_insert)(DA_TYPE* da, size_t pos, DA_ELEM_TYPE* elem);
#elif DA_STORAGE == HR_STORAGE_BORROWED_INDIRECT
DA_FUNC bool NAME_(_push)(DA_TYPE* da, DA_ELEM_TYPE* elem);
DA_FUNC DA_ELEM_TYPE* NAME_(_pop)(DA_TYPE* da);
DA_FUNC bool NAME_(_insert)(DA_TYPE* da, size_t pos, DA_ELEM_TYPE* elem);
#endif

DA_FUNC void NAME_(_remove)(DA_TYPE* da, size_t pos);
DA_FUNC DA_ELEM_TYPE* NAME_(_addr)(DA_TYPE* da, size_t pos);
DA_FUNC size_t NAME_(_size)(DA_TYPE* da);

#if DA_SCOPE != HR_SCOPE_HEADER || DA_SCOPE != HR_SCOPE_EXTERN_INLINE

struct DA_TYPE {
    size_t _used;
    size_t _size;
#if DA_STORAGE == HR_STORAGE_DIRECT
    DA_ELEM_TYPE* _data;
#else
    DA_ELEM_TYPE** _data;
#endif
};


#ifdef DA_DEBUG
DA_FUNC void NAME_(_dump)(DA_TYPE* da) {
    fprintf(stderr, "Heap: { ");
    if (da->_used > 0) {
        DA_DEBUG_DUMP(da->_data[0]);
        for (size_t i = 1; i < da->_used; i++) {
            fprintf(stderr, ", ");
            DA_DEBUG_DUMP(da->_data[i]);
        }
    }
    fprintf(stderr, " }\n");
}
#endif


DA_FUNC bool NAME_(_init)(DA_TYPE* da, size_t _size) {
    da->_size = (_size > 0 ? _size : 1);
#if DA_STORAGE == HR_STORAGE_DIRECT
    da->_data = (DA_ELEM_TYPE*)malloc(da->_size * sizeof(DA_ELEM_TYPE));
#else
    da->_data = (DA_ELEM_TYPE**)malloc(da->_size * sizeof(DA_ELEM_TYPE*));
#endif

    if (da->_data == NULL) {
        return false;
    }

    da->_used = 0;

    return true;
}


DA_FUNC void NAME_(_cleanup)(DA_TYPE* da) {
#if DA_STORAGE == HR_STORAGE_OWNED_INDIRECT
    for (size_t i = 0; i < da->_used; i++) {
        DA_FREE_ELEM((da->_data[i]));
    }
#endif
    free(da->_data);
}


DA_FUNC bool NAME_(_conditional_resize)(DA_TYPE* da, size_t req) {
    while (da->_size < req) {
        da->_size *= 2;
    }

#if DA_STORAGE == HR_STORAGE_DIRECT
    da->_data = (DA_ELEM_TYPE*)realloc(da->_data, da->_size * sizeof(DA_ELEM_TYPE));
#else
    da->_data = (DA_ELEM_TYPE**)realloc(da->_data, da->_size * sizeof(DA_ELEM_TYPE*));
#endif

    if (da->_data == NULL) {
        return false;
    }
    return true;
}


#if DA_STORAGE == HR_STORAGE_DIRECT && !defined(DA_MEMCPY_ELEM)
DA_FUNC bool NAME_(_push)(DA_TYPE* da, DA_ELEM_TYPE elem)
#else
DA_FUNC bool NAME_(_push)(DA_TYPE* da, DA_ELEM_TYPE* elem)
#endif
{
    if (!NAME_(_conditional_resize)(da, da->_used + 1)) return false;

#if (DA_STORAGE == HR_STORAGE_DIRECT && defined(DA_MEMCPY_ELEM))
    DA_MEMCPY_ELEM(&da->_data[da->_used++], elem);
#elif DA_STORAGE == HR_STORAGE_OWNED_INDIRECT
    da->_data[da->_used] = (DA_ELEM_TYPE*)(DA_MALLOC_ELEM);
    DA_MEMCPY_ELEM(da->_data[da->_used], elem);
    da->_used++;
#else
    da->_data[da->_used++] = elem;
#endif

    return true;
}


#if DA_STORAGE == HR_STORAGE_DIRECT && !defined(DA_MEMCPY_ELEM)
DA_FUNC bool NAME_(_insert)(DA_TYPE* da, size_t pos, DA_ELEM_TYPE elem)
#else
DA_FUNC bool NAME_(_insert)(DA_TYPE* da, size_t pos, DA_ELEM_TYPE* elem)
#endif
{
    if (!NAME_(_conditional_resize)(da, pos)) return false;

    if (pos < da->_used) {
        DA_MEMMOVE_ELEM((&da->_data[pos + 1]), (&da->_data[pos]), (da->_used - pos));
        da->_used++;
    } else {
        da->_used = pos + 1;
    }

#if DA_STORAGE == HR_STORAGE_DIRECT && defined(DA_MEMCPY_ELEM)
    DA_MEMCPY_ELEM((&da->_data[pos]), (elem));
#elif DA_STORAGE == HR_STORAGE_OWNED_INDIRECT
    da->_data[pos] = (DA_ELEM_TYPE*)(DA_MALLOC_ELEM);
    DA_MEMCPY_ELEM((da->_data[pos]), (elem));
#else
    da->_data[pos] = elem;
#endif

    return true;
}


DA_FUNC void NAME_(_remove)(DA_TYPE* da, size_t pos) {
    if (pos < da->_used) {
#if DA_STORAGE == HR_STORAGE_OWNED_INDIRECT
        DA_FREE_ELEM((da->_data[pos]));
#endif
        DA_MEMMOVE_ELEM((&da->_data[pos]), (&da->_data[pos + 1]), (da->_used - pos - 1));
    }

    if (da->_used > 0) {
        da->_used--;
    }
}


#if DA_STORAGE == HR_STORAGE_DIRECT && !defined(DA_MEMCPY_ELEM)
DA_FUNC DA_ELEM_TYPE NAME_(_pop)(DA_TYPE* da)
#elif DA_STORAGE == HR_STORAGE_OWNED_INDIRECT || (DA_STORAGE == HR_STORAGE_DIRECT && defined(DA_MEMCPY_ELEM))
DA_FUNC void NAME_(_pop)(DA_TYPE* da, DA_ELEM_TYPE* dst)
#elif DA_STORAGE == HR_STORAGE_BORROWED_INDIRECT
DA_FUNC DA_ELEM_TYPE* NAME_(_pop)(DA_TYPE* da)
#endif
{
    assert(da->_used > 0);

#if DA_STORAGE == HR_STORAGE_DIRECT && defined(DA_MEMCPY_ELEM)
    da->_used--;
    DA_MEMCPY_ELEM((dst), (&da->_data[da->_used]));
#elif DA_STORAGE == HR_STORAGE_DIRECT && !defined(DA_MEMCPY_ELEM)
    return da->_data[--da->_used];
#elif DA_STORAGE == HR_STORAGE_OWNED_INDIRECT || (DA_STORAGE == HR_STORAGE_DIRECT && defined(DA_MEMCPY_ELEM))
    da->_used--;
    DA_MEMCPY_ELEM((dst), (da->_data[da->_used]));
    DA_FREE_ELEM((da->_data[da->_used]));
#elif DA_STORAGE == HR_STORAGE_BORROWED_INDIRECT
    return da->_used > 0 ? da->_data[--da->_used] : NULL;
#endif
}


DA_FUNC DA_ELEM_TYPE* NAME_(_addr)(DA_TYPE* da, size_t pos) {
    #if defined(DA_BOUNDS_CHECK)
    assert(pos < da->_used);
    #endif

#if DA_STORAGE == HR_STORAGE_DIRECT
    return &da->_data[pos];
#else
    return da->_data[pos];
#endif
}


DA_FUNC size_t NAME_(_size)(DA_TYPE* da) {
    return da->_used;
}

#endif


#undef DA_ELEM_TYPE
#undef DA_NAME
#undef DA_STORAGE
#undef DA_MALLOC_ELEM
#undef DA_FREE_ELEM
#undef DA_MEMCPY_ELEM
#undef DA_TYPE
#undef DA_SCOPE
#undef DA_FUNC
#undef NAME_
#undef DA_BOUNDS_CHECK
#undef DA_DEBUG
#undef DA_DEBUG_DUMP

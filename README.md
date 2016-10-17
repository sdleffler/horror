## Horror: Generic C data structures

As a disclaimer, I am perfectly aware that there are established libraries for data structures in C out there. They're great; they're optimized; they're probably not going to blow up in your face. I wrote this library to get the feel of C back into my fingers, and while it seems to pass all the unit tests (of which there are admittedly only a few here), it is probably not safe for any real use. Eventually, that may change as I will be making heavy use of the library as part of [laser](https://github.com/sdleffler/laser).

## Running tests

Tests can be run with `make test`.

## Notes

Currently supported structures:

### Red-black tree - `horror/rbtree.c`

Example usage (edited from test):

```
#include "horror/rbtree.h"

#define RB_SCOPE HR_SCOPE_STATIC_INLINE
#define RB_ELEM_TYPE int
#define RB_TYPE rb_int
#define RB_CMP(x, y) ((x) < (y) ? -1 : ((x) > (y) ? 1 : 0))
#include "horror/rbtree.c"
```

All `#define`'d symbols are automatically `#undef`'d. The options available for `horror/rbtree.c` are:

- `RB_ELEM_TYPE` - the type of the element stored. For the example above, the tree stores `int`s.
- `RB_NAME` - the generated red-black tree type will end up as `<RB_NAME>_t`, and generated functions will be `<RB_NAME>_<FUNCTION>`.
- `RB_CMP` - the ordering for the red-black tree. Can be a macro - it will never be called with effectful arguments, so no need to worry about using macro arguments multiple times. A value of less than zero indicates the first argument is less than the second, a value of zero indicates the two arguments are equal, and a value of greater than zero indicates the first argument is greater than the second. `horror/rbtree.c` expects the returned values to be signed `int`s.
- `RB_STORAGE` - the storage type for the red-black tree. `horror` supplies three different storage types for most structures - `HR_STORAGE_DIRECT`, `HR_STORAGE_OWNED_INDIRECT`, and `HR_STORAGE_BORROWED_INDIRECT`:
    - `RB_STORAGE=HR_STORAGE_DIRECT` - the values are stored directly in the nodes of the tree, no pointers or malloc'd memory aside from that allocated for the nodes themselves.
    - `RB_STORAGE=HR_STORAGE_OWNED_INDIRECT` - unless `RB_MALLOC_ELEM` and `RB_FREE_ELEM` are specified, defaults to `malloc` and `free` from `stdlib.h` to allocate memory to store the elements.
    - `RB_STORAGE=HR_STORAGE_BORROWED_INDIRECT` - this is for if you're just using the tree to get access to stuff you've allocated somewhere else. Basically the same as `HR_STORAGE_OWNED_INDIRECT`, but the tree won't allocate or free anything. A roughly similar effect to simply storing pointers.
- `RB_MALLOC_ELEM` - the allocator for elements. If unspecified, defaults to `malloc`. *Important: `RB_MALLOC_ELEM` is expected to be a macro! `horror/rbtree.c` uses `#if defined(RB_MALLOC_ELEM)` to check if it is supplied.* `horror/rbtree.c` will `#error` if `RB_MALLOC_ELEM` is supplied without a custom `RB_FREE_ELEM`.
- `RB_FREE_ELEM` - for defining custom release behavior. If unspecified, defaults to `free`. *Important: `RB_FREE_ELEM` is expected to be a macro! `horror/rbtree.c` uses `#if defined(RB_FREE_ELEM)` to check if it is supplied.* `horror/rbtree.c` will `#error` if `RB_FREE_ELEM` is supplied without a custom `RB_MALLOC_ELEM`.
- `RB_MEMCPY_ELEM` - if `RB_MEMCPY_ELEM` is supplied, then even if `RB_STORAGE` is set to `HR_STORAGE_DIRECT`, all functions which require an element to be supplied will instead expect a pointer, and `RB_MEMCPY_ELEM` will be used to copy that data into the node. If `RB_MEMCPY_ELEM` is supplied when `RB_STORAGE` is set to `HR_STORAGE_BORROWED_INDIRECT`, then copying data into newly allocated space will be done with `RB_MEMCPY_ELEM`. If `RB_STORAGE` is set to `HR_STORAGE_BORROWED_INDIRECT`, setting `RB_MEMCPY_ELEM` will have no effect. (TODO: cause `horror/rbtree.c` to `#error` if this is done.)
- `RB_MALLOC_NODE` - `horror/rbtree.c` uses `malloc` to allocate memory for nodes by default. If you want it to use something else, then `#define RB_MALLOC_NODE (my_allocator_here(sizeof(RB_NODE)))`.
- `RB_FREE_NODE` - see `RB_MALLOC_NODE`.
- `RB_TRAV` - the identifier used for the traversal iterator. If set, the new iterator struct will be named `<RB_TRAV>_t`.
- `RB_SCOPE` - the scope to generate the functions in:
    - `RB_SCOPE=HR_SCOPE_NONE` - no special scope.
    - `RB_SCOPE=HR_SCOPE_STATIC` - functions are declared as `static`.
    - `RB_SCOPE=HR_SCOPE_STATIC_INLINE` - functions are declared as `static inline`.
    - `RB_SCOPE=HR_SCOPE_HEADER` - only prototypes are declared.
    - `RB_SCOPE=HR_SCOPE_EXTERN_INLINE` - only prototypes are declared, and declared as `extern inline`.
- `RB_TRAV_DEPTH_MAX=64` - the maximum depth for the traversal iterator. (TODO: Make the traversal iterator capable of handling arbitrary depths.) `RB_TRAV_DEPTH_MAX=64` should be enough, since due to the balancing of the red-black tree, you'd have to have `2 log n + 1 = 64  =>  log n = 32  =>  n == 2^32` nodes to run out of space.

The full list of `#undefs` for `horror/rbtree.c` is:

- `#undef RB_ELEM_TYPE`
- `#undef RB_CMP`
- `#undef RB_STORAGE`
- `#undef RB_MALLOC_ELEM`
- `#undef RB_FREE_ELEM`
- `#undef RB_MEMCPY_ELEM`
- `#undef RB_MALLOC_NODE`
- `#undef RB_FREE_NODE`
- `#undef RB_TYPE`
- `#undef RB_NODE`
- `#undef RB_TRAV`
- `#undef RB_SCOPE`
- `#undef RB_FUNC`
- `#undef NAME_`
- `#undef RB_TRAV_DEPTH_MAX`
- `#undef RB_DEBUG`
- `#undef RB_DEBUG_DUMP`

Those of which do not appear above are used internally, but since the C preprocessor is a horrible, nasty, lumbering beast which should have been put down years ago, they still may affect your code if you use them. Please be careful.

### Dynamically resizable array - `horror/dynarray.c`

TODO: Allow the array to shrink its allocated memory. (Currently only grows with insertion.)

Example usage (edited from test):
```
#define DA_SCOPE HR_SCOPE_STATIC_INLINE
#define DA_ELEM_TYPE int
#define DA_NAME da_int
#include "horror/dynarray.c"
```

As with `horror/rbtree.c`, all `#define`'d options are automatically `#undef`'d. Unlike `horror/rbtree.c`, however, `horror/dynarray.c` does not have a header file (since all generated red-black trees share use of the `rbdir_t` type. TODO: Get rid of `horror/rbtree.c`'s header requirement.) The options available for `horror/dynarray.c` are:

- `DA_ELEM_TYPE` - the type of the element stored in the array.
- `DA_NAME` - like the red-black tree, the name of the generated type. (See `RB_NAME`.)
- `DA_STORAGE` - the storage type. If `DA_STORAGE` is set to anything other than `HR_STORAGE_DIRECT`, that makes the array store pointers instead of the actual data, which kind of voids the purpose of the array. So beware. (See `RB_STORAGE` for more information about storage types.)
- `DA_MALLOC_ELEM`, `DA_FREE_ELEM`, `DA_MEMCPY_ELEM` - custom allocation, release, and copy logic. Similar behavior to `RB_MALLOC_ELEM`, `RB_FREE_ELEM`, and `RB_MEMCPY_ELEM`.
- `DA_TYPE` - if you really don't want your dynamically resizable array type to be named `<DA_NAME>_t`, you can change this.
- `DA_SCOPE` - see `RB_SCOPE` for an explanation of the available function scope options.
- `DA_BOUNDS_CHECK` - if defined, calls to `assert` will be generated ensuring that accesses into the dynamic array are within bounds. If you want to rely on this, don't directly access the memory in the array - you can, but don't. Seriously.

The full list of `#undefs` for `horror/dynarray.c` is:

- `#undef DA_ELEM_TYPE`
- `#undef DA_NAME`
- `#undef DA_STORAGE`
- `#undef DA_MALLOC_ELEM`
- `#undef DA_FREE_ELEM`
- `#undef DA_MEMCPY_ELEM`
- `#undef DA_TYPE`
- `#undef DA_SCOPE`
- `#undef DA_FUNC`
- `#undef NAME_`
- `#undef DA_BOUNDS_CHECK`
- `#undef DA_DEBUG`
- `#undef DA_DEBUG_DUMP`

### Binary heap - `horror/heap.c`

TODO: Allow the heap to shrink its allocated memory. (Currently only grows with insertion.)

Example usage (edited from test):
```
#define HP_SCOPE HR_SCOPE_STATIC_INLINE
#define HP_ELEM_TYPE int
#define HP_NAME hp_int
#define HP_CMP(x, y) ((x) < (y) ? true : false)
#define HP_STORAGE HR_STORAGE_DIRECT
#include "horror/heap.c"
```

The options available for `horror/heap.c` are:

- `HP_ELEM_TYPE` - the type of the value to store in the heap.
- `HP_NAME` - see `RB_NAME` and `DA_NAME`.
- `HP_CMP` - the heap's comparison function should return a boolean value indicating whether one element precedes the other. For the provided example, the comparator given produces a min-heap.
- `HP_STORAGE` - see `DA_STORAGE`. Internally, `horror/heap.c` is very similar to `horror/dynarray.c`, so the behavior here is the same.
- `HP_MALLOC_ELEM` - see `DA_MALLOC_ELEM`.
- `HP_FREE_ELEM` - see `DA_FREE_ELEM`.
- `HP_TYPE` - see `DA_TYPE`.
- `HP_SCOPE` - see `RB_SCOPE`

The full list of `#undefs` for `horror/heap.c` is:

- `#undef HP_ELEM_TYPE`
- `#undef HP_NAME`
- `#undef HP_CMP`
- `#undef HP_STORAGE`
- `#undef HP_MALLOC_ELEM`
- `#undef HP_FREE_ELEM`
- `#undef HP_MEMCPY_ELEM`
- `#undef HP_TYPE`
- `#undef HP_SCOPE`
- `#undef HP_FUNC`
- `#undef NAME_`
- `#undef HP_PARENT`
- `#undef HP_LEFT`
- `#undef HP_RIGHT`
- `#undef HP_DEBUG`
- `#undef HP_DEBUG_DUMP`

## License (MIT)

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

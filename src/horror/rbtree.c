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

Red-black tree code is adapted and written with inspiration from Julienne
Walker's magnificent tutorials and red black tree library.

*/


#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "horror/macro.h"
#include "horror/rbtree.h"


#if !defined(RB_ELEM_TYPE)
    #error Error: Generic red-black tree requires RB_ELEM_TYPE to be defined.
    #define RB_ELEM_TYPE char // For purposes of testing.
#endif

#if !defined(RB_NAME)
    #error Error: Generic red-black tree requires RB_NAME to be defined.
    #define RB_NAME rb_char // For purposes of testing.
#endif

#if !defined(RB_CMP)
    #error Error: Generic red-black tree requires RB_CMP to be defined. RB_CMP \
must be a function or function-like macro which takes two arguments and \
returns a signed integer indicating how those two arguments are ordered. If \
the first argument is less than the second, it should return an integer less \
than zero; if the first argument is equal to the second, it should return \
zero; and if the first argument is greater than the second, it should return \
an integer greater than zero. It is okay to define RB_CMP as a macro which \
uses its arguments multiple times; it will never be called with effectful \
arguments. Thus, one acceptable definition of RB_CMP might be \
`#define RB_CMP(x, y) (x) < (y) ? -1 : ((x) > (y) ? 1 : 0)`.
    #define RB_CMP(x, y) (x) < (y) ? -1 : ((x) > (y) ? 1 : 0) // For purposes of testing.
#endif

#if RB_STORAGE == HR_STORAGE_OWNED_INDIRECT && (!defined(RB_MALLOC_ELEM) || !defined(RB_FREE_ELEM))
    #if defined(RB_MALLOC_ELEM)
        #error Error: Generic red-black tree is given a custom RB_MALLOC_ELEM, but \
not a custom RB_FREE_ELEM! This is dangerous, potentially even more dangerous than \
deciding to use this library in the first place. Please define a custom RB_FREE_ELEM.
    #endif

    #if defined(RB_FREE_ELEM)
        #error Error: Generic red-black tree is given a custom RB_FREE_ELEM, but \
not a custom RB_MALLOC_ELEM! This is dangerous, potentially even more dangerous \
than deciding to use this library in the first place. Please define a custom \
RB_MALLOC_ELEM.
    #endif

    #define RB_MALLOC_ELEM (malloc(sizeof(RB_ELEM_TYPE)))
    #define RB_FREE_ELEM(ptr) (free(ptr))
#endif

#if RB_STORAGE == HR_STORAGE_OWNED_INDIRECT && !defined(RB_MEMCPY_ELEM)
    #define RB_MEMCPY_ELEM(dst, src) (memcpy(dst, src, sizeof(RB_ELEM_TYPE)))
#endif

#if !defined(RB_MALLOC_NODE) || !defined(RB_FREE_NODE)
    #if defined(RB_MALLOC_NODE)
        #error Error: Generic red-black tree is given a custom RB_MALLOC_NODE, but \
not a custom RB_FREE_NODE! This is dangerous, potentially even more dangerous than \
deciding to use this library in the first place. Please define a custom RB_FREE_NODE.
    #endif

    #if defined(RB_FREE_NODE)
        #error Error: Generic red-black tree is given a custom RB_FREE_NODE, but \
not a custom RB_MALLOC_NODE! This is dangerous, potentially even more dangerous \
than deciding to use this library in the first place. Please define a custom \
RB_MALLOC_NODE.
    #endif

    #define RB_MALLOC_NODE (malloc(sizeof(RB_NODE)))
    #define RB_FREE_NODE(ptr) (free(ptr))
#endif

#if !defined(RB_TRAV_NAME)
    #define RB_TRAV_NAME HR_CONCAT(RB_NAME, _trav)
#endif

#if RB_SCOPE == HR_SCOPE_NONE
    #define RB_FUNC
#elif RB_SCOPE == HR_SCOPE_STATIC
    #define RB_FUNC static
#elif RB_SCOPE == HR_SCOPE_STATIC_INLINE
    #define RB_FUNC static inline
#elif RB_SCOPE == HR_SCOPE_EXTERN_INLINE
    #define RB_FUNC extern inline
#else
    #error Error: Generic red-black tree requires RB_SCOPE to be defined.
    #define RB_FUNC // For testing purposes.
#endif

#if !defined(RB_STORAGE)
    #error Error: Generic red-black tree requires RB_STORAGE to be defined.
    #define RB_STORAGE HR_STORAGE_DIRECT // For testing purposes.
#endif

#define NAME_(n) HR_CONCAT(RB_NAME, n)
#define RB_TYPE NAME_(_t)
#define RB_NODE NAME_(_node_t)
#define RB_TRAV HR_CONCAT(RB_TRAV_NAME, _t)

#if !defined(RB_TRAV_DEPTH_MAX)
#define RB_TRAV_DEPTH_MAX 64
#endif

typedef struct NAME_(_t) NAME_(_t);
typedef struct RB_TRAV RB_TRAV;

RB_FUNC void NAME_(_trav_init)(RB_TRAV* trav, RB_TYPE* tree, rbdir_t dec);
RB_FUNC RB_ELEM_TYPE* NAME_(_next)(RB_TRAV* trav);

RB_FUNC void NAME_(_init)(RB_TYPE* tree);
RB_FUNC void NAME_(_cleanup)(RB_TYPE* tree);

#if RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)
RB_FUNC const RB_ELEM_TYPE* NAME_(_find)(const RB_TYPE* tree, const RB_ELEM_TYPE data);
RB_FUNC bool NAME_(_insert)(RB_TYPE* tree, const RB_ELEM_TYPE elem);
RB_FUNC void NAME_(_remove)(RB_TYPE* tree, const RB_ELEM_TYPE elem);
#else
RB_FUNC const RB_ELEM_TYPE* NAME_(_find)(const RB_TYPE* tree, const RB_ELEM_TYPE* data);
RB_FUNC bool NAME_(_insert)(RB_TYPE* tree, const RB_ELEM_TYPE* elem);
RB_FUNC void NAME_(_remove)(RB_TYPE* tree, const RB_ELEM_TYPE* elem);
#endif

RB_FUNC size_t NAME_(_size)(RB_TYPE* tree);
RB_FUNC RB_ELEM_TYPE* NAME_(_min)(RB_TYPE* tree);
RB_FUNC RB_ELEM_TYPE* NAME_(_max)(RB_TYPE* tree);

#if RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)
RB_FUNC RB_ELEM_TYPE NAME_(_pop_min)(RB_TYPE* tree);
RB_FUNC RB_ELEM_TYPE NAME_(_pop_max)(RB_TYPE* tree);
#elif (RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)) || RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
RB_FUNC void NAME_(_pop_min)(RB_TYPE* tree, RB_ELEM_TYPE* dst);
RB_FUNC void NAME_(_pop_max)(RB_TYPE* tree, RB_ELEM_TYPE* dst);
#elif RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
RB_FUNC RB_ELEM_TYPE* NAME_(_pop_min)(RB_TYPE* tree);
RB_FUNC RB_ELEM_TYPE* NAME_(_pop_max)(RB_TYPE* tree);
#endif

#if defined(RB_DEBUG)
RB_FUNC bool NAME_(_assert)(RB_TYPE* tree);
#if defined(RB_DEBUG_DUMP)
RB_FUNC void NAME_(_dump)(RB_TYPE* tree);
#endif
#endif

#if RB_SCOPE != HR_SCOPE_HEADER && RB_SCOPE != HR_SCOPE_EXTERN_INLINE


typedef struct RB_NODE {
    enum { BLACK = 0, RED = 1 } color;
    struct RB_NODE* link[2];
#if RB_STORAGE != HR_STORAGE_DIRECT
    RB_ELEM_TYPE* data;
#else
    RB_ELEM_TYPE data;
#endif
} RB_NODE;


struct NAME_(_t) {
    size_t size;
    RB_NODE* root;
};


struct RB_TRAV {
    RB_NODE* stack[RB_TRAV_DEPTH_MAX];
    int8_t depth;
    bool dir;
};


RB_FUNC void NAME_(_trav_init)(RB_TRAV* trav, RB_TYPE* tree, rbdir_t dec) {
    if (tree->root == NULL) {
        trav->depth = -1;
    } else {
        trav->depth = 0;
        trav->stack[0] = tree->root;

        trav->dir = dec;

        while (trav->stack[trav->depth]->link[!dec] != NULL) {
            trav->stack[trav->depth + 1] = trav->stack[trav->depth]->link[!dec];
            trav->depth++;
        }
    }
}


RB_FUNC RB_ELEM_TYPE* NAME_(_next)(RB_TRAV* trav) {
    if (trav->depth < 0) {
        return NULL;
    }

#if RB_STORAGE != HR_STORAGE_DIRECT
    RB_ELEM_TYPE* data = trav->stack[trav->depth]->data;
#else
    RB_ELEM_TYPE* data = &trav->stack[trav->depth]->data;
#endif

    if (trav->stack[trav->depth]->link[trav->dir] == NULL) {
        do {
            trav->depth--;
        } while (trav->depth >= 0 && trav->stack[trav->depth]->link[trav->dir] == trav->stack[trav->depth + 1]);
    } else {
        trav->stack[trav->depth + 1] = trav->stack[trav->depth]->link[trav->dir];
        trav->depth++;

        while (trav->stack[trav->depth]->link[!trav->dir] != NULL) {
            trav->stack[trav->depth + 1] = trav->stack[trav->depth]->link[!trav->dir];
            trav->depth++;
        }
    }

    return data;
}


RB_FUNC void NAME_(_init)(RB_TYPE* tree) {
    tree->size = 0;
    tree->root = NULL;
}


RB_FUNC void NAME_(_cleanup)(RB_TYPE* tree) {
    RB_NODE *it = tree->root;
    RB_NODE *save;

    /*
    Rotate away the left links so that
    we can treat this like the destruction
    of a linked list
    */
    while (it != NULL) {
        if (it->link[0] == NULL) {
            /* No left links, just kill the node and move on */
            save = it->link[1];

#if RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
            RB_FREE_ELEM(it->data);
#endif

            RB_FREE_NODE(it);
        } else {
            /* Rotate away the left link and check again */
            save = it->link[0];
            it->link[0] = save->link[1];
            save->link[1] = it;
        }

        it = save;
    }
}


RB_FUNC bool NAME_(_is_red)(RB_NODE* node) {
    return node != NULL && node->color == RED;
}


#if RB_STORAGE == HR_STORAGE_DIRECT
RB_FUNC const RB_ELEM_TYPE* NAME_(_find)(const RB_TYPE* tree, const RB_ELEM_TYPE data) {
#else
RB_FUNC const RB_ELEM_TYPE* NAME_(_find)(const RB_TYPE* tree, const RB_ELEM_TYPE* data) {
#endif
    const RB_NODE* q = tree->root;

    rbdir_t dir;
    while (q != NULL && RB_CMP((q->data), data) != 0) {
        dir = RB_CMP((q->data), data) < 0;

        q = q->link[dir];
    }

#if RB_STORAGE != HR_STORAGE_DIRECT
    return q != NULL ? q->data : NULL;
#else
    return q != NULL ? &q->data : NULL;
#endif
}


RB_FUNC RB_NODE* NAME_(_single_rotate)(RB_NODE* root, rbdir_t dir) {
    RB_NODE* save = root->link[!dir];

    root->link[!dir] = save->link[dir];
    save->link[dir] = root;

    root->color = RED;
    save->color = BLACK;

    return save;
}


RB_FUNC RB_NODE* NAME_(_double_rotate)(RB_NODE* root, rbdir_t dir) {
    root->link[!dir] = NAME_(_single_rotate)(root->link[!dir], !dir);

    return NAME_(_single_rotate)(root, dir);
}


#if RB_STORAGE == HR_STORAGE_DIRECT
RB_FUNC bool NAME_(_insert)(RB_TYPE* tree, const RB_ELEM_TYPE data) {
#else
RB_FUNC bool NAME_(_insert)(RB_TYPE* tree, const RB_ELEM_TYPE* data) {
#endif
    if (tree->root == NULL) {
        // The tree is empty. We may attach directly to the root.
        tree->root = (RB_NODE*)(RB_MALLOC_NODE);

        if (tree->root == NULL) {
            return false;
        }

        tree->root->color = BLACK;
        tree->root->link[0] = NULL;
        tree->root->link[1] = NULL;

#if RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
        tree->root->data = (RB_ELEM_TYPE*)(RB_MALLOC_ELEM);
#endif

#if RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
        RB_MEMCPY_ELEM(tree->root->data, data);
#elif RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)
        RB_MEMCPY_ELEM(&tree->root->data, data);
#elif RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)
        tree->root->data = data;
#elif RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
        tree->root->data = (RB_ELEM_TYPE*)data;
#endif

        tree->size += 1;
    } else {
        RB_NODE head = { .color = BLACK };

        RB_NODE* t; // The great-grandparent of the current node.
        RB_NODE* g; // The grandparent of the current node.
        RB_NODE* p; // The parent of the current node.
        RB_NODE* q; // The current node.

        rbdir_t dir = RB_LEFT, last = RB_LEFT;

        t = &head;
        g = p = NULL;
        q = t->link[1] = tree->root;

        for (;;) {
            if (q == NULL) {
                // If our iterator is null, we have hit the bottom of the tree, and
                // we may insert a red node.
                p->link[dir] = q = (RB_NODE*)(RB_MALLOC_NODE);

                if (q == NULL) {
                    return false;
                }

                q->color = RED;
                q->link[0] = NULL;
                q->link[1] = NULL;

#if RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
                q->data = (RB_ELEM_TYPE*)RB_MALLOC_ELEM;
#endif

#if RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
                RB_MEMCPY_ELEM(q->data, data);
#elif RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)
                RB_MEMCPY_ELEM(&q->data, data);
#elif RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)
                q->data = data;
#elif RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
                q->data = (RB_ELEM_TYPE*)data;
#endif

                tree->size += 1;
            } else if (NAME_(_is_red)(q->link[0]) && NAME_(_is_red)(q->link[1])) {
                // If both children of the current node are red, then we may perform
                // a color flip. This pushes black nodes further down the tree. We
                // want black nodes as far down as we can get so that we can insert
                // our red node without complications.
                q->color = RED;
                q->link[0]->color = BLACK;
                q->link[1]->color = BLACK;
            }

            if (NAME_(_is_red)(q) && NAME_(_is_red)(p)) {
                // If we've performed a color flip, we may now have a red violation
                // in between our current node `q` and our parent node `p`. If this
                // is the case, we must perform a rotation.
                rbdir_t g_dir = t->link[1] == g;

                if (q == p->link[last]) {
                    // If we've gone in the same direction twice, then we can
                    // fix the color violation with a single rotation.
                    t->link[g_dir] = NAME_(_single_rotate)(g, !last);
                } else {
                    // But if we've done a zig-zag, we have to rotate to get
                    // things in a straight line and then rotate again to fix
                    // the violation.
                    t->link[g_dir] = NAME_(_double_rotate)(g, !last);
                }
            }

            int cmp = RB_CMP((q->data), (data));

            // If we have successfully inserted a node, we may break. As a side
            // effect of this logic, duplicates are silently ignored. (!!)
            if (cmp == 0) {
                break;
            }

            last = dir;
            dir = cmp < 0;

            // If we have a grandparent, it becomes the great-grandparent.
            if (g != NULL) {
                t = g;
            }

            // Similarly, the parent becomes the grandparent, and the current
            // node becomes the parent.
            g = p, p = q;

            // And finally, the node in the compared direction becomes the
            // current.
            q = q->link[dir];
        }

        // The root may have moved thanks to tree rotations. Better put it back
        // where it belongs.
        tree->root = head.link[1];
    }

    tree->root->color = BLACK;

    return true;
}


#if RB_STORAGE == HR_STORAGE_DIRECT
RB_FUNC void NAME_(_remove)(RB_TYPE* tree, const RB_ELEM_TYPE data) {
#else
RB_FUNC void NAME_(_remove)(RB_TYPE* tree, const RB_ELEM_TYPE* data) {
#endif
    if (tree->root != NULL) {
        // The tree isn't empty, so we have work to do.
        RB_NODE head = { .color = BLACK };

        RB_NODE* g; // The grandparent of the current node.
        RB_NODE* p; // The parent of the current node.
        RB_NODE* q; // The current node.
        RB_NODE* f = NULL; // The matching node, if found.

        rbdir_t dir = RB_RIGHT;

        q = &head;
        g = p = NULL;
        q->link[1] = tree->root;

        // While we search, we push a red node down the tree while maintaining
        // invariants so that the node we eventually remove is a red one.
        while (q->link[dir] != NULL) {
            // While we have not hit the edge of the tree:
            rbdir_t last = dir;

            // Move our iterators down a notch.
            g = p, p = q;
            q = q->link[dir];
            dir = RB_CMP((q->data), (data)) < 0;

            // If we found the node, save it for later. We have violations to
            // fix.
            if (RB_CMP((q->data), (data)) == 0) {
                f = q;
            }

            // If both the current node and the current node's child in the
            // direction of search is black:
            if (!NAME_(_is_red)(q) && !NAME_(_is_red)(q->link[dir])) {
                // If the sibling of said child node is red, then we can fix it
                // with a tree rotation.
                if (NAME_(_is_red)(q->link[!dir])) {
                    // We must update our parent iterator. This here works since
                    // last tells us which child of p q is. Neat.
                    p = p->link[last] = NAME_(_single_rotate)(q, dir);
                } else if (!NAME_(_is_red)(q->link[!dir])) {
                    // This is the sibling of our current node.
                    RB_NODE* s = p->link[!last];

                    if (s != NULL) {
                        if (!NAME_(_is_red)(s->link[0]) && !NAME_(_is_red)(s->link[1])) {
                            // We've got a tree with p as red, all children of p
                            // black, and all children of children of p black.
                            // We can thus safely set all children of p to red
                            // and change p to black.
                            p->color = BLACK;
                            s->color = RED;
                            q->color = RED;
                        } else {
                            rbdir_t g_dir = g->link[1] == p ? RB_RIGHT : RB_LEFT;

                            if (NAME_(_is_red)(s->link[last])) {
                                g->link[g_dir] = NAME_(_double_rotate)(p, last);
                            } else if (NAME_(_is_red)(s->link[!last])) {
                                g->link[g_dir] = NAME_(_single_rotate)(p, last);
                            }

                            q->color = g->link[g_dir]->color = RED;
                            g->link[g_dir]->link[0]->color = BLACK;
                            g->link[g_dir]->link[1]->color = BLACK;
                        }
                    }
                }
            }
        }

        if (f != NULL) {
#if RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
            RB_FREE_ELEM(f->data);
#endif

#if RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)
            RB_MEMCPY_ELEM(&f->data, &q->data);
#else
            f->data = q->data;
#endif

            p->link[p->link[1] == q] = q->link[q->link[0] == NULL];
            RB_FREE_NODE(q);

            tree->size -= 1;
        }

        tree->root = head.link[1];

        if (tree->root != NULL) {
            tree->root->color = BLACK;
        }
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"

#if RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)
RB_FUNC RB_ELEM_TYPE NAME_(_pop_min)(RB_TYPE* tree)
#elif (RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)) || RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
RB_FUNC void NAME_(_pop_min)(RB_TYPE* tree, RB_ELEM_TYPE* dst)
#elif RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
RB_FUNC RB_ELEM_TYPE* NAME_(_pop_min)(RB_TYPE* tree)
#endif
{
#if RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)
    RB_ELEM_TYPE ret;
#elif RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
    RB_ELEM_TYPE* ret = NULL;
#endif

    if (tree->root != NULL) {
        // The tree isn't empty, so we have work to do.
        RB_NODE head = { .color = BLACK };

        RB_NODE* g; // The grandparent of the current node.
        RB_NODE* p; // The parent of the current node.
        RB_NODE* q; // The current node.
        RB_NODE* f = NULL; // The matching node, if found.

        rbdir_t dir = RB_RIGHT;

        q = &head;
        g = p = NULL;
        q->link[1] = tree->root;

        // While we search, we push a red node down the tree while maintaining
        // invariants so that the node we eventually remove is a red one.
        while (q->link[dir] != NULL) {
            // While we have not hit the edge of the tree:
            rbdir_t last = dir;

            // Move our iterators down a notch.
            g = p, p = q;
            q = q->link[dir];
            dir = RB_LEFT;

            // If we found the node, save it for later. We have violations to
            // fix.
            if (q->link[dir] == NULL) {
                f = q;
            }

            // If both the current node and the current node's child in the
            // direction of search is black:
            if (!NAME_(_is_red)(q) && !NAME_(_is_red)(q->link[dir])) {
                // If the sibling of said child node is red, then we can fix it
                // with a tree rotation.
                if (NAME_(_is_red)(q->link[!dir])) {
                    // We must update our parent iterator. This here works since
                    // last tells us which child of p q is. Neat.
                    p = p->link[last] = NAME_(_single_rotate)(q, dir);
                } else if (!NAME_(_is_red)(q->link[!dir])) {
                    // This is the sibling of our current node.
                    RB_NODE* s = p->link[!last];

                    if (s != NULL) {
                        if (!NAME_(_is_red)(s->link[0]) && !NAME_(_is_red)(s->link[1])) {
                            // We've got a tree with p as red, all children of p
                            // black, and all children of children of p black.
                            // We can thus safely set all children of p to red
                            // and change p to black.
                            p->color = BLACK;
                            s->color = RED;
                            q->color = RED;
                        } else {
                            rbdir_t g_dir = g->link[1] == p ? RB_RIGHT : RB_LEFT;

                            if (NAME_(_is_red)(s->link[last])) {
                                g->link[g_dir] = NAME_(_double_rotate)(p, last);
                            } else if (NAME_(_is_red)(s->link[!last])) {
                                g->link[g_dir] = NAME_(_single_rotate)(p, last);
                            }

                            q->color = g->link[g_dir]->color = RED;
                            g->link[g_dir]->link[0]->color = BLACK;
                            g->link[g_dir]->link[1]->color = BLACK;
                        }
                    }
                }
            }
        }

        if (f != NULL) {
#if (RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)) || RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
            ret = f->data;
#elif RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)
            RB_MEMCPY_ELEM(dst, &f->data);
#elif RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
            RB_MEMCPY_ELEM(dst, f->data);
            RB_FREE_ELEM(f->data);
#endif

#if RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)
            RB_MEMCPY_ELEM(&f->data, &q->data);
#else
            f->data = q->data;
#endif

            p->link[p->link[1] == q] = q->link[q->link[0] == NULL];
            RB_FREE_NODE(q);

            tree->size -= 1;
        }

        tree->root = head.link[1];

        if (tree->root != NULL) {
            tree->root->color = BLACK;
        }
    }

#if (RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)) || RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
    return ret;
#endif
}


#if RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)
RB_FUNC RB_ELEM_TYPE NAME_(_pop_max)(RB_TYPE* tree)
#elif (RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)) || RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
RB_FUNC void NAME_(_pop_max)(RB_TYPE* tree, RB_ELEM_TYPE* dst)
#elif RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
RB_FUNC RB_ELEM_TYPE* NAME_(_pop_max)(RB_TYPE* tree)
#endif
{
#if RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)
    RB_ELEM_TYPE ret;
#elif RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
    RB_ELEM_TYPE* ret = NULL;
#endif

    if (tree->root != NULL) {
        // The tree isn't empty, so we have work to do.
        RB_NODE head = { .color = BLACK };

        RB_NODE* g; // The grandparent of the current node.
        RB_NODE* p; // The parent of the current node.
        RB_NODE* q; // The current node.
        RB_NODE* f = NULL; // The matching node, if found.

        rbdir_t dir = RB_RIGHT;

        q = &head;
        g = p = NULL;
        q->link[1] = tree->root;

        // While we search, we push a red node down the tree while maintaining
        // invariants so that the node we eventually remove is a red one.
        while (q->link[dir] != NULL) {
            // While we have not hit the edge of the tree:
            rbdir_t last = dir;

            // Move our iterators down a notch.
            g = p, p = q;
            q = q->link[dir];
            dir = RB_RIGHT;

            // If we found the node, save it for later. We have violations to
            // fix.
            if (q->link[dir] == NULL) {
                f = q;
            }

            // If both the current node and the current node's child in the
            // direction of search is black:
            if (!NAME_(_is_red)(q) && !NAME_(_is_red)(q->link[dir])) {
                // If the sibling of said child node is red, then we can fix it
                // with a tree rotation.
                if (NAME_(_is_red)(q->link[!dir])) {
                    // We must update our parent iterator. This here works since
                    // last tells us which child of p q is. Neat.
                    p = p->link[last] = NAME_(_single_rotate)(q, dir);
                } else if (!NAME_(_is_red)(q->link[!dir])) {
                    // This is the sibling of our current node.
                    RB_NODE* s = p->link[!last];

                    if (s != NULL) {
                        if (!NAME_(_is_red)(s->link[0]) && !NAME_(_is_red)(s->link[1])) {
                            // We've got a tree with p as red, all children of p
                            // black, and all children of children of p black.
                            // We can thus safely set all children of p to red
                            // and change p to black.
                            p->color = BLACK;
                            s->color = RED;
                            q->color = RED;
                        } else {
                            rbdir_t g_dir = g->link[1] == p ? RB_RIGHT : RB_LEFT;

                            if (NAME_(_is_red)(s->link[last])) {
                                g->link[g_dir] = NAME_(_double_rotate)(p, last);
                            } else if (NAME_(_is_red)(s->link[!last])) {
                                g->link[g_dir] = NAME_(_single_rotate)(p, last);
                            }

                            q->color = g->link[g_dir]->color = RED;
                            g->link[g_dir]->link[0]->color = BLACK;
                            g->link[g_dir]->link[1]->color = BLACK;
                        }
                    }
                }
            }
        }

        if (f != NULL) {
#if (RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)) || RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
            ret = f->data;
#elif RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)
            RB_MEMCPY_ELEM(dst, &f->data);
#elif RB_STORAGE == HR_STORAGE_OWNED_INDIRECT
            RB_MEMCPY_ELEM(dst, f->data);
            RB_FREE_ELEM(f->data);
#endif

#if RB_STORAGE == HR_STORAGE_DIRECT && defined(RB_MEMCPY_ELEM)
            RB_MEMCPY_ELEM(&f->data, &q->data);
#else
            f->data = q->data;
#endif

            p->link[p->link[1] == q] = q->link[q->link[0] == NULL];
            RB_FREE_NODE(q);

            tree->size -= 1;
        }

        tree->root = head.link[1];

        if (tree->root != NULL) {
            tree->root->color = BLACK;
        }
    }

#if (RB_STORAGE == HR_STORAGE_DIRECT && !defined(RB_MEMCPY_ELEM)) || RB_STORAGE == HR_STORAGE_BORROWED_INDIRECT
    return ret;
#endif
}

#pragma GCC diagnostic pop


RB_FUNC size_t NAME_(_size)(RB_TYPE* tree) {
    return tree->size;
}


RB_FUNC RB_ELEM_TYPE* NAME_(_min)(RB_TYPE* tree) {
    RB_NODE* q = tree->root;
    if (q == NULL) {
        return NULL;
    }

    while (q->link[RB_LEFT] != NULL) {
        q = q->link[RB_LEFT];
    }

#if RB_STORAGE != HR_STORAGE_DIRECT
    return q->data;
#else
    return &q->data;
#endif
}


RB_FUNC RB_ELEM_TYPE* NAME_(_max)(RB_TYPE* tree) {
    RB_NODE* q = tree->root;
    if (q == NULL) {
        return NULL;
    }

    while (q->link[RB_RIGHT] != NULL) {
        q = q->link[RB_RIGHT];
    }

#if RB_STORAGE != HR_STORAGE_DIRECT
    return q->data;
#else
    return &q->data;
#endif
}


#if defined(RB_DEBUG)
RB_FUNC bool NAME_(_assert_rec)(RB_TYPE* tree, RB_NODE *root)
{
    int lh, rh;

    if (root == NULL)
    {
        return 1;
    }
    else
    {
        RB_NODE *ln = root->link[0];
        RB_NODE *rn = root->link[1];

        /* Consecutive red links */
        if (NAME_(_is_red)(root))
        {
            if (NAME_(_is_red)(ln) || NAME_(_is_red)(rn))
            {
                fprintf(stderr, "Red violation\n");
                return 0;
            }
        }

        lh = NAME_(_assert_rec)(tree, ln);
        rh = NAME_(_assert_rec)(tree, rn);

        /* Invalid binary search tree */
        if ((ln != NULL && RB_CMP((ln->data), (root->data)) > 0) || (rn != NULL && RB_CMP((rn->data), (root->data)) < 0))
        {
            fprintf(stderr, "Binary tree violation: ");
            if (ln != NULL) {
                fprintf(stderr, "ln ");
                int cmp = RB_CMP((ln->data), (root->data));
                if (cmp < 0) fprintf(stderr, "<");
                else if (cmp == 0) fprintf(stderr, "==");
                else if (cmp > 0) fprintf(stderr, ">");
                fprintf(stderr, " root. ");
            }

            if (rn != NULL) {
                fprintf(stderr, "root ");
                int cmp = RB_CMP((root->data), (rn->data));
                if (cmp < 0) fprintf(stderr, "<");
                else if (cmp == 0) fprintf(stderr, "==");
                else if (cmp > 0) fprintf(stderr, ">");
                fprintf(stderr, " rn. ");
            }
            fprintf(stderr, "\n");

            return 0;
        }

        /* Black height mismatch */
        if (lh != 0 && rh != 0 && lh != rh)
        {
            fprintf(stderr, "Black violation\n");
            return 0;
        }

        /* Only count black links */
        if (lh != 0 && rh != 0)
        {
            return NAME_(_is_red)(root) ? lh : lh + 1;
        }
        else
        {
            return 0;
        }
    }
}


RB_FUNC bool NAME_(_assert)(RB_TYPE* tree) {
    return NAME_(_assert_rec)(tree, tree->root);
}

#if defined(RB_DEBUG_DUMP)
RB_FUNC void NAME_(_dump_rec)(RB_NODE* root) {
    if (root->link[RB_LEFT] != NULL) NAME_(_dump_rec)(root->link[RB_LEFT]);

    RB_DEBUG_DUMP(root->data);

    if (root->link[RB_RIGHT] != NULL) NAME_(_dump_rec)(root->link[RB_RIGHT]);
}

RB_FUNC void NAME_(_dump)(RB_TYPE* tree) {
    if (tree->root != NULL) NAME_(_dump_rec)(tree->root);
}
#endif

#endif

#endif

#undef RB_ELEM_TYPE
#undef RB_NAME
#undef RB_CMP
#undef RB_STORAGE
#undef RB_MALLOC_ELEM
#undef RB_FREE_ELEM
#undef RB_MEMCPY_ELEM
#undef RB_MALLOC_NODE
#undef RB_FREE_NODE
#undef RB_TYPE
#undef RB_NODE
#undef RB_TRAV
#undef RB_SCOPE
#undef RB_FUNC
#undef NAME_
#undef RB_TRAV_DEPTH_MAX
#undef RB_DEBUG
#undef RB_DEBUG_DUMP

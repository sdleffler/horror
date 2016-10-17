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

#if !defined(HR_MACRO_H)
#define HR_MACRO_H


#define HR_CONCAT(x, y) HR_CONCAT_HELPER(x, y)
#define HR_CONCAT_HELPER(x, y) x ## y

#define HR_PARENTHESIZED(...) __VA_ARGS__


#define HR_SCOPE_HEADER        1
#define HR_SCOPE_STATIC        2
#define HR_SCOPE_STATIC_INLINE 3
#define HR_SCOPE_EXTERN_INLINE 4


#define HR_STORAGE_DIRECT   0
#define HR_STORAGE_OWNED_INDIRECT 1
#define HR_STORAGE_BORROWED_INDIRECT 2


typedef int _horror_macro_h_please_the_gods;

#endif

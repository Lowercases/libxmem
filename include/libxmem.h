/**
 * Copyright (c) 2014-2017, Ignacio Nin <nachex@gmail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined(LIBXMEM_H)
#define LIBXMEM_H

#if !defined(ENABLE_LIBXMEM)
#warning ENABLE_LIBXMEM not explicitely defined -- defaulting to disabled
#endif

#if ENABLE_LIBXMEM

#include <account.h>

#define xmalloc(sz, ...) acc_malloc((sz), __FILE__, __LINE__, __VA_ARGS__)
#define xrealloc(ptr, sz) acc_realloc((ptr), (sz), __FILE__, __LINE__)
#define xfree(ptr) acc_free((ptr), __FILE__, __LINE__)

#define xstrdup(str) acc_strdup((str), __FILE__, __LINE__)
#define xstrndup(str, sz) acc_strndup((str), (sz), __FILE__, __LINE__)

#define character(ptr) acc_character(ptr)
#define xmem_set_reentrant() acc_set_reentrant()

#define check(ptr, base) acc_check(ptr, base, __FILE__, __LINE__)
#define checkr(ptr, sz, base) acc_checkr(ptr, sz, base, __FILE__, __LINE__)

#else

#define xmalloc(sz, ...) malloc(sz)
#define xrealloc realloc
#define xfree free

#define xstrdup strdup
#define xstrndup strndup

#define xmem_set_reentrant()

#define check(ptr, base)
#define checkr(ptr, sz, base)

#endif

#endif

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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include "store.h"

#if LIBXMEM_MEMLOG
FILE *memory_log;
#endif

int acc_init(void) __attribute__ ((constructor));
void acc_finalize(void);

int
acc_init(void) {
    atexit(acc_finalize);

#if LIBXMEM_MEMLOG
    memory_log = fopen("memory.log", "w");
#endif

    return 0;

}

void
acc_set_reentrant() {
    as_set_reentrant();

}

static int
acc_print_block(void *ptr, size_t sz, char txt[],
        char *file, int line, void *arg)
{
    fprintf(stderr, "- %lu bytes allocated in %s, line %d: txt `%s'\n",
            sz, file, line, txt);

    return 0;

}

void
acc_finalize(void) {
    if (!as_count())
        return;

#if LIBXMEM_MEMLOG
    if (memory_log)
        fclose(memory_log);
#endif

    fprintf(stderr, "%d allocated blocks exist on termination:\n", as_count());
    as_walk(acc_print_block, NULL);

}

void *
acc_malloc(size_t sz, char *file, int line, char txt[], ...) {
    va_list va;
    void *ret;
#if LIBXMEM_MEMLOG
    va_list vacopy;
#endif

    ret = malloc(sz);
    if (!ret)
        return NULL;

    va_start(va, txt);

#if LIBXMEM_MEMLOG
    if (memory_log) {
        fprintf(memory_log, "%p: allocated %lu bytes at %s line %d: ",
                ret, sz, file, line);
        va_copy(vacopy, va);
        vfprintf(memory_log, txt, vacopy);
        va_end(vacopy);
        fprintf(memory_log, "\n");
    }
#endif

    as_vadd(ret, sz, file, line, txt, va);
    va_end(va);

    return ret;

}

void *
acc_realloc(void *ptr, size_t sz, char *file, int line) {
    void *ret;

    ret = realloc(ptr, sz);
    if (!ret)
        return NULL;

#if MEMORY_ZEROING
    do {
        size_t oldsz;

        if (!as_get(ptr, &oldsz))
            break;

        // Only zero out the part that's not in the new array.
        // For the special case that's the same pointer, the difference yields 0
        if (ret > ptr)
            memset(ptr, 0, ret < ptr + oldsz ? ret - ptr : oldsz);
        else // if (ptr > ret)
            memset(ptr, 0, ptr < ret + oldsz ? ptr - ret : oldsz);

    }
    while (0);
#endif

    if (ptr)
        as_replace(ptr, ret, sz, file, line);
    else
        as_add(ret, sz, file, line, "realloced from NULL memory");

#if LIBXMEM_MEMLOG
    if (memory_log)
        fprintf(memory_log, "%p: reallocated %p to %lu bytes at %s line %d",
                ret, ptr, sz, file, line);
#endif

    return ret;

}

void
acc_free(void *ptr, char *file, int line) {
#if MEMORY_ZEROING
    do {
        size_t oldsz;

        if (!as_get(ptr, &oldsz))
            break;

        memset(ptr, 0, oldsz);

    }
    while (0);
#endif

#if LIBXMEM_MEMLOG
    if (memory_log)
        fprintf(memory_log, "%p: freed from %s line %d\n", ptr, file, line);
#endif
    
    if (!as_delete(ptr)) {
        printf("Aborting trying to delete %p, %s line %d\n", ptr, file, line);
        abort();
    }
    free(ptr);

}

char *
acc_strdup(const char *str, char *file, int line) {
    char *ret;

    ret = strdup(str);
    if (!ret)
        return NULL;

#if LIBXMEM_MEMLOG
    if (memory_log)
        fprintf(memory_log, "%p: strduped %lu bytes at %s line %d: ",
                ret, strlen(str) + 1, file, line);
#endif
    
    as_add(ret, strlen(str) + 1, file, line, "%s", str);

    return ret;

}

char *
acc_strndup(const char *str, size_t sz, char *file, int line) {
    char *ret;

    ret = strndup(str, sz);
    if (!ret)
        return NULL;

#if LIBXMEM_MEMLOG
    if (memory_log)
        fprintf(memory_log, "%p: strnduped %lu bytes at %s line %d: ",
                ret, sz, file, line);
#endif
    
    as_add(ret, strlen(str) + 1, file, line, "%s", ret);

    return ret;

}

char *
acc_character(const void *ptr) {
    return as_character(ptr);

}

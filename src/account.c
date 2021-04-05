/**
 * Copyright (c) 2014-2021, Ignacio Nin <nachex@gmail.com>
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

FILE *memory_log;

int acc_init(void) __attribute__ ((constructor));
void acc_finalize(void);

int
acc_init(void) {
    atexit(acc_finalize);

    return 0;

}

void
acc_set_reentrant() {
    as_set_reentrant();

}

int
acc_enable_memlog() {
    if (!memory_log)
        memory_log = fopen("memory.log", "w");
        // Ignore any errors, we'd have to abort

    return memory_log != 0;

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

    if (memory_log)
        fclose(memory_log);

    fprintf(stderr, "%d allocated %s on termination:\n", as_count(),
            as_count() == 1 ? "block exists" : "blocks exist");
    as_walk(acc_print_block, NULL);

}

void *
acc_malloc(size_t sz, char *file, int line, char txt[], ...) {
    va_list va;
    void *ret;
    va_list vacopy;

    ret = malloc(sz);
    if (!ret)
        return NULL;

    va_start(va, txt);

    if (memory_log) {
        fprintf(memory_log, "%p: allocated %lu bytes at %s line %d: ",
                ret, sz, file, line);
        va_copy(vacopy, va);
        vfprintf(memory_log, txt, vacopy);
        va_end(vacopy);
        fprintf(memory_log, "\n");
    }

    as_vadd(ret, sz, file, line, txt, va);
    va_end(va);

    return ret;

}

void *
acc_realloc(void *ptr, size_t sz, char *file, int line) {
    void *ret;
    size_t oldsz;

    if (ptr && !as_get(ptr, &oldsz)) {
        printf("Aborting trying to realloc %p, %s line %d; not found in "
                "storage\n", ptr, file, line);
        abort();
    }

    ret = realloc(ptr, sz);
    if (!ret)
        return NULL;

    if (ptr)
        as_replace(ptr, ret, sz, file, line);
    else
        as_add(ret, sz, file, line, "realloced from NULL memory");

    if (memory_log)
        fprintf(memory_log, "%p: reallocated %p to %lu bytes at %s line %d",
                ret, ptr, sz, file, line);

    return ret;

}

void
acc_free(void *ptr, char *file, int line) {
    do {
        size_t oldsz;

        if (!as_get(ptr, &oldsz))
            break;

        memset(ptr, 0, oldsz);

    }
    while (0);

    if (memory_log)
        fprintf(memory_log, "%p: freed from %s line %d\n", ptr, file, line);
    
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

    if (memory_log)
        fprintf(memory_log, "%p: strduped %lu bytes at %s line %d: ",
                ret, strlen(str) + 1, file, line);
    
    as_add(ret, strlen(str) + 1, file, line, "%s", str);

    return ret;

}

char *
acc_strndup(const char *str, size_t sz, char *file, int line) {
    char *ret;

    ret = strndup(str, sz);
    if (!ret)
        return NULL;

    if (memory_log)
        fprintf(memory_log, "%p: strnduped %lu bytes at %s line %d: ",
                ret, sz, file, line);
    
    as_add(ret, strlen(str) + 1, file, line, "%s", ret);

    return ret;

}

char *
acc_character(const void *ptr) {
    return as_character(ptr);

}

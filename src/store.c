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

#include "store.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <pthread.h>

#include "uthash.h"

/**
 * We'll store the used pointers in a simple list. Perhaps in the future it's
 * worthy to implement a hash, but for the time it's not -- and search.h is
 * frankly unusable.
 */

struct storage {
    void *ptr;
    size_t sz;

    char *txt;
    char *file;
    int line;

    UT_hash_handle hh;

} *storage;

int as_reentrant;
int walking;
pthread_mutex_t storage_mx = PTHREAD_MUTEX_INITIALIZER;

#define LOCK() \
    do { \
        if (as_reentrant) \
            pthread_mutex_lock(&storage_mx); \
    } while(0)
#define UNLOCK() \
    do { \
        if (as_reentrant) \
            pthread_mutex_unlock(&storage_mx); \
    } while(0)

void
as_create(void) {
    // Ironic?

}

void
as_set_reentrant(void) {
    as_reentrant = 1;

}

int
as_add(void *ptr, size_t sz, char *file, int line, const char txt[], ...) {
    va_list va;
    int ret;

    va_start(va, txt);
    ret = as_vadd(ptr, sz, file, line, txt, va);
    va_end(va);

    return ret;

}
    
int
as_vadd(void *ptr, size_t sz, char *file, int line,
        const char txt[], va_list args)
{
    va_list argscopy;
    struct storage *st;
    size_t flen;

    st = malloc(sizeof(struct storage));
    if (!st)
        abort();
    memset(st, 0, sizeof(struct storage));

    st->ptr = ptr;
    st->sz = sz;

    st->file = strdup(file);
    st->line = line;

    // Calculate the sz of format string
    va_copy(argscopy, args);
    flen = vsnprintf(NULL, 0, txt, args);
    st->txt = malloc(flen + 1);
    if (!st->txt)
        abort();
    vsnprintf(st->txt, flen + 1, txt, argscopy);
    st->txt[flen] = '\0';
    
    LOCK();
    HASH_ADD_PTR(storage, ptr, st);
    UNLOCK();

    return 1;

}

int
as_replace(void *prev, void *ptr, size_t sz, char *file, int line) {
    struct storage *curr;

    LOCK();
    HASH_FIND_PTR(storage, &prev, curr);
    if (!curr) {
        UNLOCK();
        return 0;
    }

    HASH_DEL(storage, curr);

    curr->ptr = ptr;
    curr->sz = sz;

    free(curr->file);
    curr->file = strdup(file);
    curr->line = line;

    HASH_ADD_PTR(storage, ptr, curr);
    UNLOCK();

    return 1;

}

int
as_get(const void *ptr, size_t *sz) {
    struct storage *curr;

    LOCK();
    HASH_FIND_PTR(storage, &ptr, curr);

    if (!curr) {
        UNLOCK();
        return 0;
    }

    *sz = curr->sz;
    UNLOCK();

    return 1;

}

char *
as_character(const void *ptr) {
    struct storage *curr;
    char *ret;

    if (!walking)
        LOCK();
    HASH_FIND_PTR(storage, &ptr, curr);

    if (!curr) {
        if (!walking)
            UNLOCK();
        return NULL;
    }

    // TODO: This is not entirely safe.
    ret = curr->txt;
    if (!walking)
        UNLOCK();
    return ret;

}

int
as_delete(void *ptr) {
    struct storage *curr;

    LOCK();
    HASH_FIND_PTR(storage, &ptr, curr);
    if (!curr) {
        UNLOCK();
        return 0;
    }

    HASH_DEL(storage, curr);
    UNLOCK();

    free(curr->file);
    free(curr->txt);
    free(curr);

    return 1;

}

int
as_walk(callback, arg)
    int (*callback)(void *ptr, size_t sz,
            char txt[], char *file, int line, void *arg);
    void *arg;
{
    struct storage *curr;

    LOCK();
    walking = 1;
    for (curr = storage; curr; curr = curr->hh.next)
        callback(curr->ptr, curr->sz, curr->txt, curr->file, curr->line, arg);
    walking = 0;
    UNLOCK();

    return 0;

}

int
as_count(void) {
    int r;

    LOCK();
    r = HASH_COUNT(storage);
    UNLOCK();

    return r;

}

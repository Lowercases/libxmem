/**
 * Copyright (c) 2014, Ignacio Nin <nachex@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "store.h"

#include <stdlib.h>
#include <string.h>

/**
 * We'll store the used pointers in a simple list. Perhaps in the future it's
 * worthy to implement a hash, but for the time it's not -- and search.h is
 * frankly unusable.
 */

struct storage {
    void *ptr;
    size_t sz;

    char *file;
    int line;

    struct storage *next;

} storage;

void
as_create(void) {
    // Ironic?

}

int
as_add(void *ptr, size_t sz, char *file, int line) {
    struct storage *st;

    st = malloc(sizeof(struct storage));
    if (!st)
        abort();
    memset(st, 0, sizeof(struct storage));

    st->ptr = ptr;
    st->sz = sz;

    st->file = strdup(file);
    st->line = line;
    
    st->next = storage.next;
    storage.next = st;

    return 1;

}

int
as_replace(void *prev, void *ptr, size_t sz, char *file, int line) {
    struct storage *curr;

    for (curr = storage.next; curr; curr = curr->next)
        if (curr->ptr == prev) {
            curr->ptr = ptr;
            curr->sz = sz;

            free(curr->file);
            curr->file = strdup(file);
            curr->line = line;

            return 1;

       }

    return 0;

}

int
as_delete(void *ptr) {
    struct storage *curr, *next;

    for (curr = &storage; curr->next; curr = curr->next)
        if (curr->next->ptr == ptr) {
            next = curr->next->next;

            free(curr->next->file);
            free(curr->next);

            curr->next = next;

            return 1;

        }

    return 0;

}

int
as_walk(callback, arg)
    int (*callback)(void *ptr, size_t sz, char *file, int line, void *arg);
    void *arg;
{
    struct storage *curr;

    for (curr = storage.next; curr; curr = curr->next)
        callback(curr->ptr, curr->sz, curr->file, curr->line, arg);

    return 0;

}

int
as_count(void) {
    struct storage *curr;
    int r;

    for (r = 0, curr = storage.next; curr; curr = curr->next)
        r ++;

    return r;

}

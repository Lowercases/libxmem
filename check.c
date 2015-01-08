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

#include <libxmem.h>
#include <account.h>
#include "store.h"

#include <stdlib.h>
#include <stdio.h>

void
acc_check(const void *ptr, const void *base, char file[], int line) {
    size_t sz;

    if (!as_get(base, &sz)) {
        fprintf(stderr, "Aborting: base %p not found trying to access pointer "
                "%p at %s line %d\n", base, ptr, file, line);
        abort();
    }

    if (ptr < base) {
        fprintf(stderr, "Aborting: base %p lesser than desired pointer "
                "%p at %s line %d\n", base, ptr, file, line);
        abort();
    }

    if (ptr >= base + sz) {
        fprintf(stderr, "Aborting: trying to access pointer %p which is %lu "
                "bytes after base %p of length %lu at %s line %d\n",
                ptr, ptr - base, base, sz, file, line);
        abort();
    }

}

void
acc_checkr(const void *ptr, size_t checksz, const void *base,
        char file[], int line)
{
    size_t sz;

    if (!as_get(base, &sz)) {
        fprintf(stderr, "Aborting: base %p not found trying to access range "
                "%p + %lu at %s line %d\n", base, ptr, checksz, file, line);
        abort();
    }

    if (ptr < base) {
        fprintf(stderr, "Aborting: base %p lesser than desired range start "
                "%p at %s line %d\n", base, ptr, file, line);
        abort();
    }

    if (ptr >= base + sz) {
        fprintf(stderr, "Aborting: trying to access range %p + %lu whose start "                "is %lu bytes after base %p of length %lu at %s line %d\n",
                ptr, checksz, ptr - base, base, sz, file, line);
        abort();
    }

    if (ptr + checksz > base + sz) {
        fprintf(stderr, "Aborting: trying to access range %p + %lu whose end "
            "is %lu bytes after end of base %p of length %lu at %s line %d\n",
            ptr, checksz, ptr + checksz - base - sz, base, sz, file, line);
        abort();
    }

}

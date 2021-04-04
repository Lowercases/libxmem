#define ENABLE_LIBXMEM 1
#include <libxmem.h>
#include <stdlib.h>

#define ALLOCATIONS 1024

int
main(int argc, char *argv) {
    int i, r;
    void *alloc[1024];

    srand(0);

    for (i = 0; i < 1000; i ++)
        alloc[i] = xmalloc(rand(), "Random allocation %d", i);

    r = rand() % ALLOCATIONS;
    xfree(alloc[r]);
    xfree(alloc[r]);

    return 0;

}

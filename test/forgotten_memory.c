#define ENABLE_LIBXMEM 1
#include <libxmem.h>

int
main(int argc, char *argv[]) {
    int i;

    for (i = 0; i < 15; i ++)
        xmalloc((i + 1) * 1024, "%dk bytes", i);

    return 0;

}

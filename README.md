# libxmem
Memory accounting for C projects development

libxmem provides alternatives for `malloc()`, `free()` and the like (in the form of `xmalloc()`, `xfree()`, etc.)
that besides reserving the memory (themselves relying on the standard library) keep an accounting of each reservation
together with size, user comment and source file and line. When the program exits, libxmem dumps on the screen a list
of still-reserved chunks, so each of them can be debugged. libxmem will also abort() on double free.


## Basic usage
In order to use it, replace every occurrence of `malloc()` with

```C
void *xmalloc(int sz, char format[], ...);
```

where `format` accepts printf-like format and arguments, in order to make it easier to detect any piece of memory left
behind. Every `free()` should be replaced with

```C
void xfree(void *ptr);
```

### Example
```C
// input.c
#define ENABLE_LIBXMEM 1
#include <libxmem.h>

int
main(int argc, char *argv[]) {
    char *string;

    string = xmalloc(1024, "%s", "Forgotten string");

    return 0;

}
```
results in
```
1 allocated blocks exist on termination:
- 1024 bytes allocated in input.c, line 8: txt `Forgotten string'
```

### Function list
The following are provided (drop-ins for functions without the `x` prefix):
```C
void *xmalloc(size_t sz, char format[], ...);
void *xrealloc(void *ptr, size_t sz);
void xfree(void *ptr);

char *xstrdup(char *str);
char *xstrndup(char *, size_t sz);
```
The following enable certain aspects of libxmem:
```C
char *character(void *ptr);     // Returns the text associated with an allocation
void xmem_set_reentrant(void);  // Set to reentrant mode, using locks. Essential for multithreading.
void xmem_enable_memlog(void);  // Enables a log to `memory.log` detailing every operation for debugging.
```
and the following work for access checks:
```C
void check(void *ptr, void *base);
void checkr(void *ptr, size_t sz, void *base);
```

## Access check
libxmem can also help checking access to the heap and ensure that every access is to a legal area (properly malloced),
which is useful in contexts where heavy array manipulation happens (contexts ripe for accessing errors). This is a much
deeper change that requires using libxmem from the design of the code, since every access to heap memory must be
surrounded by libxmem machinery.

The checks come in the form of two functions, `check()` and `checkr()`, for checking on access to a single memory point
(strictly, a byte) or rather a range.

In order to use it, before any access to heap memory, call
```C
void check(const void *ptr, const void *base);
```
where `ptr` is the pointer being accessed and `base` the reserved chunk as returned by `xmalloc()`.

In order to check for a range (for example, when copying a chunk), before accessing the range call
```C
void checkr(const void *ptr, size_t checksz, const void *base);
```
where `ptr` is the lower end of the range of size `checksz` being accessed and `base` is again the reserved chunk.

### Example
```C
#define ENABLE_LIBXMEM 1
#include <libxmem.h>
#include <stdio.h>

int
main(int argc, char *argv[]) {
    char *string;

    string = xmalloc(1024, "%s", "Short chunk");

    check(string + 512, string);
    printf("Ok to read: %x\n", string[512]);
    checkr(string + 512, 1024, string);     // Overlaps

    return 0;

}

```
results in
```
Ok to read: 0
Aborting: trying to access range 0x1db24a0 + 1024 whose end is 512 bytes after end of base 0x1db22a0 of length 1024 at input.c line 13
Aborted
```

## Multi-threading support
pthread mutex support for the internal storage is supported, but disabled by default. If libxmem is
going to be used from different threads, be sure to call
```C
void xmem_set_reentrant(void);
```
before multi-threaded use.

## Disabling libxmem after development
A release-type build shouldn't use libxmem, but removing it should be easier than removing all calls
to `xmalloc()`, `xfree()` or worse, `check()`. In order to disable it set
```C
#define ENABLE_LIBXMEM 0
```
and the functions will be redirected to the standard library counterparts.

## Integrating to an existing project

### System-wide or local install
If libxmem is installed system-wide, it can be used directly by `#include <libxmem.h>` and linking with `-lxmem`,
probably optionally during development phase. It can also be locally installed in the project, for example by using
`make install` having configured a local path with `./configure --prefix`. Refer to the source to find other ways of
compiling and linking against it.

### Autotools project
A sample `libxmem.m4` file is provided, which can be copied into the `m4/` directory of an autotools-enabled project.
After inclusion, the `LIBXMEM_MACROS` macro is defined which can be called directly from the project's `configure.ac`
file, which adds
```sh
--enable-memacc   # enable memory accounting (ENABLE_LIBXMEM=1)
--with-libxmem    # specify libxmem's path
```
to the project's `configure` script. This allows for using a globally or locally installed libxmem (by telling it the
path), and `--enable-memacc` will toggle `ENABLE_LIBXMEM` to 1, thus enabling libxmem.

There's also the option of including libxmem as a subproject (libxmem being itself an autotools project) via
`AC_CONFIG_SUBDIRS`. Refer to `libxmem.m4` to get ideas on how to accomplish this.

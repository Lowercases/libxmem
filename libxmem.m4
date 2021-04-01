#
# Copyright (c) 2014-2017, Ignacio Nin <nachex@gmail.com>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the copyright holder nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

AC_DEFUN([LIBXMEM_MACROS], [
dnl --with-libxmem
AC_ARG_WITH([libxmem],
            [AS_HELP_STRING([--with-libxmem],
              [enable support for libxmem])],
            [
                AS_IF([test "x$withval" != x], [
                   xmempath="$(realpath "$withval")"
                   LIBXMEM_CFLAGS="-I$xmempath/include"
                   LIBXMEM_LDFLAGS="-L$xmempath/lib"
                ])
                need_libxmem=yes
            ])

dnl --enable-memacc
AC_ARG_ENABLE([memacc],
    AS_HELP_STRING([--enable-memacc], [Enable memory accounting options]),
    AS_IF([test "x$enable_memacc" = xno],
        [enable_libxmem=0],
        [enable_libxmem=1]),
    [enable_libxmem=0]
)

dnl --enable-libxmem
AC_DEFINE_UNQUOTED([ENABLE_LIBXMEM], [$enable_libxmem],
    [Enable memory accounting])

dnl --enable-zeroing
AC_ARG_ENABLE([zeroing],
    AS_HELP_STRING([--enable-zeroing], [Enable freed memory zeroing]),
    if test "x$enable_zeroing" = xyes
    then
        AC_DEFINE([LIBXMEM_ZEROING], [1], [Enable memory zeroing on free])
    fi
)

dnl --enable-memlog
AC_ARG_ENABLE([memlog],
    AS_HELP_STRING([--enable-memlog], [Enable memory logging]),
    if test "x$enable_memlog" = xyes
    then
        AC_DEFINE([LIBXMEM_MEMLOG], [1], [Enable memory logging])
    fi
)

LIBXMEM=
AS_IF([test "x$enable_libxmem" = x1], [
      xmem_save_CFLAGS="$CFLAGS" 
      xmem_save_LDFLAGS="$LDFLAGS"
      CFLAGS="$CFLAGS $LIBXMEM_CFLAGS"
      LDFLAGS="$LDFLAGS $LIBXMEM_LDFLAGS"
      AC_CHECK_LIB([xmem], [main], [
        LIBXMEM="-lxmem"
         AC_DEFINE([HAVE_LIBXMEM], [1],
                   [Define if you have libxmem])
        ],
        [AC_MSG_FAILURE(
             [--enable-memacc was given, but test for libxmem failed])]
     )
     CFLAGS="$xmem_save_CFLAGS"
     LDFLAGS="$xmem_save_LDFLAGS"
     ],
)
AC_SUBST([LIBXMEM])
AC_SUBST([LIBXMEM_CFLAGS])
AC_SUBST([LIBXMEM_LDFLAGS])


dnl LIBXMEM_DEFINE_STUBS defines all libxmem functions to their stdlib
dnl counterparts. It's useful if you're only conditionally #including
dnl <libxmem.h>, but want to have xmalloc etc. to malloc etc.
AC_DEFUN([LIBXMEM_DEFINE_STUBS], [
  AS_IF([test "x$enable_libxmem" = x0], [
    AC_DEFINE([xmalloc(sz, ...)], [malloc(sz)], [Defined by libxmem.m4])
    AC_DEFINE([xrealloc], [realloc], [Defined by libxmem.m4])
    AC_DEFINE([xfree], [free], [Defined by libxmem.m4])

    AC_DEFINE([xstrdup], [strdup], [Defined by libxmem.m4])
    AC_DEFINE([xstrndup], [strndup], [Defined by libxmem.m4])

    AC_DEFINE([xmem_set_reentrant()], [], [Defined by libxmem.m4])
    AC_DEFINE([check(ptr, base)], [], [Defined by libxmem.m4])
    AC_DEFINE([checkr(ptr, sz, base)], [], [Defined by libxmem.m4])
  ])
])

])

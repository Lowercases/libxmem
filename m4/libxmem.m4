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
AC_ARG_ENABLE([memacc],
    AS_HELP_STRING([--enable-memacc], [Enable memory accounting options]),
    if test "x$enable_memacc" = xno
    then
        enable_libxmem=0
    else
        enable_libxmem=1
    fi,
    enable_libxmem=0
)
AC_DEFINE_UNQUOTED([ENABLE_LIBXMEM], [$enable_libxmem],
    [Enable memory accounting])

AC_ARG_ENABLE([zeroing],
    AS_HELP_STRING([--enable-zeroing], [Enable freed memory zeroing]),
    if test "x$enable_zeroing" = xyes
    then
        AC_DEFINE([LIBXMEM_ZEROING], [1], [Enable memory zeroing on free])
    fi
)
AC_ARG_ENABLE([memlog],
    AS_HELP_STRING([--enable-memlog], [Enable memory logging]),
    if test "x$enable_memlog" = xyes
    then
        AC_DEFINE([LIBXMEM_MEMLOG], [1], [Enable memory logging])
    fi
)
])

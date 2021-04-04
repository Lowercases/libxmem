#!/bin/bash

if test "x$1" = x
then
    echo >&2 "Syntax: $0 testname"
    exit 99
elif ! test -e "$(dirname "$0")/$1.expect"
then
    echo >&2 "$1.expect not found"
    exit 99
fi

"$(dirname "$0")/$1" 2>&1 | diff "$(dirname "$0")/$1.expect" - || exit 1

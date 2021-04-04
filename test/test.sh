#!/bin/bash

if test "x$1" = x
then
    echo >&2 "Syntax: $0 testname"
    exit 99
elif ! test -e "$(dirname "$0")/$1.expect"
then
    echo >&2 "$1.expect not found"
    exit 99
elif test -e "$(dirname "$0")/$1.rc"
then
    expectrc="$(cat "$(dirname "$0")/$1.rc")"
else
    expectrc=0
fi

ulimit -c 0
output="$("$(dirname "$0")/$1" 2>&1)"
rc="$?"

if test "$rc" != "$expectrc"
then
    echo >&2 "Expected exit value $expectrc, got $rc."
    exit 1
fi

echo "$output" | diff "$(dirname "$0")/$1.expect" - || exit 1

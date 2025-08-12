#!/bin/sh

# `$#` expands to the number of arguments and `$@` expands to the supplied `args`
printf '%d args:' "$#"
printf " '%s'" "$@"
printf '\n'

if /contract-verify/build/src/contractverify "$@"
then
    exit 0
else
    exit 1
fi

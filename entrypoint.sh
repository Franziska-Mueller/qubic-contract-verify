#!/bin/sh

# `$#` expands to the number of arguments and `$@` expands to the supplied `args`
printf '%d args:' "$#"
printf " '%s'" "$@"
printf '\n'

EXIT_CODE=$(/contract-verify/build/src/contractverify "$@")

if [ "${EXIT_CODE}" = "0" ]
then
    echo "success=true" >> $GITHUB_OUTPUT
else
    echo "success=false" >> $GITHUB_OUTPUT
fi

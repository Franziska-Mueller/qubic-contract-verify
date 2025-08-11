#!/bin/sh

# `$#` expands to the number of arguments and `$@` expands to the supplied `args`
printf '%d args:' "$#"
printf " '%s'" "$@"
printf '\n'

/contract-verify/build/src/contractverify $@

if [[ $? -eq 0 ]]
then
    echo "success=true" >> $GITHUB_OUTPUT
else
    echo "success=false" >> $GITHUB_OUTPUT
fi

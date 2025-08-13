#!/bin/sh

# `$#` expands to the number of arguments and `$@` expands to the supplied `args`
printf '%d args:' "$#"
printf " '%s'" "$@"
printf '\n'

# Split the provided input based on the comma delimiter
filepaths=$(echo "$1" | tr "," "\n")

for file in $filepaths
do
  if ! /contract-verify/build/src/contractverify "$file"
  then
    failed=true
  fi
done

if [ "$failed" = true ]
then
  exit 1
else
  exit 0
fi

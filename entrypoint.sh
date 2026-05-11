#!/bin/bash

# Split the provided input based on the comma delimiter
filepaths=$(echo "$1" | tr "," "\n")

for file in $filepaths
do
  if [[ $file == *"oracle_interface"* ]]
  then
    printf "Running contract verification on oracle interface '%s'\n" "$file"
    if ! /contract-verify/build/src/contractverify --oi "$file"
    then
      failed=true
    fi
  else
    printf "Running contract verification on contract '%s'\n" "$file"
    if ! /contract-verify/build/src/contractverify "$file"
    then
      failed=true
    fi
  fi
done

if [ "$failed" = true ]
then
  exit 1
else
  exit 0
fi

#!/bin/sh

# Split the provided input based on the comma delimiter
filepaths=$(echo "$1" | tr "," "\n")

for file in $filepaths
do
  printf "Running contract verification on '%s'\n" "$file"
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

#!/bin/bash
cd "$(dirname "$0")"
cd ..

# Add the license file.
files=$(find ./src -name "*.hpp" -o -name "*.cpp" -o -name "*.ipp")
for file in $files
do
  if ! grep -q Copyright $file
  then
    cat ./other/license_headers.cpp $file >$file.new && mv $file.new $file
  fi
done

# Use clang-format.
for file in $files
do
  clang-format -i $file
done

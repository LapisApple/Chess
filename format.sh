#!/bin/bash

find . -name "*.cpp" -o -name "*.h" | while read -r file
do
  clang-format -i "$file"
  cpplint "$file"
done
#! /bin/bash

echo -n > source.cpp

for f in include/*.h src/*.cpp; do
  if [ "$f" == "include/hid.h" ]; then
    continue
  fi
  if [ "$f" == "include/hid_host.h" ]; then
    continue
  fi
  echo "# 1 \"`basename $f`\"" >> source.cpp
  cat $f >> source.cpp
  echo >> source.cpp
done

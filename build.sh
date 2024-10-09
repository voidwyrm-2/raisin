#! /bin/sh

if [ -d "out" ]; then
    rm -rf "out"
fi

mkdir out

# shellcheck disable=SC2164
cd compiler
clang++ -o ../out/raisic main.cpp
# shellcheck disable=SC2103
cd ..
# shellcheck disable=SC2164
cd vm
clang++ -o ../out/raisin main.cpp
cd ..
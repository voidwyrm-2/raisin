#! /bin/sh

if [ -d "out" ]; then
    rm -rf "out"
fi

mkdir out

clear
echo "building compiler..."
# shellcheck disable=SC2164
cd compiler
if ! clang++ -o ../out/raisic main.cpp; then
    echo "compiler build failed, exiting"
    exit 1
fi
clear
echo "compiler built successfully"

# shellcheck disable=SC2103
cd ..

clear
echo "building virtual machine..."
# shellcheck disable=SC2164
cd vm
if ! clang++ -o ../out/raisin main.cpp; then
    echo "virtual machine build failed, exiting"
    exit 1
fi
clear
echo "virtual machine built successfully"

cd ..
clear
echo "compiler and virtual machine built successfully"
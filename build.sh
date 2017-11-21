#!/bin/bash

echo -n "Building Cortex..."
if [[ -d ./build ]]; then
	rm -rf ./build
fi
if [[ -d ./lib ]]; then
	rm -rf ./lib
fi
if [[ -e ./CMakeLists.txt.user ]]; then
	rm ./CMakeLists.txt.user
fi

mkdir build
cd build
cmake ../
make

echo "Done!"
exit

#!/bin/bash
#

readonly BUILD_FOLDER="build_minsizerel"

# delete previous build folder
if [ -d "$BUILD_FOLDER" ]; then
  rm -rf "$BUILD_FOLDER"
fi

mkdir -p "$BUILD_FOLDER"
pushd "$BUILD_FOLDER" > /dev/null 2>&1
cmake ../src -DCMAKE_BUILD_TYPE=MINSIZEREL
make
./lock_compare 16 1000000 10
popd > /dev/null 2>&1

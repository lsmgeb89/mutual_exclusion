#!/bin/bash
#

readonly NAME="multicore_project_1"
readonly RELEASE_FOLDER="${HOME}/${NAME}"
readonly RELEASE_ZIP="${HOME}/${NAME}.zip"

# delete previous release zip
if [ -f "$RELEASE_ZIP" ]; then
  rm "$RELEASE_ZIP"
fi

mkdir -p "$RELEASE_FOLDER"/src
# copy source files
cp -ar src/* "$RELEASE_FOLDER"/src
# copy readme.txt & build_run.sh
cp readme.txt build_run.sh "$RELEASE_FOLDER"
# package all files
pushd "${HOME}" > /dev/null 2>&1
zip -r "$RELEASE_ZIP" "$NAME"/*
chmod +x "$RELEASE_ZIP"
popd > /dev/null 2>&1

# delete release folder
if [ -d "$RELEASE_FOLDER" ]; then
  rm -rf "$RELEASE_FOLDER"
fi

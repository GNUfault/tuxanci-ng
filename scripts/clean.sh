#!/usr/bin/env bash
PWD=$(pwd)
[[ ${PWD/*\//} = scripts ]] && pushd .. > /dev/null || pushd . > /dev/null
# remove compiled files
[ -e "[Mm]akefile" ] && make clean

# remove folders
find ./ -type d -name CMakeFiles -print | xargs -i rm {} -rf
# some people build Tuxanci in speacial folder
rm -rf ./build

# remove cmake files
find ./ \( -type f -name \*.so -print , -type f -name \*.cmake -print , -type f -name CMakeCache.txt -print , -type f -name Makefile -print \) | grep -v "./cmake" | xargs -i rm {} -f
find ./ \( -type f -name install_manifest.txt -print , -type f -name cmake_install.cmake -print \) | xargs -i rm {} -f

# remove precompiled in files
find ./ -name \*.in -print | while read r; do rm -f ${r/\.in/}; done

# remove compiled binaries
find ./src -name tuxanci -print | xargs -i rm {} -f
find ./src -name tuxanci-server -print | xargs -i rm {} -f

# remove compiled gettext files
find ./ -name \*.gmo -print | xargs -i rm {} -f

# remove temporary backup files
find ./ -type f -name \*~ -print | xargs -i rm {} -f

# remove so libraries
find ./src/modules -type f -name mod\*.so\* | xargs -i rm {} -f
popd >> /dev/null

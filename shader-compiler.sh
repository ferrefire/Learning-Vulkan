#! /usr/bin/bash

path=$(pwd)

while ! test -d $path/shaders; do
cd ..
path=$(pwd)
done

if ! test -f $path/build/_deps/glslang-build/StandAlone/glslang; then
echo "ERROR: glslang not found."
exit 1
fi

for x in $@; do
if test -f $path/shaders/$x; then
"$path"/build/_deps/glslang-build/StandAlone/glslang -V $path/shaders/$x -o $path/shaders/$x.spv
fi
done

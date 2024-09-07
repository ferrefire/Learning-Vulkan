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

compile_shader () {
	"$path"/build/_deps/glslang-build/StandAlone/glslang -V $1 -o $1.spv
}

if [[ $@ == "ALL" ]] || [[ $@ == "all" ]]; then
	files="$path/shaders/*"
	for f in $files; do
		if ! [[ $f == *".spv" ]]; then
			compile_shader $f
			#echo $f
		fi
	done
	exit 1
fi

for x in $@; do
	if test -f $path/shaders/$x; then
		compile_shader $path/shaders/$x
	fi
done

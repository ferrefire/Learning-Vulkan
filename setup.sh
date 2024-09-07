#! /usr/bin/bash

path=$(pwd)

fresh=""
dev="-DDEV=ON"
check_found="-DCHECK_FOUND=OFF"

run_command () {

	if [[ $1 == "fresh" ]] || [[ $1 == "fr" ]]; then
		return
	elif [[ $1 == "check" ]] || [[ $1 == "chk" ]]; then
		return
	fi

	if [[ $1 == "all" ]]; then
		run_command "build"
		run_command "compile"
		run_command "run"

	elif [[ $1 == "redo" ]] || [[ $1 == "re" ]]; then
		run_command "clean"
		run_command "build"
		run_command "compile"

	elif [[ $1 == "build" ]] || [[ $1 == "bd" ]]; then
		if ! test -d $path/build; then
			mkdir build
		fi
		cd $path/build
		cmake $dev $check_found .. $fresh

	elif [[ $1 == "clean" ]] || [[ $1 == "cl" ]]; then
		rm -rf $path/build

	elif [[ $1 == "run" ]] || [[ $1 == "r" ]]; then
		if test -f $path/build/limitless; then
			cd $path/build
			./limitless
		else
			echo "ERROR: '"$path"/build/limitless' could not be found."
			exit 1
		fi

	elif [[ $1 == "compile" ]] || [[ $1 == "cmp" ]]; then
		if test -f $path/build/Makefile; then
			cd $path/build
			make -j
		else
			echo "ERROR: '"$path"/build/Makefile' could not be found."
			exit 1
		fi

	elif [[ $1 == "release" ]] || [[ $1 == "rel" ]]; then
		if test -d $path/build/_deps; then
			cd $path/build/_deps
			rm -rf vulkan-headers-src
			rm -rf vulkan-loader-src
			rm -rf glfw-src
			rm -rf glm-src
		else
			echo "ERROR: '"$path"/build/_deps' could not be found."
			exit 1
		fi

	else
		echo "ERROR: '"$1"' is not a valid command."
		echo "Use help for a list of valid commands."
		exit 1
	fi
}

if test -f setup.sh; then
	for x in $@; do
		if [[ $x == "fresh" ]] || [[ $x == "fr" ]]; then
			fresh="--fresh"
		elif [[ $x == "release" ]] || [[ $x == "rel" ]]; then
			dev="-DDEV=OFF"
		elif [[ $x == "check" ]] || [[ $x == "chk" ]]; then
			check_found="-DCHECK_FOUND=ON"
		fi
	done
	for x in $@; do
		run_command $x
	done
else
	echo "ERROR: please run the setup script from the root project directory."
	exit 1
fi
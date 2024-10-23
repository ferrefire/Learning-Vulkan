#! /usr/bin/bash

path=$(pwd)

fresh=""
dev="-DDEV=ON"
check_found="-DCHECK_FOUND=OFF"
release=0
arguments=""
vulkan_sdk_setup_path="/home/fmolenbe/Downloads/vulkansdk-linux-x86_64-1.3.296.0/1.3.296.0/"

set_env () {

	##ARCH="$(uname -m)"
	#VULKAN_SDK="$(dirname "$(readlink -f "${BASH_SOURCE:-$0}" )" )/VK_Layers"
	#export VULKAN_SDK
	#VK_ADD_LAYER_PATH="$VULKAN_SDK/layers${VK_ADD_LAYER_PATH:+:$VK_ADD_LAYER_PATH}"
	#export VK_ADD_LAYER_PATH

	cd $vulkan_sdk_setup_path
	#./setup-env.sh

	ARCH="$(uname -m)"
	VULKAN_SDK="$(dirname "$(readlink -f "${BASH_SOURCE:-$0}" )" )/$ARCH"
	export VULKAN_SDK
	PATH="$VULKAN_SDK/bin:$PATH"
	export PATH
	LD_LIBRARY_PATH="$VULKAN_SDK/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
	export LD_LIBRARY_PATH
	VK_ADD_LAYER_PATH="$VULKAN_SDK/share/vulkan/explicit_layer.d${VK_ADD_LAYER_PATH:+:$VK_ADD_LAYER_PATH}"
	export VK_ADD_LAYER_PATH

	#$vulkan_sdk_setup_path/setup-env.sh

	echo $VULKAN_SDK
}

run_command () {

	if [[ $1 == "fresh" ]] || [[ $1 == "fr" ]]; then
		return
	elif [[ $1 == "check" ]] || [[ $1 == "chk" ]]; then
		return
	elif [[ $1 == "release" ]] || [[ $1 == "rel" ]]; then
		return
	elif [[ $1 == "-"* ]]; then
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
		echo "_______________________"
		echo "|                     |"
		echo "| building project... |"
		echo "|_____________________|"
		echo "                       "
		if ! test -d $path/build; then
			mkdir build
		fi
		cd $path/build
		mkdir VK_Layers
		cmake $dev .. $fresh
		#cmake $dev $check_found .. $fresh

	elif [[ $1 == "clean" ]] || [[ $1 == "cl" ]]; then
		rm -rf $path/build

	elif [[ $1 == "run" ]] || [[ $1 == "r" ]]; then
		if [[ $OSTYPE == "linux-gnu" ]]; then
			if test -f $path/build/limitless; then
				set_env
				cd $path/build
				./limitless $args
			else
				echo "ERROR: '"$path"/build/limitless' could not be found."
				exit 1
			fi
		elif [[ $OSTYPE == "msys" ]]; then
			if test -f $path/build/Release/limitless; then
				set_env
				cd $path/build
				./Release/limitless $args
			else
				echo "ERROR: '"$path"/build/Release/limitless' could not be found."
				exit 1
			fi
		fi
	elif [[ $1 == "compile" ]] || [[ $1 == "cmp" ]]; then
		echo "________________________"
		echo "|                      |"
		echo "| compiling project... |"
		echo "|______________________|"
		echo "                        "
		if [[ $OSTYPE == "linux-gnu" ]]; then
			if test -f $path/build/Makefile; then
				cd $path/build
				make -j
			else
				echo "ERROR: '"$path"/build/Makefile' could not be found."
				exit 1
			fi
		elif [[ $OSTYPE == "msys" ]]; then
			cd $path/build
			cmake --build . --config Release
		fi
		if [[ $release == 1 ]]; then
			if test -d $path/build/_deps; then
				cd $path/build/_deps
				rm -rf vulkan-headers-src
				rm -rf vulkan-loader-src
				rm -rf glfw-src
				rm -rf glm-src
				rm -rf stb_image-src
			else
				echo "ERROR: '"$path"/build/_deps' could not be found."
				exit 1
			fi
		fi

	#elif [[ $1 == "release" ]] || [[ $1 == "rel" ]]; then
	#	if test -d $path/build/_deps; then
	#		cd $path/build/_deps
	#		rm -rf vulkan-headers-src
	#		rm -rf vulkan-loader-src
	#		rm -rf glfw-src
	#		rm -rf glm-src
	#	else
	#		echo "ERROR: '"$path"/build/_deps' could not be found."
	#		exit 1
	#	fi

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
			release=1
		elif [[ $x == "check" ]] || [[ $x == "chk" ]]; then
			check_found="-DCHECK_FOUND=ON"
		elif [[ $x == "-"* ]]; then
			args+=${x/-/}
			args+=" "
		fi
	done
	for x in $@; do
		run_command $x
	done
else
	echo "ERROR: please run the setup script from the root project directory."
	exit 1
fi
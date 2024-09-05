# /usr/bin/bash

path=$(pwd)

fresh=""
dev="-DDEV=ON"

run_command () {

	if [[ $1 == "FRESH" ]]; then
	return
	#elif [[ $1 == "DEV" ]]; then
	#return
	fi

	if [[ $1 == "RE" ]]; then
	run_command "CLEAN"
	run_command "BUILD"
	run_command "COMPILE"

	elif [[ $1 == "BUILD" ]]; then
	if ! test -d $path/build; then
	mkdir build
	fi
	cd $path/build
	cmake $dev .. $fresh

	elif [[ $1 == "CLEAN" ]]; then
	rm -rf $path/build
	#rm -rf $path/shaders/*.spv

	elif [[ $1 == "RUN" ]]; then
	if test -f $path/build/limitless; then
	cd $path/build
	./limitless
	else
	echo "ERROR: '"$path"/build/limitless' could not be found."
	exit 1
	fi

	elif [[ $1 == "COMPILE" ]]; then
	if test -f $path/build/Makefile; then
	cd $path/build
	make -j
	else
	echo "ERROR: '"$path"/build/Makefile' could not be found."
	exit 1
	fi

	elif [[ $1 == "RELEASE" ]]; then
	if test -d $path/build/_deps; then
	cd $path/build/_deps
	rm -rf vulkan-headers-src
	rm -rf vulkan-loader-src
	else
	echo "ERROR: '"$path"/build/_deps' could not be found."
	exit 1
	fi

	else
	echo "ERROR: '"$1"' is not a valid command."
	echo "Use HELP for a list of valid commands."
	exit 1
	fi
}

if test -f setup.sh; then

for x in $@; do

if [[ $x == "FRESH" ]]; then
fresh="--fresh"
elif [[ $x == "RELEASE" ]]; then
dev="-DDEV=OFF"
fi

done

for x in $@; do

run_command $x

done

else
echo "ERROR: please run the setup script from the root project directory."
exit 1

fi
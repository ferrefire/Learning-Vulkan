# /usr/bin/bash

if [[ $@ == "FULL" ]]
then
rm -rf build
mkdir build
cd build
cmake ..
make -j

elif [[ $@ == "FRESH" ]]
then
mkdir build
cd build
cmake .. --fresh
make -j

elif [[ $@ == "CLEAN" ]]
then
rm -rf build
else
echo "Please enter a valid command:
FULL:
FRESH:
CLEAN:"

fi
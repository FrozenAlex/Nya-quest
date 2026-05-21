#!/bin/sh

rm Nya.qmod

echo "Installing Deps"
qpm restore

echo "Removing oldness"
rm -rf build
rm -rf logs
rm -rf CMakeCache.txt
rm -rf CMakeFiles

echo "Bulding da dir"
mkdir -p build
cd build

echo "Log da dir"
mkdir -p ../logs

set -e

echo "cmaking it"
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=$HOME/android/ndk/27.3.13750724/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-24 \
  -DCMAKE_BUILD_TYPE=Release \
  -G Ninja | tee ../logs/cmake_log.txt

echo "Building funnies"
ninja | tee ../logs/ninja_log.txt

echo "Generating mod.json"
cd ..
qpm qmod manifest
cd build

echo "Zipping bombing it"
mkdir -p Nya
cp libNya.so Nya
cp ../mod.json Nya
cp ../cover.jpg Nya

cd Nya

zip -r Nya.zip *

mv Nya.zip ../../Nya.qmod

rm -rf Nya

echo "Done :3"

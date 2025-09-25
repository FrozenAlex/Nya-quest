#!/bin/sh

rm Nya.qmod

echo "Removing oldness"
rm -rf build

echo "Bulding da dir"
mkdir -p build
cd build

echo "cmaking it"
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=/home/dia/android/ndk/27.2.12479018+preview-0/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-24 \
  -DCMAKE_BUILD_TYPE=Release \
  -G Ninja

echo "Building funnies"
ninja

echo "Zipping bombing it"
mkdir -p Nya
cp libNya.so Nya
cp ../mod.json Nya
cp ../cover.jpg Nya

cd Nya

zip -r Nya.zip *

mv Nya.zip ../../Nya.qmod

echo "Done :3"
#!/bin/bash -ex

cd /yuzu

ccache -s

mkdir build || true && cd build
cmake .. -G Ninja -DDISPLAY_VERSION=$1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++ -DYUZU_ENABLE_COMPATIBILITY_REPORTING=${ENABLE_COMPATIBILITY_REPORTING:-"OFF"} -DENABLE_COMPATIBILITY_LIST_DOWNLOAD=ON -DUSE_DISCORD_PRESENCE=ON -DENABLE_QT_TRANSLATION=ON

ninja

ccache -s

ctest -VV -C Release

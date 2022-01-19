#!/bin/bash
set -e
set -o pipefail

apt-get update && \
DEBIAN_FRONTEND=noninteractive apt-get -y install sudo git build-essential apt-utils x264 cmake libcpprest-dev libfmt-dev libnss3-dev libcups2-dev libxdamage-dev libxcomposite-dev libatk1.0-dev libatk-bridge2.0-dev libxkbcommon-dev libxrandr-dev libgbm-dev libasound2-dev libgstreamer-plugins-base1.0-dev gstreamer1.0-libav gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly meson ninja-build
 
cd /tmp &&
git clone https://github.com/centricular/gstcefsrc.git && \
cd gstcefsrc && mkdir build && cd build && \
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .. && \
make -j4 && make install
 
cd /tmp/ && git clone --branch v3.9.1 https://github.com/nlohmann/json.git && cd json && meson build && ninja -C build install
   
cd /tmp && git clone https://github.com/catchorg/Catch2.git && cd Catch2 && git checkout v2.13.0 && cmake -Bbuild -H. -DBUILD_TESTING=OFF && sudo cmake --build build/ --target install
    
rm -rf /tmp/*


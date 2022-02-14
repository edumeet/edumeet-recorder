#!/bin/bash
set -e
set -o pipefail

apt-get update && \
DEBIAN_FRONTEND=noninteractive apt-get -y install sudo git build-essential apt-utils x264 cmake libcpprest-dev libfmt-dev libnss3-dev libcups2-dev libxdamage-dev libxcomposite-dev libatk1.0-dev libatk-bridge2.0-dev libxkbcommon-dev libxrandr-dev libgbm-dev libasound2-dev xvfb python3-pip bison flex

pip3 install meson ninja

cd /tmp &&
git clone https://gitlab.freedesktop.org/gstreamer/gstreamer.git && \
cd  gstreamer && \
git checkout 1.20.0 && \
meson --buildtype release \
    -Dgtk_doc=disabled \
    -Ddoc=disabled \
    -Dtests=disabled \
    -Dexamples=disabled \
    -Dpython=disabled \
    -Dlibav=enabled \
    -Dugly=enabled \
    -Dbad=enabled \
    -Ddevtools=disabled \
    -Drs=disabled \
    -Dgpl=enabled \
    -Dges=disabled \
    -Drtsp_server=disabled \
    -Dvaapi=disabled \
    --strip build && \
ninja -C build install && \
ldconfig
 
cd /tmp &&
git clone https://github.com/centricular/gstcefsrc.git && \
cd gstcefsrc && mkdir build && cd build && \
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .. && \
make -j4 && make install
 
cd /tmp/ && git clone --branch v3.9.1 https://github.com/nlohmann/json.git && cd json && meson build && ninja -C build install
   
cd /tmp && git clone https://github.com/catchorg/Catch2.git && cd Catch2 && git checkout v2.13.0 && cmake -Bbuild -H. -DBUILD_TESTING=OFF && sudo cmake --build build/ --target install
    
rm -rf /tmp/*


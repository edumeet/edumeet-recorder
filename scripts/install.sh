apt-get update && \
DEBIAN_FRONTEND=noninteractive apt-get -y install sudo git build-essential libwpe-1.0-dev  gtk-doc-tools apt-utils g++ gobjc++ liborc-0.4-dev x264 apt-utils pkg-config autoconf automake autotools-dev libtool libev-dev python3-dev libmount-dev nasm libva-dev libssl-dev libglfw3-dev libgles2-mesa-dev libpulse-dev gettext libgirepository1.0-dev libfaac-dev libopus-dev libvo-aacenc-dev libsrtp2-dev libusrsctp-dev libnice-dev libx11-xcb-dev libglu1-mesa-dev flex bison libxkbcommon-dev libwayland-egl-backend-dev libegl1-mesa-dev libgl1-mesa-dev libwayland-dev libglib2.0-dev wget cmake ruby libcairo2-dev libharfbuzz-dev libjpeg-dev libepoxy-dev libgcrypt20-dev libsoup2.4-dev libsqlite3-dev libwebp-dev libatk1.0-dev libatk-bridge2.0-dev libopenjp2-7-dev libwoff-dev libtasn1-6-dev libxslt1-dev libsystemd-dev python wayland-protocols xdg-dbus-proxy ccache weston libcpprest-dev libfmt-dev libvpx-dev bubblewrap libseccomp-dev gperf libevent-dev libalsa-ocaml-dev libavcodec-dev python3-pip
 
pip3 install meson ninja

CCACHE_DIR "/ccache"

cd /tmp/ && git clone --branch v3.9.1 https://github.com/nlohmann/json.git && cd json && meson build && ninja -C build install

cd /tmp && \    
    wget https://wpewebkit.org/releases/libwpe-1.8.0.tar.xz && \
    tar -xf libwpe-1.8.0.tar.xz && \
    cd libwpe-1.8.0 && \
    cmake -DCMAKE_BUILD_TYPE=Release . && make && make install && ldconfig

cd /tmp && \
    wget https://wpewebkit.org/releases/wpebackend-fdo-1.8.0.tar.xz && \
    tar -xf wpebackend-fdo-1.8.0.tar.xz && \
    cd wpebackend-fdo-1.8.0 && \
    cmake -DCMAKE_BUILD_TYPE=Release . && make && make install && ldconfig
    
cd /tmp && git clone https://github.com/catchorg/Catch2.git && cd Catch2 && git checkout v2.13.0 && cmake -Bbuild -H. -DBUILD_TESTING=OFF && sudo cmake --build build/ --target install

cd /tmp/ && git clone --branch 1.18 https://gitlab.freedesktop.org/gstreamer/gst-build.git

cd /tmp/ && \
    cd gst-build && \
    meson --buildtype=release --prefix=/usr \
    -Dgtk_doc=disabled \
    -Dpython=disabled \
    -Dlibav=enabled \
    -Dugly=enabled \
    -Dbad=enabled \
    -Ddevtools=enabled \
    -Dges=disabled \
    -Drtsp_server=disabled \
    -Dvaapi=enabled \
    build && \
  cd subprojects/gst-plugins-bad && cp /wpe.patch . && git apply wpe.patch && \
  cd ../.. && \
  ninja -C build && \
  meson install -C build && \
  sudo ldconfig
    
cd /tmp && git clone --depth 1 --branch webkit-2.30 https://github.com/Igalia/webkit.git WebKit && \
    cd /tmp/WebKit && \
    cp /pulseaudio.patch . && git apply pulseaudio.patch && \
    cmake -DPORT=WPE -DENABLE_WEB_RTC=ON -DENABLE_MINIBROWSER=OFF -DCMAKE_BUILD_TYPE=Release -DENABLE_MEDIA_STREAM=ON -DENABLE_WPE_QT_API=OFF -GNinja . && \
    ninja -j6 && ninja install && ldconfig && \
    rm -rf /tmp/WebKit
cd /tmp/gst-build &&  meson build --reconfigure && ninja -C build && meson install -C build && ldconfig && rm -rf /tmp/*


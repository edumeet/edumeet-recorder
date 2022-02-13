FROM ubuntu:21.04
COPY scripts /
RUN ./install.sh
ENV GST_PLUGIN_PATH=/usr/local
COPY src /src
COPY CMakeLists.txt /
RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j && rm -rf /src /CMakeLists.txt
ENTRYPOINT ["/entrypoint.sh"]

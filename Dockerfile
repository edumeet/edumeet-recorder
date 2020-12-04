FROM ubuntu:20.04
COPY scripts /
RUN ./install.sh
RUN apt-get update && apt-get install -y pulseaudio-utils pulseaudio
RUN adduser root pulse-access
COPY src /src
COPY CMakeLists.txt /
RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j && rm -rf /src /CMakeLists.txt
ENTRYPOINT ["/entrypoint.sh"]

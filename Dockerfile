FROM ubuntu:latest
RUN apt update && apt upgrade -y && apt install g++ cmake git make -y
RUN git clone --recurse-submodules https://github.com/Koihik/LuaFormatter.git /tmp/luaformatter && \
    cd /tmp/luaformatter && \
    mkdir build && \
    cd build && \
    cmake .. -DBUILD_TESTS=FALSE && \
    cmake --build . --target install && \
    rm -rf /tmp/luaformatter
RUN apt remove g++ cmake git make -y && rm -rf /var/lib/apt/lists/*

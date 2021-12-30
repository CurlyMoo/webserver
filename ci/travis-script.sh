#!/bin/bash -e

if [[ ${PLATFORM} == "Unix" ]]; then
    mkdir -p build
    cd build || exit 1

    cmake -DCOVERALLS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-DWEBSERVER_MAX_SENDLIST=10 -DWEBSERVER_SENDLIST_BUFSIZE=0" ..
    make

    ./start

    cmake -DCOVERALLS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-DWEBSERVER_MAX_SENDLIST=0 -DWEBSERVER_SENDLIST_BUFSIZE=32000" ..
    make

    ./start

    cmake -DCOVERALLS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-DWEBSERVER_MAX_SENDLIST=10 -DWEBSERVER_SENDLIST_BUFSIZE=32000" ..
    make

    ./start

    cmake -DCOVERALLS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-DWEBSERVER_MAX_SENDLIST=0 -DWEBSERVER_SENDLIST_BUFSIZE=0" ..
    make

    ./start && exit 0;
fi

#!/bin/bash -e

if [[ ${PLATFORM} == "Unix" ]]; then
    mkdir -p build
    cd build || exit 1

    cmake -DCOVERALLS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-DWEBSERVER_MAX_SENDLIST=10 -DWEBSERVER_SENDLIST_BUFSIZE=0" ..
    make

    ./start 2> 1.log && tail 1.log

    cmake -DCOVERALLS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-DWEBSERVER_MAX_SENDLIST=0 -DWEBSERVER_SENDLIST_BUFSIZE=32000" ..
    make

    ./start 2> 2.log && tail 2.log

    cmake -DCOVERALLS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-DWEBSERVER_MAX_SENDLIST=10 -DWEBSERVER_SENDLIST_BUFSIZE=32000" ..
    make

    ./start 2> 3.log && tail 3.log

    cmake -DCOVERALLS=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS="-DWEBSERVER_MAX_SENDLIST=0 -DWEBSERVER_SENDLIST_BUFSIZE=0" ..
    make

    ./start 2> 4.log && tail 4.log && exit 0;
fi

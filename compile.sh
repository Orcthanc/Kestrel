#!/bin/bash

mkdir -p build && cd build && cmake -DKST_PROFILING=OFF -DCOLOR_CONSOLE=ON -DKST_COLOR_STATS=OFF -DCMAKE_BUILD_TYPE=DEBUG .. && make -j9

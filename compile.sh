#!/bin/bash

mkdir -p build && cd build && cmake -DKST_PROFILING=OFF -DCOLOR_CONSOLE=ON -DKST_COLOR_STATS=OFF -DSB_KST_TERRAIN=ON -DCMAKE_BUILD_TYPE=DEBUG .. && make -j1

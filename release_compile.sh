#!/bin/bash

mkdir -p build && cd build && cmake -DKST_PROFILING=OFF -DCOLOR_CONSOLE=ON -DCMAKE_BUILD_TYPE=RELEASE -DKST_COLOR_STATS=OFF -DSB_KST_TERRAIN=ON .. && make -j9

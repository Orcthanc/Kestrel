#!/bin/bash

mkdir -p build && cd build && cmake -DKST_PROFILING=OFF -DCOLOR_CONSOLE=ON -DCMAKE_BUILD_TYPE=RELEASE .. && make -j9

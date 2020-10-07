#!/bin/bash

mkdir -p build && cd build && cmake -DKST_PROFILING=ON -DCOLOR_CONSOLE=ON -DCMAKE_BUILD_TYPE=DEBUG .. && make -j9

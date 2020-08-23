#!/bin/bash

mkdir -p build && cd build && cmake -DCOLOR_CONSOLE=ON -DCMAKE_BUILD_TYPE=DEBUG .. && make -j 9

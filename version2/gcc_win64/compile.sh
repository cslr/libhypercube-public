#!/bin/bash
#
# requires MSYS2/MINGW environment
#

g++ -fPIC -O2 -fopenmp -mconsole test_cube_c_api.cpp -L. -lcube_param -o test_cube_c_api




#!/bin/bash -eu

# grab the g++ example line from the comment inside the source code
build_cmd=$(grep 'g++' cec-simplest.cpp | cut -d' ' -f 2-)

# build it
eval $build_cmd

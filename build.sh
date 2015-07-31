#!/bin/bash -eu

g++ -std=gnu++0x -fPIC -g -Wall -march=armv6 -mfpu=vfp -mfloat-abi=hard -isystem /opt/vc/include/ -isystem /opt/vc/include/interface/vcos/pthreads/ -isystem /opt/vc/include/interface/vmcs_host/linux/ -I/usr/local/include -L /opt/vc/lib -lcec -lbcm_host -ldl cec-simplest.cpp -o cec-simplest 


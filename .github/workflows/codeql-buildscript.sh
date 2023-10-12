#!/usr/bin/env bash

sudo apt-get install -y libsdl2-dev
make clean
make CROSS_COMPILE="" PLATFORM=x64-sandbox

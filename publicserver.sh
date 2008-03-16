#!/bin/bash
export LD_LIBRARY_PATH="/usr/debug/lib/"
ldd -u src/publicserver
ulimit -c unlimited
nohup ./src/publicserver &

#!/bin/bash
#export LD_LIBRARY_PATH="/usr/debug/lib/"
ldd -u ./publicserver
ulimit -c unlimited
nohup ./publicserver &

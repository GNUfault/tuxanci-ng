#!/bin/bash
export LD_LIBRARY_PATH="/usr/debug/lib/"
ldd -u src/tuxanci-ng
ulimit -c unlimited
src/tuxanci-ng

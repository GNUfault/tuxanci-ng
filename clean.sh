#!/bin/sh
 
make clean
rm -rf CMakeCache.txt \
       CMakeFiles \
       cmake_install.cmake \
       cmake_uninstall.cmake \
       Makefile \
			 install_manifest.txt \
			 data/tuxanci.desktop \
			 src/base/path.h \
			 {data,po}/{CMakeFiles,Makefile,cmake_install.cmake} \
			 data/{arena,conf,font,images,music,sound}/{CMakeFiles,Makefile,cmake_install.cmake} \
       src/{CMakeFiles,cmake_install.cmake,Makefile,modules/{CMakeFiles,cmake_install.cmake,Makefile}}


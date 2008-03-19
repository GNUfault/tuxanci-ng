#!/bin/bash

mv config.h zal_config.h

rm -rf deb myusr tuxanci-ng_svn*_i386.deb
rm -rf  tuxanci_ng-i386-bin tuxanci_ng-i386-bin tuxanci_ng-i386-bin.tar.bz2
rm -rf tuxanci_ng tuxanci_ng.tar tuxanci_ng.tar.bz2

make clean
mv current_config.h config.h
make
mv config.h current_config.h
./tarballbin-build.sh

make clean
./tarball-build.sh

make clean
mv path_config.h config.h
make

rm -rf myusr
mkdir myusr
mkdir myusr/bin
mkdir myusr/share

make install DESTDIR=./myusr/
mv config.h path_config.h
./deb-build.sh

mv zal_config.h config.h
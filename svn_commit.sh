#!/bin/sh
svn up
REVIZE=$(( `svn info | grep Revision |awk -F ': ' '{print $2}'` + 1 ))
echo "#define TUXANCI_NG_VERSION \"svn${REVIZE}\"" > config.h
svn ci

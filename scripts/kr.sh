#!/bin/sh
PWD=$(pwd)
[[ ${PWD/*\//} = scripts ]] && pushd .. > /dev/null || pushd . > /dev/null

if [ ! "$1" ]; then
	echo "Help:  this script indents code according to the K&R standard"
	echo "Usage: $0 path/to/file.c"
	exit 1
elif [ ! -e "$1" ]; then
	echo "Error: file '$1' doesn't exist!"
	exit 1
fi

indent -kr -l79 -ts4 -ip4 -ppi 4 "$1"

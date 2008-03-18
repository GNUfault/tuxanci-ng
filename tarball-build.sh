#!/bin/sh

PROJECT="tuxanci-ng"
DIST_DIR="$PROJECT-svn`cat r`"

rm -rf $DIST_DIR $DIST_DIR.tar $DIST_DIR.tar.bz2

mkdir $DIST_DIR
mkdir $DIST_DIR/image
mkdir $DIST_DIR/font
mkdir $DIST_DIR/arena
mkdir $DIST_DIR/data
mkdir $DIST_DIR/lang
mkdir $DIST_DIR/sound
mkdir $DIST_DIR/music
mkdir $DIST_DIR/include
mkdir $DIST_DIR/src


cp image/*.* $DIST_DIR/image
cp font/*.* $DIST_DIR/font
cp arena/*.* $DIST_DIR/arena
cp data/*.* $DIST_DIR/data
cp lang/*.* $DIST_DIR/lang
cp sound/*.* $DIST_DIR/sound
cp music/*.* $DIST_DIR/music
cp include/*.* $DIST_DIR/include
cp src/* $DIST_DIR/src
cp Makefile $DIST_DIR
cp $PROJECT.desktop $DIST_DIR

tar -cvf $DIST_DIR.tar $DIST_DIR
bzip2 $DIST_DIR.tar

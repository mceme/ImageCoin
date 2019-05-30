#!/usr/bin/env bash

CURDIR=`dirname $0`


SRC_DIR=/Users/oak/__app_support/imagecoinCore
DST_DIR=/Users/oak/.image_coin_cache


ls -l $DST_DIR

if [ ! -d $DST_DIR/$1 ]; then
    echo "Err: $DST_DIR/$1 does not exist!"
    exit
fi

rm -rf $SRC_DIR

echo "cp -rf $DST_DIR/$1 $SRC_DIR"
cp -rf $DST_DIR/$1 $SRC_DIR



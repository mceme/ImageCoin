#!/usr/bin/env bash

CURDIR=`dirname $0`

SRC_DIR=/Users/oak/__app_support/imagecoinCore
DST_DIR=/Users/oak/.image_coin_cache
#DST_DIR=/Users/oak/.go/src/github.com/mceme/ImageCoin/build/

if [ -d $DST_DIR/$1 ]; then
    echo "$DST_DIR/$1 already exists!"
    exit
fi

mkdir -p $DST_DIR


cp -rf $SRC_DIR $DST_DIR/$1



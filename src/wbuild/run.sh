#!/usr/bin/env bash

CURDIR=`dirname $0`
imagecli stop

sleep 5
cp ${CURDIR}/_imagecoin.conf cache/imagecoin.conf

#/Users/oak/.go/src/github.com/mceme/ImageCoin/src/ImageCoind -keypool=2 -rescan$@
#/Users/oak/.go/src/github.com/mceme/ImageCoin/src/ImageCoind -keypool=2 $@
${CURDIR}/../ImageCoind -keypool=2 $@

sleep 3
ps -ef|grep ImageCoind|grep -v grep

/querytcpbyport.sh ImageCoin

imagecli getwalletinfo

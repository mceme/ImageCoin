#!/usr/bin/env bash
set -e
CURDIR=`dirname $0`


pic() {

    img=`cat ${CURDIR}/imagebase64.txt`

    imagecli sendtoaddress MY7KddiJ1dw23anwHbMqJm22SKqUvgvEzH 0.001 $img

    sleep 10
    ./dump.sh
    ./seebalance.sh

    ls -l /Users/oak/__app_support/imagecoinCore/wallet.dat
    ls -l /Users/oak/__app_support/imagecoinCore/image/image.dat

}

for((index=0; index<$1;index++)) do
    pic
done


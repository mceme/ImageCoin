#!/usr/bin/env bash

CURDIR=`dirname $0`

rm -rf /Users/oak/.go/src/github.com/mceme/ImageCoin/build/cache/*
cp -rf /Users/oak/.go/src/github.com/mceme/ImageCoin/build/__home/.image_coin_cache/* /Users/oak/.go/src/github.com/mceme/ImageCoin/build/cache/
exit

imagecli getreceivedbyaddress Mjbv4mtvtU7dPQEV5NPatec47Ag5dPLE8P

imagecli getreceivedbyaddress MY7KddiJ1dw23anwHbMqJm22SKqUvgvEzH



imagecli gettransaction db1b404989471f75f0fbe7e5bc1a60a77c1423297e136c0e3950deed87e3b7dc

imagecli importprivkey XBhEgjTycfzxsm1VuxGZJ38DhKX6AdSF6VUhKaRxrgKGuAAF9KLb


#!/usr/bin/env bash


/killbyname.sh ImageCoind
../src/ImageCoind

sleep 3
ps -ef|grep ImageCoind|grep -v grep

/querytcpbyport.sh ImageCoin

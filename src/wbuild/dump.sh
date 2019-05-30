#!/usr/bin/env bash


now=$(date +%Y_%m_%d_%H_%M_%S)
imagecli dumpwallet bak/${now}.txt


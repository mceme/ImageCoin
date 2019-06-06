# version upgrade
* image core and wallet.dat upgraded from v0.13.1.2 to v0.13.1.3
* imagecoind of v0.13.1.2 and v0.13.1.3 can coexist in the network
* Low version imagecoind cannot read high version of wallet.dat
* High version imagecoind can read lower version of wallet.dat

# scenes to be used
## 1. Create a new address, import an existing address
* Related transactions for this address, data other than imagebase64 fields will still be saved to wallet.dat
* The imagebase64 field is saved in ~/.imagecoind/image/image.dat

## 2. imagecoin-qt
* No changes in use
* No perception of coin separation

## 3. imagecoind restart
* wallet.dat will still make backups in ~/.imagecoind/backups
* image.dat does not make backups

## 4. wallet.dat is damaged or missing
* Import the private key with the backup wallet.dat, the system will re-download the relevant transaction
* Subsequent process reference case#1

## 5. image.dat is damaged or missing
* If imagecoind is found to be unreadable or does not match wallet.dat at startup
* imagecoind will go online to download related transactions for all addresses in wallet.dat
* Subsequent process reference case#1

## 6. -salvagewallet parameter
* will re-download tx and image data
* Subsequent process reference case#1

## 7. -rescan parameter
* will re-download tx and image data
* Subsequent process reference case#1

## 8. Compatibility 1
* The upgraded program (>=v0.13.1.3) will move the image data from the old wallet.dat to ~/.imagecoind/image/image.dat on the first boot.
* Leave only data outside the imagebase64 field in wallet.dat
* Subsequent process reference case#1

## 9. Compatibility 2
* If the old program (<=v0.13.1.2) starts up and finds that the wallet.dat version is higher than itself (>=v0.13.1.3), it will exit with an error.

## 10. movecmd, transfer
* Not affected by any

#图币分离程序使用
## Download compiled code
* git clone https://github.com/zhongqiuwood/ImageCoin -b dev
* The compilation steps are unchanged

## case1: Back up and then upgrade
* Back up wallet.dat with <ImageCoin-cli dumpwallet backup.txt> before upgrading, then delete wallet.dat
* Upgrade ImageCoind/ImageCoind-qt, launch ImageCoind/ImageCoind-qt
* Import backup key from backup.txt
* The system will re-download the relevant tx, the image data will be saved in ~/.imagecoind/image/image.dat
* Other data is saved in ~/.imagecoind/wallet.dat

## case2: Do not directly backup the backup
* Upgrade ImageCoind/ImageCoind-qt, launch ImageCoind/ImageCoind-qt
* The system will re-download the relevant tx because it can't find ~/.imagecoind/image/image.dat
* If it is ImageCoind-qt, the following dialog box will pop up, click the OK button to continue
    * https://github.com/zhongqiuwood/ImageCoin/blob/dev/src/wbuild/imageErr.jpg
* The image data will be saved in ~/.imagecoind/image/image.dat
* Other data is saved in ~/.imagecoind/wallet.dat

## config
```
./configure LDFLAGS="-L/root/qzhong/imagec/db4/lib/" CPPFLAGS="-I/root/qzhong/imagec/db4/include/" --enable-debug --disable-bench --disable-gui-tests --disable-tests

./configure --enable-debug --disable-bench --disable-gui-tests --disable-tests
./configure --enable-debug --disable-bench --disable-gui-tests --without-gui 
./configure --enable-debug --disable-bench --disable-gui-tests --without-gui --disable-tests 
```

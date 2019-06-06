version upgrade
Image core and wallet.dat upgraded from v0.13.1.2 to v0.13.1.3
Imagecoind of v0.13.1.2 and v0.13.1.3 can coexist in the network
Low version imagecoind cannot read high version of wallet.dat
High version imagecoind can read lower version of wallet.dat
scenes to be used
1. Create a new address, import an existing address
Related transactions for this address, data other than the imagebase64 field will still be saved to wallet.dat
The imagebase64 field is saved in ~/.imagecoind/image/image.dat
2. imagecoin-qt
No change in use
No sense of separation of coins
3. imagecoind restart
Wallet.dat will still make backups in ~/.imagecoind/backups
Image.dat does not make backups
4. wallet.dat is damaged or lost
Import the private key with the backup wallet.dat, the system will re-download the relevant transaction
Then the process refers to case#1
5. image.dat is damaged or missing
Imagecoind is found to be unreadable when it starts or does not match wallet.dat
Imagecoind will go online to download related transactions for all addresses in wallet.dat
Then the process refers to case#1
6. -salvagewallet parameter
Will re-download tx and image data
Then the process refers to case#1
7. -rescan parameter
Will re-download tx and image data
Then the process refers to case#1
8. Compatibility 1
The upgraded program (>=v0.13.1.3) will move the image data from the old wallet.dat to ~/.imagecoind/image/image.dat on the first boot.
Leave only data outside the imagebase64 field in wallet.dat
Then the process refers to case#1
9. Compatibility 2
If the old program (<=v0.13.1.2) starts up and finds that the wallet.dat version is higher than itself (>=v0.13.1.3), it will exit with an error.
10. movecmd, transfer
Unaffected
Drawing currency separation program
Download compiled code
git clone https://github.com/zhongqiuwood/ImageCoin -b dev
The compilation steps are unchanged
Case1: Back up and then upgrade
Use the backup wallet.dat before upgrading, then remove wallet.dat
Upgrade ImageCoind/ImageCoind-qt, start ImageCoind/ImageCoind-qt
Import the backup key from backup.txt
The system will re-download the relevant tx, the image data will be saved in ~/.imagecoind/image/image.dat
Other data is saved in ~/.imagecoind/wallet.dat
Case2: Direct upgrade without backup
Upgrade ImageCoind/ImageCoind-qt, start ImageCoind/ImageCoind-qt
The system will re-download the relevant tx because it can't find ~/.imagecoind/image/image.dat
If it is ImageCoind-qt, the following dialog box will pop up, click the OK button to continue
https://github.com/zhongqiuwood/ImageCoin/blob/dev/src/wbuild/imageErr.jpg
The image data will then be saved in ~/.imagecoind/image/image.dat
Other data is saved in ~/.imagecoind/wallet.dat
config
./configure LDFLAGS="-L/root/qzhong/imagec/db4/lib/" CPPFLAGS="-I/root/qzhong/imagec/db4/include/" --enable-debug --disable-bench --disable-gui-tests --disable-tests

./configure --enable-debug --disable-bench --disable-gui-tests --disable-tests
./configure --enable-debug --disable-bench --disable-gui-tests --without-gui 
./configure --enable-debug --disable-bench --disable-gui-tests --without-gui --disable-tests 

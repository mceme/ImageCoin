# ImageCoin
Shell script to install a [ImageCoin Masternode](https://www.imagecoin.imagehosty.com/) on a Linux server running Ubuntu 18.04 or higher. Use it on your own risk.

***
## Installation:
```
git clone https://github.com/mceme/ImageCoin.git
cd imginstall
bash img-install.sh
```
***

## Desktop wallet setup

After the MN is up and running, you need to configure the desktop wallet accordingly. Here are the steps for Windows Wallet
1. Open the ImageCoin Coin Desktop Wallet.
2. Go to RECEIVE and create a New Address: **MN1**
3. Send **10000** **ImageCoin** to **MN1**.
4. Wait for 15 confirmations.
5. Go to **Tools -> "Debug console - Console"**
6. Type the following command: **masternode outputs**
7. Go to  ** Tools -> "Open Masternode Configuration File"
8. Add the following entry:
```
Alias Address Privkey TxHash Output_index
```
* Alias: **MN1**
* Address: **VPS_IP:PORT**
* Privkey: **Masternode Private Key**
* TxHash: **First value from Step 6**
* Output index:  **Second value from Step 6**
9. Save and close the file.
10. Go to **Masternode Tab**. If you tab is not shown, please enable it from: **Settings - Options - Wallet - Show Masternodes Tab**
11. Click **Update status** to see your node. If it is not shown, close the wallet and start it again. Make sure the wallet is unlocked.
12. Open **Debug Console** and type:
```
masternode start-alias "MN1"
```
***

## Usage:
```
ImageCoin-cli getinfo
ImageCoin-cli mnsync status
ImageCoin-cli masternode status
```
Also, if you want to check/start/stop **ImageCoin** , run one of the following commands as **root**:

**Ubuntu 18.04**:
```
systemctl status ImageCoin #To check the service is running.
systemctl start ImageCoin #To start ImageCoin service.
systemctl stop ImageCoin #To stop ImageCoin service.
systemctl is-enabled ImageCoin #To check whetether ImageCoin service is enabled on boot or not.
```
***

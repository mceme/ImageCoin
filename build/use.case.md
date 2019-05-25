## 1. 创建新地址
* 下载交易到wallet.dat
* utxo 都保存在wallet.dat
* imagebase64字段保存在image.wallet.dat

## 2. 导入已经存在的地址
* 下载交易到wallet.dat
* utxo 都保存在wallet.dat
* imagebase64字段保存在image.wallet.dat

## 3. daemon重启
* wallet.dat仍然会在${cache}/backups里做备份
* image.wallet.dat不做备份
* 
* 

## 4. imagecoin-qt
* 使用无任何变化
* 对图币分离完全无感知
* 
* 

## 5. image.wallet.dat 损坏或者丢失
### 5-1. 运行中删除image.wallet.dat
* daemon运行中删除 image.wallet.dat
* daemon重启后, 转账会受影响
* 必须用对应的wallet.dat重新导入地址
* 随后流程参照case#2
* 
* 

### 5-2. image.wallet.dat丢失
* stop daemon
* 删除 image.wallet.dat
* 再启动daemon
* 必须用对应的wallet.dat重新导入地址
* 随后流程参照case#2

### 5-3. image.wallet.dat于wallet不匹配
* 启动daemon后会做校验
* 如果2个dat不匹配，daemon会退出
* 用户需删除2个dat
* 然后用对应的wallet.dat重新导入地址
* 随后流程参照case#2



## 6. salvagewallet
* 会重新下载tx和image，并覆盖image.wallet.dat里的数据
* 
## 7. -rescan
* 会重新下载tx和image，并覆盖image.wallet.dat里的数据
* 
## 8. 转账
* 图币分离后转账可以正常工作
* 

## . 
* 
* 
## . 
* 
* 
## . 
* 
* 
## . 
* 
* 
## . 
* 
* 
## . 
* 
* 
## . 
* 
* 
## . 
* 
* 
## . 
* 
* 
## . 
* 
* 
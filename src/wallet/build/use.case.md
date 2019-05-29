# 版本升级
* image core和wallet.dat从 v0.13.1.2 升级到 v0.13.1.3
* v0.13.1.2和v0.13.1.3的imagecoind可以在网络中共存
* 低版本imagecoind不能读高版本的wallet.dat
* 高版本imagecoind可以读低版本的wallet.dat

# 使用场景
## 1. 创建新地址, 导入已经存在的地址
* 该地址的相关交易, 除imagebase64字段以外的其他数据仍然会保存到wallet.dat
* imagebase64字段则保存在~/.imagecoind/image/image.dat

## 2. imagecoin-qt
* 使用无任何变化
* 对图币分离完全无感知

## 3. imagecoind 重启
* wallet.dat仍然会在~/.imagecoind/backups里做备份
* image.dat不做备份

## 4. wallet.dat 损坏或者丢失
* 用备份的wallet.dat导入私钥, 系统会重新下载相关交易
* 随后流程参照case#1

## 5. image.dat 损坏或者丢失
* imagecoind启动时如果发现image.dat不可读或者与wallet.dat不匹配
* imagecoind会去网络下载wallet.dat里所有地址的相关交易
* 随后流程参照case#1

## 6. -salvagewallet参数
* 会重新下载tx和image数据
* 随后流程参照case#1

## 7. -rescan参数
* 会重新下载tx和image数据
* 随后流程参照case#1

## 8. 兼容性1
* 升级的程序(>=v0.13.1.3)在第一次启动时会把旧的wallet.dat里的image数据移动到~/.imagecoind/image/image.dat
* wallet.dat里只留除imagebase64字段以外的其他数据
* 随后流程参照case#1

## 9. 兼容性2
* 旧程序(<=v0.13.1.2)启动时如果发现wallet.dat版本比自己高(>=v0.13.1.3), 则报错退出.

## 10. movecmd, 转账
* 不受任何影响
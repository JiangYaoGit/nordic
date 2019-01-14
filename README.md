# nordic and silicon lab

门锁侧：运行双模协议ble+zibee
使用nordic的nrf52840+pca10056开发板
开发环境keil5,  SDK15   协议栈S140
一、蓝牙实现功能介绍：
1 为zigbee软件代码的实现空中升级。目前采用单通道，只升级application，暂不升级bootload和
softdevice具体的flash布局可通过源码设置，或是通过nrf connect查看flash布局效果。
2 固件的合并与加密。安装python2.7.10之后的版本但不是python3，然后再script中添加nrfjprog
插件，之后查看公私钥、固件合并等指令帮助
3 FDS存储系统的移植，需特别注意不可和其他分区的flash重叠。flash在擦除后才可写入，固采用
fds的存储机制。
二、zigbee实现功能介绍
1 按键触发的加网和退网机制，以及led的多种状态的指示。
2 优化加网机制，例如以2分钟、30分钟、60分钟、120分钟、、、的间隔进行搜网，每次间隔2:1共持
续24秒的搜网
3 注册zcl空中包的处理，包括控制头的解析，comman的判断，attribute的改变。
4 自定义的command和attribute的添加，以及属性改变的上报。
5 串口侧协议的实现

网关侧：
使用silicon lab的efr32mg+pcb4001开发板 
开发环境simplicity studio，在window中可选cygwin,或是linux
1 网关的运行采用分层组合的方式，将application层放在pc机上运行，而其它层在开发板上。
cygwin安装详情可见工程目录下Devolop/cygwin安装
2 cygwin/linux下，如何启动zigbee网络、添加install code、查看网络地址和端口号等命令
的详细介绍，可见工程目录下Devolop/网关命令说明。
3 工程配置，主要注意zcl clusters和plugins下的选项，zcl clusters勾选可参考zigbee 3.0标准
文档说明，对属性id、命令id、server/client等概念。

抓包工具和功耗分析
1 第三方软件ubiqua或是simplicity studio下network analyzer。抓包软件的制作可见工程目录下
Devolop/silicon抓包的制作，抓包结果分析可见Devolop/抓包数据
2 nrf52840的功耗，通过将电流表串联到电源回路中，根据电流值得大小可判断功耗问题。通过将
精确的电阻串联到电源回路中，使用示波器抓取电阻两端的电压变化情况，可分析波形的含义。




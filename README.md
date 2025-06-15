# **README**

## **ECTM**

ESP32 Communication Terminal Module

旨在开发一款通过串口通讯的wifi和蓝牙双模模块



## 硬件设计

### 硬件选型

esp-wroom-32 (esp32-D0WD)



### 外设资源

- 串口
  - uart0：下载程序+日志输出
  - uart1：数据透传端口1
  - uart2：数据透传端口2
- wifi
  - STA（从）
  - AP（主）
- 蓝牙
- Flash
  - 程序代码
  - 配置文件



## 代码结构

### xm_config

定义全局默认变量



### xm_file

flash 的读写操作

- 初始化 spiffs
- 读取文件
- 保存文件
- 从 json 树中获取指定 json 对象



### xm_wifi

wifi 的各种操作

- 初始化 wifi
- 重新连接
- 断开连接
- 修改为主机模式
- 修改为从机模式
- 获取 wifi 连接信息



### xm_btooth

bluetooth 的各种操作

- 初始化 bluetooth



### xm_uart

uart 的各种操作

- 初始化 uart



### xm_console

通讯数据的各种操作

- 从缓存区读取一条报文
- 处理报文
- 发送报文



### xm_pin

引脚模式

- pwm 模式
- 上拉下拉



### task_wifi

wifi 通讯

- 主任务
- tcp 服务器任务
- udp 服务器任务
- mqtt 客户端任务



### task_btooth

蓝牙通讯

- 主任务



### task_uart

串口通讯

- 主任务
- 数据接收任务
- 数据发送任务



### task_web

网页配置参数，展示各种设备当前信息

- 主任务



### task_pin

配置引脚功能，检测到指定电平回复数据

- 主任务


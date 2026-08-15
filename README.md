# Intelligent Vehicle System Based on NXP i.MX6ULL
基于 NXP i.MX6ULL + Qt5.12.9 嵌入式 Linux 智能车载系统

[![Platform](https://img.shields.io/badge/Platform-i.MX6ULL-orange)]()
[![Qt](https://img.shields.io/badge/Qt-5.12.9-blue)]()
[![Kernel](https://img.shields.io/badge/Linux-4.1.15-green)]()
[![Toolchain](https://img.shields.io/badge/GCC-arm--linux--gnueabihf--4.9.4-yellow)]()

##  项目介绍
本项目运行于 NXP i.MX6ULL ARMv7 嵌入式平台，采用 **Linux底层驱动 + Qt图形应用** 分层架构。
底层内核提供传感器、摄像头字符设备驱动；上层 Qt 实现图形交互界面，基于 LinuxFB 直接驱动 800×480 LCD 触摸屏。
系统集成多媒体播放、多传感器采集、V4L2摄像头预览、倒车距离预警、人体感应哨兵远程监控，适合嵌入式Linux学习与车载人机界面原型开发。

##  核心功能
###  多媒体播放
- 本地音乐播放：扫描`/music`目录，支持切歌、暂停、音量调节、进度显示
- 本地视频播放：扫描`/video`目录，页面退出自动释放 MPlayer 资源

###  多传感器实时监测
DHT11、AP3216C、ICM20608、SR04超声波、SR501人体红外、蜂鸣器驱动控制。

###  V4L2 USB摄像头
V4L2 MMAP 采集 `/dev/video1`；YUYV图像转换、实时预览、手动拍照存储至`/photo`；独立线程保证画面实时性。

###  倒车影像预警
摄像头实时画面 + SR04超声波测距；距离低于阈值触发蜂鸣器报警，支持倒车拍照保存。

###  哨兵远程监控
SR501检测人体后自动启动 MJPG-streamer，开启 HTTP-MJPEG 视频流；内网浏览器远程访问监控画面。

## 🧰 软硬件环境
### 硬件
- 主控：NXP i.MX6ULL
- 显示屏：800×480 LCD，GT9147电容触摸
- USB UVC摄像头、DHT11、AP3216C、ICM20608、SR04、SR501、蜂鸣器

### 软件栈
- Linux Kernel：4.1.15
- Qt：5.12.9（LinuxFB 图形插件）
- 交叉工具链：Linaro GCC 4.9.4 `arm-linux-gnueabihf`
- 第三方组件：MPlayer、MJPG-streamer、libjpeg-turbo
- 文件系统：NFS 网络根文件系统

## 📂 项目目录
```c
Vehicle-system/
├── app/                # 主程序、所有 UI 页面
├── services/           # 业务服务封装
├── include/uapi/       # 驱动用户态头文件
├── scripts/            # 编译、部署脚本
├── configs/
├── platform/
├── tests/
└── VehicleSystem.pro   # qmake 工程文件
```




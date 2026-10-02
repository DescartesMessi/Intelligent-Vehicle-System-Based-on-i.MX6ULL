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

## 📊 实测性能指标

同一块板实测（i.MX6ULL @792MHz / 512MB DDR3 / 800×480 LCD / eMMC rootfs）。

### 启动与资源

| 指标 | 数值 |
|------|------|
| 应用启动（进程入口 → 首页首帧） | 8.16s → **4.98s（-39%）** |
| 程序体积 | 7.98MB → **0.55MB（-93%）** |
| 首页常驻内存 VmRSS | 70MB → **28MB（-60%）** |

### 各页面资源占用（自动巡检实测）

| 页面 | CPU（单核） | 说明 |
|------|-------------|------|
| 首页空闲 | 0.5~4% | 含 1Hz 时钟与温湿度刷新 |
| 音乐页 | ~3% | FLAC 播放中 |
| 视频页 | 3.5~5.5% | LinuxFB 输出 |
| 板载设备页 | 11~17% | 6 类传感器轮询 |
| 摄像头预览 | 54~62% | 640×480 YUYV |
| 倒车影像 | 56% | 摄像头 + SR04 测距 |
| 哨兵待机 | 1.5~3.5% | SR501 轮询 |

### 摄像头（V4L2）

| 指标 | 实测 |
|------|------|
| 采集链路 | 9 个 ioctl：QUERYCAP→S_FMT→S_PARM→REQBUFS→QUERYBUF→QBUF→STREAMON→DQBUF→STREAMOFF |
| 驱动交付 / 转换 / 上屏 | 21.5~21.8fps / ~11fps / 8.6~9.7fps |
| 丢帧 | 20~28%（代码请求 320×240 但硬件不支持，实际按 640×480 采集） |
| 拍照 | 640×480 BMP，0.88MB/张，文件名带时间戳 |

### 哨兵远程视频

| 指标 | 实测 |
|------|------|
| 人体检测触发 → 推流启动 | 32~101ms |
| 推流规格 | 25.0fps / 22.5Mbps（112KB/帧，JPEG 质量 70） |
| 推流进程 CPU | 5%（单核） |

> 测量方法、口径与更多数据见 [`docs/车载系统量化指标.md`](docs/车载系统量化指标.md)。

## 🔧 运行时指标采集开关

应用内置指标开关，默认关闭、不影响正常使用：

```bash
VS_METRICS=1 ./VehicleSystem                     # 每 2s 输出 page/rss/threads/cpu
VS_METRICS=1 VS_AUTOPILOT=4000 ./VehicleSystem   # 自动巡检 7 个页面并采样
```

##  软硬件环境
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

##  项目目录
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




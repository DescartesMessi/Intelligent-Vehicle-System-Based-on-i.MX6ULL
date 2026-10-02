# 基于 i.MX6ULL 的智能车载系统

基于 NXP i.MX6ULL + Qt 5.12.9 的嵌入式 Linux 车载终端：7 个功能页面、V4L2 摄像头预览与拍照、
多传感器实时采集、倒车测距预警、SR501 哨兵远程监控，运行在 800×480 LinuxFB 触摸屏上。

[![Platform](https://img.shields.io/badge/Platform-i.MX6ULL-orange)]()
[![Qt](https://img.shields.io/badge/Qt-5.12.9-blue)]()
[![Kernel](https://img.shields.io/badge/Linux-4.1.15-green)]()
[![Toolchain](https://img.shields.io/badge/GCC-arm--linux--gnueabihf--4.9.4-yellow)]()
[![BSP](https://img.shields.io/badge/BSP-imx6ull--nxp--bsp-lightgrey)](https://github.com/DescartesMessi/imx6ull-nxp-bsp)

## 📌 项目简介

应用采用**页面层 + 服务层 + Linux 设备接口**三级分层：

- **页面层**：`QStackedWidget` 组织 7 个功能页，页面懒构造、信号槽管理跳转与资源生命周期
- **服务层**：摄像头采集、MPlayer 播放器、传感器轮询、哨兵推流等 **6 个服务模块**封装设备访问，
  页面层不直接承担设备管理逻辑（仅保留设备存在性检查）
- **设备层**：8 个设备接口 —— `/dev/video1`、`fb0`、`dht11`、`ap3216c`、`icm20608`、`sr04`、`sr501`、`beep_device`

配套底层平台（U-Boot / 内核 / 设备树 / 驱动）见
[**imx6ull-nxp-bsp**](https://github.com/DescartesMessi/imx6ull-nxp-bsp)。

## ✨ 功能特性

| 页面 | 功能 |
|------|------|
| **首页** | 系统时间/日期、DHT11 温湿度、7 个功能入口、界面化退出 |
| **音乐** | 扫描 `/music`，播放/暂停、切歌、进度与音量（MPlayer + ALSA） |
| **视频** | 扫描 `/video`，LinuxFB 输出播放（含片源规格约束，见"已知限制"） |
| **板载设备** | DHT11 温湿度、AP3216C 光照/距离、ICM20608 六轴、SR04 测距、SR501 人体检测、蜂鸣器控制 |
| **摄像头** | V4L2 实时预览、设备扫描、一键拍照（640×480 BMP 存 `/photo`，文件名带时间戳） |
| **倒车影像** | 摄像头画面 + SR04 测距 + 蜂鸣器报警联动 + 倒车抓拍（JPG） |
| **哨兵监控** | SR501 检测到人自动启动 MJPG-streamer，局域网 HTTP-MJPEG 远程查看，无人 10s 自动停流 |

**输入支持**：GT9147 五点电容触摸（主输入）、USB 鼠标与键盘按 evdev 能力位自动识别、热插拔即用。

## 🏗️ 系统架构

```
┌────────────────── 页面层（app/pages）──────────────────┐
│  首页 · 音乐 · 视频 · 板载设备 · 摄像头 · 倒车影像 · 哨兵 │
├────────────────── 服务层（services）───────────────────┤
│  V4l2CameraService · MPlayerVideoPlayer · SentinelService│
│  MPlayerAudioPlayer · BoardDeviceService · ReverseService│
│  MjpgStreamerService                                     │
├──────────── 设备接口 / 外部进程 ────────────────────────┤
│  /dev/video1 /dev/fb0 /dev/input/event*                  │
│  /dev/dht11 /dev/ap3216c /dev/icm20608 /dev/sr04 …       │
│  MPlayer、MJPG-streamer（QProcess 管理）                  │
└──────────────────────────────────────────────────────────┘
```

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
| 采集链路 | 9 个 ioctl：QUERYCAP → S_FMT → S_PARM → REQBUFS → QUERYBUF → QBUF → STREAMON → DQBUF → STREAMOFF |
| 驱动交付 / 转换 / 上屏 | 21.5~21.8fps / ~11fps / 8.6~9.7fps |
| 丢帧 | 20~28%（代码请求 320×240 但硬件不支持，实际按 640×480 采集） |
| 拍照 | 640×480 BMP，0.88MB/张 |

### 哨兵远程视频

| 指标 | 实测 |
|------|------|
| 人体检测触发 → 推流启动 | 32~101ms |
| 推流规格 | 25.0fps / 22.5Mbps（112KB/帧，JPEG 质量 70） |
| 推流进程 CPU | 5%（单核） |

> 完整测量方法、口径与更多数据见 [`docs/车载系统量化指标.md`](docs/车载系统量化指标.md)。

## 📁 目录结构

```
Vehicle-system/
├── app/
│   ├── main.cpp / MainWindow.cpp      # 程序入口、QStackedWidget 页面栈
│   ├── MetricsProbe.h                 # 运行时指标开关（VS_METRICS / VS_AUTOPILOT）
│   ├── pages/                         # 7 个功能页（home/media/video/board/camera/reverse/sentinel）
│   └── resources/                     # style.qss 与图标资源
├── services/
│   ├── camera/                        # V4L2 采集线程（V4l2CameraService）
│   ├── video|audio/                   # MPlayer 播放器封装
│   ├── sensors/                       # 板载传感器采集线程（BoardDeviceService）
│   ├── reverse/                       # 倒车测距与报警
│   └── sentinel/                      # SR501 哨兵 + MJPG-streamer 推流
├── include/uapi/                      # 驱动用户态 ioctl 头文件
├── scripts/build_app.sh               # 交叉编译 + 部署到 NFS rootfs
└── docs/车载系统量化指标.md            # 实测指标与测量口径
```

## 🚀 快速开始

### 1. 依赖

需要先准备好交叉编译环境与 Qt 5.12.9 目标库（由 BSP 仓库提供）：

```bash
# BSP 侧（一次性）
bash scripts/build_QT_full.sh
```

`scripts/build_app.sh` 里已固定这些路径：

| 变量 | 默认值 |
|------|--------|
| `QT_QMAKE` | `<BSP>/build/3rdparty/qt5-arm/qtbase/bin/qmake` |
| `SYSROOT` | `<BSP>/build/sysroot` |
| 部署目录 | `<BSP>/deploy/nfs/rootfs/usr/local/bin` |

### 2. 编译并部署

```bash
bash scripts/build_app.sh          # 增量编译，产物部署到 NFS rootfs
bash scripts/build_app.sh --clean  # 清理后重新编译
bash scripts/build_app.sh --no-deploy
```

### 3. 在开发板上运行

```bash
# 板端（NFS 已挂载到 /mnt/nfs 时可直接取用）
cp /mnt/nfs/usr/local/bin/VehicleSystem /usr/local/bin/
cd /usr/local/bin && ./VehicleSystem
```

程序默认从 `/music`、`/video`、`/photo` 读取媒体与照片目录，并把界面输出到 `/dev/fb0`。

## 🔧 运行时指标开关

应用内置指标采集（**默认关闭，不影响正常使用**）：

```bash
VS_METRICS=1 ./VehicleSystem                     # 每 2s 输出一条 page/rss/threads/cpu
VS_METRICS=1 VS_AUTOPILOT=4000 ./VehicleSystem   # 自动巡检 7 个页面并采样
VS_METRICS=1 VS_AUTOPILOT=4000 VS_AUTOPILOT_EXIT=1 ./VehicleSystem  # 巡检结束自动退出
```

配合 BSP 仓库的 `scripts/vs_metrics_run.sh` 可一键完成"部署 → 巡检 → 回收指标日志"。

## 📚 相关文档

| 文档 | 内容 |
|------|------|
| [`docs/车载系统量化指标.md`](docs/车载系统量化指标.md) | 启动/内存/CPU/摄像头/推流/传感器能力全量实测数据与口径 |
| [imx6ull-nxp-bsp / docs](https://github.com/DescartesMessi/imx6ull-nxp-bsp/tree/main/docs) | BSP 侧优化记录、启动时间分析、中断风暴排查、面试问答 |

## ⚠️ 已知限制

- **摄像头丢帧 20~28%**：代码请求 320×240，但这颗 UVC 摄像头只支持 1280×720 / 640×480，
  实际按 640×480 采集，单帧像素量是预期的 4 倍；改进方向见量化文档
- **视频片源有规格要求**：板卡 MPlayer 未编译 MP3 解码器，且 800×600 及以上 H.264 软解跑不满帧率，
  建议使用 `520×330 / Baseline / AAC` 规格的片源（转码命令见量化文档）
- 时间显示依赖 BSP 侧的 RTC 设置，板上无后备电池，**完全断电后需重新校时**

## 🙏 致谢

- 底层平台：[imx6ull-nxp-bsp](https://github.com/DescartesMessi/imx6ull-nxp-bsp)
- NXP i.MX6ULL 官方 BSP、正点原子 ALPHA 开发板资料

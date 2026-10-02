#!/usr/bin/env bash

set -Eeuo pipefail

export ARCH=arm
export CROSS_COMPILE="${CROSS_COMPILE:-arm-linux-gnueabihf-}"

# 强制 C locale：交叉工具链的 readelf/file 等会跟随系统 locale 输出本地化文本
# （例如中文环境下 readelf -h 输出“类别/系统架构”），
# 会让下面按英文关键字（Class/Machine）做的架构校验匹配失败，
# 在 set -o pipefail 下直接中断整个构建。
export LC_ALL=C
export LANG=C

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

BSP_ROOT="/home/pointer/imx6ull/projects/imx6ull-nxp-bsp"

APP_PRO="${PROJECT_ROOT}/VehicleSystem.pro"
QT_QMAKE="${BSP_ROOT}/build/3rdparty/qt5-arm/qtbase/bin/qmake"
SYSROOT="${BSP_ROOT}/build/sysroot"

CROSS_BIN="${BSP_ROOT}/build/3rdparty/cross-bin"
ALIAS_PREFIX="arm-linux-gnueabi-"

BUILD_DIR="${PROJECT_ROOT}/build/app-arm"
STAGE_DIR="${PROJECT_ROOT}/build/app-stage"

ROOTFS="${BSP_ROOT}/deploy/nfs/rootfs"
ROOTFS_BIN="${ROOTFS}/usr/local/bin"

TARGET_NAME="VehicleSystem"
TARGET_FILE="${BUILD_DIR}/${TARGET_NAME}"
STAGE_FILE="${STAGE_DIR}/usr/local/bin/${TARGET_NAME}"

JOBS="${JOBS:-$(nproc)}"
CLEAN_BUILD=0
DEPLOY_ROOTFS=1

log_info()
{
    echo -e "\033[32m[INFO]\033[0m $1"
}

log_warn()
{
    echo -e "\033[33m[WARN]\033[0m $1"
}

die()
{
    echo -e "\033[31m[ERROR]\033[0m $1"
    exit 1
}

usage()
{
    echo "Usage:"
    echo "  $0                 增量编译并部署到 rootfs"
    echo "  $0 --clean         清理后重新编译"
    echo "  $0 --no-deploy     只编译，不更新 rootfs"
    echo "  $0 --help          显示帮助"
    echo ""
    echo "Environment:"
    echo "  JOBS=4             使用 4 个线程"
    echo "  CROSS_COMPILE=...  指定交叉编译器前缀"
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --clean)
            CLEAN_BUILD=1
            ;;
        --no-deploy)
            DEPLOY_ROOTFS=0
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            usage
            die "未知参数：$1"
            ;;
    esac

    shift
done

echo "======================================"
echo " Build Vehicle-system Qt Application"
echo "======================================"
echo "Project Root : ${PROJECT_ROOT}"
echo "Project File : ${APP_PRO}"
echo "Qt qmake    : ${QT_QMAKE}"
echo "Sysroot     : ${SYSROOT}"
echo "Cross Bin   : ${CROSS_BIN}"
echo "Build Dir   : ${BUILD_DIR}"
echo "Stage Dir   : ${STAGE_DIR}"
echo "Rootfs      : ${ROOTFS}"
echo "Jobs        : ${JOBS}"
echo "Clean       : ${CLEAN_BUILD}"
echo "Deploy      : ${DEPLOY_ROOTFS}"
echo "======================================"

###############################################################################
# 基础检查
###############################################################################

log_info "Checking build environment..."

command -v make >/dev/null 2>&1 \
    || die "make 未找到"

command -v "${CROSS_COMPILE}gcc" >/dev/null 2>&1 \
    || die "${CROSS_COMPILE}gcc 未找到"

command -v "${CROSS_COMPILE}g++" >/dev/null 2>&1 \
    || die "${CROSS_COMPILE}g++ 未找到"

command -v "${CROSS_COMPILE}ar" >/dev/null 2>&1 \
    || die "${CROSS_COMPILE}ar 未找到"

command -v "${CROSS_COMPILE}strip" >/dev/null 2>&1 \
    || die "${CROSS_COMPILE}strip 未找到"

command -v "${CROSS_COMPILE}readelf" >/dev/null 2>&1 \
    || die "${CROSS_COMPILE}readelf 未找到"

[ -f "${APP_PRO}" ] \
    || die "Qt 工程文件不存在：${APP_PRO}"

[ -x "${QT_QMAKE}" ] \
    || die "Qt qmake 不存在或不可执行：${QT_QMAKE}"

[ -d "${SYSROOT}/usr/include" ] \
    || die "sysroot 头文件目录不存在：${SYSROOT}/usr/include"

[ -d "${ROOTFS}" ] \
    || die "rootfs 不存在：${ROOTFS}"

###############################################################################
# 创建 Qt 所需的 arm-linux-gnueabi 工具链别名
###############################################################################

log_info "Preparing Qt compiler aliases..."

mkdir -p "${CROSS_BIN}"

TOOLS="
gcc
g++
ar
as
ld
nm
objcopy
objdump
ranlib
strip
readelf
strings
size
addr2line
"

for tool in ${TOOLS}; do
    real_tool="${CROSS_COMPILE}${tool}"
    alias_tool="${CROSS_BIN}/${ALIAS_PREFIX}${tool}"

    command -v "${real_tool}" >/dev/null 2>&1 \
        || die "工具不存在：${real_tool}"

    ln -sfn "$(command -v "${real_tool}")" "${alias_tool}"
done

export PATH="${CROSS_BIN}:${PATH}"

command -v arm-linux-gnueabi-gcc >/dev/null 2>&1 \
    || die "arm-linux-gnueabi-gcc 别名创建失败"

command -v arm-linux-gnueabi-g++ >/dev/null 2>&1 \
    || die "arm-linux-gnueabi-g++ 别名创建失败"

log_info "Qt target compiler:"
arm-linux-gnueabi-g++ --version | head -n 1

###############################################################################
# 检查 Qt 和 sysroot
###############################################################################

log_info "Checking Qt qmake..."
"${QT_QMAKE}" -v

log_info "Checking sysroot..."

[ -f "${SYSROOT}/usr/include/stdio.h" ] \
    || die "sysroot 中缺少 stdio.h"

[ -f "${SYSROOT}/usr/lib/crt1.o" ] \
    || die "sysroot 中缺少 crt1.o"

###############################################################################
# 清理旧目录
###############################################################################

if [ "${CLEAN_BUILD}" = "1" ]; then
    log_info "Cleaning old application build and staging directories..."

    case "${BUILD_DIR}" in
        "${PROJECT_ROOT}/build/app-arm")
            rm -rf -- "${BUILD_DIR}"
            ;;
        *)
            die "拒绝清理异常 BUILD_DIR：${BUILD_DIR}"
            ;;
    esac

    case "${STAGE_DIR}" in
        "${PROJECT_ROOT}/build/app-stage")
            rm -rf -- "${STAGE_DIR}"
            ;;
        *)
            die "拒绝清理异常 STAGE_DIR：${STAGE_DIR}"
            ;;
    esac
fi

mkdir -p "${BUILD_DIR}"
mkdir -p "${STAGE_DIR}"

###############################################################################
# qmake 配置
###############################################################################

cd "${BUILD_DIR}"

log_info "Running Qt qmake..."

"${QT_QMAKE}" \
    -spec linux-arm-gnueabi-g++ \
    "${APP_PRO}" \
    CONFIG+=release \
    CONFIG-=debug \
    QMAKE_CC=arm-linux-gnueabi-gcc \
    QMAKE_CXX=arm-linux-gnueabi-g++ \
    QMAKE_LINK=arm-linux-gnueabi-g++ \
    QMAKE_AR="arm-linux-gnueabi-ar cqs" \
    QMAKE_STRIP=arm-linux-gnueabi-strip \
    QMAKE_CFLAGS+=" --sysroot=${SYSROOT}" \
    QMAKE_CXXFLAGS+=" --sysroot=${SYSROOT}" \
    QMAKE_LFLAGS+=" --sysroot=${SYSROOT}" \
    QMAKE_LFLAGS+=" -Wl,-rpath,/usr/local/qt5/lib" \
    QMAKE_LFLAGS+=" -Wl,--enable-new-dtags"

###############################################################################
# 编译
###############################################################################

log_info "Building Qt application..."

make -j"${JOBS}"

[ -f "${TARGET_FILE}" ] \
    || die "目标程序未生成：${TARGET_FILE}"

log_info "Qt application build success"

###############################################################################
# 检查架构
###############################################################################

log_info "Checking target binary..."

if command -v file >/dev/null 2>&1; then
    file "${TARGET_FILE}"
fi

"${CROSS_COMPILE}readelf" -h "${TARGET_FILE}" \
    | grep -E "Class|Machine"

###############################################################################
# 安装到 staging
###############################################################################

log_info "Installing application to staging..."

mkdir -p "${STAGE_DIR}/usr/local/bin"

install -m 0755 \
    "${TARGET_FILE}" \
    "${STAGE_FILE}"

[ -f "${STAGE_FILE}" ] \
    || die "staging 安装失败"

###############################################################################
# 部署到 NFS rootfs
###############################################################################

if [ "${DEPLOY_ROOTFS}" = "1" ]; then
    log_info "Deploying application to NFS rootfs..."

    mkdir -p "${ROOTFS_BIN}"

    install -m 0755 \
        "${STAGE_FILE}" \
        "${ROOTFS_BIN}/${TARGET_NAME}"

    [ -f "${ROOTFS_BIN}/${TARGET_NAME}" ] \
        || die "部署到 rootfs 失败"

    log_info "Application deployed:"
    ls -lh "${ROOTFS_BIN}/${TARGET_NAME}"
else
    log_warn "DEPLOY_ROOTFS=0，不更新 rootfs"
fi

echo ""
echo "======================================"
echo " Vehicle-system Build Finished"
echo "======================================"

echo ""
echo "Build binary:"
ls -lh "${TARGET_FILE}"

echo ""
echo "Staging binary:"
ls -lh "${STAGE_FILE}"

if [ "${DEPLOY_ROOTFS}" = "1" ]; then
    echo ""
    echo "Rootfs binary:"
    ls -lh "${ROOTFS_BIN}/${TARGET_NAME}"
fi

echo ""
echo "[INFO] DONE!"

/* test_sr04.c 
* 编译和安装：cd /home/pointer/imx6ull/projects/Vehicle-system/tests/sr04
* make clean --> make --> make install
* 确认可执行文件 --> ls -lh /home/pointer/imx6ull/projects/Vehicle-system/tests/sr04/test_sr04 
*/

#include <errno.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "smarthome_sr04.h"

static void print_usage(const char *program)
{
    printf("Usage:\n");
    printf("  %s                 single measurement\n", program);
    printf("  %s loop_ms         continuous measurement\n", program);
}

int main(int argc, char **argv)
{
    const char *device = "/dev/sr04";
    int fd;
    int loop_ms = 0;
    struct sr04_measurement data;
    if (argc > 1) {
        loop_ms = atoi(argv[1]);
        if (loop_ms < 0) {
            print_usage(argv[0]);
            return 1;
        }
    }

    fd = open(device, O_RDWR);
    if (fd < 0) {
        fprintf(stderr,"open %s failed: %s\n",device,strerror(errno));
        return 1;
    }

    do {
        memset(&data, 0, sizeof(data));
        if (ioctl(fd,SR04_IOC_GET_DISTANCE,&data) < 0) {
            fprintf(stderr,
                    "SR04 measurement failed: %s\n",
                    strerror(errno));
            if (loop_ms == 0)
                break;
            usleep(loop_ms * 1000);
            continue;
        }

        printf("distance=%u mm, %.1f cm, pulse=%u us, valid=%u",
            data.distance_mm,data.distance_mm / 10.0,
            data.pulse_us,data.valid);

        /*
         * 300mm 以内认为障碍物过近。
         * 这里只打印状态，实际蜂鸣器由 Qt 或上层逻辑控制。
         */
        if (data.valid && data.distance_mm <= 80)
            printf(" [NEAR - ALARM]");
        printf("\n");
        fflush(stdout);
        if (loop_ms == 0)
            break;
        usleep(loop_ms * 1000);
    } while (1);
    close(fd);
    return 0;
}
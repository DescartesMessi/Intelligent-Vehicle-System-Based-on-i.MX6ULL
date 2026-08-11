#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "liteon_ap3216c.h"

static void print_sample(const struct ap3216c_sample *sample)
{
    printf("IR=%u valid=%u, "
           "ALS=%u, "
           "PS=%u valid=%u, "
           "near=%u\n",
           sample->ir,
           sample->ir_valid,
           sample->als,
           sample->ps,
           sample->ps_valid,
           sample->object_near);
}

int main(int argc, char *argv[])
{
    int fd;
    int once = 0;

    if (argc > 1 && strcmp(argv[1], "--once") == 0)
        once = 1;

    fd = open("/dev/ap3216c", O_RDWR);
    if (fd < 0) {
        perror("open /dev/ap3216c");
        return 1;
    }

    while (1) {
        struct ap3216c_sample sample;

        memset(&sample, 0, sizeof(sample));

        if (ioctl(fd, AP3216C_GET_SAMPLE, &sample) < 0) {
            perror("AP3216C_GET_SAMPLE");
            close(fd);
            return 1;
        }

        print_sample(&sample);
        fflush(stdout);

        if (once)
            break;

        sleep(1);
    }

    close(fd);
    return 0;
}
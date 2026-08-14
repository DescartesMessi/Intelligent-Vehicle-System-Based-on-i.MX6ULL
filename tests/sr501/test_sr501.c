#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include "smarthome_sr501.h"

static volatile sig_atomic_t running = 1;

static void signal_handler(int signal_number)
{
    (void)signal_number;
    running = 0;
}

static void print_status(const char *prefix,
                         const struct sr501_status *status)
{
    printf("%s detected=%u valid=%u sequence=%u\n",
        prefix,
        status->detected,
        status->valid,
        status->sequence);
    fflush(stdout);
}

int main(int argc, char **argv)
{
    const char *device = "/dev/sr501";
    int seconds = 0;
    int fd;
    int ret;
    time_t start_time;
    struct sr501_status status;
    struct pollfd poll_fd;

    if (argc > 1)
        device = argv[1];

    if (argc > 2) {
        seconds = atoi(argv[2]);
        if (seconds < 0) {
            fprintf(stderr, "seconds must be >= 0\n");
            return 1;
        }
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    fd = open(device, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        fprintf(stderr,
                "open %s failed: %s\n",
                device,
                strerror(errno));
        return 1;
    }

    memset(&status, 0, sizeof(status));

    ret = ioctl(fd, SR501_IOC_GET_STATUS, &status);
    if (ret < 0) {
        fprintf(stderr,
                "get initial status failed: %s\n",
                strerror(errno));
        close(fd);
        return 1;
    }

    print_status("initial:", &status);

    poll_fd.fd = fd;
    poll_fd.events = POLLIN;
    poll_fd.revents = 0;

    start_time = time(NULL);

    while (running) {
        ret = poll(&poll_fd, 1, 1000);

        if (ret < 0) {
            if (errno == EINTR)
                continue;

            fprintf(stderr,
                    "poll failed: %s\n",
                    strerror(errno));
            break;
        }

        if (ret == 0) {
            if (seconds > 0 &&
                time(NULL) - start_time >= seconds)
                break;

            continue;
        }

        if (poll_fd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
            fprintf(stderr, "SR501 device poll error: 0x%x\n",
                    poll_fd.revents);
            break;
        }

        if (poll_fd.revents & POLLIN) {
            ssize_t size;

            memset(&status, 0, sizeof(status));

            size = read(fd, &status, sizeof(status));
            if (size == (ssize_t)sizeof(status)) {
                print_status("event:", &status);
            } else if (size < 0 && errno != EAGAIN) {
                fprintf(stderr,
                        "read failed: %s\n",
                        strerror(errno));
                break;
            }
        }

        if (seconds > 0 &&
            time(NULL) - start_time >= seconds)
            break;
    }

    close(fd);
    return 0;
}
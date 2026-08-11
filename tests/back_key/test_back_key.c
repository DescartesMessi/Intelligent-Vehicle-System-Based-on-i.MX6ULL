#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int find_back_key_event(char *event_path,size_t event_path_size)
{
    int i;
    int fd;
    char path[64];
    char name[256];
    for (i = 0; i < 32; i++) {
        snprintf(path, sizeof(path),"/dev/input/event%d", i);
        fd = open(path, O_RDONLY);
        if (fd < 0)
            continue;
        memset(name, 0, sizeof(name));
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0) {
            printf("event%d: %s\n", i, name);
            if (strstr(name, "smarthome-back-key") ||strstr(name, "gpio_keys")) {
                snprintf(event_path,event_path_size,"%s",path);
                close(fd);
                return 0;
            }
        }
        close(fd);
    }
    return -1;
}

int main(int argc, char *argv[])
{
    int fd;
    char event_path[64];
    struct input_event event;
    if (argc > 1) {snprintf(event_path,sizeof(event_path),"%s",argv[1]);
    } else {
        if (find_back_key_event(event_path,sizeof(event_path)) < 0) {
            fprintf(stderr,"back key input device not found\n");
            return 1;
        }
    }

    printf("open input device: %s\n", event_path);
    fd = open(event_path, O_RDONLY);
    if (fd < 0) {perror("open input device");
        return 1;
    }
    printf("waiting for KEY_BACK events...\n");

    while (1) {
        ssize_t ret;
        ret = read(fd, &event, sizeof(event));
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            perror("read input event");
            close(fd);
            return 1;
        }
        if (ret != sizeof(event))
            continue;
        if (event.type == EV_KEY &&
            event.code == KEY_BACK) {
            if (event.value == 1)
                printf("KEY_BACK PRESSED\n");
            else if (event.value == 0)
                printf("KEY_BACK RELEASED\n");
            else if (event.value == 2)
                printf("KEY_BACK REPEAT\n");
            fflush(stdout);
        }
    }
    close(fd);
    return 0;
}
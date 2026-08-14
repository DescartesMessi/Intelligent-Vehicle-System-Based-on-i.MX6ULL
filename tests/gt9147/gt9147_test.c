#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <gt9147_uapi.h>

#define EVENT_DEVICE_COUNT 32
#define INPUT_NAME_LENGTH 128

struct touch_slot {
	int active;
	int tracking_id;
	int x;
	int y;
};

static volatile sig_atomic_t running = 1;

static void signal_handler(int signal_number)
{
	(void)signal_number;
	running = 0;
}

static int find_gt9147_device(char *device_path, size_t path_size)
{
	char candidate[64];
	char name[INPUT_NAME_LENGTH];
	int index;
	int fd;

	for (index = 0; index < EVENT_DEVICE_COUNT; index++) {
		snprintf(candidate, sizeof(candidate),
			 "/dev/input/event%d", index);

		fd = open(candidate, O_RDONLY | O_NONBLOCK);
		if (fd < 0)
			continue;

		memset(name, 0, sizeof(name));

		if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0) {
			if (strcmp(name, GT9147_INPUT_NAME) == 0) {
				snprintf(device_path, path_size,
					 "%s", candidate);
				close(fd);
				return 0;
			}
		}

		close(fd);
	}

	return -1;
}

static void print_frame(const struct touch_slot *slots,
			int btn_touch)
{
	int index;
	int count = 0;

	printf("BTN_TOUCH=%d", btn_touch);

	for (index = 0; index < GT9147_MAX_TOUCHES; index++) {
		if (!slots[index].active)
			continue;

		printf(" | slot=%d id=%d x=%d y=%d",
		       index,
		       slots[index].tracking_id,
		       slots[index].x,
		       slots[index].y);

		count++;
	}

	if (!count)
		printf(" | released");

	printf("\n");
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	struct touch_slot slots[GT9147_MAX_TOUCHES];
	struct input_event events[32];
	struct pollfd poll_fd;
	char device_path[64];
	int current_slot = 0;
	int btn_touch = 0;
	ssize_t bytes;
	size_t count;
	size_t index;
	int fd;
	int ret;

	memset(slots, 0, sizeof(slots));

	for (index = 0; index < GT9147_MAX_TOUCHES; index++)
		slots[index].tracking_id = -1;

	if (argc > 1) {
		snprintf(device_path, sizeof(device_path),
			 "%s", argv[1]);
	} else {
		ret = find_gt9147_device(device_path,
					 sizeof(device_path));
		if (ret) {
			fprintf(stderr,
				"GT9147 input device was not found\n");
			return EXIT_FAILURE;
		}
	}

	fd = open(device_path, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr,
			"failed to open %s: %s\n",
			device_path, strerror(errno));
		return EXIT_FAILURE;
	}

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	printf("Reading GT9147 events from %s\n", device_path);
	printf("Touch the screen. Press Ctrl+C to exit.\n");

	poll_fd.fd = fd;
	poll_fd.events = POLLIN;
	poll_fd.revents = 0;

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

		if (ret == 0)
			continue;

		if (!(poll_fd.revents & POLLIN))
			continue;

		bytes = read(fd, events, sizeof(events));
		if (bytes < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;

			fprintf(stderr,
				"read failed: %s\n",
				strerror(errno));
			break;
		}

		count = bytes / sizeof(struct input_event);

		for (index = 0; index < count; index++) {
			struct input_event *event = &events[index];

			if (event->type == EV_ABS) {
				switch (event->code) {
				case ABS_MT_SLOT:
					if (event->value >= 0 &&
					    event->value <
					    GT9147_MAX_TOUCHES) {
						current_slot =
							event->value;
					}
					break;

				case ABS_MT_TRACKING_ID:
					slots[current_slot].tracking_id =
						event->value;
					slots[current_slot].active =
						event->value >= 0;
					break;

				case ABS_MT_POSITION_X:
					slots[current_slot].x =
						event->value;
					break;

				case ABS_MT_POSITION_Y:
					slots[current_slot].y =
						event->value;
					break;

				default:
					break;
				}
			} else if (event->type == EV_KEY &&
				   event->code == BTN_TOUCH) {
				btn_touch = event->value;
			} else if (event->type == EV_SYN &&
				   event->code == SYN_REPORT) {
				print_frame(slots, btn_touch);
			}
		}
	}

	close(fd);

	printf("GT9147 test stopped\n");

	return EXIT_SUCCESS;
}
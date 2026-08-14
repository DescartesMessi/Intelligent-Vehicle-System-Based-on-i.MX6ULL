#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "smarthome_icm20608.h"

#define ACCEL_SENSITIVITY 16384.0
#define GYRO_SENSITIVITY 131.0
#define TEMP_SENSITIVITY 326.8

static void print_usage(const char *program)
{
	printf("Usage:\n");
	printf("  %s                 single sample\n", program);
	printf("  %s loop_ms         continuous samples\n", program);
}

static void print_sample(const struct icm20608_sample *sample)
{
	double ax;
	double ay;
	double az;
	double gx;
	double gy;
	double gz;
	double temperature;

	ax = sample->accel_x / ACCEL_SENSITIVITY;
	ay = sample->accel_y / ACCEL_SENSITIVITY;
	az = sample->accel_z / ACCEL_SENSITIVITY;

	gx = sample->gyro_x / GYRO_SENSITIVITY;
	gy = sample->gyro_y / GYRO_SENSITIVITY;
	gz = sample->gyro_z / GYRO_SENSITIVITY;

	temperature = sample->temperature / TEMP_SENSITIVITY + 25.0;

	printf("WHO_AM_I=0x%02x ",
	       sample->who_am_i);

	printf("ACCEL[g]=%.3f,%.3f,%.3f ",
	       ax, ay, az);

	printf("GYRO[dps]=%.3f,%.3f,%.3f ",
	       gx, gy, gz);

	printf("TEMP=%.2f C ",
	       temperature);

	printf("RAW_ACC=%d,%d,%d ",
	       sample->accel_x,
	       sample->accel_y,
	       sample->accel_z);

	printf("RAW_GYRO=%d,%d,%d\n",
	       sample->gyro_x,
	       sample->gyro_y,
	       sample->gyro_z);

	fflush(stdout);
}

int main(int argc, char **argv)
{
	const char *device = "/dev/icm20608";
	struct icm20608_sample sample;
	unsigned char who_am_i;
	int fd;
	int loop_ms = 0;
	int ret;

	if (argc > 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	if (argc == 2) {
		loop_ms = atoi(argv[1]);

		if (loop_ms <= 0) {
			print_usage(argv[0]);
			return EXIT_FAILURE;
		}
	}

	fd = open(device, O_RDWR);
	if (fd < 0) {
		fprintf(stderr,
			"open %s failed: %s\n",
			device,
			strerror(errno));
		return EXIT_FAILURE;
	}

	ret = ioctl(fd,
		    ICM20608_IOC_GET_WHO_AM_I,
		    &who_am_i);
	if (ret < 0) {
		fprintf(stderr,
			"GET_WHO_AM_I failed: %s\n",
			strerror(errno));
		close(fd);
		return EXIT_FAILURE;
	}

	printf("ICM20608 WHO_AM_I=0x%02x\n",
	       who_am_i);

	do {
		memset(&sample, 0, sizeof(sample));

		ret = ioctl(fd,
			    ICM20608_IOC_GET_SAMPLE,
			    &sample);
		if (ret < 0) {
			fprintf(stderr,
				"GET_SAMPLE failed: %s\n",
				strerror(errno));

			if (!loop_ms)
				break;

			usleep(loop_ms * 1000);
			continue;
		}

		print_sample(&sample);

		if (!loop_ms)
			break;

		usleep(loop_ms * 1000);
	} while (1);

	close(fd);

	return EXIT_SUCCESS;
}
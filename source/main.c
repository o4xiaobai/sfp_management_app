/* vi:set ts=4 sw=4: */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include "i2cbusses.h"
#include "i2c-dev.h"

#define BSP_COMMON_EBASE   (-100)
#define BSP_COMMON_ENOENT  (BSP_COMMON_EBASE-2)
#define BSP_COMMON_EACCES  (BSP_COMMON_EBASE-3)

int embedway_sfp_get_vendor_name(int port, char *name)
{
	int address = 0x50;
	char filename[128];
	int file;
	int i;
	int res;

	snprintf(filename, 128, "/dev/i2c/sfp%d", port);
	file = open(filename, O_RDWR);
	if (file < 0 ) {
		fprintf(stderr, "Error: Could not open file"
				"`%s': %s\n", filename, strerror(errno));
		if (errno == EACCES) {
			fprintf(stderr, "Run as root?\n");
			return BSP_COMMON_EACCES;
		}
		return BSP_COMMON_ENOENT;
	}

	if (set_slave_addr(file, address, 0))  {
		fprintf(stderr, "set slave addr faied\n");
		return -1;
	}

	for(i = 0; i < 16; i++) {
		res = i2c_smbus_read_word_data(file, i+20);
		if (res < 0) {
			return -2;
		}
		name[i] = res & 0xff;
	}
	name[17] = 0;

	close(file);

	return 0;
}

int main(int argc, char ** argv)
{
	char name[20];
	int port;

	if (argc < 2) {
		printf("%s port\n", argv[0]);
		exit(0);
	} else {
		port = strtoul(argv[1], NULL, 0);
	}

	if (embedway_sfp_get_vendor_name(port, name)) {
		printf("error\n");
	} else {
		printf("name = %s\n", name);
	}

	return 0;
}

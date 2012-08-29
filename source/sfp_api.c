/* vi:set ts=4 sw=4: */
/** 
 * @file sfp_api.c
 * @brief 
 * @author Hu Liupeng, hu.liupeng2011@gmail.com
 * @version 0.0.1
 * @date 2012-07-08
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include "i2cbusses.h"
#include "i2c-dev.h"

#include "sfp_api.h"

#define BSP_COMMON_EBASE   (-100)
#define BSP_COMMON_ENOENT  (BSP_COMMON_EBASE-2)
#define BSP_COMMON_EACCES  (BSP_COMMON_EBASE-3)

#define SFP_NODE_FILENAME_SIZE_MAX 128

#define SFP_NODE_FILENAME_FORMAT "/dev/i2c/sfp%d"

int sfp_open(int port) 
{
    char filename[SFP_NODE_FILENAME_SIZE_MAX];
    int fd;

    snprintf(filename, sizeof(filename), SFP_NODE_FILENAME_FORMAT, port);
    fd = open(filename, O_RDWR);

    if (fd < 0) {
        fprintf("stderr, "Error: Could not open file"
                "`%s': %s\n", filename, strerror(errno));
        if (errno == EACCESS) {
            fprintf("stderr, "Run as root?\n");
            return BSP_COMMON_EACCES;
        }
        return BSP_COMMON_ENOENT;
    }

    return fd;

}

int sfp_close(int fd)
{
    if (fd > 0) {
        return close(fd);
    }

    return -1;
}


/* --------------------------------------------------------------------------*/
/** 
 * @brief 
 * 
 * @param port
 * @param name
 * 
 * @return 
 */
/* ----------------------------------------------------------------------------*/
#define SFP_VENDOR_NAME_BUFF_SIZE 20
int sfp_get_vendor_name(int port, char (*p_name)[SFP_VENDOR_NAME_BUFF_SIZE])
{
	int address = 0x50;
	int fd;
	int i;
	int res;
    char *name;
#define SFP_VENDOR_NAME_REAL_SIZE 16
#define SFP_VENDOR_NAME_REG_OFFSET_BASE 20

    if (p_name == NULL) {
        return -1;
    }
    
    if (*p_name == NULL) {
        return -4;
    }

    name = * p_name;
    fd = sfp_open(port);
    if (fd < 0) {
        return -2;
    }

	if (set_slave_addr(fd, address, 0))  {
		fprintf(stderr, "set slave addr faied\n");
		return -3;
	}

	for(i = 0; i < SFP_VENDOR_NAME_REAL_SIZE; i++) {
		res = i2c_smbus_read_word_data(fd, i + SFP_VENDOR_NAME_REG_OFFSET_BASE);
		if (res < 0) {
			return -2;
		}
		name[i] = res & 0xff;
	}
	name[SFP_VENDOR_NAME_REAL_SIZE] = 0;

    return sfp_close(fd);

}


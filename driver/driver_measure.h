/*
 * driver_measure.h
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license or GPL license. See the LICENSE_BSD and
 * LICENSE_GPL files for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * created: 8/27/14
 * version: 0.1.6 - add an measuring system driver for cpu stat readings
 *          0.2.0 - add support for msr registers to the measure driver
 *          0.2.1 - add support for IPMI to the measure driver
 *          0.3.3 - add cpu memory info to measure driver
 *          0.5.7 - add automatic detection of ipmi device to measure driver
 */

#ifndef __DRIVER_MEASURE_H__
#define __DRIVER_MEASURE_H__

#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel_stat.h>
#include <linux/string.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/cpu.h>
#include <linux/tick.h>
#include <linux/ipmi.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/completion.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <linux/dmi.h>
 
#include <asm/processor.h>
#include <asm/msr.h>

#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#undef MSR_RAPL_POWER_UNIT
#undef MSR_PKG_ENERGY_STATUS
#undef MSR_PP0_ENERGY_STATUS
#undef MSR_PP1_ENERGY_STATUS
#undef MSR_DRAM_ENERGY_STATUS

#include "../include/ms_driver.h"

#ifdef KERNEL_CENTOS65
#include <linux/jiffies.h>
#include <linux/smp_lock.h>
#include <asm/system.h>
#endif /* KERNEL_CENTOS65 */

#define IPMI_TIMEOUT 50

#define CPU_STAT 0x0000000000000000
#define MSR 0x0100000000000000
#define IPMI 0x0200000000000000

#ifndef LSEEK_MAX_2
#define CPU_MEMINFO 0x0300000000000000
#endif

#define WHENCE_POS 56

#define WHENCE_MASK 0xFF00000000000000
#define CPU_MASK 0x00FF000000000000
#define REG_MASK 0x0000FFFFFFFFFFFF
#define RECORD_ID_MASK 0x000000000000FFFF

#define SENSOR_NETFN 0x04
#define STORAGE_NETFN 0x0a
#define IPMI_NET_FN_OEM_DELL_GENERIC_RQ 0x30

#define IPMI_CMD_OEM_DELL_GET_POWER_CONSUMPTION_DATA 0x9C
#define IPMI_CMD_OEM_DELL_RESET_POWER_CONSUMPTION_DATA 0x9D
#define IPMI_CMD_OEM_DELL_POWER_CONSUMPTION	0xB3
#define IPMI_OEM_DELL_POWER_CONSUMPTION_ENTITY_INSTANCE_ALL	0x00
#define IPMI_OEM_DELL_RESET_POWER_CONSUMPTION_DATA_CUMULATIVE 0x01
#define IPMI_ENTITY_ID_SYSTEM_BOARD	0x07
#define IPMI_ENTITY_ID_POWER_SUPPLY	0x0A

#define CMD_GET_SENSOR_READING 0x2d
#define CMD_GET_SDR 0x23
#define READ_FULL_RECORD 0xff

#define K(x) ((x) << (PAGE_SHIFT - 10))

/*struct for ipmi related data*/
typedef struct _ipmi_data{
	ipmi_user_t user;
	struct ipmi_addr address;
	struct completion read_complete;
	long msgid;
	unsigned char datarc[IPMI_MAX_MSG_LENGTH];
	int datarc_length;
} IPMI_DATA;

// Interface to driver
static ssize_t driver_read(struct file *fileptr, char __user *user_buffer, size_t user_buffer_size, loff_t *offset);
static ssize_t driver_write(struct file *fileptr, const char __user *user_buffer, size_t user_buffer_size, loff_t *offset);
static loff_t driver_lseek(struct file *fileptr, loff_t offset, int whence);
static int driver_open(struct inode *device, struct file *fileptr);
static int driver_close(struct inode *device, struct file *fileptr);

static int read_cpu_stats(u64* cpustat, int cpu);
static int read_msr_reg(int cpu, u32 reg, u32* data);
static int read_meminfo(u64* meminfo);
static int check_msr_off(loff_t offset);
static int check_cpu_stat_off(loff_t offset);
static int check_ipmi_offset(loff_t offset);
static int check_mem_info_offset(loff_t offset);

static void ipmi_msg_handler(struct ipmi_recv_msg *msg, void *user_msg_data);
static int ipmi_request(int netfn, int cmd, unsigned char * msgdata, int size);

//Functions for file io
static struct file* file_open(const char* path, int flags, int rights);
static int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);
static void file_close(struct file* file);

// Module management
static int __init mod_init(void);
static void __exit mod_exit(void);

#endif /* __DRIVER_MEASURE_H__ */

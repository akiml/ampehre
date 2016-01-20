/*
 * driver_measure.c
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
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 8/27/14
 * version: 0.1.6 - add an measuring system driver for cpu stat readings
 *          0.2.0 - add support for msr registers to the measure driver
 *          0.2.1 - add support for IPMI to the measure driver
 *          0.3.3 - add cpu memory info to measure driver
 *          0.5.7 - add automatic detection of ipmi device to measure driver
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 */

#include "driver_measure.h"

#define BUILD_DRIVER
#include "../include/ms_version.h"

MODULE_AUTHOR("Achim Loesch (achim.loesch@upb.de) and Christoph Knorr (cknorr@mail.upb.de)");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Driver used by the Ampehre measuring framework library libmeasure.");
MODULE_SUPPORTED_DEVICE("none");
MODULE_VERSION(MS_VERSION_STRING);

static DEFINE_MUTEX(ipmi_mutex);
static DEFINE_MUTEX(ioctl_mutex);

static struct file* proc_meminfo_file = NULL;

static IPMI_DATA *ipmi_data = NULL;

//count how many times the driver is opened
static int opened = 0;

static unsigned long ipmi_timeout = IPMI_MAX_TIMEOUT;
static int ipmi_timeout_locked = 0;

static int ipmi_supported = 0;

static struct ipmi_user_hndl ipmi_hndlrs =
{
	.ipmi_recv_hndl	= ipmi_msg_handler, 
};

/*
 * DRIVER ACCESS
 */
static struct file_operations fileops = {
	.owner		= THIS_MODULE,
	.llseek		= driver_lseek,
	.read		= driver_read,
	.write		= driver_write,
	.open		= driver_open,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl		= driver_ioctl,
#else
	.unlocked_ioctl = driver_ioctl,
#endif
	.release	= driver_close
};

static loff_t driver_lseek(struct file *fileptr, loff_t offset, int whence) {
	loff_t position;
	
	struct inode *inode = fileptr->f_mapping->host;
	mutex_lock(&inode->i_mutex);
	switch(whence) {
		case 0: /* SEEK_SET: CPU stats*/
			position = CPU_STAT | offset;
			break;
		case 1: /* SEEK_CUR: MSR register*/
			position = MSR | offset;
			break;
		case 2: /* SEEK_END: IPMI */
			position = IPMI | offset;
			break;
#ifndef LSEEK_MAX_2
		case 3: /* CPU memory info */
			position = CPU_MEMINFO | offset;
			break;
#endif
		default:
			mutex_unlock(&inode->i_mutex);
			return -EINVAL;
	}
	 mutex_unlock(&inode->i_mutex);
	
	if (position >= 0) {
		fileptr->f_pos = position;
		return position;
	}
	
	return -EINVAL;
}

/* Check the Offset for msr and returns 0 if it's correct*/
static int check_msr_off(loff_t offset){
	loff_t msr_reg = 0;
	loff_t cpu = -1;
	loff_t whence_msr = (WHENCE_MASK & offset) >> WHENCE_POS;
	if(whence_msr != cpu_msr)
		return -EINVAL;
	msr_reg = REG_MASK & offset;
	cpu = (CPU_MASK & offset)>>CPU_NO_POS;
	if(cpu < 0 ||cpu >= nr_cpu_ids){
		printk("Measure: Wrong Core number!\n");
		return -EINVAL;
	}
	if(msr_reg == MSR_RAPL_POWER_UNIT || msr_reg == MSR_PKG_ENERGY_STATUS || msr_reg == MSR_PP0_ENERGY_STATUS ||
		msr_reg == MSR_PP1_ENERGY_STATUS || msr_reg == MSR_DRAM_ENERGY_STATUS || msr_reg == IA32_THERM_STATUS ||
		msr_reg == IA32_PACKAGE_THERM_STATUS || msr_reg == MSR_TEMPERATURE_TARGET || msr_reg == IA32_TIME_STAMP_COUNTER||
		msr_reg == IA32_MPERF || msr_reg == IA32_APERF){
		return 0;
	}
	else{
		return -EPERM;
	}
}

/*Check the offset for cpu_stats and return 0 if it's correct*/
static int check_cpu_stat_off(loff_t offset){
	loff_t cpu = -1;
	loff_t whence_cpustat = (WHENCE_MASK & offset) >> WHENCE_POS;
	if(whence_cpustat != cpu_stats)
		return -EINVAL;
	cpu = (CPU_MASK & offset) >> CPU_NO_POS;
#ifdef LSEEK_MAX_2
	if(cpu<0 ||cpu > nr_cpu_ids){
		if(cpu != CPU_MEMINFO){
			printk("Measure: Wrong Core number!\n");
		}
		return -EINVAL;
	}
#else
	if(cpu<0 ||cpu > nr_cpu_ids){
		printk("Measure: Wrong Core number!\n");
		return -EINVAL;
	}
#endif
	return 0;
}

/*Check the offset for ipmi and return 0 if it's correct*/
static int check_ipmi_offset(loff_t offset){
	loff_t whence_ipmi = (WHENCE_MASK & offset) >> WHENCE_POS;
	loff_t cmd = 0;
	
	if(whence_ipmi != ipmi)
		return -EINVAL;
	cmd = (CPU_MASK & offset) >> CPU_NO_POS;
	if(cmd < 0 || cmd > 4)
		return -EINVAL;
	
	return 0;
}

/*Check the offset for cpu memory info and return 0 if it's correct*/
static int check_mem_info_offset(loff_t offset){
	loff_t whence_meminfo = (WHENCE_MASK & offset) >> WHENCE_POS;
#ifdef LSEEK_MAX_2
	loff_t cpu = -1;
	cpu = (CPU_MASK & offset) >> CPU_NO_POS;
	if((whence_meminfo != cpu_meminfo) && (cpu != CPU_MEMINFO))
		return -EINVAL;
#else
	if(whence_meminfo != cpu_meminfo)
		return -EINVAL;
#endif
	return 0;
}

/* Read out the cpu_stats*/
static int read_cpu_stats(u64* cpustat, int cpu){
#ifdef KERNEL_CENTOS7
	// Get utilization of a single core
	if (cpu < nr_cpu_ids) {
		cpustat[CPUSTAT_USER]		= cputime64_to_clock_t(kcpustat_cpu(cpu).cpustat[CPUTIME_USER]		);
		cpustat[CPUSTAT_NICE]		= cputime64_to_clock_t(kcpustat_cpu(cpu).cpustat[CPUTIME_NICE]		);
		cpustat[CPUSTAT_SYSTEM]		= cputime64_to_clock_t(kcpustat_cpu(cpu).cpustat[CPUTIME_SYSTEM]	);
		cpustat[CPUSTAT_IDLE]		= cputime64_to_clock_t(get_cpu_idle_time_us(cpu, NULL)/1000L		);
		cpustat[CPUSTAT_IOWAIT]		= cputime64_to_clock_t(get_cpu_iowait_time_us(cpu, NULL)/1000L		);
		cpustat[CPUSTAT_IRQ]		= cputime64_to_clock_t(kcpustat_cpu(cpu).cpustat[CPUTIME_IRQ]		);
		cpustat[CPUSTAT_SOFTIRQ]	= cputime64_to_clock_t(kcpustat_cpu(cpu).cpustat[CPUTIME_SOFTIRQ]	);
		cpustat[CPUSTAT_STEAL]		= cputime64_to_clock_t(kcpustat_cpu(cpu).cpustat[CPUTIME_STEAL]		);
		cpustat[CPUSTAT_GUEST]		= cputime64_to_clock_t(kcpustat_cpu(cpu).cpustat[CPUTIME_GUEST]		);
		cpustat[CPUSTAT_GUEST_NICE]	= cputime64_to_clock_t(kcpustat_cpu(cpu).cpustat[CPUTIME_GUEST_NICE]);
	// Get total utilization (sum of the values of all single cores)
	} else {
		int i;
		for (i=0; i<nr_cpu_ids; ++i) {
			cpustat[CPUSTAT_USER]		+= cputime64_to_clock_t(kcpustat_cpu(i).cpustat[CPUTIME_USER]		);
			cpustat[CPUSTAT_NICE]		+= cputime64_to_clock_t(kcpustat_cpu(i).cpustat[CPUTIME_NICE]		);
			cpustat[CPUSTAT_SYSTEM]		+= cputime64_to_clock_t(kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM]	);
			cpustat[CPUSTAT_IDLE]		+= cputime64_to_clock_t(get_cpu_idle_time_us(i, NULL)/1000L		);
			cpustat[CPUSTAT_IOWAIT]		+= cputime64_to_clock_t(get_cpu_iowait_time_us(i, NULL)/1000L		);
			cpustat[CPUSTAT_IRQ]		+= cputime64_to_clock_t(kcpustat_cpu(i).cpustat[CPUTIME_IRQ]		);
			cpustat[CPUSTAT_SOFTIRQ]	+= cputime64_to_clock_t(kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ]	);
			cpustat[CPUSTAT_STEAL]		+= cputime64_to_clock_t(kcpustat_cpu(i).cpustat[CPUTIME_STEAL]		);
			cpustat[CPUSTAT_GUEST]		+= cputime64_to_clock_t(kcpustat_cpu(i).cpustat[CPUTIME_GUEST]		);
			cpustat[CPUSTAT_GUEST_NICE]	+= cputime64_to_clock_t(kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE]);
		}
	}
#else	
		// Get utilization of a single core
	if (cpu < nr_cpu_ids) {
		cpustat[CPUSTAT_USER]		= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.user      );
		cpustat[CPUSTAT_NICE]		= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.nice      );
		cpustat[CPUSTAT_SYSTEM]		= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.system    );
#ifdef KERNEL_CENTOS65
		cpustat[CPUSTAT_IDLE]		= cputime64_to_clock_t(get_cpu_idle_time_us(cpu, NULL)/1000L);
		cpustat[CPUSTAT_IOWAIT]		= cputime64_to_clock_t(get_cpu_iowait_time_us(cpu, NULL)/1000L);
#else /* NOT KERNEL_CENTOS65 */
		cpustat[CPUSTAT_IDLE]		= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.idle      );
		cpustat[CPUSTAT_IOWAIT]		= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.iowait    );
#endif /* KERNEL_CENTOS65 */
		cpustat[CPUSTAT_IRQ]		= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.irq       );
		cpustat[CPUSTAT_SOFTIRQ]	= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.softirq   );
		cpustat[CPUSTAT_STEAL]		= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.steal     );
		cpustat[CPUSTAT_GUEST]		= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.guest     );
#ifndef KERNEL_CENTOS65
		cpustat[CPUSTAT_GUEST_NICE]	= cputime64_to_clock_t(kstat_cpu(cpu).cpustat.guest_nice);
#endif /* KERNEL_CENTOS65 */
	// Get total utilization (sum of the values of all single cores)
	} else {
		int i;
		for (i=0; i<nr_cpu_ids; ++i) {
			cpustat[CPUSTAT_USER]		+= cputime64_to_clock_t(kstat_cpu(i).cpustat.user      );
			cpustat[CPUSTAT_NICE]		+= cputime64_to_clock_t(kstat_cpu(i).cpustat.nice      );
			cpustat[CPUSTAT_SYSTEM]		+= cputime64_to_clock_t(kstat_cpu(i).cpustat.system    );
#ifdef KERNEL_CENTOS65
			cpustat[CPUSTAT_IDLE]		+= cputime64_to_clock_t(get_cpu_idle_time_us(i, NULL)/1000L);
			cpustat[CPUSTAT_IOWAIT]		+= cputime64_to_clock_t(get_cpu_iowait_time_us(i, NULL)/1000L);
#else /* NOT KERNEL_CENTOS65 */
			cpustat[CPUSTAT_IDLE]		+= cputime64_to_clock_t(kstat_cpu(i).cpustat.idle      );
			cpustat[CPUSTAT_IOWAIT]		+= cputime64_to_clock_t(kstat_cpu(i).cpustat.iowait    );
#endif /* KERNEL_CENTOS65 */
			cpustat[CPUSTAT_IRQ]		+= cputime64_to_clock_t(kstat_cpu(i).cpustat.irq       );
			cpustat[CPUSTAT_SOFTIRQ]	+= cputime64_to_clock_t(kstat_cpu(i).cpustat.softirq   );
			cpustat[CPUSTAT_STEAL]		+= cputime64_to_clock_t(kstat_cpu(i).cpustat.steal     );
			cpustat[CPUSTAT_GUEST]		+= cputime64_to_clock_t(kstat_cpu(i).cpustat.guest     );
#ifndef KERNEL_CENTOS65
			cpustat[CPUSTAT_GUEST_NICE]	+= cputime64_to_clock_t(kstat_cpu(i).cpustat.guest_nice);
#endif /* KERNEL_CENTOS65 */
		}
	}
#endif /* KERNEL_CENTOS7 */
	return CPUSTATS * sizeof(u64);
}

/* Read Total, free,used RAM and swap.
 * The function si_swapinfo() is not callable from a kernel module so the needed values are read from the proc fs.
 * In future kernel versions this may be changed.*/
static int read_meminfo(u64* meminfo){
	struct sysinfo sys_info;
	u64 cached;
	int j=0;
	char* no_end;
	int no_start;
	char* line = kmalloc(28*sizeof(char), GFP_KERNEL);
	char* val_name = kmalloc(12*sizeof(char), GFP_KERNEL);
	char* val = kmalloc(10*sizeof(char), GFP_KERNEL);
	
	if(!line || !val_name || !val){
		return -ENOMEM;
	}
	
	memset(meminfo, 0, MEMINFO_SIZE * sizeof(u64));
	
	si_meminfo(&sys_info);
	
	//si_swapinfo(&sys_info);
	
	meminfo[CPU_MEM_RAM_TOTAL] = K(sys_info.totalram);
	
	/*Read every line and search for the needed values.
	 *In future this values may be read directly from the sys_info struct.*/
	for(j = 0; j < 30; j++){
		memset(line, '\0', 28);
		memset(val, 0, 10);
		memset(val_name, 0, 12);
		file_read(proc_meminfo_file, 28 *j , line, 28);
		//printk(" %28s ", line);
		snprintf(val_name, 12, "%s", line);
		//printk("%s", val_name);
		/*read cached from /proc/meminfo*/
		if(!strncmp(val_name, "Cached:", 7)){
			snprintf(val, 10, "%s", line+16);
			no_start=0;
			while((no_start < 9) && (val[no_start] == ' ')){
				no_start++;
			}
			cached = simple_strtoul(&val[no_start], &no_end, 10);
			meminfo[CPU_MEM_RAM_FREE] = K(sys_info.freeram + sys_info.bufferram) + cached;
		}
		/*read SwapTotal from /proc/meminfo*/
		if(!strncmp(val_name, "SwapTotal:", 10)){
			snprintf(val, 10, "%s", line+16);
			no_start=0;
			while((no_start < 9) && (val[no_start] == ' ')){
				no_start++;
			}
			meminfo[CPU_MEM_SWAP_TOTAL] = simple_strtoul(&val[no_start], &no_end, 10);
		}
		/*read SwapFree from /proc/meminfo*/
		if(!strncmp(val_name, "SwapFree:", 9)){
			snprintf(val, 10, "%s", line+16);
			no_start=0;
			while((no_start < 9) && (val[no_start] == ' ')){
				no_start++;
			}
			meminfo[CPU_MEM_SWAP_FREE] = simple_strtoul(&val[no_start], &no_end, 10);
		}
	}
	
	meminfo[CPU_MEM_RAM_USED] = meminfo[CPU_MEM_RAM_TOTAL] - meminfo[CPU_MEM_RAM_FREE];
	meminfo[CPU_MEM_SWAP_USED] = meminfo[CPU_MEM_SWAP_TOTAL] - meminfo[CPU_MEM_SWAP_FREE];
	
	kfree(line);
	kfree(val);
	kfree(val_name);
	
	return MEMINFO_SIZE * sizeof(u64);

}

/*Read the MSR register reg from core no. cpu, data is an 8 byte buffer*/
static int read_msr_reg(int cpu, u32 reg, u32* data){
	int err = 0;
	err = rdmsr_safe_on_cpu(cpu, reg, &data[0], &data[1]);
	if (err)
		return err;
	else
		return 8;
}

/*Send an IPMI request the response is in data->datarc*/
static int ipmi_request(int netfn, int cmd, unsigned char * msgdata, int size){
	struct kernel_ipmi_msg msg;
	int rv;
	mutex_lock(&ipmi_mutex);
	ipmi_data ->datarc_length = 0;
	
	init_completion(&ipmi_data->read_complete);

	msg.netfn = netfn;
	msg.cmd = cmd;
	msg.data_len = size;
	msg.data = msgdata;
	
	rv = ipmi_validate_addr(&ipmi_data->address, sizeof(ipmi_data->address));
	if(rv){
		printk("Measure: Bad IPMI address!\n");
		mutex_unlock(&ipmi_mutex);
		return -rv;
	}
	rv = ipmi_request_settime(ipmi_data->user, &(ipmi_data->address), ipmi_data->msgid, &msg, ipmi_data, 0, -1, 0);
	if(rv){
		printk("Measure: Error in IPMI request!\n");
		mutex_unlock(&ipmi_mutex);
		return -rv;
	}
	//The timeout value is in jiffies
	rv = wait_for_completion_timeout(&ipmi_data->read_complete, msecs_to_jiffies(ipmi_timeout));
	if(rv == 0){
		printk("Measure: IPMI request time out.\n");
		ipmi_data->msgid++;
		mutex_unlock(&ipmi_mutex);
		return -ETIMEDOUT;
	}
	ipmi_data->msgid++;
	mutex_unlock(&ipmi_mutex);	
	return 0;
	
}

/*Read data dependent on the previous lseek */
static ssize_t driver_read(struct file *fileptr, char __user *user_buffer, size_t user_buffer_size, loff_t *offset) {
	ssize_t size;
	int err = 0;
	int cpu = -1;
	u64 cpustat[CPUSTATS];
	u32 data[2];
	unsigned char ipmi_msg_data[IPMI_MAX_MSG_LENGTH];
	int msg_data_size;
	int ipmi_mode = 0;
	int ipmi_read_id = 0;
	int ipmi_netfn;
	int ipmi_cmd;
	u32 msr_reg;
	u64 meminfo[MEMINFO_SIZE];
	//printk("driver_read: whence: %llx, cpu offset: %llx \n",(WHENCE_MASK & *offset) >> WHENCE_POS, (CPU_MASK & *offset) >> CPU_NO_POS);
	if(!check_cpu_stat_off(*offset)){
		size = CPUSTATS * sizeof(u64);
		memset(cpustat, 0, size);
		cpu = (int)((CPU_MASK & *offset) >> CPU_NO_POS);
		err = read_cpu_stats(cpustat, cpu);
		if (user_buffer_size != size) {
			return -EFAULT;
		}
		if (copy_to_user(user_buffer, cpustat, size) != 0) {
			printk("Measure: Cannot copy all data.\n");
			return -EFAULT;
		}
	} else if (!check_msr_off(*offset)){
		size = 2*sizeof(u32);
		if (user_buffer_size < size) {
			return -EFAULT;
		}
		cpu = (int)((CPU_MASK & *offset) >> CPU_NO_POS);
		msr_reg = (u32)(REG_MASK & *offset);
		memset(data, 0, size);
		err = read_msr_reg(cpu, msr_reg, data);
		if(err!=8){
			memset(data, 0, size);
		}
		if (copy_to_user(user_buffer, data, size) != 0) {
			printk("Measure: Cannot copy all data.\n");
			return -EFAULT;
		}
	}else if(!check_ipmi_offset(*offset) && ipmi_supported){
		ipmi_mode = (int)((CPU_MASK & *offset) >> CPU_NO_POS);
		ipmi_read_id = *offset & RECORD_ID_MASK;
		if(ipmi_mode == IPMI_GET_SDR){
			ipmi_netfn = STORAGE_NETFN;
			ipmi_cmd = CMD_GET_SDR;
			ipmi_msg_data[0] = 0x00;
			ipmi_msg_data[1] = 0x00;
			ipmi_msg_data[2] = ipmi_read_id & 0xff;
			ipmi_msg_data[3] = (ipmi_read_id & 0xff00) >> 8;
			ipmi_msg_data[4] = 0x00;
			ipmi_msg_data[5] = READ_FULL_RECORD;
			msg_data_size = 6;
		} else if(ipmi_mode == (int)(IPMI_GET_SENSOR_READING >> CPU_NO_POS)){
			ipmi_netfn = SENSOR_NETFN;
			ipmi_cmd = CMD_GET_SENSOR_READING;
			ipmi_msg_data[0] = ipmi_read_id & 0xff;
			msg_data_size = 1;
		} else if(ipmi_mode == (int)(IPMI_DELL_RESET_ENERGY >> CPU_NO_POS)){
			ipmi_netfn = IPMI_NET_FN_OEM_DELL_GENERIC_RQ;
			ipmi_cmd = IPMI_CMD_OEM_DELL_RESET_POWER_CONSUMPTION_DATA;
			ipmi_msg_data[0] = IPMI_ENTITY_ID_SYSTEM_BOARD;
			ipmi_msg_data[1] = 0x01;
			ipmi_msg_data[2] = IPMI_OEM_DELL_RESET_POWER_CONSUMPTION_DATA_CUMULATIVE;
			msg_data_size = 3;
		}else if(ipmi_mode == (int)(IPMI_DELL_CUMULATIVE_ENERGY >> CPU_NO_POS)){
			ipmi_netfn = IPMI_NET_FN_OEM_DELL_GENERIC_RQ;
			ipmi_cmd = IPMI_CMD_OEM_DELL_GET_POWER_CONSUMPTION_DATA;
			ipmi_msg_data[0] = IPMI_ENTITY_ID_SYSTEM_BOARD;
			ipmi_msg_data[1] = 0x01;
			msg_data_size = 2;
		}else if(ipmi_mode == (int)(IPMI_DELL_CURRENT_POWER >> CPU_NO_POS)){
			ipmi_netfn = IPMI_NET_FN_OEM_DELL_GENERIC_RQ;
			ipmi_cmd = IPMI_CMD_OEM_DELL_POWER_CONSUMPTION;
			ipmi_msg_data[0] = IPMI_ENTITY_ID_POWER_SUPPLY;
			ipmi_msg_data[1] = IPMI_OEM_DELL_POWER_CONSUMPTION_ENTITY_INSTANCE_ALL;
			msg_data_size = 2;
		} else {
			return -EFAULT;
		}
		err = ipmi_request(ipmi_netfn, ipmi_cmd, ipmi_msg_data, msg_data_size);
		if(err){
			printk("Measure: Error in IPMI Request: %d\n", err);
			return err;
		}
		err = ipmi_data->datarc_length;
				
		if (copy_to_user(user_buffer, ipmi_data->datarc, err) != 0) {
			printk("Measure: Cannot copy all data.\n");
			return -EFAULT;
		}
	}else if(!check_mem_info_offset(*offset)){
		size = MEMINFO_SIZE * sizeof(u64);
		memset(meminfo, 0, size);
		err = read_meminfo(meminfo);
		if (user_buffer_size != size) {
			return -EFAULT;
		}
		if (copy_to_user(user_buffer, meminfo, size) != 0) {
			printk("Measure: Cannot copy all data.\n");
			return -EFAULT;
		}
	}else{
		printk("Measure: Cannot read! Check the offset provided with lseek!\n");
		return -EINVAL;
	}
	
	return err;
}

/*Write to msr registers*/
static ssize_t driver_write(struct file *fileptr, const char __user *user_buffer, size_t user_buffer_size, loff_t *offset){
	int cpu = -1;
	int err = 0;
	u32 msr_reg;
	u32 data[2];
	ssize_t size = sizeof(u32)*2;
	if(!check_msr_off(*offset)){
		if(user_buffer_size != size)
			return -EINVAL;
		cpu = (int)((CPU_MASK & *offset) >> CPU_NO_POS);
		msr_reg = (u32)(REG_MASK & *offset);
		if(msr_reg != IA32_MPERF &&	msr_reg != IA32_APERF){
			printk("Measure: This driver allows only to write to IA32_APERF (0x%x) and IA32_MPERF(0x%x)!\n", IA32_APERF, IA32_MPERF);
			return -EPERM;
		}
		if(copy_from_user(&data, user_buffer, size))
			return -EFAULT;
		err = wrmsr_safe_on_cpu(cpu, msr_reg, data[0], data[1]);
		if(err)
			return err;
		else
			return size;
	}
	printk("Measure: Cannot write! Check the offset provided with lseek!\n");
	return -EINVAL;
}

/*All incoming ipmi messages get passed to this handler*/
static void ipmi_msg_handler(struct ipmi_recv_msg *msg, void *user_msg_data){
	IPMI_DATA *user_data = user_msg_data;

	if(msg->msgid != user_data->msgid){
		printk("Measure: Missmatch between received msgid %li and transmitted msgid %li!\n", msg->msgid, user_data->msgid);
		user_data->datarc_length = 0;
	} else {
		user_data->datarc_length = 1;
		if (msg->msg.data_len > 0){
			user_data->datarc[0] = msg->msg.data[0];
		}else{
			user_data->datarc[0] = IPMI_UNKNOWN_ERR_COMPLETION_CODE;
		}
		if(msg->msg.data_len > 1 && msg->msg.data_len < IPMI_MAX_MSG_LENGTH){
			user_data->datarc_length = msg -> msg.data_len;
			memcpy(user_data->datarc+1, msg->msg.data + 1, user_data->datarc_length-1);
		}
		complete(&user_data->read_complete);
	}
	ipmi_free_recv_msg(msg);	
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int driver_ioctl(struct inode *inode, struct file *file, unsigned int request, unsigned long arg)
#else
static long driver_ioctl(struct file *f, unsigned int request, unsigned long arg)
#endif
{	
	unsigned long new_timeout;
	if(!ipmi_supported){
		printk("Measure: ERROR no ioctl calls are supported!\n");
		return -EINVAL;
	}
	
	switch(request){
		case IOC_GET_IPMI_TIMEOUT:
			if (copy_to_user((unsigned long*)arg, &ipmi_timeout, sizeof(unsigned long))){
				return -EACCES;
			}
			break;
		case IOC_SET_IPMI_TIMEOUT:
			if(ipmi_timeout_locked){
				return ERROR_IPMI_TIMEOUT_LOCKED;
			}
			mutex_lock(&ioctl_mutex);
			if (copy_from_user(&new_timeout, (unsigned long *)arg, sizeof(unsigned long))) {
				mutex_unlock(&ioctl_mutex);
				return -EACCES;
			}
			if(new_timeout > IPMI_MAX_TIMEOUT || new_timeout == 0){
				ipmi_timeout = IPMI_MAX_TIMEOUT;
				return ERROR_IPMI_TIMEOUT_MAX;
				mutex_unlock(&ioctl_mutex);
			} else {
				ipmi_timeout = new_timeout;
			}
			break;
		case IOC_SET_AND_LOCK_IPMI_TIMEOUT:
			mutex_lock(&ioctl_mutex);
			ipmi_timeout_locked = 1;
			if (copy_from_user(&new_timeout, (unsigned long *)arg, sizeof(unsigned long))) {
				mutex_unlock(&ioctl_mutex);
				return -EACCES;
			}
			if(new_timeout > IPMI_MAX_TIMEOUT || new_timeout == 0){
				ipmi_timeout = IPMI_MAX_TIMEOUT;
				mutex_unlock(&ioctl_mutex);
				return ERROR_IPMI_TIMEOUT_MAX;
			} else {
				ipmi_timeout = new_timeout;
			}
			break;
		default:
			printk("Measure: Error not supported ioctl request code!\n");
			return -EINVAL;
	}
	mutex_unlock(&ioctl_mutex);
	
	return 0;
}

/* Open File*/
static struct file* file_open(const char* path, int flags, int rights) {
	struct file* filp = NULL;
	mm_segment_t oldfs;
	int err = 0;

	oldfs = get_fs();
	set_fs(get_ds());
	filp = filp_open(path, flags, rights);
	set_fs(oldfs);
	if(IS_ERR(filp)) {
		err = PTR_ERR(filp);
		return NULL;
	}
	return filp;
}

/*Close file*/
static void file_close(struct file* file) {
	filp_close(file, NULL);
}

/* read file */
static int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
	mm_segment_t oldfs;
	int ret;

	oldfs = get_fs();
	set_fs(get_ds());

	ret = vfs_read(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}  

static int driver_open(struct inode *device, struct file *fileptr) {
	int if_num = iminor(device);
	int rv;
	
	mutex_lock(&ipmi_mutex);
	if(opened !=0){
		printk("Measure: Device is already opened!\n");
		opened++;
		mutex_unlock(&ipmi_mutex);
		return 0;
	}
	if(ipmi_supported){
		ipmi_data = kmalloc(sizeof(*ipmi_data), GFP_KERNEL);
		if(!ipmi_data){
			mutex_unlock(&ipmi_mutex);
			return -ENOMEM;
		}
		ipmi_data->msgid = 1;
		rv = ipmi_create_user(if_num, &ipmi_hndlrs, ipmi_data , &(ipmi_data->user));
		if (rv) {
			kfree(ipmi_data);
			mutex_unlock(&ipmi_mutex);
			return rv;
		}
	}
	opened++;
	mutex_unlock(&ipmi_mutex);
	
	if(ipmi_supported){
		ipmi_data->address.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
		ipmi_data->address.channel = IPMI_BMC_CHANNEL;
		ipmi_data->address.data[0] = 0;
		if(!ipmi_timeout_locked){
 			ipmi_timeout = IPMI_MAX_TIMEOUT;
		}
	}
	
	proc_meminfo_file = file_open("/proc/meminfo",0, O_RDONLY);
	
	printk("Measure: Opened driver.\n");
	return 0;
}

static int driver_close(struct inode *device, struct file *fileptr) {
	int rv;
	if(opened <= 1){
		if(ipmi_supported){
			rv = ipmi_destroy_user(ipmi_data->user);
			if (rv)
				return rv;
			kfree(ipmi_data);
			ipmi_data = NULL;
			ipmi_timeout_locked = 0;
		}		
		file_close(proc_meminfo_file);
		proc_meminfo_file = NULL;
	}
	printk("Measure: Closed driver.\n");
	opened--;
	return 0;
}

/*
 * MODULE/DEVICE MANAGEMENT
 */
static dev_t device_number;
static struct cdev *driver_object;
static struct class *device_class;
static struct device *device;

static int __init mod_init(void) {
	/*Check the defined number of cpus*/
	int i=0;
	struct cpuinfo_x86 *cpuinfo;
	const struct dmi_device *dev = NULL;
#ifdef KERNEL_CENTOS65
	lock_kernel();
#endif
	for(i = 0; i < nr_cpu_ids; i++){
		if(!cpu_online(i)){
			printk("Measure: ERROR defined nr_cpu_ids in driver_measure.h is larger than the actual number of cores in your System!\n");
			#ifdef KERNEL_CENTOS65
			unlock_kernel();
			#endif
			return -ENXIO; /*No such device*/
		}
		cpuinfo = &cpu_data(i);
		if (!cpu_has(cpuinfo, X86_FEATURE_MSR)){
			#ifdef KERNEL_CENTOS65
			unlock_kernel();
			#endif
			return -EIO; /* MSR not supported */
		}
	}
#ifdef KERNEL_CENTOS65
	unlock_kernel();
#endif
	
	if (alloc_chrdev_region(&device_number, 0, 1, "measure") < 0) {
		printk("Measure: Cannot load module.\n");
		return -EIO;
	}
	
	driver_object = cdev_alloc();
	if (NULL == driver_object) {
		printk("Measure: Cannot load module.\n");
		unregister_chrdev_region(device_number, 1);
		return -EIO;
	}
	
	driver_object->owner	= THIS_MODULE;
	driver_object->ops		= &fileops;
	
	if (cdev_add(driver_object, device_number, 1)) {
		printk("Measure: Cannot load module.\n");
		kobject_put(&driver_object->kobj);
		unregister_chrdev_region(device_number, 1);
		return -EIO;
	}
	
	device_class = class_create(THIS_MODULE, "measure");
	if (IS_ERR(device_class)) {
		printk("Measure: No udev support\n");
		kobject_put(&driver_object->kobj);
		unregister_chrdev_region(device_number, 1);
		return -EIO;
	}
	
	if(dmi_find_device(DMI_DEV_TYPE_IPMI, NULL, dev)){
		printk("Measure: Detected IPMI device.\n");
		ipmi_supported=1;
	}
	
	device = device_create(device_class, NULL, device_number, NULL, "%s", "measure");
	printk("Measure: Module loaded successfully.\n");
	
	return 0;
}

static void __exit mod_exit(void) {
	device_destroy(device_class, device_number);
	class_destroy(device_class);
	cdev_del(driver_object);
	unregister_chrdev_region(device_number, 1);
	
	printk("Measure: Module unloaded successfully.\n");
}

module_init( mod_init );
module_exit( mod_exit );

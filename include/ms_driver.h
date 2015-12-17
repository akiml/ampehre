/*
 * ms_driver.h
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
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
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 */

#ifndef __MS_DRIVER_H__
#define __MS_DRIVER_H__

// ----- USER DEFINED SETTINGS ---

#ifdef SHOW_WARNINGS
#warning "Don't forget to change this to KERNEL_CENTOS7 if you are running the driver on a CentOS 7 system."
/*Linux kernel 2.6.32 allows only whence values up to 2.
 If newer kernels are used LSEEK_MAX_2 can be undefined and then whence=3 is used for cpu_meminfo */
#warning "LSEEK_MAX_2 is a workaround for kernels < 3.1 but it also works with a 3.1 kernel. Defining LSEEK_MAX_2 is default."
#warning "Don't forget to change nr_cpu_ids value if you port the driver to a different system."
#endif /* SHOW_WARNINGS */

#define KERNEL_CENTOS65
#define LSEEK_MAX_2
#define nr_cpu_ids	8

// -------------------------------

/*values for whence*/
#define cpu_stats 0
#define cpu_msr 1
#define ipmi 2

#ifdef LSEEK_MAX_2
#define cpu_meminfo 0
#define MEMINFO 0x00ff000000000000
#define CPU_MEMINFO 0xff
#else
#define cpu_meminfo 3
#endif

#define CPU_NO_POS 48

#define CORE0 0x0000000000000000
#define CORE1 0x0001000000000000
#define CORE2 0x0002000000000000
#define CORE3 0x0003000000000000
#define CORE4 0x0004000000000000
#define CORE5 0x0005000000000000
#define CORE6 0x0006000000000000
#define CORE7 0x0007000000000000

#define IPMI_GET_SDR			0x0000000000000000
#define IPMI_GET_SENSOR_READING		0x0001000000000000
#define IPMI_DELL_RESET_ENERGY		0x0002000000000000
#define IPMI_DELL_CUMULATIVE_ENERGY 	0x0003000000000000
#define IPMI_DELL_CURRENT_POWER		0x0004000000000000

#define MAX_MSG_LENGTH 272

/* obtain what a single power unit is (in µWs) */
#define MSR_RAPL_POWER_UNIT			0x606
/* energy consumption of the package power domain: */
#define MSR_PKG_ENERGY_STATUS		0x611
/* energy consumption of the core power domain */
#define MSR_PP0_ENERGY_STATUS		0x639
/* energy consumption of the graphics power domain
 * currently not used in our library */
#define MSR_PP1_ENERGY_STATUS		0x641
/* energy consumption of the DRAM power domain */
#define MSR_DRAM_ENERGY_STATUS		0x619
/*temperature of a processor core (a core of a CPU) */
#define IA32_THERM_STATUS			0x19C
/* temperature of a processor package */
#define IA32_PACKAGE_THERM_STATUS	0x1B1
/*minimum TCC activation temperature
 * the temperature PROCHOT# bus signal rises */
#define MSR_TEMPERATURE_TARGET		0x1A2
/* get the time stamp counter to calculate average
 * frequency over a specific time period */
#define IA32_TIME_STAMP_COUNTER		0x10
/* get maximum performance counter
 * Intel advises us to use delta(IA32_APERF)/delta(IA32_MPERF) ratio as
 * IA32_MPERF unrelated to IA32_APERF makes no sense.*/
#define IA32_MPERF					0xE7
/* get actual performance counter
 * Intel advises us to use delate(IA32_APERF)/delate(IA32_MPERF) ratio as
 * IA32_APERF unrelated to IA32_MPERF makes no sense.*/
#define IA32_APERF					0xE8

/* memory usage from /proc/meminfo */
#define MEMINFO_SIZE 6
/* Total usable RAM*/
#define CPU_MEM_RAM_TOTAL	0
/*Free RAM (free memory + buffers + cached)*/
#define CPU_MEM_RAM_FREE	1
/*Used Ram = Total - Free*/
#define CPU_MEM_RAM_USED	2
/*Total swap memory*/
#define CPU_MEM_SWAP_TOTAL	3
/*Free swap memory*/
#define CPU_MEM_SWAP_FREE	4
/*Used swap memory = Total - Free*/
#define CPU_MEM_SWAP_USED	5

#define CPUSTAT_USER		0
#define CPUSTAT_NICE		1
#define CPUSTAT_SYSTEM		2
#define CPUSTAT_IDLE		3
#define CPUSTAT_IOWAIT		4
#define CPUSTAT_IRQ			5
#define CPUSTAT_SOFTIRQ		6
#define CPUSTAT_STEAL		7
#define CPUSTAT_GUEST		8
#ifndef KERNEL_CENTOS65
#define CPUSTAT_GUEST_NICE	9
#endif /* KERNEL_THREE */

#ifdef KERNEL_CENTOS65
#define CPUSTATS			9
#else /* NOT KERNEL_CENTOS65 */
#define CPUSTATS			10
#endif /* KERNEL_CENTOS65 */

/*supported ioctl request codes*/
#define IOC_GET_IPMI_TIMEOUT _IOR('i', 0, unsigned long*)
#define IOC_SET_IPMI_TIMEOUT _IOW('i', 1, unsigned long*)
#define IOC_SET_AND_LOCK_IPMI_TIMEOUT _IOW('i', 2, unsigned long*)

#define ERROR_IPMI_TIMEOUT_LOCKED	1
#define ERROR_IPMI_TIMEOUT_MAX		2

#endif /* __MS_DRIVER_H__ */

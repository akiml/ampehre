/*
 * ms_hetschedwrapper_client.h
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
 * created: 8/06/14
 * version: 0.1.0 - initial implementation
 */

#ifndef __MS_HETSCHEDWRAPPER_CLIENT_H__
#define __MS_HETSCHEDWRAPPER_CLIENT_H__

#define NUM_OF_MEASURES 6

enum measures {
	// CPU
	m_cpu0_energy_total_pkg, m_cpu0_energy_total_dram,
	m_cpu1_energy_total_pkg, m_cpu1_energy_total_dram,
	// GPU
	m_gpu_energy_total,
	// FPGA
	m_fpga_energy_total
};

static __attribute__((unused)) const char *measures_str[NUM_OF_MEASURES] = {
	// CPU
	"cpu0_energy_total_pkg", "cpu0_energy_total_dram",
	"cpu1_energy_total_pkg", "cpu1_energy_total_dram",
	// GPU
	"gpu_energy_total",
	// FPGA
	"fpga_energy_total"
};

// Define here what you want to transmit from scheduler to client.
typedef struct __mtransmeasurement {
	double measurement[NUM_OF_MEASURES];
} MTRANSMEASUREMENT;

typedef struct __mtransresource {
	MTRANSMEASUREMENT init;
	MTRANSMEASUREMENT compute;
	MTRANSMEASUREMENT free;
} MTRANSRESOURCE;

// Transport struct. This struct is used to copy measurements from scheduler to client.
typedef struct __mtrans {
	MTRANSRESOURCE cpu;
	MTRANSRESOURCE gpu;
	MTRANSRESOURCE fpga;
} MTRANS;

#endif /* __MS_HETSCHEDWRAPPER_CLIENT_H__ */

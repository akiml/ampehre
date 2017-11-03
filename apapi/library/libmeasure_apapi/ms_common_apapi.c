/*                                                                                                                                              * ms_common_apapi.c
 * 
 * Copyright (C) 2017, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>, Alex Wiens <awiens@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Alex Wiens (awiens@mail.uni-paderborn.de)
 * created: 6/01/17
 * version: 0.8.0 - initial implementation
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "ms_common_apapi.h"
#include "papi.h"
#define APAPI_PRINTERR(...) fprintf(stderr, "MS_COMMON_APAPI %s:%d ", __FILE__, __LINE__);fprintf(stderr, __VA_ARGS__);
#define APAPI_PRINT(...) fprintf(stdout, "MS_COMMON_APAPI: ");fprintf(stdout, __VA_ARGS__);

void __map(struct __mapper *map) {
	if (map == NULL) {
		return;
	}
	int i;
	// in case of factor=1, do a distinction?
	for(i=0; map[i].source != NULL; ++i) {
		switch(map[i].type) {
			case longlong2longlong:
				*((long long*)(map[i].destination)) = *((long long*)(map[i].source)) / map[i].factor;
			break;
			case longlong2double:
				*((double*)(map[i].destination)) = *((long long*)(map[i].source)) / map[i].factor;
			break;
			case longlong2uint32:
				*((uint32_t*)(map[i].destination)) = *((long long*)(map[i].source)) / map[i].factor;
			break;
			case longlong2uint64:
				*((uint64_t*)(map[i].destination)) = *((long long*)(map[i].source)) / map[i].factor;
			break;
			case double2double:
				*((double*)(map[i].destination)) = *((double*)(map[i].source)) / map[i].factor;
			break;
			case double2uint32:
				*((uint32_t*)(map[i].destination)) = *((double*)(map[i].source)) / map[i].factor;
			break;
			case double2uint64:
				*((uint64_t*)(map[i].destination)) = *((double*)(map[i].source)) / map[i].factor;
			break;
		}
	}
}

int __create_mapper(int msId, void *measurement, char *cmp, struct apapi_eventset *set, struct __mapper **result_map, int last_measurement){
	if (strncmp(cmp, "rapl", 4) == 0) {
		if (msId != CPU) {
			return -1;	
		}
		if (*result_map == NULL) {
			*result_map = (struct __mapper *) calloc(100, sizeof(struct __mapper));
		}
		struct __mapper *map = *result_map;
		MS_MEASUREMENT_CPU *cpu = (MS_MEASUREMENT_CPU *)measurement;
		int eNameMax = 100;
		char eName[eNameMax];
		int mapIx = 0;
		int pkgIx;
		int cpuIx;
		int eIx;
		for (pkgIx = 0; pkgIx < CPUS; ++pkgIx) {

			snprintf(eName, eNameMax, "rapl:::PACKAGE_ENERGY:PACKAGE%d", pkgIx);
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				if (1 == last_measurement) {
					// set energy total cpu 0 pkg  [mWs]:
					map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
					map[mapIx].destination =  &(cpu->msr_energy_acc[pkgIx][0]);
					map[mapIx].type = double2double;
					map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // Ws * 1000 = mWs
					mapIx++;
					// set power  avg   cpu 0 pkg  [mW ]:
					map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_AVG]); // value1 avg
					map[mapIx].destination =  &(cpu->msr_power_avg[pkgIx][0]);
					map[mapIx].type = double2double;
					map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // W * 1000 = mW
					mapIx++;
				}
				// set current power
				map[mapIx].source = &(set->last_values1[eIx]); // last value
				map[mapIx].destination =  &(cpu->msr_power_cur[pkgIx][0]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // W * 1000 = mW
				mapIx++;
			}
		
			if (last_measurement == 1) {
				snprintf(eName, eNameMax, "rapl:::PP0_ENERGY:PACKAGE%d", pkgIx);
				eIx = APAPI_apapi_eventset_find_event(set, eName);
				if (eIx != -1) {
					// set energy total cpu 0 pp0  [mWs]:
					map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
					map[mapIx].destination =  &(cpu->msr_energy_acc[pkgIx][1]);
					map[mapIx].type = double2double;
					map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // Ws * 1000 = mWs
					mapIx++;
					// set power  avg   cpu 0 pkg  [mW ]:
					map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_AVG]); // value1 avg
					map[mapIx].destination =  &(cpu->msr_power_avg[pkgIx][1]);
					map[mapIx].type = double2double;
					map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // W * 1000 = mW
					mapIx++;
				}
			}

			snprintf(eName, eNameMax, "rapl:::DRAM_ENERGY:PACKAGE%d", pkgIx);
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				if (1 == last_measurement) {
					// set energy total cpu 0 dram [mWs]:
					map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
					map[mapIx].destination =  &(cpu->msr_energy_acc[pkgIx][2]);
					map[mapIx].type = double2double;
					map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // Ws * 1000 = mWs
					mapIx++;
					// set power  avg   cpu 0 pkg  [mW ]:
					map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_AVG]); // value1 avg
					map[mapIx].destination =  &(cpu->msr_power_avg[pkgIx][2]);
					map[mapIx].type = double2double;
					map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // W * 1000 = mW
					mapIx++;
				}
				// set current power
				map[mapIx].source = &(set->last_values1[eIx]); // last value
				map[mapIx].destination =  &(cpu->msr_power_cur[pkgIx][2]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // W * 1000 = mW
				mapIx++;
			}

			snprintf(eName, eNameMax, "rapl:::THERM_STATUS:PACKAGE%d", pkgIx);
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				if (1 == last_measurement) {
					// set temp   max   cpu 0 pkg  [°C ]:
					map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
					map[mapIx].destination =  &(cpu->msr_temperature_pkg_max[pkgIx]);
					map[mapIx].type = double2uint32;
					map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C = °C
					mapIx++;
				}
				// set current temperature
				map[mapIx].source = &(set->current_samples[eIx]); // last value0
				map[mapIx].destination =  &(cpu->msr_temperature_pkg_cur[pkgIx]);
				map[mapIx].type = longlong2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C = °C
				mapIx++;
			}

			// for available cpus
			for (cpuIx = 0; cpuIx < CORES ; ++cpuIx) {

				if (1 == last_measurement) {
					snprintf(eName, eNameMax, "rapl:::THERM_STATUS:PACKAGE%d:CPU%d", pkgIx, cpuIx);
					eIx = APAPI_apapi_eventset_find_event(set, eName);
					if (eIx != -1) {
						// set temp   max   cpu 0 c 0  [°C ]:
						map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
						map[mapIx].destination =  &(cpu->msr_temperature_core_max[pkgIx][cpuIx]);
						map[mapIx].type = double2uint32;
						map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C = °C
						mapIx++;
					}
				}

				if (1 == last_measurement) {
					snprintf(eName, eNameMax, "rapl:::AVG_FREQUENCY:PACKAGE%d:CPU%d", pkgIx, cpuIx);
					eIx = APAPI_apapi_eventset_find_event(set, eName);
					if (eIx != -1) {
						// set freq   avg   cpu 0 c 0  [MHz]:
						map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
						map[mapIx].destination =  &(cpu->msr_freq_core_avg[pkgIx][cpuIx]);
						map[mapIx].type = double2double;
						map[mapIx].factor = set->event_ops[eIx].value0_prefix * 1000.0; // KHz * 1000 = MHz
						mapIx++;
					}
				}

				snprintf(eName, eNameMax, "rapl:::EFF_FREQUENCY:PACKAGE%d:CPU%d", pkgIx, cpuIx);
				eIx = APAPI_apapi_eventset_find_event(set, eName);
				if (eIx != -1) {
					if (1 == last_measurement) {
						// set freq   eff   cpu 0 c 0  [MHz]:
						map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
						map[mapIx].destination =  &(cpu->msr_freq_core_eff_avg[pkgIx][cpuIx]);
						map[mapIx].type = double2double;
						map[mapIx].factor = set->event_ops[eIx].value0_prefix * 1000.0; // KHz * 1000 = MHz
						mapIx++;
					}
					// set current frequency
					map[mapIx].source = &(set->current_samples[eIx]); // last value0
					map[mapIx].destination =  &(cpu->msr_freq_core_eff_cur[pkgIx][cpuIx]);
					map[mapIx].type = longlong2double;
					map[mapIx].factor = set->event_ops[eIx].value0_prefix * 1000.0; // KHz * 1000 = MHz
					mapIx++;
				}
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "rapl:::TIME_WORK");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set time   avg   cpus  work [ s ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(cpu->measure_util_active_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // s = s
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "rapl:::TIME_IDLE");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set time   avg   cpus  idle [ s ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(cpu->measure_util_idle_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // s = s
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "rapl:::TIME_UTIL");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set util   avg   cpus       [ % ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(cpu->measure_util_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // % = %
				mapIx++;
			}
			// set current utilization
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(cpu->measure_util_avg_cur);
			map[mapIx].type = longlong2double;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // % = %
			mapIx++;
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "rapl:::MEM_TOTAL");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set memory total cpu        [kiB]:
				map[mapIx].source = &(set->current_samples[eIx]); // last value0
				map[mapIx].destination =  &(cpu->measure_memory_cur[0]);
				map[mapIx].type = longlong2uint64;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB = kiB
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "rapl:::MEM_USED");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set memory max   cpu used   [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(cpu->measure_memory_used_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB = kiB
				mapIx++;
			}
			// set current memory
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(cpu->measure_memory_cur[CPU_MEM_RAM_USED]);
			map[mapIx].type = longlong2uint64;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB = kiB
			mapIx++;
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "rapl:::MEM_FREE");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set memory max   cpu free   [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(cpu->measure_memory_free_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB = kiB
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "rapl:::SWAP_TOTAL");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set swap   total cpu        [kiB]:
				map[mapIx].source = &(set->current_samples[eIx]); // last value
				map[mapIx].destination =  &(cpu->measure_memory_cur[3]);
				map[mapIx].type = longlong2uint64;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB = kiB
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "rapl:::SWAP_USED");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set swap   max   cpu used   [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(cpu->measure_swap_used_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB = kiB
				mapIx++;
			}
			// set current swap
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(cpu->measure_memory_cur[CPU_MEM_SWAP_USED]);
			map[mapIx].type = longlong2uint64;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB = kiB
			mapIx++;
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "rapl:::SWAP_FREE");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set swap   max   cpu free   [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(cpu->measure_swap_free_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB = kiB
				mapIx++;
			}
		}
	}

	if (strncmp(cmp, "nvml", 4) == 0) {
		if (msId != GPU) {
			return -1;
		}
		if (NULL == *result_map) {
			*result_map = (struct __mapper *) calloc(100, sizeof(struct __mapper));
		}
		struct __mapper *map = *result_map;
		MS_MEASUREMENT_GPU *gpu = (MS_MEASUREMENT_GPU *)measurement;
		int eNameMax = 100;
		char eName[eNameMax];
		int mapIx = 0;
		int eIx;

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "nvml:::Tesla_K20c:power");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set energy total gpu        [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(gpu->nvml_energy_acc);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000 = mWs
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:power");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set power  avg   gpu        [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(gpu->nvml_power_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000 = mW
				mapIx++;
			}
			// set current power
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_power_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000 = mW
			mapIx++;
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:temperature");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set temp   max   gpu        [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(gpu->nvml_temperature_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C = °C
				mapIx++;
			}
			// set current temperature
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_temperature_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C = °C
			mapIx++;
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:graphics_clock");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set freq   avg   gpu graph  [MHz]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(gpu->nvml_clock_graphics_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz = MHz
				mapIx++;
			}
			// set current frequency
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_clock_graphics_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz = MHz
			mapIx++;
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:sm_clock");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set freq   avg   gpu sm     [MHz]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(gpu->nvml_clock_sm_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz = MHz
				mapIx++;
			}
			// set current frequency
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_clock_sm_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz = MHz
			mapIx++;
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:memory_clock");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set freq   avg   gpu mem    [MHz]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(gpu->nvml_clock_mem_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz = MHz
				mapIx++;
			}
			// set current frequency
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_clock_mem_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz = MHz
			mapIx++;
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:gpu_utilization");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set util   avg   gpu gpu    [ % ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(gpu->nvml_util_gpu_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // % = %
				mapIx++;
			}
			// set current utilization
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_util_gpu_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // % = %
			mapIx++;
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:memory_utilization");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set util   avg   gpu mem    [ % ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(gpu->nvml_util_mem_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // % = %
				mapIx++;
			}
			// set current utilization
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_util_mem_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // % = %
			mapIx++;
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "nvml:::Tesla_K20c:total_memory");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set memory total gpu        [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(gpu->nvml_memory_total);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix * 1024.0; // B / 1024 = kiB
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:allocated_memory");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set memory max   gpu used   [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(gpu->nvml_memory_used_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix * 1024.0; // B / 1024 = kiB
				mapIx++;
			}
			// set current memory
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_memory_used_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix * 1024.0; // B / 1024 = kiB
			mapIx++;
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "nvml:::Tesla_K20c:unallocated_memory");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set memory max   gpu free   [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(gpu->nvml_memory_free_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix * 1024.0; // B / 1024 = kiB
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "nvml:::Tesla_K20c:processes");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set proc   max   gpu active [   ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(gpu->nvml_active_processes_count_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // # = #
				mapIx++;
			}
			// set current processes
			map[mapIx].source = &(set->current_samples[eIx]); // last value0
			map[mapIx].destination =  &(gpu->nvml_active_processes_count_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // # = #
			mapIx++;
		}
	}

	if (strncmp(cmp, "maxeler", 7) == 0) {
		if (msId != FPGA) {
			return -1;
		}
		if (NULL == *result_map) {
			*result_map = (struct __mapper *) calloc(100, sizeof(struct __mapper));
		}
		struct __mapper *map = *result_map;
		MS_MEASUREMENT_FPGA *fpga = (MS_MEASUREMENT_FPGA *)measurement;
		int eNameMax = 100;
		char eName[eNameMax];
		int mapIx = 0;
		int eIx;

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "maxeler:::vcc1v0_c_POWER:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   vcc1v0_c   [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_power_avg[VCC10]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total vcc1v0_c   [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(fpga->maxeler_energy_acc[VCC10]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "maxeler:::vcc1v5_ddr_POWER:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   vcc1v5_c   [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_power_avg[VCC15]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total vcc1v5_c   [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(fpga->maxeler_energy_acc[VCC15]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "maxeler:::vcc2v5_aux_POWER:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   vcc2v5_c   [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_power_avg[VCC25]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total vcc2v5_c   [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(fpga->maxeler_energy_acc[VCC25]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "maxeler:::imgt_1v0_POWER:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   imgt_1v0   [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_power_avg[IMGT10]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total imgt_1v0   [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(fpga->maxeler_energy_acc[IMGT10]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "maxeler:::imgt_1v2_POWER:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   imgt_1v2   [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_power_avg[IMGT12]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total imgt_1v2   [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(fpga->maxeler_energy_acc[IMGT12]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "maxeler:::mgt_1v0_POWER:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   mgt_1v0   [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_power_avg[MGT10]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total mgt_1v0   [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(fpga->maxeler_energy_acc[MGT10]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "maxeler:::mgt_1v2_POWER:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   mgt_1v2   [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_power_avg[MGT12]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total mgt_1v2   [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(fpga->maxeler_energy_acc[MGT12]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "maxeler:::fpga_POWER:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set power  avg   fpga       [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_power_avg[POWER]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total fpga       [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(fpga->maxeler_energy_acc[POWER]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
			// set current power
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(fpga->maxeler_power_cur[POWER]);
			map[mapIx].type = longlong2double;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // Ws * 1000  = mWs
			mapIx++;
		}

		snprintf(eName, eNameMax, "maxeler:::imgt_TEMPERATURE:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set temp   max   fpga  imgt [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(fpga->maxeler_temperature_max[ITEMP]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
			// set current temperature
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(fpga->maxeler_temperature_cur[ITEMP]);
			map[mapIx].type = longlong2double;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
			mapIx++;
		}

		snprintf(eName, eNameMax, "maxeler:::mmgt_TEMPERATURE:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set temp   max   fpga  mmgt [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(fpga->maxeler_temperature_max[MTEMP]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
			// set current temperature
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(fpga->maxeler_temperature_cur[MTEMP]);
			map[mapIx].type = longlong2double;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
			mapIx++;
		}

		snprintf(eName, eNameMax, "maxeler:::COMPUTE_UTIL:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set util   avg   fpga  comp [ % ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(fpga->maxeler_util_comp_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // % = %
				mapIx++;
			}
			// set current utilization
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(fpga->maxeler_util_comp_cur);
			map[mapIx].type = longlong2double;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // % = %
			mapIx++;
		}


	}

	if (strncmp(cmp, "micknc", 6) == 0) {
		if (msId != MIC) {
			return -1;
		}
		if (NULL == *result_map) {
			*result_map = (struct __mapper *) calloc(100, sizeof(struct __mapper));
		}
		struct __mapper *map = *result_map;
		MS_MEASUREMENT_MIC *mic = (MS_MEASUREMENT_MIC *)measurement;
		int eNameMax = 100;
		char eName[eNameMax];
		int mapIx = 0;
		int eIx;

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::POWER_PCIE:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   pcie       [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_power_avg[MIC_PCIE]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total pcie       [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(mic->mic_energy_acc[MIC_PCIE]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::POWER_C2X3:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   c2x3       [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_power_avg[MIC_C2X3]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total c2x3       [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(mic->mic_energy_acc[MIC_C2X3]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::POWER_C2X4:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   c2x4       [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_power_avg[MIC_C2X4]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total c2x4       [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(mic->mic_energy_acc[MIC_C2X4]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::POWER_VCCP:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   vccp       [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_power_avg[MIC_VCCP]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total vccp       [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(mic->mic_energy_acc[MIC_VCCP]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::POWER_VDDG:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   vddg       [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_power_avg[MIC_VDDG]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total vddg       [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(mic->mic_energy_acc[MIC_VDDG]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::POWER_VDDQ:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   vddq       [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_power_avg[MIC_VDDQ]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total vddq       [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(mic->mic_energy_acc[MIC_VDDQ]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "micknc:::POWER_MIC:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set power  avg   mic        [mW ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_power_avg[MIC_POWER]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
				mapIx++;
				// set energy total mic        [mWs]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(mic->mic_energy_acc[MIC_POWER]);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix / 1000.0; // Ws * 1000  = mWs
				mapIx++;
			}
			// set current power
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(mic->mic_power_cur[MIC_POWER]);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix / 1000.0; // W * 1000  = mW
			mapIx++;
		}

		snprintf(eName, eNameMax, "micknc:::TEMP_DIE:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set temp   max   die        [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_temperature_max[MIC_DIE_TEMP]);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
			// set current temperature
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(mic->mic_temperature_cur[MIC_DIE_TEMP]);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
			mapIx++;

		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::TEMP_GDDR:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set temp   max   gddr        [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_temperature_max[MIC_GDDR_TEMP]);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::TEMP_FANIN:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set temp   max   fan in      [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_temperature_max[MIC_FAN_IN_TEMP]);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::TEMP_FANOUT:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set temp   max   fan out    [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_temperature_max[MIC_FAN_OUT_TEMP]);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::TEMP_VCCP:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set temp   max   vccp       [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_temperature_max[MIC_VCCP_TEMP]);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::TEMP_VDDG:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set temp   max   vddg       [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_temperature_max[MIC_VDDG_TEMP]);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::TEMP_VDDQ:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set temp   max   vddq       [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_temperature_max[MIC_VDDQ_TEMP]);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "micknc:::FREQ_CORE:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set freq   avg   mic core   [MHz]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_freq_core_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz  = MHz
				mapIx++;
			}
			// set current frequency
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(mic->mic_freq_core_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz  = MHz
			mapIx++;
		}

		snprintf(eName, eNameMax, "micknc:::FREQ_MEM:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set freq   avg   mic mem    [MHz]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_freq_mem_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz  = MHz
				mapIx++;
			}
			// set current frequency
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(mic->mic_freq_mem_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // MHz  = MHz
			mapIx++;

		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::TIME_ACTIVE:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set time   avg   mic active [ s ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(mic->mic_util_active_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // s  = s
				mapIx++;
			}
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::TIME_IDLE:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set time   avg   mic idle   [ s ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_ACC]); // value0 acc
				map[mapIx].destination =  &(mic->mic_util_idle_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // s  = s
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "micknc:::UTIL:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set util   avg   mic        [ % ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(mic->mic_util_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // %  = %
				mapIx++;
			}
			// set current utilization
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(mic->mic_util_avg_cur);
			map[mapIx].type = longlong2double;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // %  = %
			mapIx++;

		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::MEM_TOTAL:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set memory total mic        [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_memory_total);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB  = kiB
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "micknc:::MEM_USED:DEVICE0");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set memory total mic        [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_memory_used_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB  = kiB
				mapIx++;
			}
			// set current memory
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(mic->mic_memory_used_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB  = kiB
			mapIx++;
		}

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "micknc:::MEM_FREE:DEVICE0");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set memory total mic        [kiB]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(mic->mic_memory_free_max);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // kiB  = kiB
				mapIx++;
			}
		}
	}

	if (strncmp(cmp, "ipmi", 4) == 0) {
		if (msId != SYSTEM) {
			return -1;
		}
		if (NULL == *result_map) {
			*result_map = (struct __mapper *) calloc(100, sizeof(struct __mapper));
		}
		struct __mapper *map = *result_map;
		MS_MEASUREMENT_SYS *sys = (MS_MEASUREMENT_SYS *)measurement;
		int eNameMax = 100;
		char eName[eNameMax];
		int mapIx = 0;
		int eIx;

		if (1 == last_measurement) {
			snprintf(eName, eNameMax, "ipmi:::BOARD_POWER");
			eIx = APAPI_apapi_eventset_find_event(set, eName);
			if (eIx != -1) {
				// set power  avg   board      [ W ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(sys->ipmi_power_sysboard_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // W  = W
				mapIx++;
				// set energy total board      [Ws ]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(sys->ipmi_energy_sysboard_acc);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix; // Ws  = Ws
				mapIx++;
			}
		}

		snprintf(eName, eNameMax, "ipmi:::SYSTEM_POWER");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set power  avg   system     [ W ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_AVG]); // value0 avg
				map[mapIx].destination =  &(sys->ipmi_power_server_avg);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // W  = W
				mapIx++;
				// set energy total system     [Ws ]:
				map[mapIx].source = &(set->values1[eIx * APAPI_FIELDS + APAPI_ACC]); // value1 acc
				map[mapIx].destination =  &(sys->ipmi_energy_server_acc);
				map[mapIx].type = double2uint32;
				map[mapIx].factor = set->event_ops[eIx].value1_prefix; // Ws  = Ws
				mapIx++;
			}
			// set current power
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(sys->ipmi_power_server_cur);
			map[mapIx].type = longlong2uint32;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // W  = W
			mapIx++;

		}

		snprintf(eName, eNameMax, "ipmi:::SYSTEM_TEMP");
		eIx = APAPI_apapi_eventset_find_event(set, eName);
		if (eIx != -1) {
			if (1 == last_measurement) {
				// set temp   max   system     [°C ]:
				map[mapIx].source = &(set->values0[eIx * APAPI_FIELDS + APAPI_MAX]); // value0 max
				map[mapIx].destination =  &(sys->ipmi_temperature_sysboard_max);
				map[mapIx].type = double2double;
				map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
				mapIx++;
			}
			// set current temperature
			map[mapIx].source = &(set->current_samples[eIx]); // last value
			map[mapIx].destination =  &(sys->ipmi_temperature_sysboard_cur);
			map[mapIx].type = longlong2double;
			map[mapIx].factor = set->event_ops[eIx].value0_prefix; // °C  = °C
			mapIx++;

		}
	}

	return 0;
}

long long _getcurrenttime() {
    struct timespec result;
    clock_gettime(CLOCK_MONOTONIC_RAW, &result);
    return (long long)result.tv_sec* 1000000000LL + (long long)result.tv_nsec;
}


void measure_update(void** args, int last_measurement) {
	struct __mgmt_update *update = (struct __mgmt_update *) args;
	if (0 == last_measurement) {
		__map(update->map);
	} else {
		__map(update->map_last);
		MS_MEASUREMENT_CPU *cpu;
		MS_MEASUREMENT_GPU *gpu;
		MS_MEASUREMENT_FPGA *fpga;
		MS_MEASUREMENT_MIC *mic;
		MS_MEASUREMENT_SYS *sys;
		switch(update->measurement_id) {
			case CPU:
				cpu = (MS_MEASUREMENT_CPU *)update->measurement;
				cpu->msr_time_runtime = (double)(update->set->current_time - update->set->first_time) / 1000000000.0;
			break;
			case GPU:
				gpu = (MS_MEASUREMENT_GPU *)update->measurement;
				gpu->nvml_time_runtime = (double)(update->set->current_time - update->set->first_time) / 1000000000.0;
			break;
			case FPGA:
				fpga = (MS_MEASUREMENT_FPGA *)update->measurement;
				fpga->maxeler_time_runtime = (double)(update->set->current_time - update->set->first_time) / 1000000000.0;
			break;
			case MIC:
				mic = (MS_MEASUREMENT_MIC *)update->measurement;
				mic->mic_time_runtime = (double)(update->set->current_time - update->set->first_time) / 1000000000.0;
			break;
			case SYSTEM:
				sys = (MS_MEASUREMENT_SYS *)update->measurement;
		
				sys->ipmi_time_runtime = (double)(update->set->current_time - update->set->first_time) / 1000000000.0;
			break;
		}
	}
}

MS_SYSTEM *ms_init(MS_VERSION* version, enum cpu_governor cpu_gov, uint64_t cpu_freq_min, uint64_t cpu_freq_max,
                 enum gpu_frequency gpu_freq, enum ipmi_timeout_setting timeout_setting, enum skip_ms_rate skip_ms, enum lib_variant variant) {

	MS_SYSTEM *ms = (MS_SYSTEM*) calloc(1, sizeof(MS_SYSTEM));
	if (ms == NULL) {
		return NULL;
	}
	
	struct __mgmt_internal *mgmt = (struct __mgmt_internal*) calloc(1, sizeof(struct __mgmt_internal));
	if (mgmt == NULL) {
		free(ms);
		ms = NULL;
		return NULL;
	}
	ms->mgmt = mgmt;

	ms->config = (MS_CONFIG*) calloc(1, sizeof(MS_CONFIG));
	if (ms->config == NULL) {
		free(mgmt);
		mgmt = NULL;
		free(ms);
		ms = NULL;
		return NULL;
	}


    int retv;

	// prepare papi
	retv = APAPI_init();
    if (retv != PAPI_VER_CURRENT) {
		APAPI_PRINTERR("papi init: %d\n", retv)
		free(ms->config);
		free(mgmt);
		mgmt = NULL;
		free(ms);
		ms = NULL;
		return NULL;
    }

    // read APAPI_CMPLIST environment variable
	retv = APAPI_read_env_cmplist(&(mgmt->user_cmplist_buffer), &(mgmt->user_cmplist));


	char **component_list;
    if (mgmt->user_cmplist != NULL) {
		component_list = mgmt->user_cmplist;
    } else {
		component_list = known_components;
	}


	// check known components and prepare available components array
    int32_t known_cmp_count = 0;
    for(known_cmp_count = 0; known_components[known_cmp_count] != NULL; known_cmp_count++);
    mgmt->available_components = (int *) calloc(sizeof(int), known_cmp_count+1); // +1 for NULL pointer at the end
	char **avail_cmp = (char**) calloc(sizeof(char*), known_cmp_count+1);

	mgmt->num_cmp = known_cmp_count;

    // check if cmplist components are not compiled into PAPI
    int i, papicmpIx, knowncmpIx;
    for(i=0; component_list[i] != NULL; ++i) {
		papicmpIx = PAPI_get_component_index(component_list[i]);
		knowncmpIx = APAPI_cmp_cmplist_index(component_list[i], known_components);
        if (knowncmpIx == -1) {
			APAPI_PRINTERR("Component %s is not known to libmeasure.\n", component_list[i])
			exit(1);
		}
		if (papicmpIx < 0) {
			APAPI_PRINTERR("Component %s is not available in PAPI. Check your PAPI lib.\n", component_list[i])
            exit(1);
        }
    }


	// check available papi components
    int num_components = PAPI_num_components();
    const PAPI_component_info_t *component_infos[num_components];
    int available_cmp_count = 0;
	int in_cmplist = 0;
	int cmplistIx = 0;
	int known_cmplistIx = 0;
    for(i=0; i<num_components; i++) {
        component_infos[i] = PAPI_get_component_info(i);
		in_cmplist = 1;
		cmplistIx = APAPI_cmp_cmplist_index(component_infos[i]->short_name, component_list);
		if (cmplistIx < 0) {
			in_cmplist = 0;
		}
        if (component_infos[i]->disabled != 0) {
            APAPI_PRINT("PAPI component %s disabled. Reason: %s\n", component_infos[i]->short_name, component_infos[i]->disabled_reason)
			if (in_cmplist == 1) {
				APAPI_PRINTERR("PAPI component %s is not available but should have been.\n", component_infos[i]->short_name)
				exit(1);
			}
            continue;
        }

		if (in_cmplist == 0) {
			continue;
		}
		known_cmplistIx = APAPI_cmp_cmplist_index(component_infos[i]->short_name, known_components);
        // add component to list of known and available components
		if (known_cmplistIx < 0) {
			APAPI_PRINTERR("Component %s is not known to libmeasure\n", component_infos[i]->short_name)
			exit(1);
		} else {

			mgmt->available_components[known_cmplistIx] = 1;
			avail_cmp[available_cmp_count] = known_components[known_cmplistIx];
            available_cmp_count++;

			int msId = component_mapping[known_cmplistIx];
			switch(msId) {
				case CPU:
					ms->config->cpu_enabled = 1;
				break;
				case GPU:
					ms->config->gpu_enabled = 1;
				break;
				case FPGA:
					ms->config->fpga_enabled = 1;
				break;
				case SYSTEM:
					ms->config->sys_enabled = 1;
				break;
				case MIC:
					ms->config->mic_enabled = 1;
				break;
			}
        }
    }

	if (available_cmp_count == 0) {
		APAPI_PRINTERR("No active components.\n")
		exit(1);
	}

	// read user eventops
	retv = APAPI_read_env_eventops(&(mgmt->user_eventops_file), &(mgmt->user_eventops), &(mgmt->user_eventops_num));

	// read user eventlist
	retv = APAPI_read_env_eventlist(avail_cmp, &(mgmt->user_eventlist_file), &(mgmt->user_eventlist_sorted), &(mgmt->user_eventlist_cmp));

	free(avail_cmp);
	avail_cmp = NULL;

	// prepare other arrays
	mgmt->sets = (struct apapi_eventset **) calloc(known_cmp_count, sizeof(struct apapi_eventset *));
	mgmt->timers = (struct apapi_timer **) calloc(known_cmp_count, sizeof(struct apapi_timer *));
	mgmt->update_args = (struct __mgmt_update *) calloc(known_cmp_count, sizeof(struct __mgmt_update));

	return ms;
}

void ms_init_fpga_force_idle(MS_SYSTEM *ms_system) {
	printf("ms_init_fpga_force_idle: This method is not available in ms_common_apapi\n");
}

void ms_fini(MS_SYSTEM *ms_system) {

	if (ms_system == NULL) {
		return;
	}
	if (ms_system->mgmt != NULL) {
		struct __mgmt_internal *mgmt = (struct __mgmt_internal*) ms_system->mgmt;
		if (mgmt->available_components != NULL) {
			free(mgmt->available_components);
		}
		int cmpIx;
		for (cmpIx = 0; cmpIx < mgmt->num_cmp; ++cmpIx) {
			if (mgmt->sets[cmpIx] != NULL) {
				APAPI_destroy_apapi_eventset(&(mgmt->sets[cmpIx]));
			}
			if (mgmt->timers[cmpIx] != NULL) {
				APAPI_destroy_timer(&(mgmt->timers[cmpIx]));
			}
			if (mgmt->update_args[cmpIx].map != NULL) {
				free(mgmt->update_args[cmpIx].map);
			}
			if (mgmt->update_args[cmpIx].map_last != NULL) {
				free(mgmt->update_args[cmpIx].map_last);
			}
		}
		if (mgmt->sets != NULL) {
			free(mgmt->sets);
		}
		if (mgmt->timers != NULL) {
			free(mgmt->timers);
		}
		if (mgmt->update_args != NULL) {
			free(mgmt->update_args);
		}
		if (mgmt->user_eventops_file != NULL) {
			free(mgmt->user_eventops_file);
		}
		if (mgmt->user_eventops != NULL) {
			free(mgmt->user_eventops);
		}
		free(ms_system->mgmt);
	}
	if (ms_system->config != NULL) {
		free(ms_system->config);
	}
	free(ms_system);
}

MS_LIST *ms_alloc_measurement(MS_SYSTEM *ms_system) {
	// mgmt.cpp
    MS_LIST *ms_list = NULL;
    MS_CONFIG *config = ms_system->config;
    
    if(config->cpu_enabled) {
        appendList(&ms_list, CPU);
    }   
    
    if(config->gpu_enabled) {
        appendList(&ms_list, GPU);
    }   
    
    if(config->fpga_enabled) {
        appendList(&ms_list, FPGA);
    }   
    
    if(config->sys_enabled) {
        appendList(&ms_list, SYSTEM);
    }   
    
    if(config->mic_enabled) {
        appendList(&ms_list, MIC);
    }   
    
    return ms_list;
}

void ms_free_measurement(MS_LIST *ms_list) {
	deleteList(&ms_list);
}

static void divide_sampling_rates(struct timespec *time_wait, uint64_t sec, uint64_t nsec, uint32_t check_for_exit_interrupts) {
    time_wait->tv_sec = sec / check_for_exit_interrupts;
    time_wait->tv_nsec = ((double)sec / check_for_exit_interrupts - time_wait->tv_sec) * 1000000000 + nsec / check_for_exit_interrupts;
    
}


void ms_set_timer(MS_LIST *ms_list, int flag, uint64_t sec, uint64_t nsec, uint32_t check_for_exit_interrupts) {

    if(check_for_exit_interrupts <= 0){
        check_for_exit_interrupts = 1;
    }
    void* ms_measurement = NULL;

    switch(flag) {
        case CPU:
            ms_measurement = getMeasurement(&ms_list, CPU);
            if(ms_measurement!=NULL) {
                divide_sampling_rates(&(((MS_MEASUREMENT_CPU *)ms_measurement)->msr_time_wait), sec, nsec, check_for_exit_interrupts);
                ((MS_MEASUREMENT_CPU *)ms_measurement)->msr_check_for_exit_interrupts = check_for_exit_interrupts;
            }
            break;
        case GPU:
            ms_measurement = getMeasurement(&ms_list, GPU);
            if(ms_measurement!=NULL) {
                divide_sampling_rates(&(((MS_MEASUREMENT_GPU *)ms_measurement)->nvml_time_wait), sec, nsec, check_for_exit_interrupts);
                ((MS_MEASUREMENT_GPU *)ms_measurement)->nvml_check_for_exit_interrupts = check_for_exit_interrupts;
            }
            break;
        case FPGA:
            ms_measurement = getMeasurement(&ms_list, FPGA);
            if(ms_measurement!=NULL) {
                divide_sampling_rates(&(((MS_MEASUREMENT_FPGA *)ms_measurement)->maxeler_time_wait), sec, nsec, check_for_exit_interrupts);
                ((MS_MEASUREMENT_FPGA *)ms_measurement)->maxeler_check_for_exit_interrupts = check_for_exit_interrupts;
            }
            break;
        case MIC:
            ms_measurement = getMeasurement(&ms_list, MIC);
            if(ms_measurement!=NULL) {
                divide_sampling_rates(&(((MS_MEASUREMENT_MIC *)ms_measurement)->mic_time_wait), sec, nsec, check_for_exit_interrupts);
                ((MS_MEASUREMENT_MIC *)ms_measurement)->mic_check_for_exit_interrupts = check_for_exit_interrupts;
            }
            break;
        case SYSTEM:
            ms_measurement = getMeasurement(&ms_list, SYSTEM);
            if(ms_measurement!=NULL) {
                divide_sampling_rates(&(((MS_MEASUREMENT_SYS *)ms_measurement)->ipmi_time_wait), sec, nsec, check_for_exit_interrupts);
                ((MS_MEASUREMENT_SYS *)ms_measurement)->ipmi_check_for_exit_interrupts = check_for_exit_interrupts;
            }
            break;
        default:
            //std::cout << "!!! 'mgmt' (thread main): Error: cannot set measurement timer. (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl;
            //exit(EXIT_FAILURE);
		break;
    }
	// ms_init_measurement() has to be called after ms_set_timer to apply the timer changes.
}

void getInterval(void *ms_measurement, int flag, struct timespec *time) {
	switch(flag) {
		case CPU:
			*time = ((MS_MEASUREMENT_CPU *)ms_measurement)->msr_time_wait;
		break;
		case GPU:
			*time = ((MS_MEASUREMENT_GPU *)ms_measurement)->nvml_time_wait;
		break;
		case FPGA:
			*time = ((MS_MEASUREMENT_FPGA *)ms_measurement)->maxeler_time_wait;
		break;
		case SYSTEM:
			*time = ((MS_MEASUREMENT_SYS *)ms_measurement)->ipmi_time_wait;
		break;
		case MIC:
			*time = ((MS_MEASUREMENT_MIC *)ms_measurement)->mic_time_wait;
		break;
		default:
			time->tv_sec = 0;
			time->tv_nsec = 100000000;
		break;
	}

}

void ms_init_measurement(MS_SYSTEM *ms_system, MS_LIST *ms_list, int flags) {
	struct __mgmt_internal *mgmt = (struct __mgmt_internal*) ms_system->mgmt;
	int msIx;
	int msId;
	int cmpIx;
	int ret;
	void* measurement = NULL;
	struct timespec interval;
	int papi_cmp_id;
	for (msIx = 0; msIx < (log2(ALL+1)); ++msIx) {
		msId = 1<<msIx;

		if ( (msId & flags) != msId) {
			continue;
		}

		measurement = getMeasurement(&ms_list, msId);
		if (measurement == NULL) {
			continue;
		}
		getInterval(measurement, msId, &interval);
		for (cmpIx = 0; cmpIx < mgmt->num_cmp; ++cmpIx) {
			if (component_mapping[cmpIx] == msId) {

				papi_cmp_id = PAPI_get_component_index(known_components[cmpIx]);

				// init component measurement
				if (mgmt->sets[cmpIx] == NULL) {
					char **names = NULL;
					if (NULL != mgmt->user_eventlist_sorted) {
						int eventlist_cmp = 0;
						for (eventlist_cmp = 0; mgmt->user_eventlist_cmp[eventlist_cmp] != NULL && strcmp(known_components[cmpIx], mgmt->user_eventlist_cmp[eventlist_cmp]) != 0; ++eventlist_cmp);
						if (mgmt->user_eventlist_cmp[eventlist_cmp] != NULL) {
							names = mgmt->user_eventlist_sorted[eventlist_cmp];
						}
					}
					ret = APAPI_init_apapi_eventset_cmp(&(mgmt->sets[cmpIx]), papi_cmp_id, names, mgmt->user_eventops);
					if (ret != 0) {
						APAPI_PRINTERR("Failed to create eventset for component %s\n", known_components[cmpIx])
						exit(1);
					}
				}

				mgmt->update_args[cmpIx].cmp_index = cmpIx;
				mgmt->update_args[cmpIx].mgmt = mgmt;
				mgmt->update_args[cmpIx].measurement = measurement;
				mgmt->update_args[cmpIx].measurement_id = msId;
				mgmt->update_args[cmpIx].set = mgmt->sets[cmpIx];
				__create_mapper(msId, measurement, known_components[cmpIx], mgmt->sets[cmpIx], &(mgmt->update_args[cmpIx].map), 0);
				__create_mapper(msId, measurement, known_components[cmpIx], mgmt->sets[cmpIx], &(mgmt->update_args[cmpIx].map_last), 1);

				if (mgmt->timers[cmpIx] == NULL) {
					ret = APAPI_create_timer(&(mgmt->timers[cmpIx]), interval.tv_sec, interval.tv_nsec, measure_update, (void**) &(mgmt->update_args[cmpIx]), mgmt->sets[cmpIx]);
				} else {
					if (mgmt->timers[cmpIx]->state == APAPI_TIMER_STATE_READY) {
						ret = APAPI_change_timer(mgmt->timers[cmpIx], interval.tv_sec, interval.tv_nsec, measure_update, (void**) &(mgmt->update_args[cmpIx]), mgmt->sets[cmpIx]);
					} else {
						ret = APAPI_reset_timer(mgmt->timers[cmpIx], interval.tv_sec, interval.tv_nsec, measure_update, (void**) &(mgmt->update_args[cmpIx]), mgmt->sets[cmpIx]);
					}
				}
				ret = APAPI_initstart_timer(mgmt->timers[cmpIx]);
			}
		}
	}
	(void) ret;
}

void ms_fini_measurement(MS_SYSTEM *ms_system) {
	// There is only one measurement at a time and resources are reused for next measurement.
	// Resources are actually freed on ms_fini().
}

void ms_start_measurement(MS_SYSTEM *ms_system) {
	struct __mgmt_internal *mgmt = (struct __mgmt_internal*) ms_system->mgmt;
	int ret;
    int cmpIx = 0;
    for(cmpIx = 0; cmpIx < mgmt->num_cmp; ++cmpIx){
		if (mgmt->timers[cmpIx] != NULL) {
	        ret = APAPI_start_timer(mgmt->timers[cmpIx]);
		}
	}
	(void) ret;
}
void ms_stop_measurement(MS_SYSTEM *ms_system) {
	struct __mgmt_internal *mgmt = (struct __mgmt_internal*) ms_system->mgmt;
	int ret;
    int cmpIx = 0;
    for(cmpIx = 0; cmpIx < mgmt->num_cmp; ++cmpIx){
		if (mgmt->timers[cmpIx] != NULL) {
			ret = APAPI_callstop_timer(mgmt->timers[cmpIx]);
		}
	}
	(void) ret;
}
void ms_join_measurement(MS_SYSTEM *ms_system) {
	struct __mgmt_internal *mgmt = (struct __mgmt_internal*) ms_system->mgmt;
	int ret;
    int cmpIx = 0;
    for(cmpIx = 0; cmpIx < mgmt->num_cmp; ++cmpIx){
		if (mgmt->timers[cmpIx] != NULL) {
			ret = APAPI_join_timer(mgmt->timers[cmpIx]);
		}
	}
	(void) ret;
}

void ms_reg_sighandler_start(MS_SYSTEM *ms_system, void(*signal_handler)(int)) {
	printf("ms_reg_sighandler_start: This method is not available in ms_common_apapi\n");
}
void ms_reg_sighandler_stop(MS_SYSTEM *ms_system, void(*signal_handler)(int)) {
	printf("ms_reg_sighandler_stop: This method is not available in ms_common_apapi\n");
}

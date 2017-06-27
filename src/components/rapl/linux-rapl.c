/**
 * @file    linux-rapl.c
 * @author  Vince Weaver
 *
 * @ingroup papi_components
 *
 * @brief rapl component
 *
 *  This component enables RAPL (Running Average Power Level)
 *  energy measurements on Intel SandyBridge/IvyBridge/Haswell
 *
 *  To work, either msr_safe kernel module from LLNL
 *  (https://github.com/scalability-llnl/msr-safe), or
 *  the x86 generic MSR driver must be installed
 *    (CONFIG_X86_MSR) and the /dev/cpu/?/<msr_safe | msr> files must have read permissions
 */

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <inttypes.h>

/* Headers required by PAPI */
#include "papi.h"
#include "papi_internal.h"
#include "papi_vector.h"
#include "papi_memory.h"


/*
 * Platform specific RAPL Domains.
 * Note that PP1 RAPL Domain is supported on 062A only
 * And DRAM RAPL Domain is supported on 062D only
 */


/* RAPL defines */
#define MSR_RAPL_POWER_UNIT             0x606

/* Package */
#define MSR_PKG_RAPL_POWER_LIMIT        0x610
#define MSR_PKG_ENERGY_STATUS           0x611
#define MSR_PKG_PERF_STATUS             0x613
#define MSR_PKG_POWER_INFO              0x614

/* PP0 */
#define MSR_PP0_POWER_LIMIT             0x638
#define MSR_PP0_ENERGY_STATUS           0x639
#define MSR_PP0_POLICY                  0x63A
#define MSR_PP0_PERF_STATUS             0x63B

/* PP1 */
#define MSR_PP1_POWER_LIMIT             0x640
#define MSR_PP1_ENERGY_STATUS           0x641
#define MSR_PP1_POLICY                  0x642

/* DRAM */
#define MSR_DRAM_POWER_LIMIT            0x618
#define MSR_DRAM_ENERGY_STATUS          0x619
#define MSR_DRAM_PERF_STATUS            0x61B
#define MSR_DRAM_POWER_INFO             0x61C

/* RAPL bitsmasks */
#define POWER_UNIT_OFFSET          0
#define POWER_UNIT_MASK         0x0f

#define ENERGY_UNIT_OFFSET      0x08
#define ENERGY_UNIT_MASK        0x1f

#define TIME_UNIT_OFFSET        0x10
#define TIME_UNIT_MASK          0x0f

/* RAPL POWER UNIT MASKS */
#define POWER_INFO_UNIT_MASK     0x7fff
#define THERMAL_SHIFT                 0
#define MINIMUM_POWER_SHIFT          16
#define MAXIMUM_POWER_SHIFT          32
#define MAXIMUM_TIME_WINDOW_SHIFT    48

/* RAPL TEMPERATURE */
/* temperature of a processor core (a core of a CPU) */
#define IA32_THERM_STATUS           0x19C
/* temperature of a processor package */
#define IA32_PACKAGE_THERM_STATUS   0x1B1
/*minimum TCC activation temperature
 * the temperature PROCHOT# bus signal rises */
#define MSR_TEMPERATURE_TARGET      0x1A2

/* RAPL FREQUENCY */
/* get the time stamp counter to calculate average
 * frequency over a specific time period */
#define IA32_TIME_STAMP_COUNTER     0x10
/* get maximum performance counter
 * Intel advises us to use delta(IA32_APERF)/delta(IA32_MPERF) ratio as
 * IA32_MPERF unrelated to IA32_APERF makes no sense.*/
#define IA32_MPERF                  0xE7
/* get actual performance counter
 * Intel advises us to use delate(IA32_APERF)/delate(IA32_MPERF) ratio as
 * IA32_APERF unrelated to IA32_MPERF makes no sense.*/
#define IA32_APERF                  0xE8



typedef struct _rapl_register
{
	unsigned int selector;
	unsigned int package;		// global package number
	unsigned int cpu;			// global system cpu count
	unsigned int package_cpu;	// index of cpu according to package
} _rapl_register_t;

typedef struct _rapl_native_event_entry
{
  char name[PAPI_MAX_STR_LEN];
  char units[PAPI_MIN_STR_LEN];
  char description[PAPI_MAX_STR_LEN];
  int fd_offset;
  int msr;
  int type;
  int return_type;
  _rapl_register_t resources;
} _rapl_native_event_entry_t;

typedef struct _rapl_reg_alloc
{
	_rapl_register_t ra_bits;
} _rapl_reg_alloc_t;

/* actually 32?  But setting this to be safe? */
#define RAPL_MAX_COUNTERS 128

typedef struct _rapl_control_state
{
  int being_measured[RAPL_MAX_COUNTERS];
  long long count[RAPL_MAX_COUNTERS];
  int need_difference[RAPL_MAX_COUNTERS];
  long long lastupdate;
  int need_frequency;
  int need_systemstats;
} _rapl_control_state_t;


typedef struct _rapl_context
{
  long long start_value[RAPL_MAX_COUNTERS];
  _rapl_control_state_t state;
} _rapl_context_t;


papi_vector_t _rapl_vector;

struct fd_array_t {
  int fd;
  int open;
};

static _rapl_native_event_entry_t * rapl_native_events=NULL;
static int num_events		= 0;
struct fd_array_t *fd_array=NULL;
static int num_packages=0,num_cpus=0;

int power_divisor,time_divisor;
int cpu_energy_divisor,dram_energy_divisor;
uint32_t *package_temperature_targets = NULL;
struct cpu_freq_counter {
	long long avg_freq;
	long long eff_freq;
	long long aperf;
	long long mperf;
	long long tsc;
	uint64_t time;
	
};
struct cpu_freq_counter *cpu_freq = NULL;
struct cpu_freq_counter *cpu_freq_before = NULL;
struct cpu_stat_counter {
	uint64_t user;
	uint64_t nice;
	uint64_t system;
	uint64_t idle;
	uint64_t iowait;
	uint64_t irq;
	uint64_t softirq;
	uint64_t steal;
	uint64_t guest;
	uint64_t mem_free;
	uint64_t mem_buffers;
	uint64_t mem_cached;
	
	long long time_work;
	long long time_idle;
	long long util;
	long long memory_total;
	long long memory_used;
	long long memory_free;
	long long swap_total;
	long long swap_used;
	long long swap_free;
};
struct cpu_stat_counter _cpu_stat_first = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct cpu_stat_counter _cpu_stat_last = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct cpu_stat_counter _cpu_stat = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct cpu_stat_counter *cpu_stat_last = &_cpu_stat_last;
struct cpu_stat_counter *cpu_stat = &_cpu_stat;
struct cpu_stat_counter *cpu_stat_first = NULL;
long sc_clk_tck;



#define PACKAGE_ENERGY      	0
#define PACKAGE_THERMAL     	1
#define PACKAGE_MINIMUM     	2
#define PACKAGE_MAXIMUM     	3
#define PACKAGE_TIME_WINDOW 	4
#define PACKAGE_ENERGY_CNT      5
#define PACKAGE_THERMAL_CNT     6
#define PACKAGE_MINIMUM_CNT     7
#define PACKAGE_MAXIMUM_CNT     8
#define PACKAGE_TIME_WINDOW_CNT 9
#define DRAM_ENERGY				10
#define PACKAGE_TEMPERATURE		11
#define PACKAGE_TEMPERATURE_CNT	12
#define PACKAGE_FREQUENCY		13
#define SYSTEM_STATS			14

/***************************************************************************/
/******  BEGIN FUNCTIONS  USED INTERNALLY SPECIFIC TO THIS COMPONENT *******/
/***************************************************************************/

long long rapl_getcurrenttime() {
    struct timespec result;
    clock_gettime(CLOCK_MONOTONIC_RAW, &result);
    return (long long)result.tv_sec* 1000000000LL + (long long)result.tv_nsec;
}


static long long read_msr(int fd, int which) {

  uint64_t data;

  if ( fd<0 || pread(fd, &data, sizeof data, which) != sizeof data ) {
    perror("rdmsr:pread");
    exit(127);
  }

  return (long long)data;
}

static int open_fd(int offset) {
  
  int fd=-1;
  char filename[BUFSIZ];

  if (fd_array[offset].open==0) {
	  sprintf(filename,"/dev/cpu/%d/msr_safe",offset);
      fd = open(filename, O_RDONLY);
	  if (fd<0) {
		  sprintf(filename,"/dev/cpu/%d/msr",offset);
          fd = open(filename, O_RDONLY);
	  }
	  if (fd>=0) {
		  fd_array[offset].fd=fd;
	      fd_array[offset].open=1;
      } 
  }
  else {
    fd=fd_array[offset].fd;
  }

  return fd;
}

void rapl_update_systemstats() {

	int num_read;

	// swap stats pointers
	struct cpu_stat_counter *temp;
	temp = cpu_stat;
	cpu_stat = cpu_stat_last;
	cpu_stat_last = temp;

	FILE *cpu_stat_fd;
	cpu_stat_fd = fopen("/proc/stat", "r");

	if (cpu_stat_fd != NULL) {
		char buffer[100];
		memset(buffer, 0, 100);
		int tokenIx = 0;
		size_t buffersize = 100;
		char *linebuffer = buffer;
		num_read = getline(&linebuffer, &buffersize, cpu_stat_fd);
		char *endptr = NULL;
		char *nextSpace = linebuffer;
		uint64_t result = 0;
		if (num_read == -1) {
			printf("getline failed %d\n", errno);
			errno = 0;
		} else
		if (num_read > 0) {
			while ( tokenIx < 9 && *nextSpace != 0 && (nextSpace = strstr(nextSpace, " ")) != NULL  ) {
				result = strtol(nextSpace+1, &endptr, 0);
				if (endptr == NULL) {
					break;
				}
				switch (tokenIx) {
					case 0:
						cpu_stat->user = result;
					break;
					case 1:
						cpu_stat->nice = result;
					break;
					case 2:
						cpu_stat->system = result;
					break;
					case 3:
						cpu_stat->idle = result;
					break;
					case 4:
						cpu_stat->iowait = result;
					break;
					case 5:
						cpu_stat->irq = result;
					break;
					case 6:
						cpu_stat->softirq = result;
					break;
					case 7:
						cpu_stat->steal = result;
					break;
					case 8:
						cpu_stat->guest = result;
					break;
					default:
					break;
				}
				tokenIx++;
			}
		}
		if (linebuffer != buffer) {
			free(linebuffer);
		}

		fclose(cpu_stat_fd);
	}

	FILE *mem_stat_fd;
	mem_stat_fd = fopen("/proc/meminfo", "r");
		
	if (mem_stat_fd != NULL) {
		char buffer[40];
		memset(buffer, 0, 40);
		char *linebuffer = buffer;
		size_t buffersize = 40;
		int itemstoread = 6;
		int ret = 0;
		while (ret == 0 && itemstoread > 0) { // last read returned error or all necessary items found
			num_read = getline(&linebuffer, &buffersize, mem_stat_fd);
			
			if (num_read == 0) {
				break;
			}

			// as for now /proc/meminfo only uses kB unit
			if (strncmp(buffer, "SwapFree:", 9) == 0) {
				sscanf(buffer, "SwapFree: %lld kB", &(cpu_stat->swap_free));
				itemstoread--;
				continue;
			}
			if (strncmp(buffer, "SwapTotal:", 10) == 0) {
				sscanf(buffer, "SwapTotal: %lld kB", &(cpu_stat->swap_total));	
				itemstoread--;
				continue;
			}
			if (strncmp(buffer, "Cached:", 7) == 0) {
				sscanf(buffer, "Cached: %" PRIu64 " kB", &(cpu_stat->mem_cached));
				itemstoread--;
				continue;
			}
			if (strncmp(buffer, "Buffers:", 8) == 0) {
				sscanf(buffer, "Buffers: %" PRIu64 " kB", &(cpu_stat->mem_buffers));
				itemstoread--;
				continue;
			}
			if (strncmp(buffer, "MemFree:", 8) == 0) {
				sscanf(buffer, "MemFree: %" PRIu64 " kB", &(cpu_stat->mem_free));
				itemstoread--;
				continue;
			}
			if (strncmp(buffer, "MemTotal:", 9) == 0) {
				sscanf(buffer, "MemTotal: %lld kB", &(cpu_stat->memory_total));
				itemstoread--;
				continue;
			}
		}
		if (buffer != linebuffer) {
			free(linebuffer);
		}
		fclose(mem_stat_fd);
	}
	
	if (cpu_stat_first == NULL) {
		cpu_stat_first = &_cpu_stat_first;
		memcpy( &_cpu_stat_first, cpu_stat, sizeof(struct cpu_stat_counter));
	}

		// compare to first
		cpu_stat->time_work = (long long) ( (double)(
			(cpu_stat->nice - cpu_stat_first->nice) + 
			(cpu_stat->system - cpu_stat_first->system) + 
			(cpu_stat->user - cpu_stat_first->user) +
			(cpu_stat->irq - cpu_stat_first->irq) +
			(cpu_stat->softirq - cpu_stat_first->softirq) +
			(cpu_stat->steal - cpu_stat_first->steal) +
			(cpu_stat->guest - cpu_stat_first->guest) ) / (double)sc_clk_tck / (double)num_cpus * 1000.0);
		cpu_stat->time_idle = (long long) ( (double)(
			(cpu_stat->idle - cpu_stat_first->idle) + 
			(cpu_stat->iowait - cpu_stat_first->iowait)) / (double)sc_clk_tck / (double)num_cpus * 1000.0);
			//cpu_stat->util = (long long) ((double) (cpu_stat->nice + cpu_stat->system + cpu_stat->user) / (double) (cpu_stat->nice + cpu_stat->system + cpu_stat->user + cpu_stat->idle + cpu_stat->iowait) * 100.0);

		// compare to first
		//cpu_stat->util = (long long) ( (double)cpu_stat->time_work / (double)(cpu_stat->time_work + cpu_stat->time_idle) * 100.0);
		// compare to last
		long long time_work = (long long) ( (double)(
			(cpu_stat->nice - cpu_stat_last->nice) + 
			(cpu_stat->system - cpu_stat_last->system) + 
			(cpu_stat->user - cpu_stat_last->user) +
			(cpu_stat->irq - cpu_stat_last->irq) +
			(cpu_stat->softirq - cpu_stat_last->softirq) +
			(cpu_stat->steal - cpu_stat_last->steal) +
			(cpu_stat->guest - cpu_stat_last->guest) ) / (double)sc_clk_tck / (double)num_cpus * 1000.0);
		long long time_idle = (long long) ( (double)(
			(cpu_stat->idle - cpu_stat_last->idle) + 
			(cpu_stat->iowait - cpu_stat_last->iowait)) / (double)sc_clk_tck / (double)num_cpus * 1000.0);

		if (time_work + time_idle == 0) {
			cpu_stat->util = 0;
		} else {
			cpu_stat->util = (long long) ( (double)time_work / (double)(time_work + time_idle) * 100.0);
		}


		cpu_stat->memory_free = cpu_stat->mem_free + cpu_stat->mem_buffers + cpu_stat->mem_cached;
		cpu_stat->memory_used = cpu_stat->memory_total - cpu_stat->memory_free;
		cpu_stat->swap_used = cpu_stat->swap_total - cpu_stat->swap_free;
}

long long rapl_get_systemstats(int index) {

	switch(rapl_native_events[index].msr) {
		case 0:
			return cpu_stat->time_work;
		break;
		case 1:
			return cpu_stat->time_idle;
		break;
		case 2:
			return cpu_stat->util;
		break;
		case 3:
			return cpu_stat->memory_total;
		break;
		case 4:
			return cpu_stat->memory_used;
		break;
		case 5:
			return cpu_stat->memory_free;
		break;
		case 6:
			return cpu_stat->swap_total;
		break;
		case 7:
			return cpu_stat->swap_used;
		break;
		case 8:
			return cpu_stat->swap_free;
		break;
	}
	return 0;
} 

void rapl_update_frequency() {
	int cpuIx;
	int fd;
	long long time_diff;
	long long aperf;
	long long mperf;
	struct cpu_freq_counter *temp;
	// swap pointer
	temp = cpu_freq_before;
	cpu_freq_before = cpu_freq;
	cpu_freq = temp;

	for (cpuIx = 0; cpuIx < num_cpus; ++cpuIx) {
		cpu_freq[cpuIx].time = rapl_getcurrenttime();
		fd=open_fd(cpuIx);
		cpu_freq[cpuIx].tsc = read_msr(fd, IA32_TIME_STAMP_COUNTER);
		cpu_freq[cpuIx].aperf = read_msr(fd, IA32_APERF);
		cpu_freq[cpuIx].mperf = read_msr(fd, IA32_MPERF);
	}
	for (cpuIx = 0; cpuIx < num_cpus; ++cpuIx) {

		time_diff = cpu_freq[cpuIx].time - cpu_freq_before[cpuIx].time;
		cpu_freq[cpuIx].avg_freq =
			(long long) ((double)(cpu_freq[cpuIx].tsc - cpu_freq_before[cpuIx].tsc) / (double)time_diff * 1000000.0);
		
/*		                        pMsMeasurementCpu->msr_freq_core_cur[i][j]  =
                            (pMsMeasurementCpu->internal.msr_timestamp_core_cur[i][j] - pMsMeasurementCpu->internal.msr_timestamp_core_temp[i][j]) /                            (pMsMeasurementCpu->internal.msr_time_diff_double * 1000000);
*/
		// overflow check
		if (cpu_freq[cpuIx].aperf > cpu_freq_before[cpuIx].aperf) {
			aperf = cpu_freq[cpuIx].aperf - cpu_freq_before[cpuIx].aperf;
		} else {
			aperf = UINT64_MAX - (cpu_freq_before[cpuIx].aperf - cpu_freq[cpuIx].aperf);
		}
		if (cpu_freq[cpuIx].mperf > cpu_freq_before[cpuIx].mperf) {
			mperf = cpu_freq[cpuIx].mperf - cpu_freq_before[cpuIx].mperf;
		} else {
			mperf = UINT64_MAX - (cpu_freq_before[cpuIx].mperf - cpu_freq[cpuIx].mperf);
		}
		
		cpu_freq[cpuIx].eff_freq = (long long) ( (double)(cpu_freq[cpuIx].avg_freq) * (double)aperf / (double)mperf );
/*
	pMsMeasurementCpu->msr_freq_core_eff_cur[i][j]  = pMsMeasurementCpu->msr_freq_core_cur[i][j] * diff_aperf / diff_mperf;
*/
	}

}

long long rapl_get_frequency(int index) {

	int cpu = rapl_native_events[index].resources.cpu;

	switch(rapl_native_events[index].msr) {
		case 0:
			return cpu_freq[cpu].avg_freq;
		break;
		default:
			return cpu_freq[cpu].eff_freq;
		break;
	}
	return 0;
}
static long long read_rapl_value(int index) {

	int type;
	int fd;
	type = rapl_native_events[index].type;

	switch(type) {
		case PACKAGE_FREQUENCY:
			return rapl_get_frequency(index);
		break;
		case SYSTEM_STATS:
			return rapl_get_systemstats(index);
		break;
		default:

			fd=open_fd(rapl_native_events[index].fd_offset);
			return read_msr(fd,rapl_native_events[index].msr);
		break;
	}
	return 0;
}
static long long convert_rapl_energy(int index, long long value) {

   union {
      long long ll;
      double fp;
   } return_val;

   return_val.ll = value; /* default case: return raw input value */

   if (rapl_native_events[index].type==PACKAGE_ENERGY) {
      return_val.ll = (long long)(((double)value/cpu_energy_divisor)*1e9);
   }

   if (rapl_native_events[index].type==DRAM_ENERGY) {
      return_val.ll = (long long)(((double)value/dram_energy_divisor)*1e9);
   }

   if (rapl_native_events[index].type==PACKAGE_THERMAL) {
      return_val.fp = (double)
                      ((value>>THERMAL_SHIFT)&POWER_INFO_UNIT_MASK) /
                       (double)power_divisor;
   }

   if (rapl_native_events[index].type==PACKAGE_MINIMUM) {
       return_val.fp = (double)
                       ((value>>MINIMUM_POWER_SHIFT)&POWER_INFO_UNIT_MASK)/
                        (double)power_divisor;
   }

   if (rapl_native_events[index].type==PACKAGE_MAXIMUM) {
      return_val.fp = (double)
                      ((value>>MAXIMUM_POWER_SHIFT)&POWER_INFO_UNIT_MASK)/
                       (double)power_divisor;
   }

   if (rapl_native_events[index].type==PACKAGE_TIME_WINDOW) {
      return_val.fp =  (double)
                    ((value>>MAXIMUM_TIME_WINDOW_SHIFT)&POWER_INFO_UNIT_MASK)/
                     (double)time_divisor;
   }

   if (rapl_native_events[index].type==PACKAGE_THERMAL_CNT) {
      return_val.ll = ((value>>THERMAL_SHIFT)&POWER_INFO_UNIT_MASK);
   }

   if (rapl_native_events[index].type==PACKAGE_MINIMUM_CNT) {
       return_val.ll = ((value>>MINIMUM_POWER_SHIFT)&POWER_INFO_UNIT_MASK);
   }

   if (rapl_native_events[index].type==PACKAGE_MAXIMUM_CNT) {
      return_val.ll = ((value>>MAXIMUM_POWER_SHIFT)&POWER_INFO_UNIT_MASK);
   }

   if (rapl_native_events[index].type==PACKAGE_TIME_WINDOW_CNT) {
      return_val.ll = ((value>>MAXIMUM_TIME_WINDOW_SHIFT)&POWER_INFO_UNIT_MASK);
   }

   if (rapl_native_events[index].type==PACKAGE_TEMPERATURE) {
		// Temperature = Maximal temperature - Margin to throttle
		// Temperature = target[23:16] - therm_status[22:16]
		long long temp_target = package_temperature_targets[rapl_native_events[index].resources.package];
		return_val.ll = ((temp_target>>16) & 0xff) - ((value>>16) & 0x7f);
   }

   if (rapl_native_events[index].type==PACKAGE_TEMPERATURE_CNT) {
		return_val.ll = value;
   }

   if (rapl_native_events[index].type==PACKAGE_FREQUENCY) {
		/*
		int fd;
		fd=open_fd(rapl_native_events[index].fd_offset);
		return read_msr(fd,rapl_native_events[index].msr);

		long long = read_rapl_value(i);
		base = 
		return_val.ll = base * (aperf_mperf);
		*/
		return_val.ll = value;
   }

   return return_val.ll;
}

static int
get_kernel_nr_cpus(void)
{
  FILE *fff;
  int num_read, nr_cpus = 1;
  fff=fopen("/sys/devices/system/cpu/kernel_max","r");
  if (fff==NULL) return nr_cpus;
  num_read=fscanf(fff,"%d",&nr_cpus);
  fclose(fff);
  if (num_read==1) {
    nr_cpus++;
  } else {
    nr_cpus = 1;
  }
  return nr_cpus;
}

/************************* PAPI Functions **********************************/


/*
 * This is called whenever a thread is initialized
 */
static int
_rapl_init_thread( hwd_context_t *ctx )
{
  ( void ) ctx;

  return PAPI_OK;
}



/*
 * Called when PAPI process is initialized (i.e. PAPI_library_init)
 */
static int
_rapl_init_component( int cidx )
{
     int i,j,k,fd;
     FILE *fff;
     char filename[BUFSIZ];

     int package_avail, dram_avail, pp0_avail, pp1_avail;

     long long result;
     int package;

     const PAPI_hw_info_t *hw_info;

     int nr_cpus = get_kernel_nr_cpus();
     int packages[nr_cpus];
     int cpu_to_use[nr_cpus];
	 int cpu_package[nr_cpus];  // package id
	 int cpu_core_id[nr_cpus]; // core id inside package

     /* Fill with sentinel values */
     for (i=0; i<nr_cpus; ++i) {
       packages[i] = -1;
       cpu_to_use[i] = -1;
	   cpu_package[i] = -1;
	   cpu_core_id[i] = -1;
     }


     /* check if Intel processor */
     hw_info=&(_papi_hwi_system_info.hw_info);

     /* Ugh can't use PAPI_get_hardware_info() if
	PAPI library not done initializing yet */

     if (hw_info->vendor!=PAPI_VENDOR_INTEL) {
        strncpy(_rapl_vector.cmp_info.disabled_reason,
		"Not an Intel processor",PAPI_MAX_STR_LEN);
        return PAPI_ENOSUPP;
     }

     /* check model to support */
     if (hw_info->cpuid_family==6) {
       if (hw_info->cpuid_model==42) {
	  /* SandyBridge */
          package_avail=1;
          pp0_avail=1;
          pp1_avail=1;
          dram_avail=0;
       }
       else if (hw_info->cpuid_model==45) {
	  /* SandyBridge-EP */
          package_avail=1;
          pp0_avail=1;
          pp1_avail=0;
          dram_avail=1;
       }
       else if (hw_info->cpuid_model==58) {
	  /* IvyBridge */
          package_avail=1;
          pp0_avail=1;
          pp1_avail=1;
          dram_avail=0;
       }
       else if (hw_info->cpuid_model==62) {
	  /* IvyBridge-EP */
          package_avail=1;
          pp0_avail=1;
          pp1_avail=0;
          dram_avail=1;
       }
       else if (hw_info->cpuid_model==60 || hw_info->cpuid_model==69 || hw_info->cpuid_model==70 ) {
		/* Haswell */
		package_avail=1;
		pp0_avail=1;
		pp1_avail=1;
		dram_avail=1;
	   }
	else if ( hw_info->cpuid_model==63) {
		/* Haswell-EP */
		package_avail=1;
		pp0_avail=1;
		pp1_avail=0;
		dram_avail=1;
	}
	else if (hw_info->cpuid_model==61 || hw_info->cpuid_model==71) {
	   /* Broadwell */
	   package_avail=1;
	   pp0_avail=1;
	   pp1_avail=0;
	   dram_avail=1;
	}
	else if (hw_info->cpuid_model==79) {
	   /* Broadwell-EP */
	   package_avail=1;
	   pp0_avail=1;
	   pp1_avail=0;
	   dram_avail=1;
	}
	else if (hw_info->cpuid_model==78 || hw_info->cpuid_model==94) {
		/* Skylake, Skylake H/S */
		package_avail=1;
		pp0_avail=1;
		pp1_avail=0;
		dram_avail=1;
	}
	else if (hw_info->cpuid_model==87) {
		/* Knights Landing (KNL) */
		package_avail=1;
		pp0_avail=0;
		pp1_avail=0;
		dram_avail=1;
	}
       else {
	 /* not a supported model */
	 strncpy(_rapl_vector.cmp_info.disabled_reason,
		 "CPU model not supported",
		 PAPI_MAX_STR_LEN);
	 return PAPI_ENOIMPL;
       }
     }
     else {
       /* Not a family 6 machine */
       strncpy(_rapl_vector.cmp_info.disabled_reason,
	       "CPU family not supported",PAPI_MAX_STR_LEN);
       return PAPI_ENOIMPL;
     }


     /* Detect how many packages */
     j=0;
     while(1) {
       int num_read;

       sprintf(filename,
	       "/sys/devices/system/cpu/cpu%d/topology/physical_package_id",j);
       fff=fopen(filename,"r");
       if (fff==NULL) break;
       num_read=fscanf(fff,"%d",&package);
       fclose(fff);

       sprintf(filename,
	       "/sys/devices/system/cpu/cpu%d/topology/core_id",j);
       fff=fopen(filename,"r");
       if (fff==NULL) break;
       num_read=fscanf(fff,"%d",&(cpu_core_id[j]));
       fclose(fff);
	   
       if (num_read!=1) {
    		 strcpy(_rapl_vector.cmp_info.disabled_reason, "Error reading file: ");
    		 strncat(_rapl_vector.cmp_info.disabled_reason, filename, PAPI_MAX_STR_LEN - strlen(_rapl_vector.cmp_info.disabled_reason) - 1);
    		 _rapl_vector.cmp_info.disabled_reason[PAPI_MAX_STR_LEN-1] = '\0';
    		 return PAPI_ESYS;
       }

       /* Check if a new package */
       if ((package >= 0) && (package < nr_cpus)) {
         if (packages[package] == -1) {
           SUBDBG("Found package %d out of total %d\n",package,num_packages);
	 	   packages[package]=package;
		   cpu_to_use[package]=j;
		   num_packages++;
         }
		 cpu_package[j] = package;
       } else {
	 SUBDBG("Package outside of allowed range\n");
	 strncpy(_rapl_vector.cmp_info.disabled_reason,
		"Package outside of allowed range",PAPI_MAX_STR_LEN);
	 return PAPI_ESYS;
       }

       j++;
     }
     num_cpus=j;

     if (num_packages==0) {
        SUBDBG("Can't access /dev/cpu/*/<msr_safe | msr>\n");
	strncpy(_rapl_vector.cmp_info.disabled_reason,
		"Can't access /dev/cpu/*/<msr_safe | msr>",PAPI_MAX_STR_LEN);
	return PAPI_ESYS;
     }

     SUBDBG("Found %d packages with %d cpus\n",num_packages,num_cpus);

     /* Init fd_array */

     fd_array=papi_calloc(sizeof(struct fd_array_t),num_cpus);
     if (fd_array==NULL) return PAPI_ENOMEM;

     fd=open_fd(cpu_to_use[0]);
     if (fd<0) {
        sprintf(_rapl_vector.cmp_info.disabled_reason,
		"Can't open fd for cpu0: %s",strerror(errno));
        return PAPI_ESYS;
     }

     /* Verify needed MSR is readable. In a guest VM it may not be readable*/
     if (pread(fd, &result, sizeof result, MSR_RAPL_POWER_UNIT) != sizeof result ) {
        strncpy(_rapl_vector.cmp_info.disabled_reason,
               "Unable to access RAPL registers",PAPI_MAX_STR_LEN);
        return PAPI_ESYS;
     }

     /* Calculate the units used */
     result=read_msr(fd,MSR_RAPL_POWER_UNIT);

     /* units are 0.5^UNIT_VALUE */
     /* which is the same as 1/(2^UNIT_VALUE) */

     power_divisor=1<<((result>>POWER_UNIT_OFFSET)&POWER_UNIT_MASK);
     cpu_energy_divisor=1<<((result>>ENERGY_UNIT_OFFSET)&ENERGY_UNIT_MASK);
     time_divisor=1<<((result>>TIME_UNIT_OFFSET)&TIME_UNIT_MASK);

	/* Note! On Haswell-EP DRAM energy is fixed at 15.3uJ	*/
	/* see https://lkml.org/lkml/2015/3/20/582		*/
	/* Knights Landing is the same */
	if (( hw_info->cpuid_model==63) ||  (hw_info->cpuid_model==87 )) {
		dram_energy_divisor=1<<16;
	}
	else {
		dram_energy_divisor=cpu_energy_divisor;
	}

     SUBDBG("Power units = %.3fW\n",1.0/power_divisor);
     SUBDBG("CPU Energy units = %.8fJ\n",1.0/cpu_energy_divisor);
     SUBDBG("DRAM Energy units = %.8fJ\n",1.0/dram_energy_divisor);
     SUBDBG("Time units = %.8fs\n",1.0/time_divisor);

	/* find out temperature target for each packuint32_t); */
	 package_temperature_targets = papi_calloc(sizeof(uint32_t), num_packages);
     for(j=0;j<num_packages;j++) {
	   fd = open_fd(cpu_to_use[j]);
	   long long target_raw;
	   target_raw = read_msr(fd,MSR_TEMPERATURE_TARGET);
	   package_temperature_targets[j] = target_raw;
 	 }

	 /* space for counters necessary for core frequency */
	 cpu_freq = (struct cpu_freq_counter *) papi_calloc(sizeof(struct cpu_freq_counter), num_cpus);
	 cpu_freq_before = (struct cpu_freq_counter *) papi_calloc(sizeof(struct cpu_freq_counter), num_cpus);
	 
	 /* open proc file descriptors */
	 sc_clk_tck = sysconf(_SC_CLK_TCK);

     /* Allocate space for events */
     /* Include room for both counts and scaled values */

     num_events= ((package_avail*num_packages) +
                 (pp0_avail*num_packages) +
                 (pp1_avail*num_packages) +
                 (dram_avail*num_packages) +
                 (4*num_packages) +
				 num_packages +						// package temperature
				 num_cpus							// cpu temperature
				 ) * 2 +
				 num_cpus * 2 +						// core frequency
				 9;									// system statistics (cpu utilization + memory usage)

     rapl_native_events = (_rapl_native_event_entry_t*)
          papi_calloc(sizeof(_rapl_native_event_entry_t),num_events);


     i = 0;
     k = (num_events-(num_cpus*2) -9)/2; // all events - events without CNT events

     /* Create events for package power info */

     for(j=0;j<num_packages;j++) {
     	sprintf(rapl_native_events[i].name,
			"THERMAL_SPEC_CNT:PACKAGE%d",j);
		sprintf(rapl_native_events[i].description,
		   "Thermal specification in counts; package %d",j);
		rapl_native_events[i].fd_offset=cpu_to_use[j];
		rapl_native_events[i].msr=MSR_PKG_POWER_INFO;
		rapl_native_events[i].resources.selector = i + 1;
		rapl_native_events[i].resources.package = j;
		rapl_native_events[i].type=PACKAGE_THERMAL_CNT;
		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

     	sprintf(rapl_native_events[k].name,
			"THERMAL_SPEC:PACKAGE%d",j);
		strncpy(rapl_native_events[k].units,"W",PAPI_MIN_STR_LEN);
		sprintf(rapl_native_events[k].description,
		   "Thermal specification for package %d",j);
		rapl_native_events[k].fd_offset=cpu_to_use[j];
		rapl_native_events[k].msr=MSR_PKG_POWER_INFO;
		rapl_native_events[k].resources.selector = k + 1;
		rapl_native_events[k].resources.package = j;
		rapl_native_events[k].type=PACKAGE_THERMAL;
		rapl_native_events[k].return_type=PAPI_DATATYPE_FP64;

		i++;
		k++;
     }

     for(j=0;j<num_packages;j++) {
		sprintf(rapl_native_events[i].name,
			"MINIMUM_POWER_CNT:PACKAGE%d",j);
		sprintf(rapl_native_events[i].description,
		   "Minimum power in counts; package %d",j);
		rapl_native_events[i].fd_offset=cpu_to_use[j];
		rapl_native_events[i].msr=MSR_PKG_POWER_INFO;
		rapl_native_events[i].resources.selector = i + 1;
		rapl_native_events[i].resources.package = j;
		rapl_native_events[i].type=PACKAGE_MINIMUM_CNT;
		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

		sprintf(rapl_native_events[k].name,
			"MINIMUM_POWER:PACKAGE%d",j);
		strncpy(rapl_native_events[k].units,"W",PAPI_MIN_STR_LEN);
		sprintf(rapl_native_events[k].description,
		   "Minimum power for package %d",j);
		rapl_native_events[k].fd_offset=cpu_to_use[j];
		rapl_native_events[k].msr=MSR_PKG_POWER_INFO;
		rapl_native_events[k].resources.selector = k + 1;
		rapl_native_events[k].resources.package = j;
		rapl_native_events[k].type=PACKAGE_MINIMUM;
		rapl_native_events[k].return_type=PAPI_DATATYPE_FP64;

		i++;
		k++;
     }

     for(j=0;j<num_packages;j++) {
		sprintf(rapl_native_events[i].name,
			"MAXIMUM_POWER_CNT:PACKAGE%d",j);
		sprintf(rapl_native_events[i].description,
		   "Maximum power in counts; package %d",j);
		rapl_native_events[i].fd_offset=cpu_to_use[j];
		rapl_native_events[i].msr=MSR_PKG_POWER_INFO;
		rapl_native_events[i].resources.selector = i + 1;
		rapl_native_events[i].resources.package = j;
		rapl_native_events[i].type=PACKAGE_MAXIMUM_CNT;
		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

		sprintf(rapl_native_events[k].name,
			"MAXIMUM_POWER:PACKAGE%d",j);
		strncpy(rapl_native_events[k].units,"W",PAPI_MIN_STR_LEN);
		sprintf(rapl_native_events[k].description,
		   "Maximum power for package %d",j);
		rapl_native_events[k].fd_offset=cpu_to_use[j];
		rapl_native_events[k].msr=MSR_PKG_POWER_INFO;
		rapl_native_events[k].resources.selector = k + 1;
		rapl_native_events[k].resources.package = j;
		rapl_native_events[k].type=PACKAGE_MAXIMUM;
		rapl_native_events[k].return_type=PAPI_DATATYPE_FP64;

		i++;
		k++;
     }

     for(j=0;j<num_packages;j++) {
		sprintf(rapl_native_events[i].name,
			"MAXIMUM_TIME_WINDOW_CNT:PACKAGE%d",j);
		sprintf(rapl_native_events[i].description,
		   "Maximum time window in counts; package %d",j);
		rapl_native_events[i].fd_offset=cpu_to_use[j];
		rapl_native_events[i].msr=MSR_PKG_POWER_INFO;
		rapl_native_events[i].resources.selector = i + 1;
		rapl_native_events[i].resources.package = j;
		rapl_native_events[i].type=PACKAGE_TIME_WINDOW_CNT;
		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

		sprintf(rapl_native_events[k].name,
			"MAXIMUM_TIME_WINDOW:PACKAGE%d",j);
		strncpy(rapl_native_events[k].units,"s",PAPI_MIN_STR_LEN);
		sprintf(rapl_native_events[k].description,
		   "Maximum time window for package %d",j);
		rapl_native_events[k].fd_offset=cpu_to_use[j];
		rapl_native_events[k].msr=MSR_PKG_POWER_INFO;
		rapl_native_events[k].resources.selector = k + 1;
		rapl_native_events[k].resources.package = j;
		rapl_native_events[k].type=PACKAGE_TIME_WINDOW;
		rapl_native_events[k].return_type=PAPI_DATATYPE_FP64;

		i++;
		k++;
     }

     /* Create Events for energy measurements */

     if (package_avail) {
        for(j=0;j<num_packages;j++) {
	   		sprintf(rapl_native_events[i].name,
		   		"PACKAGE_ENERGY_CNT:PACKAGE%d",j);
	   		sprintf(rapl_native_events[i].description,
		   		"Energy used in counts by chip package %d",j);
	   		rapl_native_events[i].fd_offset=cpu_to_use[j];
	   		rapl_native_events[i].msr=MSR_PKG_ENERGY_STATUS;
	   		rapl_native_events[i].resources.selector = i + 1;
	   		rapl_native_events[i].resources.package = j;
	   		rapl_native_events[i].type=PACKAGE_ENERGY_CNT;
	   		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

	   		sprintf(rapl_native_events[k].name,
		   		"PACKAGE_ENERGY:PACKAGE%d",j);
	   		strncpy(rapl_native_events[k].units,"nJ",PAPI_MIN_STR_LEN);
	   		sprintf(rapl_native_events[k].description,
		   		"Energy used by chip package %d",j);
	   		rapl_native_events[k].fd_offset=cpu_to_use[j];
	   		rapl_native_events[k].msr=MSR_PKG_ENERGY_STATUS;
	   		rapl_native_events[k].resources.selector = k + 1;
	   		rapl_native_events[k].resources.package = j;
	   		rapl_native_events[k].type=PACKAGE_ENERGY;
	   		rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;

	   		i++;
			k++;
		}
     }

     if (pp1_avail) {
        for(j=0;j<num_packages;j++) {
	   		sprintf(rapl_native_events[i].name,
		   		"PP1_ENERGY_CNT:PACKAGE%d",j);
	   		sprintf(rapl_native_events[i].description,
		   		"Energy used in counts by Power Plane 1 (Often GPU) on package %d",j);
           	rapl_native_events[i].fd_offset=cpu_to_use[j];
	   		rapl_native_events[i].msr=MSR_PP1_ENERGY_STATUS;
	   		rapl_native_events[i].resources.selector = i + 1;
	   		rapl_native_events[i].resources.package = j;
	   		rapl_native_events[i].type=PACKAGE_ENERGY_CNT;
	   		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

	   		sprintf(rapl_native_events[k].name,
		   		"PP1_ENERGY:PACKAGE%d",j);
	   		strncpy(rapl_native_events[k].units,"nJ",PAPI_MIN_STR_LEN);
	   		sprintf(rapl_native_events[k].description,
		   		"Energy used by Power Plane 1 (Often GPU) on package %d",j);
           	rapl_native_events[k].fd_offset=cpu_to_use[j];
	   		rapl_native_events[k].msr=MSR_PP1_ENERGY_STATUS;
	   		rapl_native_events[k].resources.selector = k + 1;
	   		rapl_native_events[k].resources.package = j;
	   		rapl_native_events[k].type=PACKAGE_ENERGY;
	   		rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;

	   		i++;
			k++;
		}
     }

     if (dram_avail) {
        for(j=0;j<num_packages;j++) {
	   		sprintf(rapl_native_events[i].name,
		   		"DRAM_ENERGY_CNT:PACKAGE%d",j);
           	sprintf(rapl_native_events[i].description,
		   		"Energy used in counts by DRAM on package %d",j);
	   		rapl_native_events[i].fd_offset=cpu_to_use[j];
	   		rapl_native_events[i].msr=MSR_DRAM_ENERGY_STATUS;
	   		rapl_native_events[i].resources.selector = i + 1;
	   		rapl_native_events[i].resources.package = j;
	   		rapl_native_events[i].type=PACKAGE_ENERGY_CNT;
	   		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

	   		sprintf(rapl_native_events[k].name,
		   		"DRAM_ENERGY:PACKAGE%d",j);
	   		strncpy(rapl_native_events[k].units,"nJ",PAPI_MIN_STR_LEN);
           	sprintf(rapl_native_events[k].description,
		   		"Energy used by DRAM on package %d",j);
	   		rapl_native_events[k].fd_offset=cpu_to_use[j];
	   		rapl_native_events[k].msr=MSR_DRAM_ENERGY_STATUS;
	   		rapl_native_events[k].resources.selector = k + 1;
	   		rapl_native_events[k].resources.package = j;
	   		rapl_native_events[k].type=DRAM_ENERGY;
	   		rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;

	   		i++;
			k++;
		}
     }

     if (pp0_avail) {
        for(j=0;j<num_packages;j++) {
			sprintf(rapl_native_events[i].name,
		   		"PP0_ENERGY_CNT:PACKAGE%d",j);
	   		sprintf(rapl_native_events[i].description,
		   		"Energy used in counts by all cores in package %d",j);
	   		rapl_native_events[i].fd_offset=cpu_to_use[j];
	   		rapl_native_events[i].msr=MSR_PP0_ENERGY_STATUS;
	   		rapl_native_events[i].resources.selector = i + 1;
	   		rapl_native_events[i].resources.package = j;
	   		rapl_native_events[i].type=PACKAGE_ENERGY_CNT;
	   		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

			sprintf(rapl_native_events[k].name,
		   		"PP0_ENERGY:PACKAGE%d",j);
	   		strncpy(rapl_native_events[k].units,"nJ",PAPI_MIN_STR_LEN);
	   		sprintf(rapl_native_events[k].description,
		   		"Energy used by all cores in package %d",j);
	   		rapl_native_events[k].fd_offset=cpu_to_use[j];
	   		rapl_native_events[k].msr=MSR_PP0_ENERGY_STATUS;
	   		rapl_native_events[k].resources.selector = k + 1;
	   		rapl_native_events[k].resources.package = j;
	   		rapl_native_events[k].type=PACKAGE_ENERGY;
	   		rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;

	   		i++;
			k++;
		}
     }

	 // add package temperature events
	 for(j=0;j<num_packages;j++) {
			sprintf(rapl_native_events[i].name,
		   		"THERM_STATUS_CNT:PACKAGE%d",j);
	   		sprintf(rapl_native_events[i].description,
		   		"Raw temperature register for package %d",j);
	   		rapl_native_events[i].fd_offset=cpu_to_use[j];
	   		rapl_native_events[i].msr=IA32_PACKAGE_THERM_STATUS;
	   		rapl_native_events[i].resources.selector = i + 1;
	   		rapl_native_events[i].resources.package = j;
	   		rapl_native_events[i].type=PACKAGE_TEMPERATURE_CNT;
	   		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;

			sprintf(rapl_native_events[k].name,
		   		"THERM_STATUS:PACKAGE%d",j);
	   		strncpy(rapl_native_events[k].units,"°C",PAPI_MIN_STR_LEN);
	   		sprintf(rapl_native_events[k].description,
		   		"Temperature for package %d",j);
	   		rapl_native_events[k].fd_offset=cpu_to_use[j];
	   		rapl_native_events[k].msr=IA32_PACKAGE_THERM_STATUS;
	   		rapl_native_events[k].resources.selector = k + 1;
	   		rapl_native_events[k].resources.package = j;
	   		rapl_native_events[k].type=PACKAGE_TEMPERATURE;
	   		rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;

		 i++;
		 k++;
	 }

	 // add cpu temperature events
	 for(j=0;j<num_cpus;j++) {
			sprintf(rapl_native_events[i].name,
		   		"THERM_STATUS_CNT:PACKAGE%d:CPU%d", cpu_package[j], cpu_core_id[j]);
	   		sprintf(rapl_native_events[i].description,
		   		"Raw temperature register for package %d cpu %d", cpu_package[j], cpu_core_id[j]);
	   		rapl_native_events[i].fd_offset=j;
	   		rapl_native_events[i].msr=IA32_THERM_STATUS;
	   		rapl_native_events[i].resources.selector = i + 1;
	   		rapl_native_events[i].resources.package = cpu_package[j];
	   		rapl_native_events[i].type=PACKAGE_TEMPERATURE_CNT;
	   		rapl_native_events[i].return_type=PAPI_DATATYPE_UINT64;


			sprintf(rapl_native_events[k].name,
		   		"THERM_STATUS:PACKAGE%d:CPU%d", cpu_package[j], cpu_core_id[j]);
	   		strncpy(rapl_native_events[k].units,"°C",PAPI_MIN_STR_LEN);
	   		sprintf(rapl_native_events[k].description,
		   		"Temperature for package %d cpu %d", cpu_package[j], cpu_core_id[j]);
	   		rapl_native_events[k].fd_offset=j;
	   		rapl_native_events[k].msr=IA32_THERM_STATUS;
	   		rapl_native_events[k].resources.selector = k + 1;
	   		rapl_native_events[k].resources.package = cpu_package[j];
	   		rapl_native_events[k].type=PACKAGE_TEMPERATURE;
	   		rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;

		 i++;
		 k++;
	 }

	for (j=0;j<num_cpus;j++) {
		sprintf(rapl_native_events[k].name,
		   		"AVG_FREQUENCY:PACKAGE%d:CPU%d", cpu_package[j], cpu_core_id[j]);
	   	strncpy(rapl_native_events[k].units,"MHz",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Base frequency for package %d cpu %d", cpu_package[j], cpu_core_id[j]);
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=0;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].resources.package = cpu_package[j];
	   	rapl_native_events[k].resources.cpu = j;
	   	rapl_native_events[k].resources.package_cpu = cpu_core_id[j];
	   	rapl_native_events[k].type=PACKAGE_FREQUENCY;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"EFF_FREQUENCY:PACKAGE%d:CPU%d", cpu_package[j], cpu_core_id[j]);
	   	strncpy(rapl_native_events[k].units,"MHz",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Effective frequency for package %d cpu %d", cpu_package[j], cpu_core_id[j]);
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=1;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].resources.package = cpu_package[j];
	   	rapl_native_events[k].resources.cpu = j;
	   	rapl_native_events[k].resources.package_cpu = cpu_core_id[j];
	   	rapl_native_events[k].type=PACKAGE_FREQUENCY;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;
	}

		/* system statistics */
		sprintf(rapl_native_events[k].name,
		   		"TIME_WORK");
	   	strncpy(rapl_native_events[k].units,"s",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Cpu work time");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=0;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"TIME_IDLE");
	   	strncpy(rapl_native_events[k].units,"s",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Cpu idle time");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=1;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"TIME_UTIL");
	   	strncpy(rapl_native_events[k].units,"%",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Cpu time utilization ratio");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=2;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"MEM_TOTAL");
	   	strncpy(rapl_native_events[k].units,"kB",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Memory total");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=3;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"MEM_USED");
	   	strncpy(rapl_native_events[k].units,"kB",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Memory used");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=4;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"MEM_FREE");
	   	strncpy(rapl_native_events[k].units,"kB",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Cpu work time");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=5;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"SWAP_TOTAL");
	   	strncpy(rapl_native_events[k].units,"kB",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Swap total");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=6;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"SWAP_USED");
	   	strncpy(rapl_native_events[k].units,"kB",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Swap used");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=7;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;

		sprintf(rapl_native_events[k].name,
		   		"SWAP_FREE");
	   	strncpy(rapl_native_events[k].units,"kB",PAPI_MIN_STR_LEN);
	   	sprintf(rapl_native_events[k].description,
		   		"Swap free");
		rapl_native_events[k].fd_offset=j;
	   	rapl_native_events[k].msr=8;
	   	rapl_native_events[k].resources.selector = k + 1;
	   	rapl_native_events[k].type=SYSTEM_STATS;
	   	rapl_native_events[k].return_type=PAPI_DATATYPE_UINT64;
		k++;



     /* Export the total number of events available */
     _rapl_vector.cmp_info.num_native_events = num_events;

     _rapl_vector.cmp_info.num_cntrs = num_events;
     _rapl_vector.cmp_info.num_mpx_cntrs = num_events;


     /* Export the component id */
     _rapl_vector.cmp_info.CmpIdx = cidx;

     return PAPI_OK;
}


/*
 * Control of counters (Reading/Writing/Starting/Stopping/Setup)
 * functions
 */
static int 
_rapl_init_control_state( hwd_control_state_t *ctl)
{

  _rapl_control_state_t* control = (_rapl_control_state_t*) ctl;
  int i;
  
  for(i=0;i<RAPL_MAX_COUNTERS;i++) {
     control->being_measured[i]=0;
  }

  return PAPI_OK;
}

static int 
_rapl_start( hwd_context_t *ctx, hwd_control_state_t *ctl)
{
  _rapl_context_t* context = (_rapl_context_t*) ctx;
  _rapl_control_state_t* control = (_rapl_control_state_t*) ctl;
  long long now = PAPI_get_real_usec();
  int i;

  for( i = 0; i < RAPL_MAX_COUNTERS; i++ ) {
     if ((control->being_measured[i]) && (control->need_difference[i])) {
        context->start_value[i]=read_rapl_value(i);
     }
  }

  control->lastupdate = now;

  cpu_stat_first = NULL;

  return PAPI_OK;
}

static int 
_rapl_stop( hwd_context_t *ctx, hwd_control_state_t *ctl )
{

    /* read values */
    _rapl_context_t* context = (_rapl_context_t*) ctx;
    _rapl_control_state_t* control = (_rapl_control_state_t*) ctl;
    long long now = PAPI_get_real_usec();
    int i;
    long long temp;

	if (control->need_frequency == 1) {
		rapl_update_frequency();
	}
	if (control->need_systemstats == 1) {
		rapl_update_systemstats();
	}

    for ( i = 0; i < RAPL_MAX_COUNTERS; i++ ) {
		
		if (control->being_measured[i]) {
			temp = read_rapl_value(i);
			if (context->start_value[i])
			if (control->need_difference[i]) {
				/* test for wrap around */
				if (temp < context->start_value[i] ) {
	        		SUBDBG("Wraparound!\nstart:\t%#016x\ttemp:\t%#016x",
	        			(unsigned)context->start_value[i], (unsigned)temp);
	           		temp += (0x100000000 - context->start_value[i]);
	           		SUBDBG("\tresult:\t%#016x\n", (unsigned)temp);
		  		} else {
					temp -= context->start_value[i];
		  		}
			}
			control->count[i] = convert_rapl_energy( i, temp );
		}
    }
    control->lastupdate = now;
    return PAPI_OK;
}

/* Shutdown a thread */
static int
_rapl_shutdown_thread( hwd_context_t *ctx )
{
  ( void ) ctx;
  return PAPI_OK;
}

int 
_rapl_read( hwd_context_t *ctx, hwd_control_state_t *ctl,
	    long long **events, int flags)
{
    (void) flags;

    _rapl_stop( ctx, ctl );

    /* Pass back a pointer to our results */
    *events = ((_rapl_control_state_t*) ctl)->count;

    return PAPI_OK;
}


/*
 * Clean up what was setup in  rapl_init_component().
 */
static int 
_rapl_shutdown_component( void ) 
{
    int i;

    if (rapl_native_events) papi_free(rapl_native_events);
    if (fd_array) {
       for(i=0;i<num_cpus;i++) {
	  if (fd_array[i].open) close(fd_array[i].fd);
       }
       papi_free(fd_array);
    }
	if (NULL != package_temperature_targets) {
		papi_free(package_temperature_targets);
		package_temperature_targets = NULL;
	}
	if (NULL != cpu_freq) {
		papi_free(cpu_freq);
		cpu_freq = NULL;
	}
	if (NULL != cpu_freq_before) {
		papi_free(cpu_freq_before);
		cpu_freq_before = NULL;
	}

    return PAPI_OK;
}


/* This function sets various options in the component
 * The valid codes being passed in are PAPI_SET_DEFDOM,
 * PAPI_SET_DOMAIN, PAPI_SETDEFGRN, PAPI_SET_GRANUL * and PAPI_SET_INHERIT
 */
static int
_rapl_ctl( hwd_context_t *ctx, int code, _papi_int_option_t *option )
{
    ( void ) ctx;
    ( void ) code;
    ( void ) option;

    return PAPI_OK;
}


static int
_rapl_update_control_state( hwd_control_state_t *ctl,
			    NativeInfo_t *native, int count,
			    hwd_context_t *ctx )
{
  int i, index;
    ( void ) ctx;

    _rapl_control_state_t* control = (_rapl_control_state_t*) ctl;

    /* Ugh, what is this native[] stuff all about ?*/
    /* Mostly remap stuff in papi_internal */

    for(i=0;i<RAPL_MAX_COUNTERS;i++) {
       control->being_measured[i]=0;
    }

	control->need_frequency = 0;
	control->need_systemstats = 0;
    for( i = 0; i < count; i++ ) {
       index=native[i].ni_event&PAPI_NATIVE_AND_MASK;
       native[i].ni_position=rapl_native_events[index].resources.selector - 1;
       control->being_measured[index]=1;
		if (rapl_native_events[index].type == PACKAGE_FREQUENCY) {
			control->need_frequency = 1;
		}
		if (rapl_native_events[index].type == SYSTEM_STATS) {
			control->need_systemstats = 1;
		}

       /* Only need to subtract if it's a PACKAGE_ENERGY or ENERGY_CNT type */
/*
       control->need_difference[index]=
	 	(rapl_native_events[index].type==PACKAGE_ENERGY ||
		rapl_native_events[index].type==DRAM_ENERGY ||
	 	rapl_native_events[index].type==PACKAGE_ENERGY_CNT);
*/
    }

    return PAPI_OK;
}


/*
 * This function has to set the bits needed to count different domains
 * In particular: PAPI_DOM_USER, PAPI_DOM_KERNEL PAPI_DOM_OTHER
 * By default return PAPI_EINVAL if none of those are specified
 * and PAPI_OK with success
 * PAPI_DOM_USER is only user context is counted
 * PAPI_DOM_KERNEL is only the Kernel/OS context is counted
 * PAPI_DOM_OTHER  is Exception/transient mode (like user TLB misses)
 * PAPI_DOM_ALL   is all of the domains
 */
static int
_rapl_set_domain( hwd_control_state_t *ctl, int domain )
{
    ( void ) ctl;
    
    /* In theory we only support system-wide mode */
    /* How to best handle that? */
    if ( PAPI_DOM_ALL != domain )
	return PAPI_EINVAL;

    return PAPI_OK;
}


static int
_rapl_reset( hwd_context_t *ctx, hwd_control_state_t *ctl )
{
    ( void ) ctx;
    ( void ) ctl;
	
    return PAPI_OK;
}


/*
 * Native Event functions
 */
static int
_rapl_ntv_enum_events( unsigned int *EventCode, int modifier )
{

     int index;

     switch ( modifier ) {

	case PAPI_ENUM_FIRST:

	   if (num_events==0) {
	      return PAPI_ENOEVNT;
	   }
	   *EventCode = 0;

	   return PAPI_OK;
		

	case PAPI_ENUM_EVENTS:
	
	   index = *EventCode & PAPI_NATIVE_AND_MASK;

	   if ( index < num_events - 1 ) {
	      *EventCode = *EventCode + 1;
	      return PAPI_OK;
	   } else {
	      return PAPI_ENOEVNT;
	   }
	   break;
	
	default:
		return PAPI_EINVAL;
	}

	return PAPI_EINVAL;
}

/*
 *
 */
static int
_rapl_ntv_code_to_name( unsigned int EventCode, char *name, int len )
{

     int index = EventCode & PAPI_NATIVE_AND_MASK;

     if ( index >= 0 && index < num_events ) {
	strncpy( name, rapl_native_events[index].name, len );
	return PAPI_OK;
     }

     return PAPI_ENOEVNT;
}

/*
 *
 */
static int
_rapl_ntv_code_to_descr( unsigned int EventCode, char *name, int len )
{
     int index = EventCode;

     if ( index >= 0 && index < num_events ) {
	strncpy( name, rapl_native_events[index].description, len );
	return PAPI_OK;
     }
     return PAPI_ENOEVNT;
}

static int
_rapl_ntv_code_to_info(unsigned int EventCode, PAPI_event_info_t *info) 
{

  int index = EventCode;

  if ( ( index < 0) || (index >= num_events )) return PAPI_ENOEVNT;

  strncpy( info->symbol, rapl_native_events[index].name, sizeof(info->symbol)-1);
  info->symbol[sizeof(info->symbol)-1] = '\0';

  strncpy( info->long_descr, rapl_native_events[index].description, sizeof(info->long_descr)-1);
  info->long_descr[sizeof(info->long_descr)-1] = '\0';

  strncpy( info->units, rapl_native_events[index].units, sizeof(info->units)-1);
  info->units[sizeof(info->units)-1] = '\0';

  info->data_type = rapl_native_events[index].return_type;

  return PAPI_OK;
}



papi_vector_t _rapl_vector = {
    .cmp_info = { /* (unspecified values are initialized to 0) */
       .name = "rapl",
       .short_name = "rapl",
       .description = "Linux RAPL energy measurements",
       .version = "5.3.0",
       .default_domain = PAPI_DOM_ALL,
       .default_granularity = PAPI_GRN_SYS,
       .available_granularities = PAPI_GRN_SYS,
       .hardware_intr_sig = PAPI_INT_SIGNAL,
       .available_domains = PAPI_DOM_ALL,
    },

	/* sizes of framework-opaque component-private structures */
    .size = {
	.context = sizeof ( _rapl_context_t ),
	.control_state = sizeof ( _rapl_control_state_t ),
	.reg_value = sizeof ( _rapl_register_t ),
	.reg_alloc = sizeof ( _rapl_reg_alloc_t ),
    },
	/* function pointers in this component */
    .init_thread =          _rapl_init_thread,
    .init_component =       _rapl_init_component,
    .init_control_state =   _rapl_init_control_state,
    .start =                _rapl_start,
    .stop =                 _rapl_stop,
    .read =                 _rapl_read,
    .shutdown_thread =      _rapl_shutdown_thread,
    .shutdown_component =   _rapl_shutdown_component,
    .ctl =                  _rapl_ctl,

    .update_control_state = _rapl_update_control_state,
    .set_domain =           _rapl_set_domain,
    .reset =                _rapl_reset,

    .ntv_enum_events =      _rapl_ntv_enum_events,
    .ntv_code_to_name =     _rapl_ntv_code_to_name,
    .ntv_code_to_descr =    _rapl_ntv_code_to_descr,
    .ntv_code_to_info =     _rapl_ntv_code_to_info,
};

CC=gcc
CCP=g++
CFLAGS= -Wall -O3 -g -I../../include/ -I../../libmeasure/mic_intel_knc/ -I../../libmeasure/fpga_maxeler_max3a/ -I../../libmeasure/gpu_nvidia_tesla_kepler/ -I../../libmeasure/sys_dell_idrac7/ -I../../libmeasure/cpu_intel_xeon_sandy/ -I../ -I../papi/src/
LDFLAGS= -L../ -lapapi



all:
	#$(CC) $(CFLAGS) -c ms_common.c -o ms_common.o
	#ar rcs libms_common.a ms_common.o
	$(CC) $(CFLAGS) $(LFLAGS) -c -fPIC ms_common.c -o ms_common.o
	$(CCP) $(CFLAGS) $(LFLAGS) -c -fPIC list.cpp -o list.o
	$(CC) $(CFLAGS) $(LFLAGS) -c -fPIC ms_cpu_intel_xeon_sandy.c -o ms_cpu_intel_xeon_sandy.o
	$(CC) $(CFLAGS) $(LFLAGS) -c -fPIC ms_fpga_maxeler_max3a.c -o ms_fpga_maxeler_max3a.o
	$(CC) $(CFLAGS) $(LFLAGS) -c -fPIC ms_gpu_nvidia_tesla_kepler.c -o ms_gpu_nvidia_tesla_kepler.o
	$(CC) $(CFLAGS) $(LFLAGS) -c -fPIC ms_mic_intel_knc.c -o ms_mic_intel_knc.o
	$(CC) $(CFLAGS) $(LFLAGS) -c -fPIC ms_sys_dell_idrac7.c -o ms_sys_dell_idrac7.o
	#$(CC) -fPIC -shared -Wl,-soname,libms_common.so.1 -Wl,unresolved-symbols,ignore-in-shared-libs -o libms_common.so ms_common.o -lapapi
	$(CC) -Wall -fPIC -shared -Wl,-soname,libms_common.so.1 -o libms_common.so ms_common.o list.o ms_cpu_intel_xeon_sandy.o ms_fpga_maxeler_max3a.o ms_gpu_nvidia_tesla_kepler.o ms_mic_intel_knc.o ms_sys_dell_idrac7.o -L../ -lapapi


%: %.cpp
	$(CC) $(CFLAGS) $(LIBDIRS) -o $@ $< $(LIBS)

%: %.c
	$(CC) $(CFLAGS) $(LIBDIRS) -o $@ $< $(LIBS)


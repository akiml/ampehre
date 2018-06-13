NAME=nvml
include ../../Makefile_comp_tests
include ../Makefile.nvml

#NVCC = $(CUDA_DIR)/bin/nvcc
NVCC = gcc
NVCFLAGS = -L$(NVML_INCDIR) -ccbin=$(CC)
CUDALIBS = -L$(NVML_LIBDIR)/lib64 -lnvidia-ml
PAPILIB := ../../../libpapi.a  -ldl

%.o:%.cu
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -c -o $@ $<

TESTS = HelloWorld nvml_power_limiting_test

nvml_tests: $(TESTS) 

HelloWorld: HelloWorld.o $(UTILOBJS) 
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -o HelloWorld HelloWorld.o $(UTILOBJS) $(PAPILIB) $(CUDALIBS)

nvml_power_limiting_test: nvml_power_limiting_test.o $(UTILOBJS) 
	$(NVCC) $(NVCFLAGS) $(INCLUDE) -o nvml_power_limiting_test nvml_power_limiting_test.o $(UTILOBJS) $(PAPILIB) $(CUDALIBS)
	cp nvml_power_limiting_test ~/nvml_power_limiting_test

clean:
	rm -f $(TESTS) *.o

FORCE:

NAME=nvml
include ../../Makefile_comp_tests
include ../Makefile.nvml

#NVCC = $(CUDA_DIR)/bin/nvcc
NVCC = gcc
NVCFLAGS =
CUDALIBS = -L$(NVML_DIR)/lib64 -lnvidia-ml

%.o:%.c
	$(NVCC) $(INCLUDE) $(NVCFLAGS) -c -o $@ $<

TESTS = HelloWorld

nvml_tests: $(TESTS) 

HelloWorld: HelloWorld.o $(UTILOBJS)
	$(CC) $(INCLUDE) -o HelloWorld HelloWorld.o $(UTILOBJS) $(PAPILIB) $(CUDALIBS)

clean:
	rm -f $(TESTS) *.o






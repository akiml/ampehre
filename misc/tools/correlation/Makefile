#
# Makefile
#
# Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Achim Lösch (achim.loesch@upb.de)
# created: 2/04/15
# version: 0.3.1 - add correlation example application
#

NVCC=nvcc
CC=gcc

GCC_VERSION = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
GPP_VERSION = $(shell g++ --version | grep ^g++ | sed 's/^.* //g')
NVCC_VERSION = $(shell nvcc --version | grep ^Cuda | sed 's/^.* V//g')

CFLAGS=-O3
GCCFLAGS=$(CFLAGS) -Wall
NVCCFLAGS=$(CFLAGS) -gencode=arch=compute_35,code=compute_35

BASE_DIR=/usr/ampehre

SOURCES_CPU= \
	main.c \
	printer.c \
	measure.c \
	correlation.c \
	time_watch.c

SOURCES_GPU= \
	correlation_gpu.cu \
	correlation_gpu_kernel.cu

OBJECTS_CPU=$(SOURCES_CPU:.c=.o)

OBJECTS_GPU=$(SOURCES_GPU:.cu=.o)

.PHONY=all msg build clean cleanall install

all: msg build

build: $(OBJECTS_CPU) $(OBJECTS_GPU)
	$(NVCC) $(NVCCFLAGS) $? -L$(BASE_DIR)/lib -lms_common -lrt -lcjson -o correlation_original

msg:
	@echo "Build environment:"
	@echo "GCC  version: $(GCC_VERSION)"
	@echo "G++  version: $(GPP_VERSION)"
	@echo "NVCC version: $(NVCC_VERSION)"
	@echo ""

$(OBJECTS_CPU): %.o : %.c
	$(CC) $(GCCFLAGS) $< -c -I../../include -o $@

$(OBJECTS_GPU): %.o : %.cu
	$(NVCC) $(NVCCFLAGS) $< -c -I../../include -o $@

install: build
	rm -f $(BASE_DIR)/bin/correlation_original
	cp -v correlation_original $(BASE_DIR)/bin

clean:
	rm -f *.o

cleanall: clean
	rm -f correlation_original

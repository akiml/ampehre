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
# created: 1/11/14
# version: 0.2.5 - add gaussblur example application
#          0.7.3 - add enum for ipmi_timeout_setting in libmeasure
#

PGCC=pgcc
CC=gcc

CFLAGS=-O3
PGFLAGS=-acc -ta=tesla,cuda6.0 -Minfo

BASE_DIR=/usr/ampehre

INCLUDE_FILES = \
	-I../../../libmeasure/cpu_intel_xeon_sandy \
	-I../../../libmeasure/gpu_nvidia_tesla_kepler \
	-I../../../libmeasure/fpga_maxeler_max3a \
	-I../../../libmeasure/mic_intel_knc \
	-I../../../libmeasure/sys_dell_idrac7\
	-I../../../include

SOURCES_CPU= \
	main.c \
	printer.c \
	measure.c

SOURCES_GPU= \
	gaussblur.c \
	gaussblur_gpu.c

OBJECTS_CPU=$(SOURCES_CPU:.c=.o)

OBJECTS_GPU=$(SOURCES_GPU:.c=.o)

.PHONY=all build clean cleanall install

build: $(OBJECTS_CPU) $(OBJECTS_GPU)
	$(PGCC) $(CFLAGS) $(PGFLAGS) $? -L$(BASE_DIR)/lib -lms_common -lrt -lcjson -o gaussblur_original

$(OBJECTS_CPU): %.o : %.c
	$(CC) $(CFLAGS) $< -c ${INCLUDE_FILES} -o $@

$(OBJECTS_GPU): %.o : %.c
	$(PGCC) $(CFLAGS) $(PGFLAGS) $< -c ${INCLUDE_FILES} -o $@

install: build
	rm -f $(BASE_DIR)/bin/gaussblur_original
	cp -v gaussblur_original $(BASE_DIR)/bin

clean:
	rm -f *.o

cleanall: clean
	rm -f gaussblur_original

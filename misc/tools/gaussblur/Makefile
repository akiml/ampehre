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
#

PGCC=pgcc
CC=gcc

CFLAGS=-O3
PGFLAGS=-acc -ta=tesla,cuda6.0 -Minfo

BASE_DIR=/usr/ampehre

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
	$(PGCC) $(CFLAGS) $(PGFLAGS) $? -L$(BASE_DIR)/lib -lms_common_light -lrt -lcjson -o gaussblur_original

$(OBJECTS_CPU): %.o : %.c
	$(CC) $(CFLAGS) $< -c -I../../include -o $@

$(OBJECTS_GPU): %.o : %.c
	$(PGCC) $(CFLAGS) $(PGFLAGS) $< -c -I../../include -o $@

install: build
	rm -f $(BASE_DIR)/bin/gaussblur_original
	cp -v gaussblur_original $(BASE_DIR)/bin

clean:
	rm -f *.o

cleanall: clean
	rm -f gaussblur_original

#
# Makefile
#
# Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license or GPL license. See the LICENSE_BSD and
# LICENSE_GPL files for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Achim Lösch (achim.loesch@upb.de)
# created: 8/28/14
# version: 0.1.6 - add an measuring system driver for cpu stat readings
#	   0.2.0 - add support for msr registers to the measure driver
#

.PHONY: all load unload clean install

EXTRA_CFLAGS = "-DSHOW_WARNINGS"

obj-m = driver_measure.o

KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(shell pwd) modules

install: unload
	sudo rm -f /lib/modules/$(KVERSION)/kernel/drivers/driver_measure.ko
	sudo cp driver_measure.ko /lib/modules/$(KVERSION)/kernel/drivers
	sudo depmod
	make load

load:
	sudo modprobe driver_measure
	sudo chmod 666 /dev/measure

unload:
	sudo modprobe -r driver_measure

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(shell pwd) clean

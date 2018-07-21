.PHONY: all load unload clean install

EXTRA_CFLAGS = "-DSHOW_WARNINGS"

obj-m = amsr.o

KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(shell pwd) modules

install: unload
	sudo rm -f /lib/modules/$(KVERSION)/kernel/drivers/amsr.ko
	sudo cp amsr.ko /lib/modules/$(KVERSION)/kernel/drivers
	sudo depmod
	make load

load:
	sudo modprobe amsr
	sudo chmod 666 /dev/cpu/*/amsr

unload:
	sudo modprobe -r amsr

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(shell pwd) clean

#
# Makefile
#
# Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>, Ahmad El-Ali <aelali@mail.upb.de>
# All rights reserved.
#
# This software may be modified and distributed under the terms
# of the BSD license. See the LICENSE file for details.
#
# encoding: UTF-8
# tab size: 4
#
# author: Achim Lösch (achim.loesch@upb.de)
# created: 8/11/14
# version: 0.1.0 - initial make file
#          0.1.6 - add an measuring system driver for cpu stat readings
#          0.2.3 - add gpu_management tool and use the tool in the libmeasure
#          0.2.5 - add gaussblur example application
#          0.3.1 - add correlation example application
#

BASE_DIR=/usr/ampehre/
GCC=/usr/bin/gcc-4.6
GXX=/usr/bin/g++-4.6

.PHONY: all install debug clean driver driver_install gaussblur gaussblur_install correlation correlation_install mic_start docs client client_clean

all:
	mkdir -p build
	cd build && cmake -DSIGNALS=ON  -DDEBUG_SYMS=OFF -DCMAKE_INSTALL_PREFIX=$(BASE_DIR) -DCMAKE_C_COMPILER=$(GCC) -DCMAKE_CXX_COMPILER=$(GXX) .. && make
	cd utilities/msmonitor_cs/client && qmake-qt4 PREFIX=$(BASE_DIR) msmonitor.pro && make

debug:
	mkdir -p build
	cd build && cmake -DSIGNALS=ON  -DDEBUG_SYMS=ON  -DCMAKE_INSTALL_PREFIX=$(BASE_DIR) -DCMAKE_C_COMPILER=$(GCC) -DCMAKE_CXX_COMPILER=$(GXX) .. && make

install: all
	sudo mkdir -p $(BASE_DIR) $(BASE_DIR)/bin $(BASE_DIR)/lib $(BASE_DIR)/include $(BASE_DIR)/share $(BASE_DIR)/share/data
	sudo chown root:user $(BASE_DIR) $(BASE_DIR)/bin $(BASE_DIR)/lib $(BASE_DIR)/include $(BASE_DIR)/share $(BASE_DIR)/share/data
	sudo chmod 775 $(BASE_DIR) $(BASE_DIR)/bin $(BASE_DIR)/lib $(BASE_DIR)/include $(BASE_DIR)/share $(BASE_DIR)/share/data
	sudo rm -f $(BASE_DIR)/bin/gpu_management
	cd build && make install
	cd utilities/msmonitor_cs/client && make install
	sudo chown root:root $(BASE_DIR)/bin/gpu_management
	sudo chmod 4755 $(BASE_DIR)/bin/gpu_management
	cd papi/src && make install
	sudo ldconfig

clean:
	rm -rf build
	cd driver && make clean
	cd misc/tools/gaussblur && make cleanall
	cd misc/tools/correlation && make cleanall
	cd docs/het_node_doc && make clean
	cd utilities/msmonitor_cs/client && rm -rf build Makefile
	cd papi/src && make clean

docs:
	cd docs/het_node_doc && make

driver:
	cd driver && make

driver_install:
	cd driver && make install

gaussblur:
	cd misc/tools/gaussblur && make build

gaussblur_install:
	cd misc/tools/gaussblur && make install

correlation:
	cd misc/tools/correlation && make build

correlation_install:
	cd misc/tools/correlation && make install

mic_start:
	sudo service mpss restart

client:
	cd utilities/msmonitor_cs/client && qmake-qt4 PREFIX=/usr/local/bin msmonitor.pro && make

clean_client:
	cd utilities/msmonitor_cs/client && rm -rf build Makefile

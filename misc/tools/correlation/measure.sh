#!/bin/bash

#
# measure.sh
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

#run measurements
app_name=./correlation_original
app_meas=10
#number of executions while measuring
app_itrs=10

function build {
	make cleanall && make
}

function first_exec {
	`$app_name -g $1`
	sleep 20
}

function measure {
	build
	first_exec $2
	
	hettime -e ./run_app.sh -a "$app_itrs $app_name $2" > measurements.txt
	
	itrs=$(($app_itrs+1))
	
	echo "$itrs" > measurement.txt
	cat measurements.txt | grep "time total exec child" | sed 's/.*]: //g' >> measurement.txt 
	cat measurements.txt | grep "energy total cpu 0 pkg" | sed 's/.*]: //g' >> measurement.txt
	cat measurements.txt | grep "energy total cpu 0 dram" | sed 's/.*]: //g' >> measurement.txt
	cat measurements.txt | grep "energy total cpu 1 pkg" | sed 's/.*]: //g' >> measurement.txt
	cat measurements.txt | grep "energy total cpu 1 dram" | sed 's/.*]: //g' >> measurement.txt
	cat measurements.txt | grep "energy total gpu" | sed 's/.*]: //g' >> measurement.txt
	cat measurements.txt | grep "energy total fpga" | sed 's/.*]: //g' >> measurement.txt
	
	num=0
	
	for i in `cat measurement.txt`; do
		val[$num]=$i
		num=$((num+1))
	done
	
	rm -f measurements.txt
	rm -f measurement.txt
	
	divider=${val[0]}
	runtime=${val[1]}
	energy=`bc -l <<< "scale=8; ${val[2]}+${val[3]}+${val[4]}+${val[5]}+${val[6]}+${val[7]}"`
	
	runtime=`bc -l <<< "scale=8; $runtime/$divider.0"`
	energy=`bc -l <<< "scale=8; $energy/$divider.0"`
	
	echo "runtime: $runtime" > result_$1.txt
	echo "energy: $energy" >> result_$1.txt
}

function run {
	for i in `seq 0 $app_meas`; do
		measure $i $1
	done
	
	echo -n ";$1;runtime;" > result_$2_$1.csv
	
	for i in `seq 0 $app_meas`; do
		cat result_$i.txt | grep runtime | sed 's/.*: //g' | tr -d '\n' >> result_$2_$1.csv
		echo -n ";" >> result_$2_$1.csv
	done
	
	echo "" >> result_$2_$1.csv
	
	echo -n ";$1;energy;" >> result_$2_$1.csv
	
	for i in `seq 0 $app_meas`; do
	        cat result_$i.txt | grep energy | sed 's/.*: //g' | tr -d '\n' >> result_$2_$1.csv
	        echo -n ";" >> result_$2_$1.csv
	done
	
	echo "" >> result_$2_$1.csv
	
	mail_func $2 $1
		
	rm -f result_*.txt
}

function mail_func {
	echo "Successfully finished $1:$2" | mail -a result_$1_$2.csv -s "$1:$2 finished" achim.loesch@upb.de
}

function run_16384 {
	echo "#define N_CORRELATION 16384" > settings_dynamic.h
	app_itrs=1
	run 16384 16384
	run 8192 16384
	run 5462 16384
	run 4096 16384
}

function run_4096 {
	echo "#define N_CORRELATION 4096" > settings_dynamic.h
	app_itrs=10
	run 4096 4096
	run 2048 4096
	run 1366 4096
	run 1024 4096
}

run_4096
run_16384

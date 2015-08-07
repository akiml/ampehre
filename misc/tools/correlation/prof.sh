#!/bin/bash

#
# prof.sh
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

metrics=sm_efficiency,achieved_occupancy,sm_efficiency_instance,warp_execution_efficiency,issue_slot_utilization,warp_nonpred_execution_efficiency,cf_executed,inst_executed
events=active_warps,warps_launched,threads_launched,inst_executed,thread_inst_executed,active_cycles

function prof {
	/usr/local/cuda-6.5/bin/nvprof --print-gpu-trace --kernels correlation_calc_gpuOPT2 --events $events --metrics $metrics ./correlation_original -g $1 &> corr_$1
}

prof 4096
prof 16384


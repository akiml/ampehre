#!/bin/bash

#
# run_app.sh
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

for i in `seq 0 $1`; do
	$2 -g $3
done


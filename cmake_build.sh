#!/bin/bash

MAXELER_INCDIR="/upb/departments/agce/modules/software/maxeler/compiler/Maxcompiler-2013.3_vectis/lib/maxeleros-sim/include"
MAXELER_LIBDIR="/opt/maxeler/maxeleros/lib"

# get directory of this script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# check number of parameters
if [[ $# -ne 2 ]]; then
	echo "PAPI: $DIR/`basename $0` misses the prefix parameter or base dir parameter"
	echo "$DIR/`basename $0` [PREFIX] [BASEDIR]"
	exit 1
fi

# get install directory
PREFIX="$1"
BASEDIR="$2"

cd "$DIR"
cd src

# Configure
if [ ! -f Makefile ]; then
	echo "PAPI: configure nvml"
	cd components/nvml
	./configure --with-nvml-dir=/
	RET="$?"
	if [ $RET -ne 0 ]; then
		echo "PAPI: configure nvml failed"
		exit "$RET"
	fi

	cd ../..
	echo "PAPI: configure maxeler"
	cd components/maxeler
	./configure --with-maxeler-incdir="$MAXELER_INCDIR" --with-maxeler-libdir="$MAXELER_LIBDIR" --with-cjson-incdir="$BASEDIR/cjson" --with-cjson-libdir="$BASEDIR/build/cjson"
	RET="$?"
	if [ $RET -ne 0 ]; then
		echo "PAPI: configure maxeler failed"
		exit "$RET"
	fi

	cd ../..
	echo "PAPI: configure ipmi"
	cd components/ipmi
	./configure --with-ipmimeasure-libdir="$BASEDIR/build/misc/libraries/ipmiwrapper" --with-ipmimeasure-incdir="$BASEDIR/include"
	RET="$?"
	if [ $RET -ne 0 ]; then
		echo "PAPI: configure ipmi failed"
		exit "$RET"
	fi

	cd ../..
	echo "PAPI: configure PAPI"
	./configure "--with-components=rapl nvml maxeler ipmi micknc" "--prefix=$PREFIX"
	RET="$?"
	if [ $RET -ne 0 ]; then
		echo "PAPI: configure PAPI failed"
		exit "$RET"
	fi

	# Remove component tests
	sed -i 's/^TESTS.*$/TESTS =/' Makefile
	# Remove utilities
	sed -i 's/^UTIL_TARGETS.*$/UTIL_TARGETS =/' Makefile
fi


echo "PAPI: make"
make
RET="$?"
if [ $RET -ne 0 ]; then
	echo "PAPI: build failed"
	exit "$RET"
fi


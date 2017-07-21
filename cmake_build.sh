#!/bin/bash

# get directory of this script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# check number of parameters
if [[ $# -ne 6 ]]; then
	echo "PAPI: $DIR/`basename $0` misses the prefix parameter or base dir parameter"
	echo "$DIR/`basename $0` [PREFIX] [BASEDIR] [NVML_LIBDIR] [NVML_HEADERDIR] [MAXELER_LIBDIR] [MAXELER_HEADERDIR]"
	exit 1
fi

# get install directory
PREFIX="$1"
BASEDIR="$2"
NVML_LIBDIR="$3"
NVML_INCDIR="$4"
MAXELER_LIBDIR="$5"
MAXELER_INCDIR="$6"

cd "$DIR"
cd src

# Configure
if [ ! -f Makefile ]; then
	echo "PAPI: configure nvml"
	cd components/nvml
	./configure --with-nvml-libdir="$NVML_LIBDIR" --with-nvml-incdir="$NVML_INCDIR"
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


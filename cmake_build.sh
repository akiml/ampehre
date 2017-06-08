#!/bin/bash

# get directory of this script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# check number of parameters
if [[ $# -ne 1 ]]; then
	echo "PAPI: $DIR/`basename $0` misses the prefix parameter"
	exit 1
fi

# get install directory
PREFIX="$1"

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


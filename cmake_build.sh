#!/bin/bash

# get directory of this script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# check number of parameters
if [[ $# -ne 0 ]]; then
    cat << EOM
PAPI: $DIR/`basename $0` has unnecessary arguments
	parameters should be passed as environment variables
	List of environment variables
		PREFIX			install directory
		BASEDIR			base directory of source code
		USE_GPU			set to "ON" to configure the nvml component
		USE_MAXELER		set to "ON" to configure the maxeler component
		USE_IPMI		set to "ON" to configure the ipmi component
		USE_MIC			set to "ON" to configure the micknc component
		NVML_LIBDIR		nvml library directory for linker
		NVML_INCDIR		nvml header files directory for compiler
		MAXELER_LIBDIR		maxeler library directory for linker
		MAXELER_INCDIR		maxeler header files directory for compiler
EOM
	exit 1
fi

# check parameters passed using environment variables
if [ "$PREFIX" == "" ] || [ ! -d "$PREFIX" ]; then
    echo "PREFIX not set or invalid"
    exit 1
fi
if [ "$BASEDIR" == "" ] || [ ! -d "$BASEDIR" ]; then
    echo "BASEDIR not set or invalid"
    exit 1
fi
if [ "$USE_GPU" == "ON" ]; then
    if [ "$NVML_LIBDIR" == "" ] || [ ! -d "$NVML_LIBDIR" ]; then
        echo "USE_GPU == ON, but NVML_LIBDIR not set or invalid"
        exit 1
    fi
    if [ "$NVML_INCDIR" == "" ] || [ ! -d "$NVML_INCDIR" ]; then
        echo "USE_GPU == ON, but NVML_INCDIR not set or invalid"
        exit 1
    fi
fi
if [ "$USE_MAXELER" == "ON" ]; then
    if [ "$MAXELER_LIBDIR" == "" ] || [ ! -d "$MAXELER_LIBDIR" ]; then
        echo "USE_MAXELER == ON, but MAXELER_LIBDIR not set or invalid"
        exit 1
    fi
    if [ "$MAXELER_INCDIR" == "" ] || [ ! -d "$MAXELER_INCDIR" ]; then
        echo "USE_MAXELER == ON, but MAXELER_INCDIR not set or invalid"
        exit 1
    fi
fi

cd "$DIR"
cd src

# Configure
echo "PAPI: already configured, to reconfigure run 'make clean' or delete generated PAPI Makefile"
if [ ! -f Makefile ]; then
    COMP_GPU=""
    if [ "$USE_GPU" == "ON" ]; then
        echo "PAPI: configure nvml"
        cd components/nvml
        ./configure --with-nvml-libdir="$NVML_LIBDIR" --with-nvml-incdir="$NVML_INCDIR"
        RET="$?"
        if [ $RET -ne 0 ]; then
            echo "PAPI: configure nvml failed"
            exit "$RET"
        fi

        cd ../..
        COMP_GPU="nvml"
    fi

    COMP_MAXELER=""
    if [ "$USE_MAXELER" == "ON" ]; then
        echo "PAPI: configure maxeler"
        cd components/maxeler
        ./configure --with-maxeler-incdir="$MAXELER_INCDIR" --with-maxeler-libdir="$MAXELER_LIBDIR" --with-cjson-incdir="$BASEDIR/cjson" --with-cjson-libdir="$BASEDIR/build/cjson"
        RET="$?"
        if [ $RET -ne 0 ]; then
            echo "PAPI: configure maxeler failed"
            exit "$RET"
        fi

        cd ../..
        COMP_MAXELER="maxeler"
    fi

    COMP_IPMI=""
    if [ "$USE_IPMI" == "ON" ]; then
        echo "PAPI: configure ipmi"
        cd components/ipmi
        ./configure --with-ipmimeasure-libdir="$BASEDIR/build/misc/libraries/ipmiwrapper" --with-ipmimeasure-incdir="$BASEDIR/include"
        RET="$?"
        if [ $RET -ne 0 ]; then
            echo "PAPI: configure ipmi failed"
            exit "$RET"
        fi

        cd ../..
        COMP_IPMI="ipmi"
    fi

    COMP_MIC=""
    if [ "$USE_MIC" == "ON" ]; then
        # no explicit configuration necessary for this component
        COMP_MIC="micknc"
    fi

	echo "PAPI: configure PAPI"
	./configure "--with-components=rapl $COMP_GPU $COMP_MAXELER $COMP_IPMI $COMP_MIC" "--prefix=$PREFIX"
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


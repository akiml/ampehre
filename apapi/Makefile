
LIBPAPI=papi/src
CC=gcc
CFLAGS=-Wall -O3 -g -I . -I "$(LIBPAPI)" -Ilibpfm4/include
LFLAGS= -L "$(LIBPAPI)" -ldl -pthread -lrt

all: papi/src/libpapi.so
	$(CC) $(CFLAGS) -c apapi.c -o apapi.o
	ar rcs libapapi.a apapi.o
	$(CC) $(CFLAGS) -c -fPIC apapi.c -o apapi.o
	$(CC) -fPIC -L "$(LIBPAPI)" -lpapi -shared -Wl,-soname,libapapi.so.1 -o libapapi.so.1.0.1 apapi.o 

clean:
	rm -f *.o *.a libapapi.so*

# for this need to compile whole papi with CFLAGS = -fPIC
sharedwithpapi:
	$(CC) $(CFLAGS) $(LFLAGS) -fPIC -c apapi.cpp -o apapi.o
	ar rcs libapapi.a apapi_defaults.o apapi.o
	$(CC) $(CFLAGS) $(LFLAGS) -c -fPIC apapi.cpp -o apapi.o
	$(CC) -fPIC -shared -Wl,-soname,libapapi.so.1 -o libapapi.so.1.0.1 apapi.o $(LIBPAPI)/papi.o

papi/src/libpapi.so:
	cd papi/src/components/nvml/; ./configure --with-nvml-dir=/
	cd papi/src/; ./configure --with-components="rapl nvml"; make

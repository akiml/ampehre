
LIBPAPI=../papi/src
LIBAPAPI=..
CC=gcc
LD=ld
CFLAGS=-Wall -O3 -g -I "$(LIBPAPI)" -Ilibpfm4/include -I $(LIBAPAPI)
LIBDIRS= -L "$(LIBPAPI)" -L "$(LIBPAPI)/libpfm4/lib" -L "$(LIBPAPI)/testlib" -L $(LIBAPAPI) -Wl,-rpath,$(LIBPAPI),-rpath,$(LIBPAPI)/libpfm4/lib,-rpath,$(LIBAPAPI)/apapi
RPATH= -rpath=$(LIBPAPI) -rpath=$(LIBAPAPI)/apapi -rpath=$(LIBPAPI)/libpfm4/lib
LIBS= -lapapi -lpapi -ldl -pthread -lrt


SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $(LIBDIRS) -o $@ $< $(LIBS)

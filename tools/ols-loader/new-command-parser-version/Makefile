CC = gcc
CFLAGS = -g -O0 -std=c99
LDFLAGS =

OBJS =  main.o pump.o data_file.o serial.o

ifeq ($(shell uname), Darwin)
	CFLAGS = -g -O0 -std=c99 -DMACOSX=1 -force_cpusubtype_ALL -arch i386 -arch ppc
endif

all: pump-loader

pump-loader:  $(OBJS)
	$(CC) $(CFLAGS) -o pump-loader $(OBJS) $(LFD_OBJS) $(LDFLAGS)

%.o:	%.c
	$(CC) $(CFLAGS) $(DEFS) -c $<


clean:
	rm -f pump-loader *.o


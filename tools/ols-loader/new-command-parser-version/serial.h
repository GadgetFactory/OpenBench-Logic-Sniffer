/*
 * OS independent serial interface
 *
 * Heavily based on Pirate-Loader:
 * http://the-bus-pirate.googlecode.com/svn/trunk/bootloader-v4/pirate-loader/source/pirate-loader.c
 *
 */
#ifndef MYSERIAL_H_
#define MYSERIAL_H_
//  #define _WIN32
#ifdef _WIN32

	#include <windows.h>
	#include <time.h>

	#define O_NOCTTY 0
	#define O_NDELAY 0
	#define B115200 115200
	#define B921600 921600

	#define OS WINDOWS
    #ifndef _UWIN
       #define _UWIN
    #endif
    int DPwrite(int fd, const void* buf, int len);
    int DPread(int fd, void* buf, int len);
    int DPclose(int fd);
    int DPopen(const char* path, unsigned long flags);
    int __stdcall select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfs, const struct timeval* timeout);

#else
	#include <unistd.h>
	#include <termios.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <sys/time.h>

	#ifdef MACOSX
		#include <IOKit/serial/ioss.h>
		#include <sys/ioctl.h>

		#define B1500000 1500000
		#define B1000000 1000000
		#define B921600  921600
	#endif

	#define DPwrite  write
	#define DPopen   open
	#define DPclose  close
	#define DPread   read
  #endif


#include <stdint.h>


int readWithTimeout(int fd, uint8_t* out, int length, int timeout);
int configurePort(int fd, unsigned long baudrate);
int openPort(const char* dev, unsigned long flags);


#endif


/*
 * OS independent serial interface
 *
 * Heavily based on Pirate-Loader:
 * http://the-bus-pirate.googlecode.com/svn/trunk/bootloader-v4/pirate-loader/source/pirate-loader.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <string.h>

#include "serial.h"

#ifdef _WIN32
	int DPwrite(int fd, const void* buf, int len)
	{
		HANDLE hCom = (HANDLE)fd;
		int res = 0;
		unsigned long bwritten = 0;


		res = WriteFile(hCom, buf, len, &bwritten, NULL);

		if( res == FALSE ) {
			return -1;
		} else {
			return bwritten;
		}
	}

	int DPread(int fd, void* buf, int len)
	{
		HANDLE hCom = (HANDLE)fd;
		int res = 0;
		unsigned long bread = 0;

		res = ReadFile(hCom, buf, len, &bread, NULL);

		if( res == FALSE ) {
			return -1;
		} else {
			return bread;
		}
	}

	int DPclose(int fd)
	{
		HANDLE hCom = (HANDLE)fd;

		CloseHandle(hCom);
		return 0;
	}

	int DPopen(const char* path, unsigned long flags)
	{
		static char full_path[32] = {0};

		HANDLE hCom = NULL;

		if( path[0] != '\\' ) {
			_snprintf(full_path, sizeof(full_path) - 1, "\\\\.\\%s", path);
			path = full_path;
		}

		hCom = CreateFileA(path, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if( !hCom || hCom == INVALID_HANDLE_VALUE ) {
			return -1;
		} else {
			return (int)hCom;
		}
	}

	int __stdcall select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfs, const struct timeval* timeout)
	{
		time_t maxtc = time(0) + (timeout->tv_sec);
		COMSTAT cs = {0};
	//	unsigned long dwErrors = 0;

		if( readfds->fd_count != 1 ) {
			return -1;
		}

		while( time(0) <= maxtc )
		{ //only one file supported
			if( ClearCommError( (HANDLE)readfds->fd_array[0], 0, &cs) != TRUE ){
				return -1;
			}

			if( cs.cbInQue > 0 ) {
				return 1;
			}

			Sleep(10);
		}
		return 0;
	}

#else
#endif

int readWithTimeout(int fd, uint8_t *out, int length, int timeout)
{
	fd_set fds;
	struct timeval tv = {timeout, 0};
	int res = -1;
	int got = 0;

	do {

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		res = select(fd + 1, &fds, NULL, NULL, &tv);

		if( res > 0 ) {
			res = DPread(fd, out, length);
			if( res > 0 ) {
				length -= res;
				got    += res;
				out    += res;
			} else {
				break;
			}
		} else {
			return res;
		}
	} while( length > 0);

	return got;
}

int configurePort(int fd, unsigned long baudrate)
{
#ifdef WIN32
	DCB dcb = {0};
	HANDLE hCom = (HANDLE)fd;

	dcb.DCBlength = sizeof(dcb);

	dcb.BaudRate = baudrate;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	if( !SetCommState(hCom, &dcb) ){
		return -1;
	}

	return (int)hCom;
#else
    speed_t baud = B921600;
	struct termios g_new_tio;

	switch (baudrate) {
		case 921600:
			baud = B921600;
			break;
		case 115200:
			baud = B115200;
			break;
		case 1000000:
			baud = B1000000;
			break;
		case 1500000:
			baud = B1500000;
		default:
			printf("unknown speed setting \n");
			return -1;
			break;
	}

	memset(&g_new_tio, 0x00 , sizeof(g_new_tio));
	cfmakeraw(&g_new_tio);

	g_new_tio.c_cflag |=  (CS8 | CLOCAL | CREAD);
	g_new_tio.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
	g_new_tio.c_oflag = 0;
	g_new_tio.c_lflag = 0;

	g_new_tio.c_cc[VTIME] = 0;
	g_new_tio.c_cc[VMIN] = 1;

#ifdef MACOSX

	if( tcsetattr(fd, TCSANOW, &g_new_tio) < 0 ) {
		return -1;
	}

	return ioctl( fd, IOSSIOSPEED, &baud );
#else
	cfsetispeed (&g_new_tio, baudrate);
	cfsetospeed (&g_new_tio, baudrate);

	tcflush(fd, TCIOFLUSH);

	return tcsetattr(fd, TCSANOW, &g_new_tio);
#endif //#ifdef MACOSX

#endif
}

int openPort(const char* dev, unsigned long flags)
{
	return DPopen(dev, O_RDWR | O_NOCTTY | O_NDELAY | flags);
}


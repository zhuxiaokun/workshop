#ifndef _SOCK_HEADER_H_
#define _SOCK_HEADER_H_

#include "KType.h"
#include "KMacro.h"

#if defined(WIN32) || defined(WINDOWS)
	#include <WinSock2.h>
#else
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <sys/poll.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <netdb.h>
	extern int h_errno;
#endif

#endif
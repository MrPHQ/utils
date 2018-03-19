#ifndef __COMMON_CONFIG__
#define __COMMON_CONFIG__

#if (defined( __WIN32__ ) || defined( _WIN32 )) && !defined(XXX_STATIC)
#   ifdef UTILS_EXPORTS
#       define UTILS_API __declspec(dllexport)
#   else
#       define UTILS_API __declspec(dllimport)
#   endif
#else
#   define UTILS_API
#endif

#define ENABLE_ZIP
#define ENABLE_CRYPTOPP

#include <sys/types.h>  
#include <sys/stat.h>  

#ifdef _WIN32 //windows相关

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <Windows.h>
#include <Winsock2.h>
#include <Ws2TcpIp.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <functional>
#include <cassert>
#include <vector>

#else //linux相关

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/wait.h>
#include <sys/statvfs.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <netdb.h>
#include <dlfcn.h>
#include <signal.h>
#include <dirent.h>
#include <ctype.h>
#include <mntent.h>
#include <getopt.h>
#include <stdint.h>
#include <iconv.h>
#include <netinet/tcp.h>
#include <algorithm>
#include <list>
#include <map>
#include <string>
#endif //WIN32

extern HINSTANCE ghUtilsInstance;

#endif //_CONFIG_H__
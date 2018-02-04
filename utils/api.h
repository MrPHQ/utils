#ifndef __UTILS_API__
#define __UTILS_API__
#include <utils/utils.h>

namespace UTILS { namespace API {
	
	void* Malloc(int size, bool init=false);
	void Free(void* buf);
	void* Memcpy(void* dest, const void* src, int count);
	void* Memset(void* dest, int c, int count, int bCatch=0);
	void* Memmove(void* dest, const void* src, int count);
	int	Memcmp(const void* dest, const void* src, int count);

	int sprintf(char* buffer, int size, const char* format, ...);
}}
#endif
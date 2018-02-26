#ifndef __UTILS_API__
#define __UTILS_API__
#include <utils/utils.h>

namespace UTILS { namespace API {
	
	UTILS_API void* Malloc(int size, bool init=false);
	UTILS_API void Free(void* buf);
	UTILS_API void* Memcpy(void* dest, const void* src, int count);
	UTILS_API void* Memset(void* dest, int c, int count, int bCatch=0);
	UTILS_API void* Memmove(void* dest, const void* src, int count);
	UTILS_API int	Memcmp(const void* dest, const void* src, int count);

	UTILS_API int Sprintf(char* buffer, int size, const char* format, ...);
	UTILS_API int Strcpy(char* _Destination, char const* _Source, int _MaxCount);

	UTILS_API const char* GetCurrentPath(HINSTANCE hInstance = NULL);
	UTILS_API int CharacterConvert(const char* tocode, const char* fromcode,
		char *inbuf, int inlen, char *outbuf,
		int outlen, int* OutIdleLen, int* NoConvertLen);

	UTILS_API void SleepTime(int ms);
	/**
		获取当前进程ID
	*/
	UTILS_API int GetCurrentProcessID();


	UTILS_API bool IsPathExists(const char*);
}}
#endif
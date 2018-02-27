#ifndef __UTILS_API__
#define __UTILS_API__
#include <utils/utils.h>

#define ENABLE_ZIP

namespace UTILS { namespace API {
	
	/*
		内存操作相关.
	*/
	UTILS_API void* Malloc(int size, bool init=false);
	UTILS_API void Free(void* buf);
	UTILS_API void* Memcpy(void* dest, const void* src, int count);
	UTILS_API void* Memset(void* dest, int c, int count, int bCatch=0);
	UTILS_API void* Memmove(void* dest, const void* src, int count);
	UTILS_API int	Memcmp(const void* dest, const void* src, int count);

	/*
		字符串操作相关.
	*/
	UTILS_API int Sprintf(char* buffer, int size, const char* format, ...);
	UTILS_API int Strcpy(char* _Destination, char const* _Source, int _MaxCount);

	UTILS_API int CharacterConvert(const char* tocode, const char* fromcode,
		char *inbuf, int inlen, char *outbuf,
		int outlen, int* OutIdleLen, int* NoConvertLen);
	UTILS_API void CharConvert(char*, char s, char d);

	UTILS_API void SleepTime(int ms);

	/**
		获取当前进程ID
	*/
	UTILS_API int GetCurrentProcessID();


	/*
		文件路径相关.
	*/
	UTILS_API const char* GetCurrentPath(HINSTANCE hInstance = NULL);
	UTILS_API bool IsPathExists(const char*);
	UTILS_API void CreateFolders(const char*);
	UTILS_API void EnumDirectoryFiles(const char* pDir,
		char pExt[][16],
		int iExtNum,
		std::list<std::string>* lstDirs,
		std::list<std::string>* lstFiles,
		bool bRecursive);


	/*
		压缩相关.
	*/
#ifdef ENABLE_ZIP
	UTILS_API bool ZipDirectory(const char*, char* toFile);
	UTILS_API bool UnZipFile(const char* file, const char* toDirectory);
#endif
}}
#endif
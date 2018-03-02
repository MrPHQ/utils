#ifndef __UTILS_API__
#define __UTILS_API__
#include <utils/utils.h>

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

	/**
		\brief
			重新分配内存.
			重新分配内存,会保留原来的数据.
		\param iNewLen
			新分配内存大小.
		\param pBuff
			缓存区地址.
		\param iDataLen
			缓存区原有数据大小.
		\param bDel
			是否释放源缓存区.
	*/
	UTILS_API int MallocMemory(unsigned int iNewLen, BYTE*& pBuff, int iDataLen, BOOL bDel /*= TRUE*/);
	/*
		字符串操作相关.
	*/
	UTILS_API int Sprintf(char* buffer, int size, const char* format, ...);
	UTILS_API int Strcpy(char* _Destination, char const* _Source, int _MaxCount);
	UTILS_API int Strcmp(const char*, const char*);

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
	UTILS_API bool DelFolders(const char*);
	UTILS_API void EnumDirectoryFiles(const char* pDir,
		char pExt[][16],
		int iExtNum,
		std::list<std::string>* lstDirs,
		std::list<std::string>* lstFiles,
		bool bRecursive);
	/// 去除文件路径扩展名
	UTILS_API void RemoveExtension(char*);
	/// 去除文件名，得到目录
	UTILS_API void RemoveFileSpec(char*);
	/// 去掉路径中的目录部分,得到文件名
	UTILS_API void StripPath(char*);
	/// 查找路径的扩展名
	UTILS_API char* FindExtension(char*);

	/// 获取文件大小
	UTILS_API uint64_t FileSize(const char*);

	/*
		压缩相关.
	*/
#ifdef ENABLE_ZIP
	typedef struct _ZIP_COMMENT{
		char name[256];
		int entry;//0-文件夹 1-文件
		int len;//数据长度
		BYTE buff[4];
	}ZIP_COMMENT, *PZIP_COMMENT;
	UTILS_API bool ZipDirectory(const char*, char* toFile);
	UTILS_API bool UnZipFile(const char* file, const char* toDirectory, bool del = false);
	UTILS_API bool UnZipFile(const char* file, PZIP_COMMENT buff, int len, bool del = false);
#endif

	/*
		加解密相关.
	*/
#ifdef ENABLE_CRYPTOPP
	UTILS_API int EncryptionFile(const char* src, const char* des, const char* key, const char* iv);
	UTILS_API int EncryptionFile(const char* src, char* buff, int len, const char* key, const char* iv);
	UTILS_API int DecryptionFile(const char* src, const char* des, const char* key, const char* iv);
	UTILS_API int DecryptionFile(const char* src, char* buff, int len, const char* key, const char* iv);
	UTILS_API std::string FileSHA(const char*);
	UTILS_API std::string StringSHA256(const char*, int len);
	UTILS_API std::string StringSHA1(const char*, int len);
#endif

	/*
		打印调试信息
	*/
	UTILS_API void DEBUG_INFO(char*, ...);
}}
#endif
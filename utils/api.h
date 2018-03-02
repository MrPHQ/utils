#ifndef __UTILS_API__
#define __UTILS_API__
#include <utils/utils.h>

namespace UTILS { namespace API {
	
	/*
		�ڴ�������.
	*/
	UTILS_API void* Malloc(int size, bool init=false);
	UTILS_API void Free(void* buf);
	UTILS_API void* Memcpy(void* dest, const void* src, int count);
	UTILS_API void* Memset(void* dest, int c, int count, int bCatch=0);
	UTILS_API void* Memmove(void* dest, const void* src, int count);
	UTILS_API int	Memcmp(const void* dest, const void* src, int count);

	/**
		\brief
			���·����ڴ�.
			���·����ڴ�,�ᱣ��ԭ��������.
		\param iNewLen
			�·����ڴ��С.
		\param pBuff
			��������ַ.
		\param iDataLen
			������ԭ�����ݴ�С.
		\param bDel
			�Ƿ��ͷ�Դ������.
	*/
	UTILS_API int MallocMemory(unsigned int iNewLen, BYTE*& pBuff, int iDataLen, BOOL bDel /*= TRUE*/);
	/*
		�ַ����������.
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
		��ȡ��ǰ����ID
	*/
	UTILS_API int GetCurrentProcessID();


	/*
		�ļ�·�����.
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
	/// ȥ���ļ�·����չ��
	UTILS_API void RemoveExtension(char*);
	/// ȥ���ļ������õ�Ŀ¼
	UTILS_API void RemoveFileSpec(char*);
	/// ȥ��·���е�Ŀ¼����,�õ��ļ���
	UTILS_API void StripPath(char*);
	/// ����·������չ��
	UTILS_API char* FindExtension(char*);

	/// ��ȡ�ļ���С
	UTILS_API uint64_t FileSize(const char*);

	/*
		ѹ�����.
	*/
#ifdef ENABLE_ZIP
	typedef struct _ZIP_COMMENT{
		char name[256];
		int entry;//0-�ļ��� 1-�ļ�
		int len;//���ݳ���
		BYTE buff[4];
	}ZIP_COMMENT, *PZIP_COMMENT;
	UTILS_API bool ZipDirectory(const char*, char* toFile);
	UTILS_API bool UnZipFile(const char* file, const char* toDirectory, bool del = false);
	UTILS_API bool UnZipFile(const char* file, PZIP_COMMENT buff, int len, bool del = false);
#endif

	/*
		�ӽ������.
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
		��ӡ������Ϣ
	*/
	UTILS_API void DEBUG_INFO(char*, ...);
}}
#endif
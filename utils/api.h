#ifndef __UTILS_API__
#define __UTILS_API__
#include <utils/utils.h>
#include <utils\define.h>

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
	UTILS_API int Strcpy(char* _Destination, int len, char const* _Source);
	UTILS_API int Strcmp(const char*, const char*);
	UTILS_API int Strcat(char*, int len, const char*);
	/// 默认转小写
	UTILS_API int Transform(std::string&, bool tolow = true);
	UTILS_API int Transform(char*, bool tolow = true);

	/// 使用libiconv
	UTILS_API int CharacterConvert(const char* tocode, const char* fromcode,
		char *inbuf, int inlen, char *outbuf,
		int outlen, int* OutIdleLen, int* NoConvertLen);
	/// 转换字符串中的指定所有字符
	UTILS_API void CharConvert(char*, char s, char d);
	/// WideCharToMultiByte
	UTILS_API char* WcharToChar(wchar_t* wc);
	UTILS_API wchar_t* CharToWchar(char* c);
	/// 限制在1024Bytes
	UTILS_API void WcharToChar(wchar_t* wc, char* buff, int len);
	UTILS_API void CharToWchar(char* c, wchar_t* buff, int len);

	/*
		系统相关.
	*/
	UTILS_API void SleepTime(int ms);
	UTILS_API uint64_t fnGetTickCount();
	UTILS_API int SetWorkPath(const char*);
	UTILS_API int CreateGuid(char* buff, int len);
	/*
		事件相关
	*/
	UTILS_API HANDLE fnCreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset,BOOL bInitialState,const char* lpName);
	UTILS_API int fnCloseHandle(HANDLE hObject);
	UTILS_API int fnSetEvent(HANDLE hEvent);
	UTILS_API int fnResetEvent(HANDLE hEvent);
	UTILS_API uint32_t fnWaitForSingleObject(HANDLE hHandle,
		uint32_t dwMilliseconds);
	UTILS_API uint32_t fnWaitForMultipleObjects(int iCount,
		HANDLE *lpHandles,BOOL bWaitAll,uint32_t dwMilliseconds);

	/*
		线程相关
	*/
	UTILS_API HTHREAD fnCreateThread(THREAD_RET(CALLBACK* lpStartAddress)(void* lpParameter),
		void* lpParameter,uint32_t* lpThreadId);
	UTILS_API int fnCloseThread(HTHREAD hThread);
	UTILS_API int fnGetExitCodeThread(HTHREAD hThread, uint32_t* lpExitCode);
	UTILS_API int fnTerminateThread(HTHREAD hThread, uint32_t dwExitCode);
	UTILS_API uint32_t fnWaitForThreadExit(HTHREAD hHandle, uint32_t dwMilliseconds);
	UTILS_API void fnExitThread(uint32_t dwExitCode);
	UTILS_API int fnSetThreadPriority(int iPriority);
	UTILS_API uint32_t fnGetThreadId(HTHREAD hThread);

	/**
		进程相关..
	*/
	//获取当前进程ID
	UTILS_API int GetCurrentProcessID();
	UTILS_API int ForceKillProcess(const char* name);
	UTILS_API BOOL ForceKillProcess(DWORD dwPID);
	UTILS_API int GetprocessIDs(const char* lpszProName, DWORD dwPID[], int iSize);

	/**
	\brief
		运行程序

	\param cmd.
		命令行.
	\param uiTimeOut.
		超时时间. <=0 永远等待,直到程序退出.
	\return
		char*
	*/
	typedef struct _PROCESS_PROPERTY {
		uint32_t uiProcessID;
		HANDLE hProcessHandle;
	}PROCESS_PROPERTY, *PPROCESS_PROPERTY;
	UTILS_API int RunProcess(const char* cmd, PPROCESS_PROPERTY pProcessProperty = NULL, unsigned int uiTimeOut = 0);

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
	/// 拷贝目录下文件及其子目录到新的路径下,
	UTILS_API int CopyFolders(const char* src, const char* des, bool bRecursive = true);
	/// 替换目录下所有同名文件
	UTILS_API int ReplaceFiles(const char* file, const char* des, bool bRecursive = true);

	/// 去除文件路径扩展名
	UTILS_API void RemoveExtension(char*);
	/// 去除文件名，得到目录
	UTILS_API void RemoveFileSpec(char*);
	/// 去掉路径中的目录部分,得到文件名
	UTILS_API void StripPath(char*);
	/// 查找路径的扩展名
	UTILS_API char* FindExtension(char*);
	/// 获取相对路径 FILE_ATTRIBUTE_DIRECTORY FILE_ATTRIBUTE_NORMAL
	UTILS_API BOOL RelativePathTo(char* pszPath, char* pszFrom, DWORD dwAttrFrom, char* pszTo, DWORD dwAttrTo);
	/// 是否是相对路径
	UTILS_API BOOL IsRelativePath(char* path);
	/// 转换到相对路径
	UTILS_API void ConverToRelative(char* from, char* to, int len);
	/// 合并两个路径 "C:" + "One\Two\Three" = "C:\One\Two\Three",,可使用相对路径 "C:\One\Two\Three" + "..\..\four" = "C:\One\four"
	UTILS_API void CombinePath(char* buff, const char* first, const char* second);
	/// 删除文件
	UTILS_API BOOL DelFile(const char*);
	/// 拷贝文件
	UTILS_API BOOL FileCopy(const char*, const char*, BOOL bFailIfExists);

	/// 获取文件大小
	UTILS_API uint64_t FileSize(const char*);


	/*
		系统目录相关
	*/
	enum {
		OS_VERSION_NONE = 0,
		OS_VERSION_2000 = 1,
		OS_VERSION_XP = 2,
		OS_VERSION_2003 = 3,
		OS_VERSION_VISTA = 4,
		OS_VERSION_2008 = 5,
		OS_VERSION_WIN7 = 6,
		OS_VERSION_2012 = 7,
		OS_VERSION_WIN8 = 8,
		OS_VERSION_WIN10 = 9,
		OS_VERSION_2016 = 10,
	};
	UTILS_API int GetOSVersion();
	UTILS_API BOOL Is64();
	enum {
		FOLDER_ADMINTOOLS = 1,
		FOLDER_APPDATA = 2,
		FOLDER_COMMON_ADMINTOOLS = 3,
		FOLDER_COMMON_APPDATA = 4,//%ALLUSERSPROFILE% (%ProgramData%, %SystemDrive%\ProgramData)
		FOLDER_COMMON_DOCUMENTS = 5,
		FOLDER_COOKIES = 6,
		FOLDER_FLAG_CREATE = 7,
		FOLDER_FLAG_DONT_VERIFY = 8,
		FOLDER_HISTORY = 9,
		FOLDER_INTERNET_CACHE = 10,
		FOLDER_LOCAL_APPDATA = 11,
		FOLDER_MYPICTURES = 12,
		FOLDER_PERSONAL = 13,
		FOLDER_PROGRAM_FILES = 14,//C:\Program Files.
		FOLDER_PROGRAM_FILES_COMMON = 15,//C:\Program Files\Common. Valid only for Windows XP.
		FOLDER_SYSTEM = 16, //The Windows System folder. A typical path is C:\Windows\System32.
		FOLDER_WINDOWS = 17,//%windir% or %SYSTEMROOT% environment variables. A typical path is C:\Windows.
	};
	/// 自动根据操作系统版本判断,调用'GetOSFolderPathUseCSIDL', 'GetOSFolderPathUseID'
	UTILS_API int GetOSFolderPath(int flag, char* buff, int len);
	UTILS_API int GetOSFolderPathUseCSIDL(int csidl, char* buff, int len);
	UTILS_API int GetOSFolderPathUseID(GUID id, char* buff, int len);


	/*
		压缩相关.
	*/
#ifdef UTILS_ENABLE_ZIP
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
#ifdef UTILS_ENABLE_CRYPTOPP
	UTILS_API int EncryptionFile(const char* src, const char* des, const char* key, const char* iv);
	UTILS_API int EncryptionFile(const char* src, char* buff, int len, const char* key, const char* iv);
	UTILS_API int DecryptionFile(const char* src, const char* des, const char* key, const char* iv);
	UTILS_API int DecryptionFile(const char* src, char* buff, int len, const char* key, const char* iv);
	UTILS_API std::string FileSHA(const char*);
	UTILS_API std::string StringSHA256(const char*, int len);
	UTILS_API std::string StringSHA1(const char*, int len);
#endif

	/*
		注册表相关..
	*/
	/// 创建KEY ，如果存在则打开, close创建后是否关闭
	UTILS_API HKEY CreateRegKey(HKEY key, const char* subkey, bool close = true);
	UTILS_API int CloseRegKey(HKEY key);
	UTILS_API int WriteRegString(HKEY key, const char* subkey, const char* value, const char* val, int len);
	UTILS_API int WriteRegInt(HKEY key, const char* subkey, const char* value, int val);
	// HKEY_LOCAL_MACHINE..  section.=SOFTWARE\\InControl\\Install  Entry..= install
	UTILS_API int ReadRegString(HKEY key, const char* subkey, const char* value, char* buff, int len);
	UTILS_API int ReadRegInt(HKEY key, const char* subkey, const char* value, int& v);
	UTILS_API int EnumRegKey(HKEY key, const char* subkey, std::list<std::string>* pSubKey, std::list<std::string>* pValues);

	/*
		打印调试信息
	*/
	UTILS_API void DEBUG_INFO(char* flag, char*, ...);

	/*
		随机数c++11
	*/
	UTILS_API int Random(int start, int end);
}}
#endif
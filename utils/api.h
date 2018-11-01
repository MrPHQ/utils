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
	* @brief 重新分配内存.
	* 	重新分配内存,会保留原来的数据.
	* @param iNewLen 新分配内存大小.
	* @param pBuff 缓存区地址.
	* @param iDataLen 缓存区原有数据大小.
	* @param bDel 是否释放源缓存区.
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

	/*
		压缩相关.
	*/
#ifdef UTILS_ENABLE_ICONV
	/// 使用libiconv
	UTILS_API int CharacterConvert(const char* tocode, const char* fromcode,
		char *inbuf, int inlen, char *outbuf,
		int outlen, int* OutIdleLen, int* NoConvertLen);
#endif
	/**
	* @brief GBK字符串转 UTF-8字符串
	* @param buff. 存放结果的缓存区.
	* @param iBuffLen. 存放结果的缓存区大小.
	* @param pSrc. 源字符串
	* @param iSrcLen. 源字符串大小,包括字符串结束符 strlen(x)+1
	* @return 返回缓存区存放转换后的数据大小
	*/
	UTILS_API int GBKToUtf8(char* buff, int iBuffLen, const char* pSrc, int iSrcLen);
	/**
	* @brief UTF-8字符串 转 GBK字符串
	* @param buff. 存放结果的缓存区.
	* @param iBuffLen. 存放结果的缓存区大小.
	* @param pSrc. 源字符串
	* @param iSrcLen. 源字符串大小,包括字符串结束符 strlen(x)+1
	* @return 返回缓存区存放转换后的数据大小
	*/
	UTILS_API int Utf8ToGBK(char* buff, int iBuffLen, const char* pSrc, int iSrcLen);

	/// 转换字符串中的指定所有字符
	UTILS_API void CharConvert(char*, char s, char d);
	/// WideCharToMultiByte
	UTILS_API char* WcharToChar(wchar_t* wc);
	UTILS_API wchar_t* CharToWchar(char* c);
	/// 限制在1024Bytes
	UTILS_API void WcharToChar(wchar_t* wc, char* buff, int len);
	UTILS_API void CharToWchar(char* c, wchar_t* buff, int len);
	UTILS_API void StringReplace(const char* src, const char* from, const char* to, char* buff, int len);
	/**
	* @brief 根据分隔符查找子串
	* 	注意 ,输出的,子串有可以是空字符串, 比如 x...x ,,查找 分隔符位 'x'的子串
	* @param src. 源字符串.
	* @param delims. 分隔符.
	* @param index. 第几个分隔符.从0开始.
	* @param buff. 输出缓存区.
	* @param bufflen. 输出缓存区大小.
	* @param bMutil. 多分隔符..
	* @return None
	*/
	UTILS_API void StrTok(const char* src,
		const char* delims, 
		int index, 
		char* buff, 
		int bufflen,
		BOOL bMutil = TRUE);
	UTILS_API void StrTok(const char*,
		const char* delims, 
		std::vector<std::string>&,
		BOOL bMutil = TRUE);

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
	/**
	* @brief 根据进程名获取所有的进程ID
	* @param lpszProName. 进程名.
	* @param dwPID. 存储进程ID的缓存区. 
	* @param iSize. 存储进程ID的缓存区大小. 
	*/
	UTILS_API int GetprocessIDs(const char* lpszProName, DWORD dwPID[], int iSize);

	/**
	* @brief 运行程序
	* @param cmd. 命令行.
	* @param uiTimeOut. 超时时间. 
	* @return char*
	*/
	typedef struct _PROCESS_PROPERTY {
		uint32_t uiProcessID;
		HANDLE hProcessHandle;
	}PROCESS_PROPERTY, *PPROCESS_PROPERTY;
	UTILS_API int RunProcess(const char* cmd, PPROCESS_PROPERTY pProcessProperty = NULL, unsigned int uiTimeOut = 0, BOOL bShow=FALSE);

	/*
		文件路径相关.
	*/
	UTILS_API const char* GetCurrentPath(HINSTANCE hInstance = NULL);
	UTILS_API const char* GetParentPath(const TCHAR*);
	UTILS_API bool IsPathExists(const char*);
	UTILS_API void CreateFolders(const char*);
	UTILS_API bool DelFolders(const char*);
	UTILS_API void CreateFoldersFromFilePath(const char*);
	/**
	* @brief 枚举目录下的文件/子目录
	* @param pDir. 指定目录.
	* @param pExt. 扩展名数组. 如 szExt[2][16] = {".log",".exe"} 
	* @param iExtNum 搜索的扩展名数量
	* @param lstDirs 子目录缓存区, 如果不需要,传入 nullptr
	* @param lstFiles 文件缓存区, 如果不需要,传入 nullptr
	* @param bRecursive 是否递归搜索
	* @return char*
	*/
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

	///SHFileOperation
	// 路径拷贝
	UTILS_API bool PathCopy(const TCHAR *_pFrom, const TCHAR *_pTo);
	// 路径重命名
	UTILS_API bool PathReName(const TCHAR *_pFrom, const TCHAR *_pTo);
	// 路径删除 递归删除
	UTILS_API bool PathDelete(const TCHAR* _pFrom);
	// 路径移动 
	UTILS_API bool PathMove(const TCHAR *_pFrom, const TCHAR *_pTo);


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

	//本系统显示器信息
	struct MONITOR_PROPERTY
	{
		unsigned int index;	//序号，从0开始  -1表示无效
		RECT rect; //显示区域
		BOOL bPrimary; //主显示器标识
	};
	//根据窗口句柄获得所处显示器
	UTILS_API BOOL GetMonitorProperty(HWND hwnd, MONITOR_PROPERTY* pMonitorInfo);
	//检索主显示器的工作区大小
	UTILS_API BOOL GetMonitorWorkArea(RECT& rc);


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
	/*
	* @brief 创建/打开键，如果存在则打开, close创建后是否关闭
	* @param key. 注册表根键 [HKEY_LOCAL_MACHINE]...
	* @param subkey. 子键.
	* @param close. 打开后是否关闭,如果关闭返回值一定是NULL
	* @return [HKEY]. 操作句柄.
	*/
	UTILS_API HKEY CreateRegKey(HKEY key, const char* subkey, bool close = true);
	UTILS_API int CloseRegKey(HKEY key);
	/*
	* @brief 写/修改注册表值项
	* @param key. 操作句柄
	* @param subkey. 子键.
	* @param value. 值项
	* @param val. 值
	* @param len. 值大小
	* @return [int]. 成功返回0.否则返回非0.
	*/
	UTILS_API int WriteRegString(HKEY key, const char* subkey, const char* value, const char* val, int len);
	UTILS_API int WriteRegInt(HKEY key, const char* subkey, const char* value, int val);
	// HKEY_LOCAL_MACHINE..  section.=SOFTWARE\\InControl\\Install  Entry..= install
	UTILS_API int ReadRegString(HKEY key, const char* subkey, const char* value, char* buff, int len);
	UTILS_API int ReadRegInt(HKEY key, const char* subkey, const char* value, int& v);
	/*
	* @brief 枚举注册表[子健/值项]
	* @param key. 操作句柄
	* @param subkey. 子键.
	* @param pSubKey. 存放子健队列,可以传NULL
	* @param pValues. 存放值项队列,可以传NULL
	* @return [int]. 成功返回0.否则返回非0.
	*/
	UTILS_API int EnumRegKey(HKEY key, const char* subkey, std::list<std::string>* pSubKey, std::list<std::string>* pValues);

	/*
		打印调试信息
	*/
	UTILS_API void DEBUG_INFO(char* flag, char*, ...);
	UTILS_API void DEBUG_INFO2(const char* flag, const char* file, int line, const char* fmt, ...);

	/*
		随机数c++11
	*/
	UTILS_API int Random(int start, int end);

	/*
	@brief	IEEE754单精度浮点数格式
		4个Byte
	*/
	UTILS_API BYTE GetBit(unsigned int dwValue, int iIndex);
	UTILS_API float ByteToFloat(BYTE*);

	/*
	* @brief 读取文件版本信息
	* @param pFile 文件路径.
	* @param pVerInfo 存放文件版本信息缓存区.
	* @param pProductVerInfo 存放产品版本信息缓存区
	* @param pBuffer 版本信息缓存区. 如果不为空,则拷贝VS_FIXEDFILEINFO信息到缓存区
	* @param iBuffLen 缓存区大小
	*/
	struct VERSION_PROPERTY
	{
		DWORD dwMajorVersion;
		DWORD dwMinorVersion;
		DWORD dwBuildNumber;
		DWORD dwRevisionNumber;
	};
	UTILS_API int GetFileVersion(const char* pFile, VERSION_PROPERTY* pVerInfo, VERSION_PROPERTY* pProductVerInfo = nullptr, BYTE* pBuffer = nullptr, int iBuffLen = 0);
	struct FILE_VERSION_PROPERTY
	{
		char szFile[MAX_PATH];
		VERSION_PROPERTY stFileVersion;
		VERSION_PROPERTY stProductVersion;
	};
	/*
	* @brief 读取文件版本信息
	* 	枚举执行目录下的所有文件.并读取版本信息
	* @param pFolder 目录.
	* @param lstFileVersions 存放文件版本信息缓存区.
	* @param bRecursive 是否递归搜索
	*/
	UTILS_API int GetFileVersionForFolder(const char* pFolder, std::list<FILE_VERSION_PROPERTY>& lstFileVersions, bool bRecursive = true);


	/************************************************************************/
	/* 正则表达式                                                           */
	/************************************************************************/

	/*
	* @brief 完全匹配字符串
	* @param pStr 源字符串.
	* @param pPattern 正则表达式.
	* @return 匹配成功返回TRUE, 否则FALSE
	*/
	UTILS_API BOOL RegeFullMatch(const char* pStr, const char* pPattern);

	/************************************************************************/
	/* 网络		                                                           */
	/************************************************************************/
	enum TCP_STATE{
		TCP_STATE_CLOSED = 1,
		TCP_STATE_LISTEN = 2,
		TCP_STATE_SYN_SENT = 3,
		TCP_STATE_SYN_RCVD = 4,
		TCP_STATE_ESTAB = 5,
		TCP_STATE_FIN_WAIT1 = 6,
		TCP_STATE_FIN_WAIT2 = 7,
		TCP_STATE_CLOSE_WAIT = 8,
		TCP_STATE_CLOSING = 9,
		TCP_STATE_LAST_ACK = 10,
		TCP_STATE_TIME_WAIT = 11,
		TCP_STATE_DELETE_TCB = 12,
	};
	/*
	* @brief 获取端口的状态
	* @param port TCP端口.
	*/
	UTILS_API TCP_STATE GetTcpPortState(int port);


	/************************************************************************/
	/* 其他		                                                           */
	/************************************************************************/
	UTILS_API unsigned int ConvertIp(std::string ip);
	UTILS_API std::string ConvertIp(unsigned int ip);
}}
#endif
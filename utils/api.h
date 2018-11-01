#ifndef __UTILS_API__
#define __UTILS_API__
#include <utils/utils.h>
#include <utils\define.h>

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
	* @brief ���·����ڴ�.
	* 	���·����ڴ�,�ᱣ��ԭ��������.
	* @param iNewLen �·����ڴ��С.
	* @param pBuff ��������ַ.
	* @param iDataLen ������ԭ�����ݴ�С.
	* @param bDel �Ƿ��ͷ�Դ������.
	*/
	UTILS_API int MallocMemory(unsigned int iNewLen, BYTE*& pBuff, int iDataLen, BOOL bDel /*= TRUE*/);
	/*
		�ַ����������.
	*/
	UTILS_API int Sprintf(char* buffer, int size, const char* format, ...);
	UTILS_API int Strcpy(char* _Destination, int len, char const* _Source);
	UTILS_API int Strcmp(const char*, const char*);
	UTILS_API int Strcat(char*, int len, const char*);
	/// Ĭ��תСд
	UTILS_API int Transform(std::string&, bool tolow = true);
	UTILS_API int Transform(char*, bool tolow = true);

	/*
		ѹ�����.
	*/
#ifdef UTILS_ENABLE_ICONV
	/// ʹ��libiconv
	UTILS_API int CharacterConvert(const char* tocode, const char* fromcode,
		char *inbuf, int inlen, char *outbuf,
		int outlen, int* OutIdleLen, int* NoConvertLen);
#endif
	/**
	* @brief GBK�ַ���ת UTF-8�ַ���
	* @param buff. ��Ž���Ļ�����.
	* @param iBuffLen. ��Ž���Ļ�������С.
	* @param pSrc. Դ�ַ���
	* @param iSrcLen. Դ�ַ�����С,�����ַ��������� strlen(x)+1
	* @return ���ػ��������ת��������ݴ�С
	*/
	UTILS_API int GBKToUtf8(char* buff, int iBuffLen, const char* pSrc, int iSrcLen);
	/**
	* @brief UTF-8�ַ��� ת GBK�ַ���
	* @param buff. ��Ž���Ļ�����.
	* @param iBuffLen. ��Ž���Ļ�������С.
	* @param pSrc. Դ�ַ���
	* @param iSrcLen. Դ�ַ�����С,�����ַ��������� strlen(x)+1
	* @return ���ػ��������ת��������ݴ�С
	*/
	UTILS_API int Utf8ToGBK(char* buff, int iBuffLen, const char* pSrc, int iSrcLen);

	/// ת���ַ����е�ָ�������ַ�
	UTILS_API void CharConvert(char*, char s, char d);
	/// WideCharToMultiByte
	UTILS_API char* WcharToChar(wchar_t* wc);
	UTILS_API wchar_t* CharToWchar(char* c);
	/// ������1024Bytes
	UTILS_API void WcharToChar(wchar_t* wc, char* buff, int len);
	UTILS_API void CharToWchar(char* c, wchar_t* buff, int len);
	UTILS_API void StringReplace(const char* src, const char* from, const char* to, char* buff, int len);
	/**
	* @brief ���ݷָ��������Ӵ�
	* 	ע�� ,�����,�Ӵ��п����ǿ��ַ���, ���� x...x ,,���� �ָ���λ 'x'���Ӵ�
	* @param src. Դ�ַ���.
	* @param delims. �ָ���.
	* @param index. �ڼ����ָ���.��0��ʼ.
	* @param buff. ���������.
	* @param bufflen. �����������С.
	* @param bMutil. ��ָ���..
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
		ϵͳ���.
	*/
	UTILS_API void SleepTime(int ms);
	UTILS_API uint64_t fnGetTickCount();
	UTILS_API int SetWorkPath(const char*);
	UTILS_API int CreateGuid(char* buff, int len);
	/*
		�¼����
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
		�߳����
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
		�������..
	*/
	//��ȡ��ǰ����ID
	UTILS_API int GetCurrentProcessID();
	UTILS_API int ForceKillProcess(const char* name);
	UTILS_API BOOL ForceKillProcess(DWORD dwPID);
	/**
	* @brief ���ݽ�������ȡ���еĽ���ID
	* @param lpszProName. ������.
	* @param dwPID. �洢����ID�Ļ�����. 
	* @param iSize. �洢����ID�Ļ�������С. 
	*/
	UTILS_API int GetprocessIDs(const char* lpszProName, DWORD dwPID[], int iSize);

	/**
	* @brief ���г���
	* @param cmd. ������.
	* @param uiTimeOut. ��ʱʱ��. 
	* @return char*
	*/
	typedef struct _PROCESS_PROPERTY {
		uint32_t uiProcessID;
		HANDLE hProcessHandle;
	}PROCESS_PROPERTY, *PPROCESS_PROPERTY;
	UTILS_API int RunProcess(const char* cmd, PPROCESS_PROPERTY pProcessProperty = NULL, unsigned int uiTimeOut = 0, BOOL bShow=FALSE);

	/*
		�ļ�·�����.
	*/
	UTILS_API const char* GetCurrentPath(HINSTANCE hInstance = NULL);
	UTILS_API const char* GetParentPath(const TCHAR*);
	UTILS_API bool IsPathExists(const char*);
	UTILS_API void CreateFolders(const char*);
	UTILS_API bool DelFolders(const char*);
	UTILS_API void CreateFoldersFromFilePath(const char*);
	/**
	* @brief ö��Ŀ¼�µ��ļ�/��Ŀ¼
	* @param pDir. ָ��Ŀ¼.
	* @param pExt. ��չ������. �� szExt[2][16] = {".log",".exe"} 
	* @param iExtNum ��������չ������
	* @param lstDirs ��Ŀ¼������, �������Ҫ,���� nullptr
	* @param lstFiles �ļ�������, �������Ҫ,���� nullptr
	* @param bRecursive �Ƿ�ݹ�����
	* @return char*
	*/
	UTILS_API void EnumDirectoryFiles(const char* pDir,
		char pExt[][16],
		int iExtNum,
		std::list<std::string>* lstDirs,
		std::list<std::string>* lstFiles,
		bool bRecursive);
	/// ����Ŀ¼���ļ�������Ŀ¼���µ�·����,
	UTILS_API int CopyFolders(const char* src, const char* des, bool bRecursive = true);
	/// �滻Ŀ¼������ͬ���ļ�
	UTILS_API int ReplaceFiles(const char* file, const char* des, bool bRecursive = true);

	/// ȥ���ļ�·����չ��
	UTILS_API void RemoveExtension(char*);
	/// ȥ���ļ������õ�Ŀ¼
	UTILS_API void RemoveFileSpec(char*);
	/// ȥ��·���е�Ŀ¼����,�õ��ļ���
	UTILS_API void StripPath(char*);
	/// ����·������չ��
	UTILS_API char* FindExtension(char*);
	/// ��ȡ���·�� FILE_ATTRIBUTE_DIRECTORY FILE_ATTRIBUTE_NORMAL
	UTILS_API BOOL RelativePathTo(char* pszPath, char* pszFrom, DWORD dwAttrFrom, char* pszTo, DWORD dwAttrTo);
	/// �Ƿ������·��
	UTILS_API BOOL IsRelativePath(char* path);
	/// ת�������·��
	UTILS_API void ConverToRelative(char* from, char* to, int len);
	/// �ϲ�����·�� "C:" + "One\Two\Three" = "C:\One\Two\Three",,��ʹ�����·�� "C:\One\Two\Three" + "..\..\four" = "C:\One\four"
	UTILS_API void CombinePath(char* buff, const char* first, const char* second);
	/// ɾ���ļ�
	UTILS_API BOOL DelFile(const char*);
	/// �����ļ�
	UTILS_API BOOL FileCopy(const char*, const char*, BOOL bFailIfExists);

	/// ��ȡ�ļ���С
	UTILS_API uint64_t FileSize(const char*);

	///SHFileOperation
	// ·������
	UTILS_API bool PathCopy(const TCHAR *_pFrom, const TCHAR *_pTo);
	// ·��������
	UTILS_API bool PathReName(const TCHAR *_pFrom, const TCHAR *_pTo);
	// ·��ɾ�� �ݹ�ɾ��
	UTILS_API bool PathDelete(const TCHAR* _pFrom);
	// ·���ƶ� 
	UTILS_API bool PathMove(const TCHAR *_pFrom, const TCHAR *_pTo);


	/*
		ϵͳĿ¼���
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
	/// �Զ����ݲ���ϵͳ�汾�ж�,����'GetOSFolderPathUseCSIDL', 'GetOSFolderPathUseID'
	UTILS_API int GetOSFolderPath(int flag, char* buff, int len);
	UTILS_API int GetOSFolderPathUseCSIDL(int csidl, char* buff, int len);
	UTILS_API int GetOSFolderPathUseID(GUID id, char* buff, int len);

	//��ϵͳ��ʾ����Ϣ
	struct MONITOR_PROPERTY
	{
		unsigned int index;	//��ţ���0��ʼ  -1��ʾ��Ч
		RECT rect; //��ʾ����
		BOOL bPrimary; //����ʾ����ʶ
	};
	//���ݴ��ھ�����������ʾ��
	UTILS_API BOOL GetMonitorProperty(HWND hwnd, MONITOR_PROPERTY* pMonitorInfo);
	//��������ʾ���Ĺ�������С
	UTILS_API BOOL GetMonitorWorkArea(RECT& rc);


	/*
		ѹ�����.
	*/
#ifdef UTILS_ENABLE_ZIP
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
		ע������..
	*/
	/*
	* @brief ����/�򿪼�������������, close�������Ƿ�ر�
	* @param key. ע������ [HKEY_LOCAL_MACHINE]...
	* @param subkey. �Ӽ�.
	* @param close. �򿪺��Ƿ�ر�,����رշ���ֵһ����NULL
	* @return [HKEY]. �������.
	*/
	UTILS_API HKEY CreateRegKey(HKEY key, const char* subkey, bool close = true);
	UTILS_API int CloseRegKey(HKEY key);
	/*
	* @brief д/�޸�ע���ֵ��
	* @param key. �������
	* @param subkey. �Ӽ�.
	* @param value. ֵ��
	* @param val. ֵ
	* @param len. ֵ��С
	* @return [int]. �ɹ�����0.���򷵻ط�0.
	*/
	UTILS_API int WriteRegString(HKEY key, const char* subkey, const char* value, const char* val, int len);
	UTILS_API int WriteRegInt(HKEY key, const char* subkey, const char* value, int val);
	// HKEY_LOCAL_MACHINE..  section.=SOFTWARE\\InControl\\Install  Entry..= install
	UTILS_API int ReadRegString(HKEY key, const char* subkey, const char* value, char* buff, int len);
	UTILS_API int ReadRegInt(HKEY key, const char* subkey, const char* value, int& v);
	/*
	* @brief ö��ע���[�ӽ�/ֵ��]
	* @param key. �������
	* @param subkey. �Ӽ�.
	* @param pSubKey. ����ӽ�����,���Դ�NULL
	* @param pValues. ���ֵ�����,���Դ�NULL
	* @return [int]. �ɹ�����0.���򷵻ط�0.
	*/
	UTILS_API int EnumRegKey(HKEY key, const char* subkey, std::list<std::string>* pSubKey, std::list<std::string>* pValues);

	/*
		��ӡ������Ϣ
	*/
	UTILS_API void DEBUG_INFO(char* flag, char*, ...);
	UTILS_API void DEBUG_INFO2(const char* flag, const char* file, int line, const char* fmt, ...);

	/*
		�����c++11
	*/
	UTILS_API int Random(int start, int end);

	/*
	@brief	IEEE754�����ȸ�������ʽ
		4��Byte
	*/
	UTILS_API BYTE GetBit(unsigned int dwValue, int iIndex);
	UTILS_API float ByteToFloat(BYTE*);

	/*
	* @brief ��ȡ�ļ��汾��Ϣ
	* @param pFile �ļ�·��.
	* @param pVerInfo ����ļ��汾��Ϣ������.
	* @param pProductVerInfo ��Ų�Ʒ�汾��Ϣ������
	* @param pBuffer �汾��Ϣ������. �����Ϊ��,�򿽱�VS_FIXEDFILEINFO��Ϣ��������
	* @param iBuffLen ��������С
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
	* @brief ��ȡ�ļ��汾��Ϣ
	* 	ö��ִ��Ŀ¼�µ������ļ�.����ȡ�汾��Ϣ
	* @param pFolder Ŀ¼.
	* @param lstFileVersions ����ļ��汾��Ϣ������.
	* @param bRecursive �Ƿ�ݹ�����
	*/
	UTILS_API int GetFileVersionForFolder(const char* pFolder, std::list<FILE_VERSION_PROPERTY>& lstFileVersions, bool bRecursive = true);


	/************************************************************************/
	/* ������ʽ                                                           */
	/************************************************************************/

	/*
	* @brief ��ȫƥ���ַ���
	* @param pStr Դ�ַ���.
	* @param pPattern ������ʽ.
	* @return ƥ��ɹ�����TRUE, ����FALSE
	*/
	UTILS_API BOOL RegeFullMatch(const char* pStr, const char* pPattern);

	/************************************************************************/
	/* ����		                                                           */
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
	* @brief ��ȡ�˿ڵ�״̬
	* @param port TCP�˿�.
	*/
	UTILS_API TCP_STATE GetTcpPortState(int port);


	/************************************************************************/
	/* ����		                                                           */
	/************************************************************************/
	UTILS_API unsigned int ConvertIp(std::string ip);
	UTILS_API std::string ConvertIp(unsigned int ip);
}}
#endif
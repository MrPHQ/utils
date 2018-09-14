#include "../utils/api.h"
#include "internal.h"
#include <fstream>
#include <random>
#ifdef _WIN32
#include <io.h>
#include <algorithm>
#include <TlHelp32.h>
#include <shellapi.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <Shlobj.h>
#include <Rpc.h> //guid
#pragma comment(lib, "Rpcrt4.lib")
#endif
#pragma comment(lib, "version")
#ifdef UTILS_ENABLE_ZIP
#include <zlib\include\zlib.h>
#include <libzip\include\zip.h>
#include <libzippp\libzippp.h>
	#ifdef _WIN32
		#ifdef _DEBUG
		#pragma comment(lib, "zlib/lib/zlibd.lib")
		#pragma comment(lib, "libzip/lib/zipd.lib")
		#pragma comment(lib, "libzippp/libzipppd.lib")
		#else
		#pragma comment(lib, "zlib/lib/zlib.lib")
		#pragma comment(lib, "libzip/lib/zip.lib")
		#pragma comment(lib, "libzippp/libzippp.lib")
		#endif
	#endif
#endif

#ifdef UTILS_ENABLE_ICONV
#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "libiconv/lib/libiconv_debug.lib")
#else
#pragma comment(lib, "libiconv/lib/libiconv.lib")
#endif
#endif
#endif

#ifdef ENABLE_CRYPTOPP
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/include/cryptlib.h>
#include <cryptopp/include/filters.h>
#include <cryptopp/include/aes.h>
#include <cryptopp/include/Hex.h>     // StreamTransformationFilter  
#include <cryptopp/include/modes.h>    // CFB_Mode  
#include <cryptopp/include/md5.h>
#include <cryptopp/include/base64.h>
#include <cryptopp/include/sha.h>
#include <cryptopp/include/hmac.h>
#include <cryptopp/include/files.h>

#ifdef _DEBUG
#pragma comment(lib,"cryptopp/lib/cryptlibd.lib") 
#else
#pragma comment(lib,"cryptopp/lib/cryptlib.lib")
#endif
#endif

#include "win32/dll.h"

namespace UTILS {namespace API {

	void* Malloc(int size, bool init /*= false*/){
		void* buf = NULL;
		if (size > 0){
			buf = malloc(size);
			if (init && buf != NULL){
				memset(buf, 0, size);
			}
		}
		return buf;
	}

	void Free(void* buf){
		if (buf != NULL){
			free(buf);
		}
	}

	void* Memcpy(void* dest, const void* src, int count){
		return memcpy(dest, src, count);
	}

	void* Memset(void* dest, int c, int count, int bCatch /*=0*/){
		return memset(dest, c, count);
	}

	void* Memmove(void* dest, const void* src, int count){
		return memmove(dest, src, count);
	}

	int Memcmp(const void* dest, const void* src, int count){
		return memcmp(dest, src, count);
	}
	

	int Sprintf(char* buffer, int size, const char* format, ...) {
		if ((buffer == NULL) || (size <= 0)){
			return -1;
		}
		va_list va;
		int iRet = 0;
		va_start(va, format);
#ifdef _WIN32
		iRet = _vsnprintf_s(buffer, size, size-1, format, va);
#else
		iRet = vsnprintf(buffer, count, format, argptr);
#endif
		va_end(va);
		buffer[min(iRet, size-1)] = '\0';
		return iRet;
	}

	int Strcpy(char* _Destination, int len, char const* _Source) {
		if ((_Destination == nullptr) || (_Source == nullptr) || len <=0) {
			return -1;
		}
		int iRet = 0;
#ifdef _WIN32
		iRet = strncpy_s(_Destination, len, _Source, min((unsigned int)(len - 1), strlen(_Source)));
#else
		iRet = strncpy(_Destination, _Source, _MaxCount);
#endif
		return iRet;
	}

	UTILS_API int Strcmp(const char* s, const char* d) {
		if ((s == nullptr) || (d == nullptr)) {
			return -1;
		}
		int iRet = 0;
#ifdef _WIN32
		iRet = strcmp(s, d);
#else
		iRet = strcmp(s, d);
#endif
		return iRet;
	}

	UTILS_API int Strcat(char* s, int len, const char*d ) {
		if ((s == nullptr) || (d == nullptr)) {
			return -1;
		}
		int iRet = 0;
#ifdef _WIN32
		iRet = strncat_s(s, len, d, min((unsigned int)(len - 1 - strlen(s)), strlen(d)));
#else
		iRet = strcmp(s, d);
#endif
		return iRet;
	}

	int Transform(std::string& str, bool tolow /*= true*/) {
		if (tolow) {
			std::transform(str.begin(), str.end(), str.begin(), tolower);
		}
		else {
			std::transform(str.begin(), str.end(), str.begin(), toupper);
		}
		return 0;
	}

	int Transform(char* str, bool tolow /*= true*/) {
		if (nullptr == str) {
			return -1;
		}
		if (tolow) {
			_strlwr_s(str, strlen(str)+1);
		}
		else {
			_strupr_s(str, strlen(str) + 1);
		}
		return 0;
	}

	const char* GetCurrentPath(HINSTANCE hInstance /*= NULL*/)
	{
		static char gstrCurrentPath[MAX_PATH] = { 0 };
		DWORD dwLength = GetModuleFileName(hInstance, gstrCurrentPath, MAX_PATH);
		if (dwLength != 0 && gstrCurrentPath[0] != '\0') {
			char *p = strrchr(gstrCurrentPath, '\\');
			if (p) {
				//*(++p) = _T('\0');
				*(p) = '\0';
			}
		}
		return gstrCurrentPath;
	}

	UTILS_API const char* GetParentPath(const TCHAR* path)
	{
		if (nullptr == path){
			return nullptr;
		}
		static char gstrParentPath[MAX_PATH] = { 0 };
		Strcpy(gstrParentPath, MAX_PATH, path);
		CharConvert(gstrParentPath, '/', '\\');
		char *p = strrchr(gstrParentPath, '\\');
		if (p) {
			*(p) = '\0';
		}
		return gstrParentPath;
	}

	int CharacterConvert(const char* tocode,
		const char* fromcode,
		char *inbuf,
		int inlen,
		char *outbuf,
		int outlen,
		int* OutIdleLen,
		int* NoConvertLen)
	{
		if (tocode == NULL || fromcode == NULL || inbuf == NULL || outbuf == NULL){
			return UTILS_ERROR_PAR;
		}
#ifdef UTILS_ENABLE_ICONV
		iconv_t cd;
		char **pin = &inbuf;
		char **pout = &outbuf;
		cd = iconv_open(tocode, fromcode);
		if (cd == 0)
			return UTILS_ERROR_FAIL;
		size_t lenIn = inlen;
		size_t lenOut = outlen;
		int ret = iconv(cd, pin, &lenIn, pout, &lenOut);
		if (NoConvertLen != NULL)
			*NoConvertLen = (int)lenIn;
		if (OutIdleLen != NULL)
			*OutIdleLen = (int)lenOut;
		if (ret == -1) {
			switch (errno)
			{
			case E2BIG:
				//printf("E2BiG\n");
				break;
			case EILSEQ:
				//printf("EILSEQ\n");
				break;
			case EINVAL:
				//printf("EINVAL\n");
				break;
			default:
				//printf("errno:%d\n", errno);
				break;
			}
		}
		iconv_close(cd);
		return ret != -1 ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
#else
		if (!libiconvLoadSucc()) {
			return UTILS_ERROR_EXISTS;
		}
		iconv_t cd;
		char **pin = &inbuf;
		char **pout = &outbuf;
		cd = DLLIMPORTCALL(__libiconv, libiconv_open)(tocode, fromcode);
		if (cd == 0)
			return UTILS_ERROR_FAIL;
		size_t lenIn = inlen;
		size_t lenOut = outlen;
		int ret = DLLIMPORTCALL(__libiconv, libiconv)(cd, pin, &lenIn, pout, &lenOut);
		if (NoConvertLen != NULL)
			*NoConvertLen = (int)lenIn;
		if (OutIdleLen != NULL)
			*OutIdleLen = (int)lenOut;
		if (ret == -1) {
			switch (errno)
			{
			case E2BIG:
				//printf("E2BiG\n");
				break;
			case EILSEQ:
				//printf("EILSEQ\n");
				break;
			case EINVAL:
				//printf("EINVAL\n");
				break;
			default:
				//printf("errno:%d\n", errno);
				break;
			}
		}
		DLLIMPORTCALL(__libiconv, libiconv_close)(cd);
		return ret != -1 ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
#endif

	}

	int GetCurrentProcessID() {

#ifdef _WIN32
		return GetCurrentProcessId();
#else
#endif

	}

	int ForceKillProcess(const char* name) {
		if (nullptr == name) {
			return -1;
		}
		HANDLE hSnapshot = NULL, hProcess = NULL;
		PROCESSENTRY32	pe;
		BOOL bMore;
		DWORD dwExitCode;
		char szProName[128] , szModuleName[128];
		Sprintf(szProName, sizeof(szProName), "%s", name);
		Transform(szProName);

		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hSnapshot)
			return FALSE;
		ZeroMemory(&pe, sizeof(pe));
		pe.dwSize = (DWORD)sizeof(pe);

		bMore = Process32First(hSnapshot, &pe);
		while (bMore) {
			bMore = Process32Next(hSnapshot, &pe);
			strcpy_s(szModuleName, 128, pe.szExeFile);
			_strlwr_s(szModuleName, 128);
			if (strstr(szModuleName, szProName) != nullptr) {
				hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
				if (NULL != hProcess) {
					if (GetExitCodeProcess(hProcess, &dwExitCode)) {
						TerminateProcess(hProcess, dwExitCode);
					}
					CloseHandle(hProcess);
				}
			}
		}
		CloseHandle(hSnapshot);
		hSnapshot = NULL;
		return TRUE;
	}

	BOOL ForceKillProcess(DWORD dwPID)
	{
		DWORD dwExitCode = 0;
		SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwPID);
		if (NULL != hProcess) {
			GetExitCodeProcess(hProcess, &dwExitCode);
			TerminateProcess(hProcess, dwExitCode);
			CloseHandle(hProcess);
			hProcess = NULL;
		}
		SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
		return TRUE;
	}

	int GetprocessIDs(const char* lpszProName, DWORD dwPID[], int iSize)
	{
		if (iSize <= 0){
			return 0;
		}

		ZeroMemory(dwPID, sizeof(DWORD)*iSize);
		HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hProcessSnapshot){
			return 0;
		}

		char szProName[128] = { 0 };
		_snprintf_s(szProName, _TRUNCATE, "%s", lpszProName);
		int iCount = 0;
		PROCESSENTRY32 pe32 = { 0 };
		pe32.dwSize = sizeof(PROCESSENTRY32);
		BOOL bMorePe = ::Process32First(hProcessSnapshot, &pe32);
		while (bMorePe) {
			Transform(szProName);
			Transform(pe32.szExeFile);
			if (strcmp(szProName, pe32.szExeFile) == 0) {
				if (iCount < iSize) {
					dwPID[iCount] = pe32.th32ProcessID;
					iCount++;
				}
			}
			bMorePe = ::Process32Next(hProcessSnapshot, &pe32);
		}
		CloseHandle(hProcessSnapshot);
		return iCount;
	}

	int RunProcess(const char* cmd, PPROCESS_PROPERTY pProcessProperty /*= NULL*/, unsigned int uiTimeOut /*= 0*/, BOOL bShow /*= FALSE*/) {
		int ret = UTILS::UTILS_ERROR_SUCCESS;
		if (cmd == NULL){
			ret = UTILS::UTILS_ERROR_PAR;
			return ret;
		}
		STARTUPINFO	stStartInfo;
		PROCESS_INFORMATION	stProcessInfo = { 0 };

		Memset(&stStartInfo, 0, sizeof(STARTUPINFO));
		stStartInfo.cb = sizeof(STARTUPINFO);
		stStartInfo.dwFlags = STARTF_USESHOWWINDOW;
		stStartInfo.wShowWindow = bShow ? SW_SHOW : SW_HIDE;
		BOOL bRet = CreateProcess(NULL, (char*)cmd, NULL, NULL, FALSE, 0, NULL, NULL, &stStartInfo, &stProcessInfo);
		if (stProcessInfo.hProcess == NULL) {
			ret = UTILS::UTILS_ERROR_FAIL;
			return ret;
		}
		if (uiTimeOut > 0){
			DWORD dwTimeout = WaitForSingleObject(stProcessInfo.hProcess, uiTimeOut);
			if (dwTimeout == WAIT_TIMEOUT) {
				ret = UTILS::UTILS_ERROR_TIMEOUT;
				return ret;
			}
		}
		else {
			if (pProcessProperty != NULL) {
				pProcessProperty->hProcessHandle = stProcessInfo.hProcess;
				pProcessProperty->uiProcessID = stProcessInfo.dwProcessId;
			}
			else {
				CloseHandle(stProcessInfo.hProcess);
			}
		}
		return ret;
	}

	UTILS_API void SleepTime(int ms) {
#ifdef _WIN32
		Sleep(ms);
#else
		usleep(ms*1000);
#endif
	}

	uint64_t fnGetTickCount() {
#ifdef _WIN32
		return GetTickCount();
#else

#endif
	}

	int SetWorkPath(const char* path) {
		if (path == nullptr) {
			return -1;
		}
#ifdef _WIN32
		return SetCurrentDirectory(path);
#else

#endif
	}
	UTILS_API bool IsPathExists(const char* path){
		if (NULL == path){
			return false;
		}
#ifdef _WIN32
		if (_access(path, 0) == 0){
			return true;
		}
#else
		
#endif
		return false;
	}

#ifdef UTILS_ENABLE_ZIP
	UTILS_API bool ZipDirectory(const char* pPath, char* toFile) {
#ifdef _WIN32
		if (nullptr == pPath || nullptr == toFile) {
			return false;
		}
		std::list<std::string> dirs, files;
		EnumDirectoryFiles(pPath, nullptr, 0, &dirs, &files, true);
		libzippp::ZipArchive zipFile(toFile);
		if (!zipFile.open(libzippp::ZipArchive::WRITE)) {
			return false;
		}
		char path[MAX_PATH], tmp[MAX_PATH];
		char buff[1024],  buff2[1024];
		int iOutIdleLen = 0, iNoConvertLen = 0, error=0;
		char* p = nullptr;
		for (auto& it : dirs)
		{
			path[0] = '\0';
			//相对路径
			PathRelativePathTo(path,
				pPath,
				FILE_ATTRIBUTE_DIRECTORY,
				it.data(),
				FILE_ATTRIBUTE_DIRECTORY);
			if (PathIsRelative(path))
			{//	如: ./x/y  去掉前面的./  得到x/y
				p = (path + 2);
				Sprintf(tmp, sizeof(tmp), "%s", p);
				Strcpy(path, sizeof(path), tmp);
			}
			//在末尾加 '/'
			PathAddBackslash(path);
			CharConvert(path, '\\', '/');
			buff[0] = '\0';
			error = UTILS::API::CharacterConvert("UTF-8", "GBK", path, strlen(path), buff, 1024, &iOutIdleLen, &iNoConvertLen);
			if (error != 0) {
				MSG_INFO("[function]ZipDirectory, ERROR, LINE:%d", __LINE__);
				return false;
			}
			buff[1024 - iOutIdleLen] = '\0';
			if (!zipFile.addEntry(buff)) {
				MSG_INFO("[function]ZipDirectory, ERROR, [des]Entry:%s LINE:%d", path, __LINE__);
				return false;
			}
		}
		for (auto& it : files)
		{
			path[0] = '\0';
			//相对路径
			PathRelativePathTo(path,
				pPath,
				FILE_ATTRIBUTE_DIRECTORY,
				it.data(),
				FILE_ATTRIBUTE_NORMAL);
			if (PathIsRelative(path)){
				Sprintf(tmp, sizeof(tmp), "%s", path + 2);
				Strcpy(path, sizeof(path), tmp);
			}

			//去掉末尾的 '/'
			PathRemoveBackslash(path);
			CharConvert(path, '\\', '/');

			Strcpy(tmp, sizeof(tmp), it.data());
			CharConvert(tmp, '\\', '/');

			error = UTILS::API::CharacterConvert("UTF-8", "GBK", path, strlen(path), buff, 1024, &iOutIdleLen, &iNoConvertLen);
			if (error != 0) {
				MSG_INFO("[function]ZipDirectory, ERROR, LINE:%d", __LINE__);
				return false;
			}
			buff[1024 - iOutIdleLen] = '\0';
			error = UTILS::API::CharacterConvert("UTF-8", "GBK", tmp, strlen(tmp), buff2, 1024, &iOutIdleLen, &iNoConvertLen);
			if (error != 0) {
				MSG_INFO("[function]ZipDirectory, ERROR, LINE:%d", __LINE__);
				return false;
			}
			buff2[1024 - iOutIdleLen] = '\0';
			std::string str(buff, 1024 - iOutIdleLen);
			if (!zipFile.addFile(buff, buff2)) {
				MSG_INFO("[function]ZipDirectory, ERROR, [des]Entry:%s file:%s LINE:%d", path, tmp, __LINE__);
				return false;
			}
		}
		zipFile.close();
#else
#endif
		return true;
	}

	UTILS_API bool UnZipFile(const char* file, const char* toDirectory, bool del /*= false*/) {
		if (nullptr == file || nullptr == toDirectory) {
			return false;
		}
#ifdef _WIN32
		if (_access(toDirectory, 0) != 0) {
			return false;
		}
		char szSrcFile[1024], path[MAX_PATH], buff[1024];
		int iOutIdleLen = 0, iNoConvertLen = 0, error = 0;

		error = UTILS::API::CharacterConvert("UTF-8", "GBK", (char*)file, strlen(file), szSrcFile, 1024, &iOutIdleLen, &iNoConvertLen);
		if (error != 0) {
			MSG_INFO("[function]UnZipFile, ERROR, LINE:%d  :%s", __LINE__, file);
			return false;
		}
		szSrcFile[1024 - iOutIdleLen] = '\0';

		libzippp::ZipArchive zipFile(szSrcFile);
		if (!zipFile.open(libzippp::ZipArchive::READ_ONLY)) {
			return false;
		}
		std::vector<libzippp::ZipEntry> vEntrys = zipFile.getEntries();
		for (auto& it : vEntrys)
		{
			error = UTILS::API::CharacterConvert("GBK", "UTF-8", (char*)it.getName().data(), it.getName().length(), buff, 1024, &iOutIdleLen, &iNoConvertLen);
			if (error != 0) {
				MSG_INFO("[function]UnZipFile, ERROR, LINE:%d  :%s", __LINE__, it.getName().data());
				return false;
			}
			buff[1024 - iOutIdleLen] = '\0';

			CharConvert(buff, '/', '\\');
			Sprintf(path, MAX_PATH, "%s\\%s", toDirectory, buff);

			if (it.isDirectory()) {
				CreateDirectory(path, nullptr);
			}
			else if (it.isFile()) {

				std::ofstream ofUnzippedFile;
				ofUnzippedFile.open(path, std::ios::binary);
				if (!static_cast<bool>(ofUnzippedFile)) {
					MSG_INFO("解压.读取文件[%s].失败", path);
					return false;
				}
				if (it.getSize() > 0){

					int err = it.readContent(ofUnzippedFile);
					if (err != 0) {
						MSG_INFO("解压.读取文件[%s]..失败..", path);
						return false;
					}
				}
				ofUnzippedFile.close();
			}
		}
		zipFile.close();
		if (del) {
			zipFile.unlink(); //会删除zip文件
		}
		//
		return true;
#else
#endif
	}


	UTILS_API bool UnZipFile(const char* file, PZIP_COMMENT buff, int len, bool del /*= false*/) {
		if (nullptr == file || nullptr == buff) {
			return false;
		}
#ifdef _WIN32
		libzippp::ZipArchive zipFile(file);
		if (!zipFile.open(libzippp::ZipArchive::READ_ONLY)) {
			return false;
		}
		int iTotal = 0, num = sizeof(ZIP_COMMENT);
		PZIP_COMMENT pInfo = nullptr;
		BYTE* pComment = nullptr;
		std::vector<libzippp::ZipEntry> vEntrys = zipFile.getEntries();

		for (auto& it : vEntrys){
			iTotal += num;
			if (it.isFile()) {
				iTotal += it.getSize();
			}
		}
		if (len < iTotal) {
			return false;
		}

		for (auto& it : vEntrys){
			if (!it.isDirectory() && !it.isFile()) {
				continue;
			}

			pInfo = (PZIP_COMMENT)(buff + iTotal);
			iTotal += num;
			Strcpy(pInfo->name,sizeof(pInfo->name), it.getName().data());
			if (it.isDirectory()) {
				pInfo->entry = 0;
			}
			else if (it.isFile()) {
				pInfo->entry = 1;
				pInfo->len = it.getSize();
				if (pInfo->len > 0){
					pComment = (BYTE*)it.readAsBinary();
					if (nullptr != pComment) {
						iTotal += pInfo->len;
						memcpy(pInfo->buff, pComment, pInfo->len);
						delete[] pComment;
					}
					else {
						pInfo->len = 0;
					}
				}
			}
		}
		zipFile.close();
		if (del) {
			zipFile.unlink(); //会删除zip文件
		}
		return true;
#else
#endif
	}

#endif

	void EnumDirectoryFiles(const char* pDir,
		char pExt[][16],
		int iExtNum,
		std::list<std::string>* lstDirs,
		std::list<std::string>* lstFiles,
		bool bRecursive) {
#ifdef _WIN32
		if ((pDir == NULL) || strlen(pDir) == 0){
			MSG_INFO("err LINE:%d",__LINE__);
			return;
		}
		if (!IsPathExists(pDir)){
			MSG_INFO("err LINE:%d par:%s", __LINE__, pDir);
			return;
		}

		WIN32_FIND_DATAA FindFileData;
		BOOL IsFinded = TRUE;
		char szPath[MAX_PATH], szSubDir[MAX_PATH];
		char szDir[MAX_PATH], szFile[MAX_PATH];

		szDir[0] = '\0';
		Strcpy(szDir,sizeof(szDir), pDir);
		if (szDir[strlen(szDir) - 1] == '\\' || szDir[strlen(szDir) - 1] == '/'){
			szDir[strlen(szDir) - 1] = '\0';
		}

		Memset(&FindFileData, 0, sizeof(WIN32_FIND_DATAA));
		Sprintf(szPath, sizeof(szPath), "%s\\*.*", szDir);
		HANDLE hFile = FindFirstFile(szPath, &FindFileData);
		if (hFile == INVALID_HANDLE_VALUE) {
			MSG_INFO("err LINE:%d", __LINE__);
			return;
		}
		while (IsFinded){
			if (strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, "..")) {

				Sprintf(szFile, sizeof(szFile), "%s\\%s", szDir, FindFileData.cFileName);
				if (!PathIsDirectory(szFile)){
					if (lstFiles){
						char* p = PathFindExtension(FindFileData.cFileName);//.txt
						if (pExt != NULL && p != NULL && iExtNum > 0){
							for (int i = 0; i < iExtNum; i++){
								if (!strcmp(p, pExt[i])){
									lstFiles->push_back(szFile);
									break;
								}
							}
						}
						else {
							lstFiles->push_back(szFile);
						}
					}
				}
				else {
					if (lstDirs){
						lstDirs->push_back(szFile);
					}
					if (bRecursive){
						Sprintf(szSubDir, sizeof(szSubDir), "%s\\%s", szDir, FindFileData.cFileName);
						EnumDirectoryFiles(szSubDir, pExt, iExtNum, lstDirs, lstFiles, bRecursive);
					}
				}
			}
			IsFinded = FindNextFile(hFile, &FindFileData);
		}
		if (hFile != INVALID_HANDLE_VALUE) {
			FindClose(hFile);
		}
#else
#endif
	}

	int CopyFolders(const char* src, const char* des, bool bRecursive /*= true*/){
		if (nullptr == src || nullptr == des) {
			return UTILS_ERROR_PAR;
		}
		if (strlen(src) == 0 || strlen(des) == 0) {
			return UTILS_ERROR_PAR;
		}
		int err = UTILS_ERROR_SUCCESS;
		char tmp[MAX_PATH], relative[MAX_PATH];
		std::list<std::string> lstDirs, lstFiles;

		EnumDirectoryFiles(src, nullptr, 0, &lstDirs, &lstFiles, bRecursive);

		for (auto& it : lstDirs)
		{
			//相对路径
			UTILS::API::RelativePathTo(tmp,
				(char*)src,
				FILE_ATTRIBUTE_DIRECTORY,
				(char*)it.data(),
				FILE_ATTRIBUTE_DIRECTORY);
			relative[0] = '\0';
			if (!IsRelativePath(tmp)){
				ConverToRelative(tmp, relative, MAX_PATH);
			}
			else {
				Strcpy(relative, sizeof(relative), tmp);
			}
			//合并路径
			tmp[0] = '\0';
			UTILS::API::CombinePath(tmp, des, relative);
			if (!IsPathExists(tmp)){
				UTILS::API::CreateFolders(tmp);
			}
			if (!IsPathExists(tmp)){
				err = UTILS_ERROR_FAIL;
			}
		}
		for (auto& it : lstFiles)
		{
			//相对路径
			UTILS::API::RelativePathTo(tmp,
				(char*)src,
				FILE_ATTRIBUTE_DIRECTORY,
				(char*)it.data(),
				FILE_ATTRIBUTE_NORMAL);
			relative[0] = '\0';
			if (!IsRelativePath(tmp)) {
				ConverToRelative(tmp, relative, MAX_PATH);
			}
			else {
				Strcpy(relative, sizeof(relative), tmp);
			}

			//合并路径
			tmp[0] = '\0';
			UTILS::API::CombinePath(tmp, des, relative);
			//DEBUG_INFO("合并路径 :%s", tmp);
			if (IsPathExists(tmp)) {
				UTILS::API::DelFile(tmp);
			}
			if (!FileCopy(it.c_str(), tmp, FALSE)){
				err = UTILS_ERROR_FAIL;
				MSG_INFO("拷贝文件[%s] 到[%s] ..失败 LINE:%d", it.c_str(), tmp, __LINE__);
			}
		}
		return err;
	}

	int ReplaceFiles(const char* file, const char* des, bool bRecursive /*= true*/){
		if (nullptr == file || nullptr == des) {
			return UTILS_ERROR_PAR;
		}
		if (strlen(file) == 0 || strlen(des) == 0) {
			return UTILS_ERROR_PAR;
		}
		int err = UTILS_ERROR_SUCCESS;
		const char* p = nullptr;
		char szName[MAX_PATH];
		std::list<std::string> lstFiles;
		Strcpy(szName, MAX_PATH, file);
		StripPath(szName);
		if (strlen(szName) == 0) {
			return UTILS_ERROR_PAR;
		}

		EnumDirectoryFiles(des, nullptr, 0, nullptr, &lstFiles, bRecursive);
		for (auto& it : lstFiles){
			p = strstr(it.data(), szName);
			if (nullptr == p){
				continue;
			}
			UTILS::API::DelFile(it.data());
			if (!FileCopy(file, it.c_str(), FALSE)){
				err = UTILS_ERROR_FAIL;
				MSG_INFO("拷贝文件[%s] 到[%s] ..失败 LINE:%d", file, it.c_str(), __LINE__);
			}
		}
		return err;
	}

	void CharConvert(char* str, char s, char d) {
		if (nullptr == str) {
			return;
		}
		int len = strlen(str);
		for (int i = 0; i < len; i++){
			if (str[i] == s) {
				str[i] = d;
			}
		}
	}

	void CreateFolders(const char* path) {
		if (nullptr == path) {
			return;
		}
#ifdef _WIN32
		char tmp[MAX_PATH];
		char* p = nullptr;
		char* ptr = nullptr;
		tmp[0] = '\0';
		Strcpy(tmp, sizeof(tmp),path);
		CharConvert(tmp, '/', '\\');
		ptr = tmp;
		do
		{
			p = strchr(ptr, '\\');
			if (nullptr == p) {
				if (!IsPathExists(tmp)) {
					CreateDirectory(tmp, nullptr);
				}
				break;
			}
			else {
				*p = '\0';
				if (!IsPathExists(tmp)) {
					CreateDirectory(tmp, nullptr);
				}
				*p = '\\';
			}
			ptr = p + 1;
		} while (true);
#else

#endif
	}

	void CreateFoldersFromFilePath(const char* file) {
		if (nullptr == file) {
			return;
		}
#ifdef _WIN32
		char tmp[MAX_PATH];
		char* p = nullptr;
		char* ptr = nullptr;
		tmp[0] = '\0';
		Strcpy(tmp, sizeof(tmp), file);
		CharConvert(tmp, '/', '\\');
		RemoveFileSpec(tmp);
		ptr = tmp;
		do
		{
			p = strchr(ptr, '\\');
			if (nullptr == p) {
				if (!IsPathExists(tmp)) {
					CreateDirectory(tmp, nullptr);
				}
				break;
			}
			else {
				*p = '\0';
				if (!IsPathExists(tmp)) {
					CreateDirectory(tmp, nullptr);
				}
				*p = '\\';
			}
			ptr = p + 1;
		} while (true);
#else

#endif
	}

#ifdef ENABLE_CRYPTOPP
	int EncryptionFile(const char* src, const char* des, const char* key, const char* iv) {
		if (nullptr == src || nullptr == des || nullptr == key || nullptr == iv) {
			return UTILS_ERROR_PAR;
		}
		if (!IsPathExists(src)) {
			return UTILS_ERROR_EXISTS;
		}
		std::string strKey(key);
		std::string strKeyIV(iv);

		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
		e.SetKeyWithIV((byte*)key, 16, (byte*)iv);

		try{
			CryptoPP::FileSource(src, true, new CryptoPP::StreamTransformationFilter(e, new CryptoPP::FileSink(des)));
		}
		catch (const CryptoPP::Exception& e){
			MSG_INFO("ERROR:%d, LINE:%d", e.GetErrorType(), __LINE__);
			//std::cout << "errnr:" << e.GetErrorType() << std::endl;
			//std::cout << "error:" << e.what() << std::endl;
			return UTILS_ERROR_FAIL;
		}
		return UTILS_ERROR_SUCCESS;
	}

	UTILS_API int EncryptionFile(const char* src, char* buff, int len, const char* key, const char* iv) {
		if (nullptr == src || nullptr == buff || nullptr == key || nullptr == iv) {
			return UTILS_ERROR_PAR;
		}
		if (!IsPathExists(src)) {
			return UTILS_ERROR_EXISTS;
		}
		std::string strDes;

		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
		e.SetKeyWithIV((byte*)key, 16, (byte*)iv);

		try {
			CryptoPP::FileSource(src, true, new CryptoPP::StreamTransformationFilter(e, new CryptoPP::StringSink(strDes)));
			/*CryptoPP::FileSource(src, true,
				new CryptoPP::StreamTransformationFilter(e,
					new CryptoPP::StringSink(strDecTxt),
					CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme::ONE_AND_ZEROS_PADDING,
					true)*/

		}
		catch (const CryptoPP::Exception& e) {
			MSG_INFO("ERROR:%d, LINE:%d", e.GetErrorType(), __LINE__);
			//std::cout << "errnr:" << e.GetErrorType() << std::endl;
			//std::cout << "error:" << e.what() << std::endl;
			return UTILS_ERROR_FAIL;
		}
		if ((unsigned int)len < strDes.length()) {
			return UTILS_ERROR_BUFFER_SMALL;
		}
		Memcpy(buff, strDes.data(), strDes.length());
		return UTILS_ERROR_SUCCESS;
	}

	int DecryptionFile(const char* src, const char* des, const char* key, const char* iv) {
		if (nullptr == src || nullptr == des || nullptr == key || nullptr == iv) {
			return UTILS_ERROR_PAR;
		}
		if (!IsPathExists(src)) {
			return UTILS_ERROR_EXISTS;
		}
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption e;
		e.SetKeyWithIV((byte*)key, 16, (byte*)iv);

		try{
			CryptoPP::FileSource(src, true, new CryptoPP::StreamTransformationFilter(e, new CryptoPP::FileSink(des)));
		}
		catch (const CryptoPP::Exception& e){
			MSG_INFO("ERROR: errno:%d, error:%s, file:%s LINE:%d", e.GetErrorType(), e.GetWhat().data(), src, __LINE__);
			return UTILS_ERROR_FAIL;
		}
		return UTILS_ERROR_SUCCESS;
	}

	int DecryptionFile(const char* src, char* buff, int len, const char* key, const char* iv) {
		if (nullptr == src || nullptr == buff || nullptr == key || nullptr == iv) {
			return UTILS_ERROR_PAR;
		}
#if 0
		if (!IsPathExists(src)) {
			return UTILS_ERROR_EXISTS;
		}
		std::string strDes;
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption e;
		e.SetKeyWithIV((byte*)key, 16, (byte*)iv);

		try {
			CryptoPP::FileSource(src, true, new CryptoPP::StreamTransformationFilter(e, new CryptoPP::StringSink(strDes)));
		}
		catch (const CryptoPP::Exception& e) {
			MSG_INFO("ERROR:%d, LINE:%d", e.GetErrorType(), __LINE__);
			return UTILS_ERROR_FAIL;
		}
		if ((unsigned int)len < strDes.length()) {
			return UTILS_ERROR_BUFFER_SMALL;
		}
		Memcpy(buff, strDes.data(), strDes.length());
		return UTILS_ERROR_SUCCESS;
#else
		if (!IsPathExists(src)) {
			return UTILS_ERROR_EXISTS;
		}
		std::string strDes;
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption e;
		e.SetKeyWithIV((byte*)key, 16, (byte*)iv);

		try {
			CryptoPP::ArraySink* p = new CryptoPP::ArraySink((byte*)buff,len);
			CryptoPP::FileSource(src, true, new CryptoPP::StreamTransformationFilter(e, p));
			MSG_INFO("缓存区剩余多少字节:%d, 处理了多少字节:%d", p->AvailableSize(), p->TotalPutLength());
			
		}
		catch (const CryptoPP::Exception& e) {
			MSG_INFO("ERROR:%d, ERR:%s LINE:%d", e.GetErrorType(), e.what(), __LINE__);
			return UTILS_ERROR_FAIL;
		}
		return UTILS_ERROR_SUCCESS;
#endif
	}
	std::string FileSHA(const char* file) {
		CryptoPP::SHA256 sha256;
		std::string dst;
		try {
			CryptoPP::FileSource((const char*)file, true, new CryptoPP::HashFilter(sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(dst))));
		}
		catch (const CryptoPP::Exception& e) {
			MSG_INFO("ERROR:%d, LINE:%d", e.GetErrorType(), __LINE__);
			return dst;
		}
		return dst;
	}
	std::string StringSHA256(const char* data, int len) {
		CryptoPP::SHA256 sha256;
		std::string dst;
		try {
			CryptoPP::StringSource((const byte*)data, len, true, new CryptoPP::HashFilter(sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(dst))));
		}
		catch (const CryptoPP::Exception& e) {
			MSG_INFO("ERROR:%d, LINE:%d", e.GetErrorType(), __LINE__);
			return dst;
		}
		return dst;
	}
	std::string StringSHA1(const char* data, int len) {
		CryptoPP::SHA1 sha1;
		std::string dst;
		try {
			CryptoPP::StringSource((const byte*)data, len, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(dst))));
		}
		catch (const CryptoPP::Exception& e) {
			MSG_INFO("ERROR:%d, LINE:%d", e.GetErrorType(), __LINE__);
			return dst;
		}
		return dst;
	}
#endif

	void DEBUG_INFO(char* flag, char* fmt, ...) {
#ifdef _WIN32
		va_list args;
		char sLog[1024];
		char sOut[1024];
		sLog[0] = '\0';
		sOut[0] = '\0';
		va_start(args, fmt);
		_vsnprintf_s(sOut, sizeof(sOut)-1, fmt, args);
		va_end(args);
		if (nullptr != flag && strlen(flag) > 0) {
			strncat_s(sLog, 1024, "[", min(1023-strlen(sLog), 1));
			strncat_s(sLog, 1024, flag, min(1023-strlen(sLog), strlen(flag)));
			strncat_s(sLog, 1024, "] ", min(1023-strlen(sLog), 2));
		}
		strncat_s(sLog, 1024, sOut, min(1023-strlen(sLog), strlen(sOut)));
		OutputDebugString(sLog);
#else
#endif
	}

	void DEBUG_INFO2(const char* flag, const char* file, int line, const char* fmt, ...)
	{
#ifdef _WIN32
		va_list args;
		char sLog[1024];
		char sOut[1024];
		sLog[0] = '\0';
		sOut[0] = '\0';
		va_start(args, fmt);
		_vsnprintf_s(sOut, sizeof(sOut)-1, fmt, args);
		va_end(args);
		if (NULL != flag && strlen(flag) > 0) {
			strncat_s(sLog, _TRUNCATE, "[", min(1, sizeof(sLog)-strlen(sLog) - 1));
			strncat_s(sLog, _TRUNCATE, flag, min(strlen(flag), sizeof(sLog)-strlen(sLog) - 1));
			strncat_s(sLog, _TRUNCATE, "] ", min(2, sizeof(sLog)-strlen(sLog) - 1));
		}

		strncat_s(sLog, _TRUNCATE, sOut, min(strlen(sOut), sizeof(sLog)-strlen(sLog) - 1));

		if (NULL != file && strlen(file) > 0) {
			strncat_s(sLog, _TRUNCATE, ".[", min(2, sizeof(sLog)-strlen(sLog) - 1));
			strncat_s(sLog, _TRUNCATE, file, min(strlen(file), sizeof(sLog)-strlen(sLog) - 1));
			strncat_s(sLog, _TRUNCATE, "] ", min(2, sizeof(sLog)-strlen(sLog) - 1));
		}
		if (line >= 0) {
			char tmp[32];
			_snprintf_s(tmp, _TRUNCATE, ".[%d]", line);
			strncat_s(sLog, _TRUNCATE, tmp, min(strlen(tmp), sizeof(sLog)-strlen(sLog) - 1));
		}
		OutputDebugString(sLog);
#else
#endif
	}

	bool DelFolders(const char* folders) {
		if (folders == NULL){
			return false;
		}
		std::list<std::string> lstDirs;
		std::list<std::string> lstFiles;
		EnumDirectoryFiles(folders, NULL, 0, &lstDirs, &lstFiles, true);
		bool bFail = false;
		for (auto& it : lstFiles){
			if (!DeleteFile(it.c_str())) {
				bFail = true;
			}
		}
		auto it = lstDirs.crbegin();
		auto itEnd = lstDirs.crend();
		for (; it != itEnd; it++){
			if (!RemoveDirectory(it->c_str())) {
				bFail = true;
			}
		}
		if (IsPathExists(folders)){
			if (!RemoveDirectory(folders)) {
				bFail = true;
			}
		}
		return !bFail;
	}

	void RemoveExtension(char* path) {
#ifdef _WIN32
		if (NULL == path || strlen(path) <= 0){
			return;
		}
		PathRemoveExtension(path);
#else

#endif // _WIN32

	}

	void RemoveFileSpec(char* path) {
#ifdef _WIN32
		if (NULL == path || strlen(path) <= 0){
			return;
		}
		PathRemoveFileSpec(path);
#else

#endif // _WIN32
	}

	void StripPath(char* path) {
#ifdef _WIN32
		if (NULL == path || strlen(path) <= 0){
			return;
		}
		PathStripPath(path);
#else

#endif // _WIN32
	}

	char* FindExtension(char* path) {
#ifdef _WIN32
		if (NULL == path || strlen(path) <= 0){
			return NULL;
		}
		return PathFindExtension(path);
#else

#endif // _WIN32
	}

	uint64_t FileSize(const char* file) {
#ifdef _WIN32
		if (NULL == file) {
			return 0;
		}
		
		HANDLE hFile = nullptr;
		LARGE_INTEGER stFileSize;
		Memset(&stFileSize,0, sizeof(LARGE_INTEGER));
		hFile = CreateFile(file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == nullptr) {
			return 0;
		}
		GetFileSizeEx(hFile, &stFileSize);
		//printf("File size are %lld bytes and %4.2f KB and %4.2f MB and %4.2f GB\n", FileSize.QuadPart, (float)FileSize.QuadPart / 1024, (float)FileSize.QuadPart / (1024 * 1024), (float)FileSize.QuadPart / (1024 * 1024 * 1024));
		CloseHandle(hFile);
		return stFileSize.QuadPart;
#else

#endif // _WIN32
	}

	int MallocMemory(unsigned int iNewLen, BYTE*& pBuff, int iDataLen, BOOL bDel /*= TRUE*/) {
		if (iNewLen <= 0){
			return 0;
		}
		BYTE* tmp = NULL;
		int datalen = 0, bufflen = 0;
		if (pBuff != NULL){
			if (iDataLen > 0){
				datalen = iDataLen;
				tmp = new BYTE[datalen];
				if (tmp != NULL){
					memcpy(tmp, pBuff, datalen);
				}
			}
			if (bDel){
				delete[] pBuff;
				pBuff = NULL;
			}
		}

		bufflen = PAD_SIZE(iNewLen);
		pBuff = new BYTE[bufflen];
		if (pBuff == NULL){
			return 0;
		}
		if (datalen > 0 && tmp != NULL && pBuff != NULL){
			Memcpy(pBuff, tmp, min(datalen, bufflen));
		}
		if (tmp != NULL){
			delete[] tmp;
		}
		return bufflen;
	}

	int Random(int start, int end) {
		//static std::default_random_engine generator;
		static std::default_random_engine generator(time(0));
		static std::uniform_int_distribution<int> distribution(start, end);
		int dice_roll = distribution(generator);
		return dice_roll;
		/*
			For repeated uses, both can be bound together:

			auto dice = std::bind ( distribution, generator );
			int wisdom = dice()+dice()+dice();
		*/
	}

	BOOL RelativePathTo(char* pszPath, char* pszFrom, DWORD dwAttrFrom, char* pszTo, DWORD dwAttrTo) {
		return PathRelativePathTo(pszPath,pszFrom,FILE_ATTRIBUTE_DIRECTORY,pszTo,FILE_ATTRIBUTE_DIRECTORY);
	}

	BOOL IsRelativePath(char* path) {
		return PathIsRelative(path);
	}

	void ConverToRelative(char* from, char* to, int len) {
		char tmp[256];
		tmp[0] = '\0';
		if (nullptr == from || nullptr == to) {
			return;
		}
		Sprintf(tmp, 256, ".%s", from);
		Strcpy(to, len, tmp);
	}

	void CombinePath(char* buff, const char* first, const char* second) {
		if (NULL == buff || NULL == first || NULL == second){
			return;
		}
		PathCombine(buff, first, second);
	}

	BOOL DelFile(const char* file) {
		if (nullptr == file) {
			return FALSE;
		}
		BOOL ret = DeleteFile(file);
		if (!ret) {
			MSG_INFO("ERROR [FUNCTION]DelFile file:%s err:%d LINE:%d", file, GetLastError(), __LINE__);
		}
		return ret;
	}

	BOOL FileCopy(const char* from, const char* to, BOOL bFailIfExists) {
		if (nullptr == from || nullptr == to) {
			return FALSE;
		}
		return CopyFile(from, to, bFailIfExists);
	}

	BOOL VerifyOSVersionInfo(DWORD dwMajorVersion, DWORD dwMinorVersion, DWORD wProductType)
	{
		OSVERSIONINFOEX osvi;
		DWORDLONG dwlConditionMask = 0;
		int op = VER_EQUAL;//等于

		// Initialize the OSVERSIONINFOEX structure.

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		osvi.dwMajorVersion = dwMajorVersion;
		osvi.dwMinorVersion = dwMinorVersion;
		osvi.wProductType = (BYTE)wProductType;

		// Initialize the condition mask.

		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, (BYTE)op);
		VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, (BYTE)op);
		VER_SET_CONDITION(dwlConditionMask, VER_PRODUCT_TYPE, (BYTE)op);

		// Perform the test.

		return VerifyVersionInfo(
			&osvi,
			VER_MAJORVERSION | VER_MINORVERSION | VER_PRODUCT_TYPE,
			dwlConditionMask);
	}

	BOOL Is64() {
		SYSTEM_INFO si;
		GetNativeSystemInfo(&si);
		if ((si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) || 
			(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)) {
			return TRUE;
		}
		return FALSE;
	}
	int GetOSVersion() {
		OSVERSIONINFOEX osvi;
		SYSTEM_INFO si;

		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		//GetVersionEx((OSVERSIONINFO*)&osvi);
		GetNativeSystemInfo(&si);

		if (VerifyOSVersionInfo(5, 0, VER_NT_SERVER)) {
			return OS_VERSION_2000;
		}
		if (VerifyOSVersionInfo(5, 1, VER_NT_WORKSTATION) || 
			((VerifyOSVersionInfo(5, 2, VER_NT_WORKSTATION) && (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)))) {
			return OS_VERSION_XP;
		}
		if (VerifyOSVersionInfo(5, 2, VER_NT_SERVER)) {
			return OS_VERSION_2003;
		}
		if (VerifyOSVersionInfo(6, 0, VER_NT_WORKSTATION)) {
			return OS_VERSION_VISTA;
		}
		if ((VerifyOSVersionInfo(6, 0, VER_NT_SERVER)) || 
			(VerifyOSVersionInfo(6, 1, VER_NT_SERVER))) {
			return OS_VERSION_2008;
		}
		if (VerifyOSVersionInfo(6, 1, VER_NT_WORKSTATION)) {
			return OS_VERSION_WIN7;
		}
		if ((VerifyOSVersionInfo(6, 2, VER_NT_SERVER)) || 
			(VerifyOSVersionInfo(6, 3, VER_NT_SERVER))) {
			return OS_VERSION_2012;
		}
		if ((VerifyOSVersionInfo(6, 2, VER_NT_WORKSTATION)) ||
			(VerifyOSVersionInfo(6, 3, VER_NT_WORKSTATION))) {
			return OS_VERSION_WIN8;
		}
		if (VerifyOSVersionInfo(10, 0, VER_NT_WORKSTATION)) {
			return OS_VERSION_WIN10;
		}
		if (VerifyOSVersionInfo(10, 0, VER_NT_SERVER)) {
			return OS_VERSION_2016;
		}
#if 0
		bIsWindowsXPorLater =((osvi.dwMajorVersion > 5) ||
			((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1)));
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0){
			return OS_VERSION_2000;
		}
		if ((osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) || 
			(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 && 
			osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)) {
			return OS_VERSION_XP;
		}
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
			if ((osvi.wSuiteMask & VER_SUITE_WH_SERVER) != VER_SUITE_WH_SERVER) {
				//GetSystemMetrics(SM_SERVERR2) == 0
				//GetSystemMetrics(SM_SERVERR2) != 0 2003 R2
				return OS_VERSION_2003;
			}
		}
		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION) {
			return OS_VERSION_VISTA;
		}
		if ((osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType != VER_NT_WORKSTATION) || 
			(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && osvi.wProductType != VER_NT_WORKSTATION)) {
			return OS_VERSION_2008;
		}
		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && osvi.wProductType == VER_NT_WORKSTATION) {
			return OS_VERSION_WIN7;
		}
		if ((osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && osvi.wProductType != VER_NT_WORKSTATION) || 
			(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 && osvi.wProductType != VER_NT_WORKSTATION)) {
			return OS_VERSION_2012;
		}
		if ((osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && osvi.wProductType == VER_NT_WORKSTATION) || 
			(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 && osvi.wProductType == VER_NT_WORKSTATION)) {
			return OS_VERSION_WIN8;
		}
		if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION) {
			return OS_VERSION_WIN10;
		}
		if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.wProductType != VER_NT_WORKSTATION) {
			return OS_VERSION_2016;
		}
#endif
		return OS_VERSION_NONE;
	}

	int GetOSFolderPath(int flag, char* buff, int len) {
		BOOL b64 = Is64();
		int os = GetOSVersion();
		if (os < OS_VERSION_VISTA) {
			int csidl = 0;
			// CSIDL_PERSONAL | CSIDL_FLAG_CREATE
			switch (flag)
			{
			case FOLDER_ADMINTOOLS:csidl = CSIDL_ADMINTOOLS;break;
			case FOLDER_APPDATA:csidl = CSIDL_APPDATA; break;
			case FOLDER_COMMON_ADMINTOOLS:csidl = CSIDL_COMMON_ADMINTOOLS; break;
			case FOLDER_COMMON_APPDATA:csidl = CSIDL_COMMON_APPDATA; break;
			case FOLDER_COMMON_DOCUMENTS:csidl = CSIDL_COMMON_DOCUMENTS; break;
			case FOLDER_COOKIES:csidl = CSIDL_COOKIES; break;
			case FOLDER_FLAG_CREATE:csidl = CSIDL_FLAG_CREATE; break;
			case FOLDER_FLAG_DONT_VERIFY:csidl = CSIDL_FLAG_DONT_VERIFY; break;
			case FOLDER_HISTORY:csidl = CSIDL_HISTORY; break;
			case FOLDER_INTERNET_CACHE:csidl = CSIDL_INTERNET_CACHE; break;
			case FOLDER_LOCAL_APPDATA:csidl = CSIDL_LOCAL_APPDATA; break;
			case FOLDER_MYPICTURES:csidl = CSIDL_MYPICTURES; break;
			case FOLDER_PERSONAL:csidl = CSIDL_PERSONAL; break;
			case FOLDER_PROGRAM_FILES:csidl = CSIDL_PROGRAM_FILES; break;
			case FOLDER_PROGRAM_FILES_COMMON:csidl = CSIDL_PROGRAM_FILES_COMMON; break;
			case FOLDER_SYSTEM:csidl = CSIDL_SYSTEM; break;
			case FOLDER_WINDOWS:csidl = CSIDL_WINDOWS; break;
			default:
				return UTILS_ERROR_FAIL;
			}
			return GetOSFolderPathUseCSIDL(csidl, buff, len);
		}
		else {
			GUID _KNOWNFOLDERID;
			// CSIDL_PERSONAL | CSIDL_FLAG_CREATE
			switch (flag)
			{
			case FOLDER_ADMINTOOLS:_KNOWNFOLDERID = FOLDERID_AdminTools; break;
			case FOLDER_APPDATA:_KNOWNFOLDERID = FOLDERID_RoamingAppData; break;
			case FOLDER_COMMON_ADMINTOOLS:_KNOWNFOLDERID = FOLDERID_CommonAdminTools; break;
			case FOLDER_COMMON_APPDATA:_KNOWNFOLDERID = FOLDERID_ProgramData; break;
			case FOLDER_COMMON_DOCUMENTS:_KNOWNFOLDERID = FOLDERID_PublicDocuments; break;
			case FOLDER_COOKIES:_KNOWNFOLDERID = FOLDERID_Cookies; break;
			case FOLDER_FLAG_CREATE:return UTILS_ERROR_FAIL;
			case FOLDER_FLAG_DONT_VERIFY:return UTILS_ERROR_FAIL;
			case FOLDER_HISTORY:_KNOWNFOLDERID = FOLDERID_History; break;
			case FOLDER_INTERNET_CACHE:_KNOWNFOLDERID = FOLDERID_InternetCache; break;
			case FOLDER_LOCAL_APPDATA:_KNOWNFOLDERID = FOLDERID_LocalAppData; break;
			case FOLDER_MYPICTURES:_KNOWNFOLDERID = FOLDERID_Pictures; break;
			case FOLDER_PERSONAL:_KNOWNFOLDERID = FOLDERID_Documents; break;
			case FOLDER_PROGRAM_FILES:_KNOWNFOLDERID = FOLDERID_ProgramFiles; break;//FOLDERID_ProgramFilesX64 ..FOLDERID_ProgramFilesX86
			case FOLDER_PROGRAM_FILES_COMMON:_KNOWNFOLDERID = FOLDERID_ProgramFilesCommon; break;
			case FOLDER_SYSTEM:_KNOWNFOLDERID = FOLDERID_System; break;//FOLDERID_SystemX86
			case FOLDER_WINDOWS:_KNOWNFOLDERID = FOLDERID_Windows; break;
			default:
				return UTILS_ERROR_FAIL;
			}
			return GetOSFolderPathUseID(_KNOWNFOLDERID, buff, len);
		}
	}
	int GetOSFolderPathUseCSIDL(int csidl, char* buff, int len) {
		if (buff == nullptr) {
			return UTILS_ERROR_PAR;
		}
		char szPath[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, csidl, NULL, 0, szPath))) {

			Strcpy(buff,sizeof(buff),szPath);
			return 0;
		}
		return UTILS_ERROR_FAIL;
	}

	int GetOSFolderPathUseID(GUID id, char* buff, int len) {
		char szPath[MAX_PATH];
		wchar_t* p = nullptr;
		if (SUCCEEDED(SHGetKnownFolderPath(id, 0, NULL, &p))) {
			if (nullptr != p) {
				szPath[0] = '\0';
				WcharToChar(p, szPath, MAX_PATH);
				Strcpy(buff,sizeof(buff), szPath);
			}
			return 0;
		}
		return UTILS_ERROR_FAIL;
	}

	char* WcharToChar(wchar_t* wc){
		int len = WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), NULL, 0, NULL, NULL);
		char* p = new char[len + 1];
		if (nullptr == p) {
			return nullptr;
		}
		WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), p, len, NULL, NULL);
		p[len] = '\0';
		return p;
	}
	wchar_t* CharToWchar(char* c){
		int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
		wchar_t* p = new wchar_t[len + 1];
		if (nullptr == p) {
			return nullptr;
		}
		MultiByteToWideChar(CP_ACP, 0, c, strlen(c), p, len);
		p[len] = '\0';
		return p;
	}
	void WcharToChar(wchar_t* wc, char* buff, int len){
		char _buff[1024];
		int _len = WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), NULL, 0, NULL, NULL);
		if (_len >= 1024) {
			return;
		}
		WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), _buff, 1024, NULL, NULL);
		_buff[_len] = '\0';
		Strcpy(buff, sizeof(buff),_buff);
	}

	void CharToWchar(char* c, wchar_t* buff, int len){
		wchar_t _buff[1024];
		int _len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
		if (_len >= 1024) {
			return;
		}
		MultiByteToWideChar(CP_ACP, 0, c, strlen(c), _buff, 1024);
		_buff[_len] = '\0';
		wcsncpy_s(buff, 1024, _buff, min((unsigned int)len, wcslen(_buff)));
	}

	HKEY CreateRegKey(HKEY hKey, const char* subkey, bool close /*= true*/) {
		HKEY hAppKey = NULL;
		DWORD dw = 0;
		REGSAM samDesiredOpen = KEY_ALL_ACCESS;

		if (nullptr == subkey) {
			return NULL;
		}
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
		}
		if (RegCreateKeyEx(hKey, subkey, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, samDesiredOpen, NULL,&hAppKey, &dw) == ERROR_SUCCESS)
		{
			if (close) {
				RegCloseKey(hAppKey);
				hAppKey = NULL;
			}
			return hAppKey;
			//dw =.
			//REG_CREATED_NEW_KEY，0x00000001L 该键是新创建的键
			//REG_OPENED_EXISTING_KEY，0x00000002L 该键是已经存在的键
		}
		return NULL;
	}
	int WriteRegString(HKEY hKey, const char* subkey, const char* value, const char* val, int len) {
		if (nullptr == value) {
			return UTILS_ERROR_PAR;
		}
		bool bClose = false;
		HKEY _hKey = NULL;
		REGSAM samDesiredOpen = KEY_WRITE;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
		}
		if (subkey != NULL) {
			if (RegOpenKeyEx(hKey, subkey, 0, samDesiredOpen, &_hKey) != ERROR_SUCCESS) {
				return UTILS_ERROR_FAIL;
			}
			bClose = true;
		}
		else {
			_hKey = hKey;
		}
		if (_hKey == NULL) {
			return UTILS_ERROR_FAIL;
		}
		int lRetCode = RegSetValueEx(_hKey, value,0,REG_SZ,(BYTE *)val,len);
		if (bClose) {
			RegCloseKey(_hKey);
		}
		return lRetCode == ERROR_SUCCESS ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
	}

	int CloseRegKey(HKEY hKey) {
		if (hKey != NULL) {
			RegCloseKey(hKey);
		}
		return 0;
	}
	int WriteRegInt(HKEY hKey, const char* subkey, const char* value, int val) {
		HKEY _hKey = NULL;
		if (nullptr == value) {
			return UTILS_ERROR_PAR;
		}
		REGSAM samDesiredOpen = KEY_WRITE;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
		}
		bool bClose = false;
		if (subkey != NULL) {
			if (RegOpenKeyEx(hKey, subkey, 0, samDesiredOpen, &_hKey) != ERROR_SUCCESS) {
				return UTILS_ERROR_FAIL;
			}
			bClose = true;
		}
		else {
			_hKey = hKey;
		}
		if (_hKey == NULL) {
			return UTILS_ERROR_FAIL;
		}
		int lRetCode = RegSetValueEx(_hKey, value, 0, REG_DWORD, (BYTE *)&val, sizeof(int));
		if (bClose) {
			RegCloseKey(_hKey);
		}
		return lRetCode == ERROR_SUCCESS ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
	}

	int ReadRegString(HKEY hKey, const char* subkey, const char* value, char* buff, int len) {
		HKEY _hKey = NULL;
		DWORD DataType = REG_SZ, BuffLen = 1024;
		char str[1024];
		bool bClose = false;
		if ((nullptr == value) || (nullptr == buff)) {
			return UTILS_ERROR_PAR;
		}

		REGSAM samDesiredOpen = KEY_READ;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
		}

		if (subkey != NULL) {
			if (RegOpenKeyEx(hKey, subkey, 0, samDesiredOpen, &_hKey) != ERROR_SUCCESS) {
				return UTILS_ERROR_FAIL;
			}
			bClose = true;
		}
		else {
			_hKey = hKey;
		}
		if (_hKey == NULL){
			return UTILS_ERROR_FAIL;
		}

		str[0] = '\0';
		//RegQueryValueEx(_hKey, value, 0, &DataType, NULL, &BuffLen);在不知道长度的时候,可以先读取需要长度.
		int lRetCode = RegQueryValueEx(_hKey, value, 0, &DataType, (BYTE*)&str, &BuffLen);
		if (lRetCode == ERROR_SUCCESS) {
			Strcpy(buff, len, str);
		}
		if (bClose) {
			RegCloseKey(_hKey);
		}
		return lRetCode == ERROR_SUCCESS ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
	}

	int ReadRegInt(HKEY hKey, const char* subkey, const char* value, int& v) {
		HKEY _hKey = NULL;
		DWORD DataType = REG_DWORD, BuffLen = 4;
		bool bClose = false;
		if (nullptr == value) {
			return UTILS_ERROR_PAR;
		}
		REGSAM samDesiredOpen = KEY_READ;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
			//samDesiredOpen |= KEY_WOW64_64KEY; 64.app
		}

		if (subkey != NULL) {
			if (RegOpenKeyEx(hKey, subkey, 0, samDesiredOpen, &_hKey) != ERROR_SUCCESS) {
				return UTILS_ERROR_FAIL;
			}
			bClose = true;
		}
		else {
			_hKey = hKey;
		}

		if (_hKey == NULL){
			return UTILS_ERROR_FAIL;
		}

		int lRetCode = RegQueryValueEx(_hKey, value, 0, &DataType, (BYTE*)&v, &BuffLen);
		if (bClose) {
			RegCloseKey(_hKey);
		}
		return lRetCode == ERROR_SUCCESS ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
	}

	int EnumRegKey(HKEY key, const char* subkey, std::list<std::string>* pSubKey, 
		std::list<std::string>* pValues) {
		char achKey[256];   // buffer for subkey name
		DWORD cbName; // size of name string 
		char achClass[MAX_PATH] = TEXT("");  // buffer for class name 
		DWORD cchClassName = MAX_PATH;  // size of class string 
		DWORD cSubKeys = 0; // number of subkeys 
		DWORD cbMaxSubKey; // longest subkey size 
		DWORD cchMaxClass; // longest class string 
		DWORD cValues; // number of values for key 
		DWORD cchMaxValue; // longest value name 
		DWORD cbMaxValueData; // longest value data 
		DWORD cbSecurityDescriptor; // size of security descriptor 
		FILETIME ftLastWriteTime; // last write time 

		DWORD i, retCode;

		char achValue[256];
		DWORD cchValue = 256;

		HKEY _hKey = NULL;
		REGSAM samDesiredOpen = KEY_READ;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
			//samDesiredOpen |= KEY_WOW64_64KEY; 64.app
		}

		bool bClose = false;
		if (subkey != NULL) {
			if (RegOpenKeyEx(key, subkey, 0, samDesiredOpen, &_hKey) != ERROR_SUCCESS) {
				return UTILS_ERROR_FAIL;
			}
			bClose = true;
		}
		else {
			_hKey = key;
		}

		if (_hKey == NULL){
			return UTILS_ERROR_FAIL;
		}

		// Get the class name and the value count. 
		retCode = RegQueryInfoKey(
			_hKey, // key handle 
			achClass, // buffer for class name 
			&cchClassName, // size of class string 
			NULL, // reserved 
			&cSubKeys, // number of subkeys 
			&cbMaxSubKey, // longest subkey size 
			&cchMaxClass, // longest class string 
			&cValues, // number of values for this key 
			&cchMaxValue, // longest value name 
			&cbMaxValueData, // longest value data 
			&cbSecurityDescriptor, // security descriptor 
			&ftLastWriteTime); // last write time 
		if (retCode != ERROR_SUCCESS) {
			return UTILS_ERROR_FAIL;
		}
		// Enumerate the subkeys, until RegEnumKeyEx fails.
		if (cSubKeys){
			//printf("\nNumber of subkeys: %d\n", cSubKeys);

			if (pSubKey != nullptr) {

				for (i = 0; i < cSubKeys; i++)
				{
					cbName = 256;
					retCode = RegEnumKeyEx(_hKey, i,
						achKey,
						&cbName,
						NULL,
						NULL,
						NULL,
						&ftLastWriteTime);
					if (retCode == ERROR_SUCCESS)
					{
						pSubKey->emplace_back(achKey);
						//_tprintf(TEXT("(%d) %s\n"), i + 1, achKey);
					}
				}
			}
		}

		// Enumerate the key values. 

		if (cValues)
		{
			//printf("\nNumber of values: %d\n", cValues);

			if (nullptr != pValues) {
				for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
				{
					cchValue = 256;
					achValue[0] = '\0';
					retCode = RegEnumValue(_hKey, i,
						achValue,
						&cchValue,
						NULL,
						NULL,
						NULL,
						NULL);

					if (retCode == ERROR_SUCCESS)
					{
						pValues->emplace_back(achValue);
						//_tprintf(TEXT("(%d) %s\n"), i + 1, achValue);
					}
				}
			}
		}
		return UTILS_ERROR_SUCCESS;
	}
	HANDLE fnCreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset,BOOL bInitialState,const char* lpName){
		return CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
	}

	BOOL fnCloseHandle(HANDLE hObject){
		return CloseHandle(hObject);
	}

	BOOL fnSetEvent(HANDLE hEvent){
		return SetEvent(hEvent);
	}

	BOOL fnResetEvent(HANDLE hEvent){
		return ResetEvent(hEvent);
	}

	uint32_t fnWaitForSingleObject(
		HANDLE hHandle,
		uint32_t dwMilliseconds){
		return ::WaitForSingleObject(hHandle, dwMilliseconds);
	}

	uint32_t fnWaitForMultipleObjects(int iCount,
		HANDLE *lpHandles,
		BOOL bWaitAll,
		uint32_t dwMilliseconds){
		return ::WaitForMultipleObjects(iCount, lpHandles, bWaitAll, dwMilliseconds);
	}

	HTHREAD fnCreateThread(THREAD_RET(CALLBACK* lpStartAddress)(void* lpParameter),
		void* lpParameter,
		uint32_t* lpThreadId){
#ifdef _WIN32
		return CreateThread(NULL, 0, lpStartAddress, lpParameter, 0, (unsigned long*)lpThreadId);
#else
		int			iRet;
		HTHREAD			hObject = 0;
		pthread_attr_t	attr, *pattr = &attr;

		pthread_attr_init(&attr);
		//pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		//因为你的线程不便于等待的关系，设置为分离线程吧    
		iRet = pthread_create(&hObject, pattr, lpStartAddress, lpParameter);
		if (iRet != 0){
			hObject = 0;
		}

		if (pattr != NULL){
			pthread_attr_destroy(pattr);
		}
#endif
	}

	BOOL fnGetExitCodeThread(HTHREAD hThread, uint32_t* lpExitCode){
#ifdef _WIN32
		return GetExitCodeThread(hThread, (unsigned long*)lpExitCode);
#else
		return TRUE;
#endif
		
	}

	BOOL fnTerminateThread(HTHREAD hThread, uint32_t dwExitCode){
#ifdef _WIN32
		return TerminateThread(hThread, dwExitCode);
#else
		if (::pthread_cancel(hThread) == 0){
			return TRUE;
		}
		return FALSE;
#endif
	}

	BOOL fnCloseThread(HTHREAD hThread){
#ifdef _WIN32
		return CloseHandle(hThread);
#else
		return TRUE;
#endif
	}

	uint32_t fnWaitForThreadExit(HTHREAD hHandle, uint32_t dwMilliseconds){
#ifdef _WIN32
		return WaitForSingleObject(hHandle, dwMilliseconds);
#else
		void* retval = NULL;
		return pthread_join(hHandle, (void**)&retval);
#endif
	}

	void fnExitThread(uint32_t dwExitCode){
#ifdef _WIN32
		ExitThread(dwExitCode);
#else
		pthread_exit((void*)dwExitCode);
#endif
	}

	BOOL fnSetThreadPriority(int iPriority){
#ifdef _WIN32
		::SetThreadPriority(GetCurrentThread(), iPriority);
		return TRUE;
#else
		struct sched_param param;
		param.sched_priority = iPriority;
		sched_setscheduler(0, SCHED_FIFO, &param);
		return TRUE;
#endif
	}

	uint32_t fnGetThreadId(HTHREAD hThread){
#ifdef _WIN32
		return (uint32_t)::GetCurrentThreadId();
#else
		return (uint32_t)hThread;
#endif
	}

	int CreateGuid(char* buff, int len) {
		if (nullptr == buff) {
			return UTILS_ERROR_PAR;
		}
#ifdef _WIN32
		UUID guid = { 0 };
		UuidCreate(&guid);
		Sprintf(buff, len, "%x-%x-%x-%x%x-%x%x%x%x%x%x",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		return 0;
#else
		return 0;
#endif

	}

	BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
	{
		std::map<UINT, MONITOR_PROPERTY>* plstMonitorInfo = (std::map<UINT, MONITOR_PROPERTY>*) dwData;
		MONITOR_PROPERTY info = { 0 };
		MONITORINFO stMonitorInfo = { 0 };
		stMonitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &stMonitorInfo);
		info.rect = stMonitorInfo.rcMonitor;
		info.index = plstMonitorInfo->size();
		if (MONITORINFOF_PRIMARY == stMonitorInfo.dwFlags){
			info.bPrimary = TRUE;
		}
		plstMonitorInfo->insert(std::map<UINT, MONITOR_PROPERTY>::value_type(info.index, info));
		return TRUE;
	}

	BOOL GetMonitorProperty(HWND hwnd, MONITOR_PROPERTY* pMonitorInfo)
	{
		if (NULL == pMonitorInfo){
			return FALSE;
		}

		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		if (NULL != hMonitor) {
			MONITORINFO stMonitorInfo = { 0 };
			stMonitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(hMonitor, &stMonitorInfo);
			pMonitorInfo->index = -1;
			pMonitorInfo->rect = stMonitorInfo.rcMonitor;
			pMonitorInfo->bPrimary = (MONITORINFOF_PRIMARY == stMonitorInfo.dwFlags);
			std::map<int, MONITOR_PROPERTY> lstMonitorInfo;
			lstMonitorInfo.clear();
			EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&lstMonitorInfo);
			int i = 0;
			bool bFind = false;
			std::map<int, MONITOR_PROPERTY>::iterator var = lstMonitorInfo.begin();
			for (; var != lstMonitorInfo.end(); var++, i++) {
				if (EqualRect(&var->second.rect, &pMonitorInfo->rect)) {
					bFind = true;
					break;
				}
			}
			if (bFind) {
				pMonitorInfo->index = i;
			}
		}
		else {
			pMonitorInfo->index = -1;
			pMonitorInfo->rect.left = 0;
			pMonitorInfo->rect.top = 0;
			pMonitorInfo->rect.right = GetSystemMetrics(SM_CXSCREEN);
			pMonitorInfo->rect.bottom = GetSystemMetrics(SM_CYSCREEN);
			pMonitorInfo->bPrimary = TRUE;
		}
		return TRUE;
	}

	BOOL GetMonitorWorkArea(RECT& rc){
		return SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rc, 0);
	}

	void StringReplace(const char* src, const char* from, const char* to, char* buff, int len)
	{
		if (nullptr == src || nullptr == from || nullptr == to || nullptr == buff || len <= 0){
			return;
		}
		int toLen = strlen(to);
		std::vector<std::string> v;
		StrTok(src, from, v, FALSE);
		buff[0] = '\0';

		int iCnt = v.size();
		for (int i = 0; i < iCnt; i++)
		{
			strncat_s(buff, len, v.at(i).data(), min(len - 1 - strlen(buff), v.at(i).length()));
			if (i + 1 < iCnt){
				strncat_s(buff, len, to, min((int)(len - 1 - strlen(buff)), toLen));
			}
		}
	}

	void StrTok(const char* src,const char* delims, int index,
		char* buff, int bufflen, BOOL bMutil /*= TRUE*/)
	{
		const int ciBuffLen = 1024;
		int iBuffLen = ciBuffLen, iProIndex = 0;
		char szDelims[64], szSrc[ciBuffLen];
		char* pSrc = NULL;
		char* pDelims = szDelims;
		char *strToken = nullptr;
		char *next_token = nullptr;
		if (delims == NULL || src == NULL || buff == NULL || bufflen <= 0){
			return;
		}
		_snprintf_s(szDelims, _TRUNCATE, "%s", delims);
		if (strlen(src) > ciBuffLen){
			iBuffLen = PAD_SIZE(strlen(src));
			pSrc = new char[iBuffLen];
			if (pSrc == NULL){
				return;
			}
		}
		else {
			pSrc = szSrc;
		}
		strncpy_s(pSrc, iBuffLen, src, min(iBuffLen - 1, (int)strlen(src)));

		if (bMutil){
			strToken = strtok_s(pSrc, szDelims, &next_token);//linux - strtok_r
			if (strToken == NULL){
				if (iBuffLen > ciBuffLen){
					delete[] pSrc;
				}
				return;
			}
			while (strToken != NULL) {
				strncpy_s(buff, bufflen, strToken, min(bufflen - 1, (int)strlen(strToken)));
				if (iProIndex == index){
					break;
				}
				iProIndex++;
				strToken = strtok_s(nullptr, szDelims, &next_token);
			}
		}
		else {
			int len = 0, iIdx = 0;
			bool bFind = false;
			strToken = pSrc;
			next_token = strstr(strToken, pDelims);
			while (next_token != NULL) {
				len = next_token - strToken;
				if (iIdx == index){
					strncpy_s(buff, bufflen, strToken, min(bufflen - 1, len));
					bFind = true;
					break;
				}
				iIdx++;
				strToken = next_token + strlen(pDelims);
				next_token = strstr(strToken, pDelims);
			}
			if (!bFind && strlen(strToken) > 0){
				if (iIdx == index){
					strncpy_s(buff, bufflen, strToken, min((int)(bufflen - 1), (int)strlen(strToken)));
				}
			}
		}

		if (iBuffLen > ciBuffLen){
			delete[] pSrc;
		}
	}

	void StrTok(const char* src, const char* delims,
		std::vector<std::string>& vStr, BOOL bMutil /*= TRUE*/)
	{
		const int ciBuffLen = 1024;
		int iBuffLen = ciBuffLen;
		char szDelims[64], szSrc[ciBuffLen];
		char* pSrc = NULL;
		char* pDelims = szDelims;
		char *strToken = nullptr;
		char *next_token = nullptr;
		if (delims == NULL || src == NULL){
			return;
		}
		_snprintf_s(szDelims, _TRUNCATE, "%s", delims);
		if (strlen(src) > ciBuffLen){
			iBuffLen = PAD_SIZE(strlen(src));
			pSrc = new char[iBuffLen];
			if (pSrc == NULL){
				return;
			}
		}
		else {
			pSrc = szSrc;
		}
		strncpy_s(pSrc, iBuffLen, src, min(iBuffLen - 1, (int)strlen(src)));

		if (bMutil){
			strToken = strtok_s(pSrc, szDelims, &next_token);
			if (strToken == NULL){
				if (iBuffLen > ciBuffLen){
					delete[] pSrc;
				}
				return;
			}
			while (strToken != NULL) {
				vStr.emplace_back(strToken);
				/*if (strlen(strToken) > 0){
					vStr.emplace_back(strToken);
				}*/
				strToken = strtok_s(nullptr, szDelims, &next_token);
			}
		}
		else {
			int len = 0;
			strToken = pSrc;
			next_token = strstr(strToken, pDelims);
			while (next_token != NULL) {
				len = next_token - strToken;
				vStr.emplace_back(strToken, len);
				strToken = next_token + strlen(pDelims);
				next_token = strstr(strToken, pDelims);
			}
			if (strToken != nullptr/*strlen(strToken) > 0*/){
				vStr.emplace_back(strToken);
			}
		}

		if (iBuffLen > ciBuffLen){
			delete[] pSrc;
		}
	}

	bool PathCopy(const TCHAR *_pFrom, const TCHAR *_pTo)
	{
		TCHAR szTo[MAX_PATH] = { 0 }, szFrom[MAX_PATH] = { 0 };
#if 1 //路径后2个\0\0
		Memcpy(szTo, _pTo, lstrlen(_pTo));
		Memcpy(szFrom, _pFrom, lstrlen(_pFrom));
#else
		Strcpy(szTo, MAX_PATH, _pTo);
		Strcpy(szFrom, MAX_PATH, _pFrom);
#endif

		SHFILEOPSTRUCT FileOp = { 0 };
		FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
		FileOp.pFrom = szFrom;
		FileOp.pTo = szTo;
		FileOp.wFunc = FO_COPY;
		return SHFileOperation(&FileOp) == 0;
	}

	bool PathReName(const TCHAR *_pFrom, const TCHAR *_pTo)
	{
		TCHAR szTo[MAX_PATH] = { 0 }, szFrom[MAX_PATH] = { 0 };
#if 1 //路径后2个\0\0
		Memcpy(szTo, _pTo, lstrlen(_pTo));
		Memcpy(szFrom, _pFrom, lstrlen(_pFrom));
#else
		Strcpy(szTo, MAX_PATH, _pTo);
		Strcpy(szFrom, MAX_PATH, _pFrom);
#endif
		SHFILEOPSTRUCT FileOp = { 0 };
		FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
		FileOp.pFrom = szFrom;
		FileOp.pTo = szTo;
		FileOp.wFunc = FO_RENAME;
		return SHFileOperation(&FileOp) == 0;
	}

	bool PathDelete(const TCHAR* _pFrom)
	{
		//SHFileOperation将永久删除文件，除非您在由lpFileOp指向的SHFILEOPSTRUCT结构的fFlags成员中设置FOF_ALLOWUNDO标志
		// 删除是递归的，除非您在lpFileOp中设置FOF_NORECURSION标志
		TCHAR szFrom[MAX_PATH] = { 0 };
		Memcpy(szFrom, _pFrom, lstrlen(_pFrom));

		SHFILEOPSTRUCT FileOp = { 0 };
		FileOp.pFrom = szFrom;
		FileOp.pTo = NULL;//一定要是NULL
		FileOp.fFlags = FOF_ALLOWUNDO|FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;;
		FileOp.wFunc = FO_DELETE; //删除操作
		return SHFileOperation(&FileOp) == 0;
	}

	bool PathMove(const TCHAR *_pFrom, const TCHAR *_pTo)
	{
		TCHAR szTo[MAX_PATH] = { 0 }, szFrom[MAX_PATH] = { 0 };
		Memcpy(szTo, _pTo, lstrlen(_pTo));
		Memcpy(szFrom, _pFrom, lstrlen(_pFrom));

		SHFILEOPSTRUCT FileOp = { 0 };
		FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
		FileOp.pFrom = szFrom;
		FileOp.pTo = szTo;
		FileOp.wFunc = FO_MOVE;
		return SHFileOperation(&FileOp) == 0;
	}

	BYTE GetBit(unsigned int dwValue, int iIndex)
	{
		unsigned int dwTemp = dwValue;
		dwTemp = dwTemp << (32 - iIndex);
		dwTemp = dwTemp >> 31;
		return (BYTE)dwTemp;
	}

	UTILS_API float ByteToFloat(BYTE* buff)
	{
		//得到三部分数，直接相乘
		int sign = 1;//为正数
		float exp = 0;//阶数
		float mantissa = 1;//尾数,默认为1
		unsigned int dwVal = 0;
		memcpy(&dwVal, buff, sizeof(unsigned int));
		if (dwVal == 0){
			return 0.0;
		}

		// 求符号位
		if (GetBit(dwVal, 32) == 0)
			sign = 1;
		else
			sign = -1;

		//求阶码
		int iPar = 0;
		unsigned int dwPar = 0;
		dwPar = dwVal;
		dwPar <<= 1;
		dwPar >>= 24;
		exp = (float)(dwPar - 127);
		exp = (float)pow(2, exp);

		//求尾码
		for (int i = 1; i < 24; i++)
		{
			mantissa += (float)(GetBit(dwVal, i)* pow(2, 0 - (24 - i)));
		}
		return sign * exp * mantissa;
	}
	int GBKToUtf8(char* buff, int iBuffLen, const char* pSrc, int iSrcLen)
	{
		if (buff == nullptr || pSrc == nullptr){
			return 0;
		}
		unsigned short* wszUtf8 = nullptr;
		char *szUtf8 = nullptr;
		char tmp1[1024 * 64], tmp2[1024 * 64];
		const int cilen = 1024 * 64;
		int iNeedLen1 = cilen, iNeedLen2 = cilen;
#if 1
		int len = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)pSrc, iSrcLen, NULL, 0);
		if (len > cilen){
			wszUtf8 = new unsigned short[len];
			iNeedLen1 = len;
		}
		else{
			wszUtf8 = (unsigned short*)tmp1;
		}
		if (wszUtf8 == NULL){
			return 0;
		}

		//memset(wszUtf8, 0, len);
		MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)pSrc, iSrcLen, (LPWSTR)wszUtf8, len);
		len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
		if (len > cilen){
			szUtf8 = new char[len];
			iNeedLen2 = len;
		}
		else{
			szUtf8 = tmp2;
		}

		if (szUtf8 == NULL){
			return 0;
		}

		//memset(szUtf8, 0, len);
		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL, NULL);
		int tmp = min(iBuffLen - 1, len);
		memcpy(buff, szUtf8, tmp);
		buff[tmp] = '\0';
		if (iNeedLen1 > cilen){
			delete[] wszUtf8;
		}
		if (iNeedLen2 > cilen){
			delete[] szUtf8;
		}
		return tmp;
#else
		int len = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)pSrc, iSrcLen, NULL, 0);
		unsigned short * wszUtf8 = new unsigned short[len];
		if (wszUtf8 == NULL){
			return;
		}
		memset(wszUtf8, 0, len);
		MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)pSrc, iSrcLen, (LPWSTR)wszUtf8, len);
		len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
		char *szUtf8 = new char[len];
		if (szUtf8 == NULL){
			return;
		}
		memset(szUtf8, 0, len);
		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL, NULL);
		if (len < 1024)
		{
			int tmp = min(iBuffLen - 1, len);
			strncpy_s(buff, iBuffLen, szUtf8, tmp);
			buff[tmp] = '\0';
		}
		else {
			int tmp = min(iBuffLen - 1, len);
			memcpy(buff, szUtf8, tmp);
			buff[tmp] = '\0';
		}

		delete[] szUtf8;
		delete[] wszUtf8;
#endif
	}

	int Utf8ToGBK(char* buff, int iBuffLen, const char *pSrc, int iSrcLen)
	{
		if (buff == nullptr || pSrc == nullptr){
			return 0;
		}
		unsigned short* wszGBK = nullptr;
		char *szGBK = nullptr;
		char tmp1[1024 * 64], tmp2[1024 * 64];
		const int cilen = 1024 * 64;
		int iNeedLen1 = cilen, iNeedLen2 = cilen;

#if 1
		int len = MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pSrc, iSrcLen, NULL, 0);
		if (len > cilen){
			wszGBK = new unsigned short[len];
			iNeedLen1 = len;
		}
		else{
			wszGBK = (unsigned short*)tmp1;
		}
		if (wszGBK == NULL){
			return 0;
		}
		//memset(wszGBK, 0, len);
		MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pSrc, iSrcLen, (LPWSTR)wszGBK, len);
		len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
		if (len > cilen){
			szGBK = new char[len];
			iNeedLen2 = len;
		}
		else{
			szGBK = tmp2;
		}

		if (szGBK == NULL){
			return 0;
		}
		//memset(szGBK, 0, len);
		WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
		int tmp = min(iBuffLen - 1, len);
		memcpy(buff, szGBK, tmp);
		buff[tmp] = '\0';
		if (iNeedLen1 > cilen){
			delete[] wszGBK;
		}
		if (iNeedLen2 > cilen){
			delete[] szGBK;
		}
		return tmp;
#else
		int len = MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pSrc, iSrcLen, NULL, 0);
		unsigned short * wszGBK = new unsigned short[len];
		if (wszGBK == NULL){
			return;
		}
		memset(wszGBK, 0, len);
		MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pSrc, iSrcLen, (LPWSTR)wszGBK, len);
		len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
		char *szGBK = new char[len];
		if (szGBK == NULL){
			return;
		}
		memset(szGBK, 0, len);
		WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
		if (len < 1024)
		{
			int tmp = min(iBuffLen - 1, len);
			strncpy_s(buff, iBuffLen, szGBK, tmp);
			buff[tmp] = '\0';
		}
		else {
			int tmp = min(iBuffLen - 1, len);
			memcpy(buff, szGBK, tmp);
			buff[tmp] = '\0';
		}

		delete[] szGBK;
		delete[] wszGBK;
#endif
	}

	int GetFileVersion(const char* pFile, VERSION_PROPERTY* pVerInfo, VERSION_PROPERTY* pProductVerInfo /*= nullptr*/, BYTE* pBuffer /*= nullptr*/, int iBuffLen /*= 0*/)
	{
		DWORD dwHandle;
		BYTE buff[1024];
		BYTE* lpVersionData = nullptr;
		DWORD dwBuffLen = 1024;
		lpVersionData = buff;

		if (pVerInfo == nullptr){
			return -1;
		}
		DWORD dwDataSize = ::GetFileVersionInfoSize(pFile, &dwHandle);
		if (dwDataSize == 0){
			MSG_INFO("err no:%d line:%d", GetLastError(), __LINE__);
			return -1;
		}
		// Allocate buffer and retrieve version information  
		if (dwBuffLen < dwDataSize){
			dwBuffLen = dwDataSize;
			lpVersionData = new BYTE[dwBuffLen];
		}
		if (lpVersionData == nullptr){
			return -2;
		}
		if (!::GetFileVersionInfo(pFile, dwHandle, dwDataSize, (void*)lpVersionData))
		{
			if (dwBuffLen > 1024){
				delete[] lpVersionData;
				lpVersionData = nullptr;
			}
			MSG_INFO("err no:%d line:%d", GetLastError(), __LINE__);
			return -3;
		}

		UINT nQuerySize = 0;
		VS_FIXEDFILEINFO* pVsffi = nullptr;
		if (!::VerQueryValue((void **)lpVersionData, "\\",(void**)&pVsffi, &nQuerySize))
		{
			if (dwBuffLen > 1024){
				delete[] lpVersionData;
				lpVersionData = nullptr;
			}
			MSG_INFO("err no:%d line:%d", GetLastError(), __LINE__);
			return -4;
		}

		pVerInfo->dwMajorVersion = HIWORD(pVsffi->dwFileVersionMS);
		pVerInfo->dwMinorVersion = LOWORD(pVsffi->dwFileVersionMS);
		pVerInfo->dwBuildNumber = HIWORD(pVsffi->dwFileVersionLS);
		pVerInfo->dwRevisionNumber = LOWORD(pVsffi->dwFileVersionLS);
		if (pProductVerInfo != nullptr){
			pProductVerInfo->dwMajorVersion = HIWORD(pVsffi->dwProductVersionMS);
			pProductVerInfo->dwMinorVersion = LOWORD(pVsffi->dwProductVersionMS);
			pProductVerInfo->dwBuildNumber = HIWORD(pVsffi->dwProductVersionLS);
			pProductVerInfo->dwRevisionNumber = LOWORD(pVsffi->dwProductVersionLS);
		}

		if ((pBuffer != nullptr) && (iBuffLen >= sizeof(VS_FIXEDFILEINFO))){
			memcpy(pBuffer, &pVsffi, min(iBuffLen, sizeof(VS_FIXEDFILEINFO)));
		}
		if (dwBuffLen > 1024){
			delete[] lpVersionData;
			lpVersionData = nullptr;
		}
		return 0;
	}

	UTILS_API int GetFileVersionForFolder(const char* pFolder, std::list<FILE_VERSION_PROPERTY>& lstFileVersions, bool bRecursive /*= true*/)
	{
		FILE_VERSION_PROPERTY stFileVersion;
		std::list<std::string> lstFiles;
		int ret = 0;
		EnumDirectoryFiles(pFolder, nullptr, 0, nullptr, &lstFiles, bRecursive);
		for (auto& it : lstFiles)
		{
			ret = GetFileVersion(it.data(), &stFileVersion.stFileVersion, &stFileVersion.stProductVersion);
			//MSG_INFO("...%s..%d", it.data(), ret);
			if (ret == 0){
				_snprintf_s(stFileVersion.szFile, _TRUNCATE, "%s", it.data());
				lstFileVersions.emplace_back(stFileVersion);
			}
		}
		return 0;
	}

	UTILS_API BOOL RegeFullMatch(const char* pStr, const char* pPattern)
	{
		if ((NULL == pPattern) || (NULL == pStr))
		{
			return FALSE;
		}
		CDll_PCRE cDll;
		if (!cDll.IsProcAddress())
		{
			MSG_ERROR;
			return FALSE;
		}
		const char *error = NULL;
		int erroffset = 0;
		int ovector[30];/* should be a multiple of 3 */

		std::string wrapped = "(?:";  // A non-counting grouping operator
		wrapped += pPattern;
		wrapped += ")\\z";

		pcre *re = cDll.pfnCompile()(
			wrapped.data(),              /* the pattern */
			0,                    /* default options */
			&error,               /* for error message */
			&erroffset,           /* for error offset */
			NULL);                /* use default character tables */
		if (re == NULL)
		{
			if (NULL != error)
			{
				MSG_INFO("PCRE compilation failed at offset %d: %s", erroffset, error);
			}
			return FALSE;
		}
		int rc = cDll.pfnExec()(
			re,                   /* the compiled pattern */
			NULL,                 /* no extra data - we didn't study the pattern */
			pStr,				  /* the subject string */
			strlen(pStr),         /* the length of the subject */
			0,                    /* start at offset 0 in the subject */
			0,                    /* default options */
			ovector,              /* output vector for substring information */
			30);				  /* number of elements in the output vector */
		if (rc < 0)
		{
			switch (rc)
			{
			case PCRE_ERROR_NOMATCH: MSG_INFO("No match"); break;
				/*
				Handle other special cases if you like
				*/
			default: MSG_INFO("Matching error %d", rc); break;
			}
			(*cDll.pfnFree())(re);     /* Release memory used for the compiled pattern */
			return FALSE;
		}
		(*cDll.pfnFree())(re);
		return TRUE;
	}
}}
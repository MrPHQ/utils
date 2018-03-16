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

#ifdef ENABLE_ZIP
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

	int CharacterConvert(const char* tocode,
		const char* fromcode,
		char *inbuf,
		int inlen,
		char *outbuf,
		int outlen,
		int* OutIdleLen,
		int* NoConvertLen)
	{
		if (tocode == NULL || fromcode == NULL || inbuf == NULL || outbuf == NULL)
		{
			return UTILS_ERROR_PAR;
		}
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
		Transform(szProName, 128);

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

	int RunProcess(const char* cmd, PPROCESS_PROPERTY pProcessProperty /*= NULL*/, unsigned int uiTimeOut /*= 0*/) {
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
		stStartInfo.wShowWindow = SW_HIDE;
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

#ifdef ENABLE_ZIP
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
		char tmp[MAX_PATH], path[MAX_PATH];
		char buff[1024];
		int iOutIdleLen = 0, iNoConvertLen = 0, error = 0;
		libzippp::ZipArchive zipFile(file);
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
				std::ofstream ofUnzippedFile(path, std::ios::binary);
				if (!static_cast<bool>(ofUnzippedFile)) {
					MSG_INFO("解压.读取文件[%s].失败", path);
					return false;
				}
				int err = it.readContent(ofUnzippedFile);
				if (err != 0) {
					MSG_INFO("解压.读取文件[%s]..失败", path);
					return false;
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
		if (pDir == NULL){
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
			strncat_s(sLog, 1024, "[", 1);
			strncat_s(sLog, 1024, flag, min(1023, strlen(flag)));
			strncat_s(sLog, 1024, "] ", 2);
		}
		strncat_s(sLog, 1024, sOut, min(1023-strlen(sLog), strlen(sOut)));
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
		if (NULL == path){
			return;
		}
		PathRemoveExtension(path);
#else

#endif // _WIN32

	}

	void RemoveFileSpec(char* path) {
#ifdef _WIN32
		if (NULL == path){
			return;
		}
		PathRemoveFileSpec(path);
#else

#endif // _WIN32
	}

	void StripPath(char* path) {
#ifdef _WIN32
		if (NULL == path){
			return;
		}
		PathStripPath(path);
#else

#endif // _WIN32
	}

	char* FindExtension(char* path) {
#ifdef _WIN32
		if (NULL == path){
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
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(start, end);
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
		osvi.wProductType = wProductType;

		// Initialize the condition mask.

		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
		VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
		VER_SET_CONDITION(dwlConditionMask, VER_PRODUCT_TYPE, op);

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

	HKEY CreateRegKey(HKEY hKey, const char* section, bool close /*= true*/) {
		HKEY hAppKey = NULL;
		DWORD dw = 0;
		REGSAM samDesiredOpen = KEY_ALL_ACCESS;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
		}
		if (RegCreateKeyEx(hKey, section, 0, REG_NONE,
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
	int WriteRegString(HKEY hKey, const char* section, const char* name, const char* value, int len) {
		if (nullptr == name) {
			return UTILS_ERROR_PAR;
		}
		bool bClose = false;
		HKEY _hKey = NULL;
		REGSAM samDesiredOpen = KEY_WRITE;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
		}
		if (section != NULL) {
			if (RegOpenKeyEx(hKey, section, 0, samDesiredOpen, &_hKey) != ERROR_SUCCESS) {
				return UTILS_ERROR_FAIL;
			}
			bClose = true;
		}
		else {
			_hKey = hKey;
		}
		int lRetCode = RegSetValueEx(_hKey,name,0,REG_SZ,(BYTE *)value,len);
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
	int WriteRegInt(HKEY hKey, const char* section, const char* name, int value) {
		HKEY _hKey = NULL;
		if (nullptr == name) {
			return UTILS_ERROR_PAR;
		}
		REGSAM samDesiredOpen = KEY_WRITE;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
		}
		bool bClose = false;
		if (section != NULL) {
			if (RegOpenKeyEx(hKey, section, 0, samDesiredOpen, &_hKey) != ERROR_SUCCESS) {
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
		int lRetCode = RegSetValueEx(_hKey, name, 0, REG_DWORD, (BYTE *)&value, sizeof(int));
		if (bClose) {
			RegCloseKey(_hKey);
		}
		return lRetCode == ERROR_SUCCESS ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
	}

	int ReadRegString(HKEY hKey, const char* section, const char* Entry, char* buff, int len) {
		HKEY _hKey = NULL;
		DWORD DataType = REG_SZ, BuffLen = 1024;
		char str[1024];

		if ((nullptr == section) || (nullptr == Entry) || (nullptr == buff)) {
			return UTILS_ERROR_PAR;
		}
		REGSAM samDesiredOpen = KEY_READ;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
		}
		str[0] = '\0';
		if (RegOpenKeyEx(hKey, section, 0, samDesiredOpen, &_hKey) == ERROR_SUCCESS){
			if (RegQueryValueEx(_hKey, Entry, 0, &DataType, (BYTE*)&str, &BuffLen) == ERROR_SUCCESS){
				Strcpy(buff, len, str);
				return 0;
			}
			RegCloseKey(_hKey);
		}
		return UTILS_ERROR_FAIL;
	}

	int ReadRegInt(HKEY hKey, const char* section, const char* Entry, int& v) {
		HKEY _hKey = NULL;
		DWORD DataType = REG_DWORD, BuffLen = 4;

		if ((nullptr == section) || (nullptr == Entry)) {
			return UTILS_ERROR_PAR;
		}
		REGSAM samDesiredOpen = KEY_READ;
		if (API::Is64()) {
			samDesiredOpen |= KEY_WOW64_32KEY;
			//samDesiredOpen |= KEY_WOW64_64KEY; 64.app
		}
		if (RegOpenKeyEx(hKey, section, 0, samDesiredOpen, &_hKey) == ERROR_SUCCESS) {
			if (RegQueryValueEx(_hKey, Entry, 0, &DataType, (BYTE*)&v, &BuffLen) == ERROR_SUCCESS) {
				return 0;
			}
			RegCloseKey(_hKey);
		}
		return UTILS_ERROR_FAIL;
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
}}
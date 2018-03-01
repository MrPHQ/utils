#include "../utils/api.h"
#include "internal.h"
#include <fstream>

#ifdef _WIN32
#include <io.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
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

	int Strcpy(char* _Destination, char const* _Source, int _MaxCount) {
		if ((_Destination == nullptr) || (_Source == nullptr)) {
			return -1;
		}
		int iRet = 0;
#ifdef _WIN32
		iRet = strncpy_s(_Destination, _TRUNCATE, _Source, _MaxCount);
#else
		iRet = strncpy(_Destination, _Source, _MaxCount);
#endif
		return iRet;
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

	int CharacterConvert(
		const char* tocode,
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

	UTILS_API void SleepTime(int ms) {
#ifdef _WIN32
		Sleep(ms);
#else
		usleep(ms*1000);
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
				Strcpy(path, tmp, min(MAX_PATH-1,strlen(tmp)));
			}
			//在末尾加 '/'
			PathAddBackslash(path);
			CharConvert(path, '\\', '/');
			if (!zipFile.addEntry(path)) {
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
				Strcpy(path, tmp, min(MAX_PATH - 1, strlen(tmp)));
			}

			//去掉末尾的 '/'
			PathRemoveBackslash(path);
			CharConvert(path, '\\', '/');

			Strcpy(tmp, it.data(), min(MAX_PATH - 1, it.length()));
			CharConvert(tmp, '\\', '/');
			if (!zipFile.addFile(path, tmp)) {
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
		libzippp::ZipArchive zipFile(file);
		if (!zipFile.open(libzippp::ZipArchive::READ_ONLY)) {
			return false;
		}
		std::vector<libzippp::ZipEntry> vEntrys = zipFile.getEntries();
		for (auto& it : vEntrys)
		{
			Strcpy(tmp, it.getName().data(), min(MAX_PATH - 1, it.getName().length()));
			CharConvert(tmp, '/', '\\');
			Sprintf(path, MAX_PATH, "%s\\%s", toDirectory, tmp);

			if (it.isDirectory()) {
				CreateDirectory(path, nullptr);
			}
			else if (it.isFile()) {
				std::ofstream ofUnzippedFile(path, std::ios::binary);
				if (!static_cast<bool>(ofUnzippedFile)) {
					return false;
				}
				int err = it.readContent(ofUnzippedFile);
				if (err != 0) {
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
		char tmp[MAX_PATH], path[MAX_PATH];
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
			return UTILS_ERROR_BUFFER_SMALL;
		}

		for (auto& it : vEntrys){
			if (!it.isDirectory() && !it.isFile()) {
				continue;
			}

			pInfo = (PZIP_COMMENT)(buff + iTotal);
			iTotal += num;
			Strcpy(pInfo->name, it.getName().data(), min(MAX_PATH - 1, it.getName().length()));
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
		Strcpy(szDir, pDir, min(MAX_PATH - 1, strlen(pDir)));
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
		Strcpy(tmp, path, min(MAX_PATH-1,strlen(path)));
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
			//std::cout << "errnr:" << e.GetErrorType() << std::endl;
			//std::cout << "error:" << e.what() << std::endl;
			return UTILS_ERROR_FAIL;
		}
		if (len < strDes.length()) {
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
			return UTILS_ERROR_FAIL;
		}
		if (len < strDes.length()) {
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
			return dst;
		}
		return dst;
	}
#endif

	void DEBUG_INFO(char* fmt, ...) {
#ifdef _WIN32
		va_list args;
		char sOut[256];
		va_start(args, fmt);
		_vsnprintf_s(sOut, sizeof(sOut)-1, fmt, args);
		va_end(args);
		OutputDebugString(sOut);
#else
#endif
	}

	bool DreateFolders(const char* folders) {
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
}}
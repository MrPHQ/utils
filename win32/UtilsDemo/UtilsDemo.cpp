// UtilsDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#define UTILS_ENABLE_ZIP
#define UTILS_ENABLE_CRYPTOPP
#define UTILS_ENABLE_REGEDIT

#include <utils\utils.h>
#include <utils\logger.h>
#include <iostream>
#include <bitset>
#include <Shlobj.h>

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <winperf.h>

#pragma comment(lib, "utils/utils.lib")

void QueryKey(HKEY hKey);

#define TOTALBYTES    8192
#define BYTEINCREMENT 4096

int main()
{

	/*std::cin.ignore();
	{
		HKEY hKey = UTILS::API::CreateRegKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\phq"), false);
		if (hKey != NULL) {
			UTILS::API::WriteRegString(hKey, NULL, "str", "test_str", 8);
			UTILS::API::WriteRegInt(hKey, NULL, "int", 100);
			UTILS::API::CloseRegKey(hKey);
		}
	}
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ms725501(v=vs.85).aspx
	std::cin.ignore();
	{
		TCHAR   inBuf[80];
		HKEY   hKey1, hKey2;
		DWORD  dwDisposition;
		LONG   lRetCode;
		TCHAR   szData[] = TEXT("USR:App Name\\Section1");

		// Create the .ini file key. 
		lRetCode = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
			TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\IniFileMapping\\appname.ini"),
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE,
			NULL,
			&hKey1,
			&dwDisposition);

		if (lRetCode != ERROR_SUCCESS)
		{
			printf("Error in creating appname.ini key (%d).\n", lRetCode);
			return (0);
		}

		// Set a section value 
		lRetCode = RegSetValueEx(hKey1,
			TEXT("Section1"),
			0,
			REG_SZ,
			(BYTE *)szData,
			sizeof(szData));

		if (lRetCode != ERROR_SUCCESS)
		{
			printf("Error in setting Section1 value\n");
			// Close the key
			lRetCode = RegCloseKey(hKey1);
			if (lRetCode != ERROR_SUCCESS)
			{
				printf("Error in RegCloseKey (%d).\n", lRetCode);
				return (0);
			}
		}
	}
	std::cin.ignore();
	{

#if 0
		DWORD BufferSize = TOTALBYTES;
		DWORD cbData;
		DWORD dwRet;

		PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK)malloc(BufferSize);
		cbData = BufferSize;

		printf("\nRetrieving the data...");

		dwRet = RegQueryValueEx(HKEY_PERFORMANCE_DATA,
			TEXT("Global"),
			NULL,
			NULL,
			(LPBYTE)PerfData,
			&cbData);
		while (dwRet == ERROR_MORE_DATA)
		{
			// Get a buffer that is big enough.

			BufferSize += BYTEINCREMENT;
			PerfData = (PPERF_DATA_BLOCK)realloc(PerfData, BufferSize);
			cbData = BufferSize;

			printf(".");
			dwRet = RegQueryValueEx(HKEY_PERFORMANCE_DATA,
				TEXT("Global"),
				NULL,
				NULL,
				(LPBYTE)PerfData,
				&cbData);
		}
		if (dwRet == ERROR_SUCCESS)
			printf("\n\nFinal buffer size is %d\n", BufferSize);
		else printf("\nRegQueryValueEx failed (%d)\n", dwRet);
#else
		DWORD BufferSize = TOTALBYTES;
		DWORD cbData;
		DWORD dwRet;

		PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK)malloc(1024*16);
		cbData = BufferSize;
		char str[256];
		DWORD ValueLen = 256;
		DWORD DataType = REG_SZ;
		DWORD DataType2 = REG_DWORD;
		int v = 2;
		DWORD vl = 4;
		HKEY hKey;
		//if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Wow6432Node\\DT Soft\\DAEMON Tools Pro"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\DT Soft\\DAEMON Tools Pro"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if (RegQueryValueEx(hKey, "Version Major", 0, &DataType, (BYTE*)&str, &ValueLen) == ERROR_SUCCESS)
			{
				std::cout << "XXXXXXXXXXX "<< str << std::endl;
			}
			else
			{
				printf("Get SystemPrefix from regedit error!\n");
			}
			if (RegQueryValueEx(hKey, "Config\\AdapterStateDT", 0, &DataType2, (BYTE*)&v, &vl) == ERROR_SUCCESS)
			{
				std::cout << "ccccccc " << v << std::endl;
			}
			else
			{
				printf("Get SystemPrefix from regedit error!\n");
			}
		}
		else
		{
			printf("Get SystemPrefix from regedit error!\n");
		}


		UTILS::API::ReadRegString(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Wow6432Node\\Google\\Update"), "LastInstallerSuccessLaunchCmdLine", str, 256);
		std::cout << "sdfsdfasdf " << str << std::endl;
		UTILS::API::ReadRegInt(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Wow6432Node\\Google\\Update"), "IsMSIHelperRegistered", v);
		std::cout << "xazc " << v << std::endl;
		//write key
		//https://msdn.microsoft.com/en-us/library/windows/desktop/ms725501(v=vs.85).aspx
#endif
	}

	std::cin.ignore();
	{
		HKEY hTestKey;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("SOFTWARE\\Wow6432Node\\DT Soft\\DAEMON Tools Pro"),0,KEY_READ,&hTestKey) == ERROR_SUCCESS)
		{
			QueryKey(hTestKey);
		}

		RegCloseKey(hTestKey);
	}

	std::cin.ignore();
	{
		char buff[1024];
		std::cout << UTILS::API::GetOSVersion() << std::endl;
		std::cout << UTILS::API::GetOSFolderPath(FOLDERID_System, buff, 1024) << std::endl;
		std::cout << buff << std::endl;
		buff[0] = '\0';
		std::cout << UTILS::API::GetOSFolderPath(CSIDL_SYSTEM, buff, 1024) << std::endl;
		std::cout << buff << std::endl;
	}*/





	std::cin.ignore();
	{
		char str[] = "E:\\SVN\\SelfSVN\\SampleCode\\OpfsUpdate\\bin\\F1B74DA7-6520-4AE5-B196-114C53F5C881";
		char* pstr = str;
		char file[] = "E:\\SVN\\SelfSVN\\SampleCode\\OpfsUpdate\\bin\\F1B74DA7-6520-4AE5-B196-114C53F5C881.zip";
		char* pfile = file;
		UTILS::API::ZipDirectory(pstr, pfile);

		char key[] = "2E3234F6591B4E69";
		char* pkey = key;
		char iv[] = "B91D10A7BE5B1FAB";
		char* piv = iv;

		char file2[] = "E:\\SVN\\SelfSVN\\SampleCode\\OpfsUpdate\\bin\\F1B74DA7-6520-4AE5-B196-114C53F5C881.patch";
		char* pfile2 = file2;

		int err = UTILS::API::EncryptionFile(pfile, pfile2, pkey, piv);
		std::cout << "加密:" << err << std::endl;
		err = UTILS::API::DecryptionFile(pfile2, "x.zip", pkey, piv);
		std::cout << "解密:" << err << std::endl;
	}


	std::cin.ignore();
	{
		char str[] = "E:\\SVN\\SelfSVN\\SampleCode\\OpfsUpdate\\bin\\F1B74DA7-6520-4AE5-B196-114C53F5C881\\patch_file.zip";
		char* pstr = str;
		std::string strx = UTILS::API::FileSHA(pstr);
		std::cout << strx << std::endl;
	}

	std::cin.ignore();
	{
		char str[] = "E:\\SVN\\SelfSVN\\SampleCode\\OpfsUpdate\\bin\\zip";
		char* pstr = str;
		char file[] = "E:\\SVN\\SelfSVN\\SampleCode\\OpfsUpdate\\bin\\patch_file.zip";
		char* pfile = file;
		int err = UTILS::API::ZipDirectory(pstr, pfile);
		std::cout << "压缩:" << err << std::endl;
	}


	std::cin.ignore();
	{
		int v = 2383459;
		char str[] = "2383459";
		char* pstr = str;
		std::string strx = UTILS::API::StringSHA256(pstr,strlen(pstr));
		std::cout << strx.data() << std::endl;
		std::string strx2 = UTILS::API::StringSHA1(pstr, strlen(pstr));
		std::cout << strx2.data() << std::endl;
	}

	std::cin.ignore();
	{
		char key[] = "puhuaqiang123456";
		char* pkey = key;
		char iv[] = "0123456789654321";
		char* piv = iv;
		int err = UTILS::API::EncryptionFile("proj.zip", "_proj.bin", pkey, piv);
		std::cout << "加密:" << err << std::endl;
		err = UTILS::API::DecryptionFile("_proj.bin", "proj2.zip", pkey, piv);
		std::cout << "解密:" << err << std::endl;
	}

	std::cin.ignore();
	{
		char dir[] = "F:\\X\\Y\\Z\\";
		char* pdir = dir;
		char dir2[] = "F:\\XX\\YY\\ZZ";
		char* pdir2 = dir2;
		UTILS::API::CreateFolders(pdir);
		UTILS::API::CreateFolders(pdir2);
	}
	std::cin.ignore();
	{
		UTILS::LOG::LOG_INIT("test.properties", false);
		UTILS::LOG::LOG_DEBUG("日志:%d 内容:%s", 111, "sdfa 撒旦法");
		UTILS::LOG::LOG_WARN("日志:%d ", 3454);
		UTILS::LOG::LOG_UNINIT();
	}
	std::cin.ignore();
	{
		std::string str("字符集转换示例...");
		std::string strOut, strOut2;
		char szStr[] = "字符集转换示例...";
		char* pStr = szStr;
		strOut.reserve(1024);
		char buff[1024], buff2[1024];
		int iOutIdleLen = 0, iNoConvertLen = 0;
		std::cout << "源字符串:" << "\t" << pStr << std::endl;
		//std::cout << strOut.capacity() <<"\t"<< strOut.max_size() << std::endl;
		int error = UTILS::API::CharacterConvert("UTF-8", "GBK", pStr, strlen(pStr), buff, 1024, &iOutIdleLen, &iNoConvertLen);
		if (error == 0)
		{
			buff[1024 - iOutIdleLen] = '\0';
			std::cout << "转换后字符串:" << "\t" << buff << std::endl;
			error = UTILS::API::CharacterConvert("GBK", "UTF-8", buff, strlen(buff), buff2, 1024, &iOutIdleLen, &iNoConvertLen);
			buff2[1024 - iOutIdleLen] = '\0';
			std::cout << "还原后字符串:" << "\t" << buff2 << std::endl;
		}
	}
	std::cin.ignore();
	{
		char dir[] = "F:\\Proj";
		char* pDir = dir;
		char file[] = "F:\\Proj\\proj.zip";
		char* pFile = file;
		bool err = UTILS::API::ZipDirectory(pDir, pFile);
		std::cout << "压缩 err:" << err << std::endl;

		char dir2[] = "F:\\tmp";
		char* pDir2 = dir2;
		CreateDirectory(dir2,nullptr);
		err = UTILS::API::UnZipFile(pFile, pDir2);
		std::cout << "解压 err:" << err << std::endl;
	}
	std::cin.ignore();
    return 0;
}

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void QueryKey(HKEY hKey)
{
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode;

	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

								 // Enumerate the subkeys, until RegEnumKeyEx fails.

	if (cSubKeys)
	{
		printf("\nNumber of subkeys: %d\n", cSubKeys);

		for (i = 0; i<cSubKeys; i++)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i,
				achKey,
				&cbName,
				NULL,
				NULL,
				NULL,
				&ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
			{
				_tprintf(TEXT("(%d) %s\n"), i + 1, achKey);
			}
		}
	}

	// Enumerate the key values. 

	if (cValues)
	{
		printf("\nNumber of values: %d\n", cValues);

		for (i = 0, retCode = ERROR_SUCCESS; i<cValues; i++)
		{
			cchValue = MAX_VALUE_NAME;
			achValue[0] = '\0';
			retCode = RegEnumValue(hKey, i,
				achValue,
				&cchValue,
				NULL,
				NULL,
				NULL,
				NULL);

			if (retCode == ERROR_SUCCESS)
			{
				_tprintf(TEXT("(%d) %s\n"), i + 1, achValue);
			}
		}
	}
}
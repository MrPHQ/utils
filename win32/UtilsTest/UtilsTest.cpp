// UtilsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define UTILS_ENABLE_ICONV
#define UTILS_ENABLE_ZIP
#define UTILS_ENABLE_CRYPTOPP
#define UTILS_ENABLE_REGEDIT

#include <utils\utils.h>
#include <utils\logger.h>
#include <utils\socket.h>
#include <utils\file.h>
#include <iostream>

#pragma comment(lib, "utils/utils.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	std::cin.ignore();
	{
		HANDLE hEvent = ::OpenEvent(EVENT_MODIFY_STATE, FALSE, "Global\\E2F370BB-6824-4659-B640-E0C434D7D29C");
		if (hEvent == NULL) {
			OutputDebugString("1111111111111111");
			return FALSE;
		}
		OutputDebugString("2222222222222222222");
		CloseHandle(hEvent);
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
		char key[] = "2E3234F6591B4E69";
		char* pkey = key;
		char iv[] = "B91D10A7BE5B1FAB";
		char* piv = iv;

		char file2[] = "E:\\SvnWorking\\SampleCode\\OpfsUpdate\\bin\\tmp_folder\\091622C9-0D08-4C6A-B96C-B34DF9CA66D5.patch";
		char* pfile2 = file2;

		int err = UTILS::API::DecryptionFile(pfile2, "x.zip", pkey, piv);
		std::cout << "解密:" << err << std::endl;
	}

	std::cin.ignore();
	{
		std::cout<<CopyFile("d:\\zeno.log", "e:\\zeno.log", FALSE)<<std::endl;
		std::cout << CopyFile("d:\\zeno.log", "e:\\SDK", FALSE) << std::endl;
	}
	std::cin.ignore();
	return 0;
}


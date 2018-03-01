// UtilsDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <utils\utils.h>
#include <utils\logger.h>
#include <iostream>

#pragma comment(lib, "utils/utils.lib")
int main()
{
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


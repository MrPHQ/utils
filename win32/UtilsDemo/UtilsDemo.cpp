// UtilsDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <utils\utils.h>
#include <utils\logger.h>
#include <iostream>
#include <bitset>

#pragma comment(lib, "utils/utils.lib")
int main()
{
	std::cin.ignore();
	{
		std::bitset<32> _bit;

		_bit.set(0);
		_bit.set(1);
		_bit.set(2);
		_bit.set(3);
		std::cout << _bit.to_ulong() << std::endl;
		std::cout << _bit.test(0) << std::endl;
		std::cout << _bit.test(1) << std::endl;
		std::cout << _bit.test(2) << std::endl;
		std::cout << _bit.test(3) << std::endl;
		std::cout << _bit.test(4) << std::endl;
		std::cout << _bit.test(5) << std::endl;
		std::cout << std::endl;
		std::bitset<32> _bit2(17);
		std::cout << _bit2.to_ulong() << std::endl;
		std::cout << _bit2.test(0) << std::endl;
		std::cout << _bit2.test(1) << std::endl;
		std::cout << _bit2.test(2) << std::endl;
		std::cout << _bit2.test(3) << std::endl;
		std::cout << _bit2.test(4) << std::endl;
		std::cout << _bit2.test(5) << std::endl;

		for (int i = 0; i < _bit2.size(); i++)
		{
			std::cout << _bit2.test(i) << std::endl;
		}

		std::cout << ((_bit2 & std::bitset<32>(1)) != 1) << std::endl;
		std::cout << ((_bit2 & std::bitset<32>(2)) == 0) << std::endl;
		std::cout << (_bit2 & std::bitset<32>(10)) << std::endl;
		std::cout << ((_bit2 & std::bitset<32>(16)) == std::bitset<32>(16)) << std::endl;

		std::bitset<32> _bit3(8);
		std::bitset<32> _bit4(32);
		std::cout << (_bit3 |= _bit4).to_string() << std::endl;

	}

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


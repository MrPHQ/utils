// UtilsDemo.cpp : �������̨Ӧ�ó������ڵ㡣
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
		std::cout << "����:" << err << std::endl;
		err = UTILS::API::DecryptionFile("_proj.bin", "proj2.zip", pkey, piv);
		std::cout << "����:" << err << std::endl;
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
		UTILS::LOG::LOG_DEBUG("��־:%d ����:%s", 111, "sdfa ������");
		UTILS::LOG::LOG_WARN("��־:%d ", 3454);
		UTILS::LOG::LOG_UNINIT();
	}
	std::cin.ignore();
	{
		std::string str("�ַ���ת��ʾ��...");
		std::string strOut, strOut2;
		char szStr[] = "�ַ���ת��ʾ��...";
		char* pStr = szStr;
		strOut.reserve(1024);
		char buff[1024], buff2[1024];
		int iOutIdleLen = 0, iNoConvertLen = 0;
		std::cout << "Դ�ַ���:" << "\t" << pStr << std::endl;
		//std::cout << strOut.capacity() <<"\t"<< strOut.max_size() << std::endl;
		int error = UTILS::API::CharacterConvert("UTF-8", "GBK", pStr, strlen(pStr), buff, 1024, &iOutIdleLen, &iNoConvertLen);
		if (error == 0)
		{
			buff[1024 - iOutIdleLen] = '\0';
			std::cout << "ת�����ַ���:" << "\t" << buff << std::endl;
			error = UTILS::API::CharacterConvert("GBK", "UTF-8", buff, strlen(buff), buff2, 1024, &iOutIdleLen, &iNoConvertLen);
			buff2[1024 - iOutIdleLen] = '\0';
			std::cout << "��ԭ���ַ���:" << "\t" << buff2 << std::endl;
		}
	}
	std::cin.ignore();
	{
		char dir[] = "F:\\Proj";
		char* pDir = dir;
		char file[] = "F:\\Proj\\proj.zip";
		char* pFile = file;
		bool err = UTILS::API::ZipDirectory(pDir, pFile);
		std::cout << "ѹ�� err:" << err << std::endl;

		char dir2[] = "F:\\tmp";
		char* pDir2 = dir2;
		CreateDirectory(dir2,nullptr);
		err = UTILS::API::UnZipFile(pFile, pDir2);
		std::cout << "��ѹ err:" << err << std::endl;
	}
	std::cin.ignore();
    return 0;
}


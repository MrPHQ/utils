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
    return 0;
}


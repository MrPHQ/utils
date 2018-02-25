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
    return 0;
}


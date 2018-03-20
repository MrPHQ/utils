// UtilsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
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
		std::cout<<CopyFile("d:\\zeno.log", "e:\\zeno.log", FALSE)<<std::endl;
		std::cout << CopyFile("d:\\zeno.log", "e:\\SDK", FALSE) << std::endl;
	}
	std::cin.ignore();
	return 0;
}


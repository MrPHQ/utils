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
		char key[] = "2E3234F6591B4E69";
		char* pkey = key;
		char iv[] = "B91D10A7BE5B1FAB";
		char* piv = iv;

		char file2[] = "E:\\SvnWorking\\SampleCode\\OpfsUpdate\\bin\\tmp_folder\\091622C9-0D08-4C6A-B96C-B34DF9CA66D5.patch";
		char* pfile2 = file2;

		int err = UTILS::API::DecryptionFile(pfile2, "x.zip", pkey, piv);
		std::cout << "½âÃÜ:" << err << std::endl;
	}

	std::cin.ignore();
	{
		std::cout<<CopyFile("d:\\zeno.log", "e:\\zeno.log", FALSE)<<std::endl;
		std::cout << CopyFile("d:\\zeno.log", "e:\\SDK", FALSE) << std::endl;
	}
	std::cin.ignore();
	return 0;
}


// UtilsTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <functional>
#include <thread>
#include <mutex> // std::mutex, std::unique_lock
#define UTILS_ENABLE_ICONV
#define UTILS_ENABLE_ZIP
#define UTILS_ENABLE_CRYPTOPP
#define UTILS_ENABLE_REGEDIT

#include <utils\utils.h>
#include <utils\logger.h>
#include <utils\socket.h>
#include <utils\file.h>
#include <utils\Thread.h>
#include <iostream>

#pragma comment(lib, "utils/utils.lib")

void func2()
{
	Sleep(2000);

	UTILS::ClientSocket clt(UTILS::PROTO_TYPE_UDP);

	std::string str;
	char buff[1024];
	int len = 0;
	struct sockaddr_in src;
	sockaddr src2;
	int deslen = sizeof(sockaddr_in);
	ZeroMemory(&src, sizeof(sockaddr_in));

	UTILS::GetSockAddr(UTILS::PROTO_TYPE_UDP, "127.0.0.1", 8999, (sockaddr*)&src);
	clt.UDPBind(nullptr, 9001);
#if 0
	sockaddr_in* p = (sockaddr_in*)&src;
	char IPdotdec[64];
	IPdotdec[0] = '\0';
	inet_ntop(AF_INET, &p->sin_addr, IPdotdec, 64);
	std::cout << IPdotdec << std::endl;
	std::cout << p->sin_port << std::endl;
#endif

	while (true)
	{
		str = "";
		std::cout << "输入:" << std::endl;
		std::cin >> str;

		if (str.length()) {
			len = clt.write_to((const char*)str.data(), str.length(), (sockaddr*)&src, sizeof(sockaddr_in));
		}
		buff[0] = '\0';
		len = clt.read_from(buff, 1024, (sockaddr*)&src2, &deslen, 0, 2000);
		//std::cout << "Client:" << len << std::endl;
		if (len > 0) {
			buff[len] = '\0';
			std::cout << "接收到数据:" << buff << std::endl;
		}
	}

}

void func1(UTILS::ClientSocket& skt)
{
	char szData[1024];
	int len = 0;
	while (true)
	{
		szData[0] = '\0';
		len = skt.read(szData, 1024);
		if (len <= 0) {
			continue;
		}
		szData[len] = '\0';
		std::cout << szData << std::endl;

		strncat_s(szData, _TRUNCATE, "—SERVER", _TRUNCATE);
		skt.write(szData, strlen(szData));
	}
}

void func3()
{
	Sleep(2000);

	UTILS::ClientSocket clt;

	clt.Connect("127.0.0.1", 8060);
	if (clt.isConnect()) {
		std::cout << "链接成功." << std::endl;
		std::string str;
		char buff[1024];
		int len = 0;

		while (true)
		{
			str = "";
			std::cout << "输入:" << std::endl;
			std::cin >> str;

			if (str.length()) {
				len = clt.write((const char*)str.data(), str.length());
				std::cout << "w:" << len << std::endl;
			}

			len = clt.read(buff, 1024);
			if (len > 0) {
				buff[len] = '\0';
				std::cout << "接收到数据:" << buff << std::endl;
			}
		}
	}
	else {
		std::cout << "链接失败." << std::endl;
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	std::cin.ignore();
	{
		UTILS::CThreadBox::RunEx([](void*){
			std::cout << "UTILS::CThreadBox::RunEx...." << std::endl;
		}, nullptr);

	}

	std::cin.ignore();
	{
		char str[] = "E:\\TMP\\de";
		char* pstr = str;
		std::cout << UTILS::API::PathDelete(pstr) << std::endl;

		char str1[] = "E:\\TMP\\xxx";
		char* pstr1 = str1;
		char str2[] = "E:\\TMP\\yyy";
		char* pstr2 = str2;
		std::cout<<UTILS::API::PathReName(pstr1, pstr2)<<std::endl;

		char str11[] = "E:\\TMP\\sd.txt";
		char* pstr11 = str11;
		char str22[] = "E:\\TMP\\sddd.txt";
		char* pstr22 = str22;
		std::cout << UTILS::API::PathReName(pstr11, pstr22) << std::endl;

	}

	std::cin.ignore();
	{
		UTILS::CFile file;
		file.Open(UTILS::PATH_FILE_OPENMODE_OUT, "E:\\中文\\txt.txt");
		file.Write("xxxxxxxxxx", strlen("xxxxxxxxxx"));
		file.Close();
	}
	//TCP
	std::cin.ignore();
	{
		UTILS::AbstractSocket::InitEnv();
		UTILS::ServerSocket tcp_server(UTILS::PROTO_TYPE_TCP,8060);
		UTILS::ClientSocket clt;
		std::thread clt2(func3);
		while (true)
		{
			if (tcp_server.Accept(clt) && clt.isOpen()) {
				new std::thread(func1, std::ref(clt));
			}
			Sleep(10);
		}
	}

	//UDP
	std::cin.ignore();
	{
		UTILS::AbstractSocket::InitEnv();
		UTILS::ServerSocket udp_server;
		char buff[1024];
		struct sockaddr_in src, src2;
		int addr_len = sizeof(sockaddr_in);
		std::thread clt2(func2);

		udp_server.Init(UTILS::PROTO_TYPE_UDP, nullptr, 8999);

#if 0
		int len = sizeof(sockaddr);
		udp_server.GetSockName((sockaddr*)&src2, &len);
		sockaddr_in* p = (sockaddr_in*)&src2;
		char IPdotdec[64];
		IPdotdec[0] = '\0';
		inet_ntop(AF_INET, &p->sin_addr, IPdotdec, 64);
		std::cout << IPdotdec << std::endl;
		std::cout << p->sin_port << std::endl;
#endif

		while (true)
		{
			buff[0] = '\0';
			/* 填写sockaddr_in 结构 */

			int iDataLen = udp_server.read_from(buff, 1024,
				(sockaddr*)&src, &addr_len, 0,
				2000);
			//std::cout << "Server:" << iDataLen << std::endl;
			if (iDataLen > 0 && iDataLen < 1024) {
#if 0
				sockaddr_in* p = (sockaddr_in*)&src;
				char IPdotdec[64];
				IPdotdec[0] = '\0';
				inet_ntop(AF_INET, &p->sin_addr, IPdotdec, 64);
				std::cout << IPdotdec << std::endl;
				std::cout << p->sin_port << std::endl;
#endif

				buff[iDataLen] = '\0';
				strncat_s(buff, sizeof(buff), " Server ok", strlen(" Server ok"));
				udp_server.write_to(buff, strlen(buff), (sockaddr*)&src, addr_len);
			}
		}
	}

	std::cin.ignore();
	{
		HANDLE hEvent = ::OpenEvent(EVENT_MODIFY_STATE, FALSE, "Global\\E2F370BB-6824-4659-B640-E0C434D7D29C");
		if (hEvent == NULL) {
			return FALSE;
		}
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


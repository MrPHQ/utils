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
#include <utils\file.h>
#include <utils\Thread.h>
#include <iostream>
#include <utils\net.h>
#include <utils\buff.h>
#include <utils\LogFile.h>
#include <utils\Time.h>
#pragma comment(lib, "utils/utils.lib")


class CTest
{
public:
	CTest(){
		std::cout << "CTest()" << std::endl;
	}
	~CTest(){
		std::cout << "~CTest()" << std::endl;
	}

};

struct Employee {
	Employee(std::string id) : id(id) {}
	std::string id;
	std::vector<std::string> lunch_partners;
	std::mutex m;
	std::string output() const
	{
		std::string ret = "Employee " + id + " has lunch partners: ";
		for (const auto& partner : lunch_partners)
			ret += partner + " ";
		return ret;
	}
};
void send_mail(Employee &, Employee &)
{
	// simulate a time-consuming messaging operation  
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void assign_lunch_partner(Employee &e1, Employee &e2)
{
	{
		static std::mutex io_mutex;
		std::lock_guard<std::mutex> lk(io_mutex);
		std::cout << e1.id << " and " << e2.id << " are waiting for locks" << std::endl;
	}

	// use std::lock to acquire two locks without worrying about   
	// other calls to assign_lunch_partner deadlocking us  
	{
		std::lock(e1.m, e2.m);
		std::lock_guard<std::mutex> lk1(e1.m, std::adopt_lock);
		std::lock_guard<std::mutex> lk2(e2.m, std::adopt_lock);
		// Equivalent code (if unique_lock's is needed, e.g. for condition variables)  
		//        std::unique_lock<std::mutex> lk1(e1.m, std::defer_lock);  
		//        std::unique_lock<std::mutex> lk2(e2.m, std::defer_lock);  
		//        std::lock(lk1, lk2);  
		std::cout << e1.id << " and " << e2.id << " got locks" << std::endl;
		e1.lunch_partners.push_back(e2.id);
		e2.lunch_partners.push_back(e1.id);
	}
	send_mail(e1, e2);
	send_mail(e2, e1);
}

UTILS::CProcessLock lock;

/// 模块退出通知事件
#define TEST_EVENT "Global\\{ECEF3391-EC8B-487F-8BBB-AE444B4EF4E2}"
#define TEST_MUTEX "Global\\{AAC6E35D-537B-4CC4-A935-E18B8780C04A}"


int _tmain(int argc, _TCHAR* argv[])
{
	std::cin.ignore();
	{
		std::cout << UTILS::API::Random(1, 100) << std::endl;
		std::cout << UTILS::API::Random(1, 100) << std::endl;
		std::cout << UTILS::API::Random(1, 100) << std::endl;
		std::cout << UTILS::API::Random(1, 100) << std::endl;
		std::cout << UTILS::API::Random(1, 100) << std::endl;
	}

	std::cin.ignore();
	{
		UTILS::Time::TIME_INFO stTime;
		UTILS::Time::GetNowTime(stTime);
		time_t t = UTILS::Time::Convert(stTime);
		std::cout << t << std::endl;
	}

	std::cin.ignore();
	{
		UTILS::CLock lock;
		UTILS::CThreadBox::RunEx([&](void*){

			std::unique_lock<std::mutex> lck(lock.GetMutex());

			if (lock.WaitAck(lck, 2000)){

			}
			else{
				OutputDebugString("vvvvvvvvvvvvvvvvvvvvvvvv");
			}
		},nullptr);

		while (true)
		{
			Sleep(100);
		}
	}

	std::cin.ignore();
	{
		char szStr[] = "字符集转换示例...字符集转换示例...字符集转换示例...字符集转换示例...字符集转换示例...字符集转换示例...x";
		char* pStr = szStr;
		char buff[1024], buff2[1024];
		int iOutIdleLen = 0, iNoConvertLen = 0;
		std::cout << "源字符串:" << "\t" << pStr << std::endl;
		//std::cout << strOut.capacity() <<"\t"<< strOut.max_size() << std::endl;
		buff[0] = '\0';
		UTILS::API::GBKToUtf8(buff, 1024, pStr, strlen(pStr)+1);
		if (strlen(buff) > 0)
		{
			std::cout << "转换后字符串:" << "\t" << buff << std::endl;
			UTILS::API::Utf8ToGBK(buff2, 1024, buff, strlen(buff)+1);
			std::cout << "还原后字符串:" << "\t" << buff2 << std::endl;
		}
	}

	std::cin.ignore();
	{
		int iValue = 0;
		UTILS::CRWLock rwLock;
		std::thread wtr([](int& v, UTILS::CRWLock& lock){
			for (int i = 0; i < 10; i++)
			{
				UTILS::CAutoLock lck(&lock, false);
				v++;
				Sleep(500);
			}
		}, std::ref(iValue), std::ref(rwLock));

		std::thread rdt[10];
		for (int i = 0; i < 10; i++)
		{
			rdt[i].swap(std::thread([](int& v, UTILS::CRWLock& lock){
				DWORD dwTimeOut = GetTickCount();
				while (true)
				{
					Sleep(UTILS::API::Random(100, 5000));
					UTILS::CAutoLock lck(&lock);
					std::cout << GetCurrentThreadId() << ":" << v << std::endl;
					if (abs((int)(GetTickCount() - dwTimeOut)) > 20000){
						break;
					}
				}
				std::cout << "end" << std::endl;

			}, std::ref(iValue), std::ref(rwLock)));
		}
		std::cin.ignore();
	}
	std::cin.ignore();
	{
		int iValue = 0;
		UTILS::CRWLock rwLock;
		std::thread wtr([](int& v, UTILS::CRWLock& lock){
			for (int i = 0; i < 10; i++)
			{
				lock.WLock();
				v++;
				Sleep(500);
				lock.WUnLock();
			}
		}, std::ref(iValue), std::ref(rwLock));

		std::thread rdt[10];
		for (int i = 0; i < 10; i++)
		{
			rdt[i].swap(std::thread([](int& v, UTILS::CRWLock& lock){
				DWORD dwTimeOut = GetTickCount();
				while (true)
				{
					Sleep(UTILS::API::Random(100, 5000));
					lock.RLock();
					std::cout <<GetCurrentThreadId()<<":"<<  v << std::endl;
					lock.RUnLock();
					if (abs((int)(GetTickCount() - dwTimeOut)) > 30000){
						break;
					}
				}
				std::cout << "end" << std::endl;

			}, std::ref(iValue), std::ref(rwLock)));
		}
		std::cin.ignore();
	}
	std::cin.ignore();
	{
		std::list<UTILS::API::FILE_VERSION_PROPERTY> lstFileVersions;
		UTILS::API::GetFileVersionForFolder("F:\\GitRepository\\utils\\bin", lstFileVersions, true);
		char szVer[64];
		for (auto& it : lstFileVersions)
		{
			_snprintf_s(szVer, _TRUNCATE, "%d.%d.%d.%d/%d.%d.%d.%d", 
				it.stFileVersion.dwMajorVersion,
				it.stFileVersion.dwMinorVersion,
				it.stFileVersion.dwBuildNumber,
				it.stFileVersion.dwRevisionNumber,
				it.stProductVersion.dwMajorVersion,
				it.stProductVersion.dwMinorVersion,
				it.stProductVersion.dwBuildNumber,
				it.stProductVersion.dwRevisionNumber);
			std::cout << it.szFile << "\t" << szVer<< std::endl;
		}
		std::cout<<"end"<<std::endl;
	}
	std::cin.ignore();
	{
		DWORD dwLogHeadFlag = 0;
		dwLogHeadFlag |= UTILS::LOG_FILE_HEAD_TIME;
		dwLogHeadFlag |= UTILS::LOG_FILE_HEAD_PROC_NAME;
		dwLogHeadFlag |= UTILS::LOG_FILE_HEAD_PROC_ID;
		dwLogHeadFlag |= UTILS::LOG_FILE_HEAD_THREAD_ID;
		dwLogHeadFlag |= UTILS::LOG_FILE_HEAD_FILE_NAME;
		dwLogHeadFlag |= UTILS::LOG_FILE_HEAD_FILE_LINE;
		char szFile[256];
		int iCnt = 0;
		_snprintf_s(szFile, _TRUNCATE, "%s\\utilsTest.log", UTILS::API::GetCurrentPath());
		UTILS::LOG_INIT(UTILS::LOG_FILE_MODE_ASYNC_IN, dwLogHeadFlag, szFile, 2, 1024*1024);
		UTILS::LOG_WRITE_EX("启动.");
		while (true)
		{
			Sleep(100);
			UTILS::LOG_WRITE_EX("日志..xxsdf no:%d..", iCnt++);
			if (iCnt > 20){
				break;
			}
		}
		std::cout << "..." << std::endl;
	}
	std::cin.ignore();
	{// tcp
		UTILS::NET::SELECT::CNetServer cNetServer;
		int err = cNetServer.OpenSocket(UTILS::NET::TRANS_PROTOCOL_TYPE_TCP, nullptr, 9898);
		if (err != 0 || cNetServer.IsError() || !cNetServer.IsValidSkt()){
			std::cout << "OpenSocket err" << std::endl;
			return -1;
		}
		cNetServer.SetSktNoBlock();
		cNetServer.SetTcpNoDelay();
		cNetServer.SetSktReuseAddr(true);
		UTILS::NET::SELECT::CNetClient* pNetClient = nullptr;
		std::thread clt([](){
			Sleep(3000);
			UTILS::NET::SELECT::CNetClient clt;

			clt.ConnectSocket(UTILS::NET::TRANS_PROTOCOL_TYPE_TCP, "127.0.0.1", 9898);
			clt.SetSktNoBlock();
			if (!clt.IsError() && clt.IsValidSkt()) {
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
						len = clt.Write((const char*)str.data(), str.length(), 2000);
						std::cout << "w:" << len << std::endl;
					}

					len = clt.Read(buff, 1024, 0, 2000);
					if (len > 0) {
						buff[len] = '\0';
						std::cout << "客户端接收到数据:" << buff << std::endl;
					}
				}
			}
			else {
				std::cout << "链接失败." << std::endl;
			}
		});
		while (true)
		{
			if (cNetServer.Accept(pNetClient)){
				pNetClient->SetSktNoBlock();
				new std::thread([](UTILS::NET::SELECT::CNetClient*& skt){
					char szData[1024];
					int len = 0;
					while (true)
					{
						szData[0] = '\0';
						len = skt->Read(szData, 1024);
						if (len <= 0) {
							continue;
						}
						szData[len] = '\0';
						std::cout << szData << std::endl;

						strncat_s(szData, _TRUNCATE, "—SERVER", _TRUNCATE);
						skt->Write(szData, strlen(szData), 2000);
					}
				}, std::ref(pNetClient));
			}
			else{
				Sleep(10);
			}
		}
	}
	std::cin.ignore();
	{
		uint8_t output[512] = { 0 };
		uint8_t data[256] = { 0 };
		for (int i = 0; i < 256; i++)
			data[i] = i;

		UTILS::BUFFER::CRingBuffer fifo(128);
		fifo.Write((char*)data, 100);

		fifo.Read((char*)output, 50);

		fifo.Write((char*)data, 30);

		auto c = fifo.Write((char*)(data + 10), 92);

		std::cout << "Empty:" << fifo.IsEmpty() << std::endl;
		std::cout << "Left Space:" << fifo.Space() << std::endl;
		std::cout << "Length:" << fifo.Size() << std::endl;
		//uint8_t a = fifo.size - fifo.in + fifo.out;
		//uint8_t b = fifo.in - fifo.out;

		std::cout << "=======================================" << std::endl;
		fifo.Read((char*)output, 128);
		std::cout << "Empty:" << fifo.IsEmpty() << std::endl;
		std::cout << "Left Space:" << fifo.Space() << std::endl;
		std::cout << "Length:" << fifo.Size() << std::endl;

		std::cout << "======================================" << std::endl;
		fifo.Write((char*)output, 100);
		std::cout << "Empty:" << fifo.IsEmpty() << std::endl;
		auto d = static_cast<uint8_t>(fifo.Space());
		auto e = static_cast<uint8_t>(fifo.Size());
		printf("Left Space:%d\n", d);
		printf("Length:%d\n", e);
	}

	std::cin.ignore();
	{
		char buff[1024];
		sockaddr stUdpSrc;
		int len = sizeof(sockaddr);
		UTILS::NET::SELECT::CNetServer cNetServer;
		int err = cNetServer.OpenSocket(UTILS::NET::TRANS_PROTOCOL_TYPE_UDP, nullptr, 9898);
		if (err != 0 || cNetServer.IsError() || !cNetServer.IsValidSkt()){
			std::cout << "OpenSocket err" << std::endl;
			return -1;
		}
		cNetServer.SetSktNoBlock();
		cNetServer.SetSktReuseAddr(true);
		UTILS::NET::SELECT::CNetClient* pNetClient = nullptr;
		std::thread clt([](){
			Sleep(3000);
			UTILS::NET::SELECT::CNetClient clt;

			clt.ConnectSocket(UTILS::NET::TRANS_PROTOCOL_TYPE_UDP, "127.0.0.1", 9898);
			clt.SetSktNoBlock();
			if (!clt.IsError() && clt.IsValidSkt()) {
				std::cout << "链接成功." << std::endl;
				std::string str;
				char buff[1024];
				int len = 0, iAddrLen = sizeof(sockaddr);
				sockaddr stUdpSrc;

				while (true)
				{
					str = "";
					std::cout << "输入:" << std::endl;
					std::cin >> str;

					if (str.length()) {
						len = clt.Write((const char*)str.data(), str.length(), 2000);
						std::cout << "w:" << len << std::endl;
					}

					len = clt.ReadFromUDP(buff, 1024, (struct sockaddr&)stUdpSrc, iAddrLen, 2000);
					if (len > 0) {
						buff[len] = '\0';
						std::cout << "客户端接收到数据:" << buff << std::endl;
					}
				}
			}
			else {
				std::cout << "链接失败." << std::endl;
			}
		});
		while (true)
		{
			buff[0] = '\0';

			int iDataLen = cNetServer.ReadFromUDP(buff, 1024, (struct sockaddr&)stUdpSrc, len, 2000);
			if (iDataLen > 0 && iDataLen < 1024) {

				buff[iDataLen] = '\0';
				strncat_s(buff, sizeof(buff), " Server ok", strlen(" Server ok"));
				cNetServer.WriteToUDP(buff, strlen(buff), (sockaddr&)stUdpSrc, len);
			}
			else{
				//std::cout << "UDP Server Not Read Data." << std::endl;
			}
		}
	}

	std::cin.ignore();
	{
		lock.Init(TEST_MUTEX, TEST_EVENT);

		std::cout << "加锁锁" << std::endl;
		lock.Lock();
		std::cout << "加锁成功" << std::endl;

		UTILS::API::RunProcess("E:\\GitRep\\utils\\bin\\UtilsDemo.exe", nullptr, 0, TRUE);

		Sleep(5000);
		std::cout << "释放所" << std::endl;
		lock.Ack();
		lock.UnLock();
	

		Sleep(1000);
		std::cout << "加锁" << std::endl;
		lock.Lock();
		std::cout << "加锁成功" << std::endl;
		lock.UnLock();

		std::cin.ignore();

	}
	std::cin.ignore();
	{
		Employee alice("alice"), bob("bob"), christina("christina"), dave("dave");

		// assign in parallel threads because mailing users about lunch assignments  
		// takes a long time  
		std::vector<std::thread> threads;
		threads.emplace_back(assign_lunch_partner, std::ref(alice), std::ref(bob));
		threads.emplace_back(assign_lunch_partner, std::ref(christina), std::ref(bob));
		threads.emplace_back(assign_lunch_partner, std::ref(christina), std::ref(alice));
		threads.emplace_back(assign_lunch_partner, std::ref(dave), std::ref(bob));

		for (auto &thread : threads) 
			thread.join();
		std::cout << alice.output() << '\n' << bob.output() << '\n'
			<< christina.output() << '\n' << dave.output() << '\n';
		std::cout << "END" << std::endl;
	}
	std::cin.ignore();
	{
		char szTxt[128],szBuff[128];
		_snprintf_s(szTxt, _TRUNCATE, "GIS;xChannel_id;x:2;ssssssssx");
		UTILS::API::StringReplace(szTxt, ";x", "_+_", szBuff,128);
		std::cout << "源:" << szTxt << std::endl;
		std::cout << "Replace:"<<szBuff << std::endl;
		std::cout << std::endl;

		_snprintf_s(szTxt, _TRUNCATE, "'GIS;xChann'el_';ssssssssx'");
		UTILS::API::StringReplace(szTxt, "'", "\'", szBuff, 128);
		std::cout << "源:" << szTxt << std::endl;
		std::cout << "Replace:" << szBuff << std::endl;
		std::cout << std::endl;

		std::vector<std::string> v;
		UTILS::API::StrTok(szTxt, "'", v, FALSE);
		std::cout << "SRC:" << szTxt << std::endl;
		for (auto& it : v)
		{
			std::cout << ":"<<it.data() << std::endl;
		}
		std::cout << std::endl;

		_snprintf_s(szTxt, _TRUNCATE, "GIS;xChann'el_';ssssssssx'");
		UTILS::API::StringReplace(szTxt, "'", "\'", szBuff, 128);
		std::cout << "源:" << szTxt << std::endl;
		std::cout << "Replace:" << szBuff << std::endl;
		std::cout << std::endl;

		_snprintf_s(szTxt, _TRUNCATE, "'GIS;xChann'el_';ssssssssx");
		UTILS::API::StringReplace(szTxt, "'", "\'", szBuff, 128);
		std::cout << "源:" << szTxt << std::endl;
		std::cout << "Replace:" << szBuff << std::endl;
		std::cout << std::endl;

		_snprintf_s(szTxt, _TRUNCATE, "GISxChann'el_'xssssssss");
		UTILS::API::StringReplace(szTxt, "x", "v", szBuff, 128);
		std::cout << "源:" << szTxt << std::endl;
		std::cout << "Replace:" << szBuff << std::endl;
		std::cout << std::endl;
		_snprintf_s(szTxt, _TRUNCATE, "xGISxChann'el_'xssssssssx");
		UTILS::API::StringReplace(szTxt, "x", "v", szBuff, 128);
		std::cout << "源:" << szTxt << std::endl;
		std::cout << "Replace:" << szBuff << std::endl;
		std::cout << std::endl;
		_snprintf_s(szTxt, _TRUNCATE, "xGISxChann'el_'xssssssss");
		UTILS::API::StringReplace(szTxt, "x", "v", szBuff, 128);
		std::cout << "源:" << szTxt << std::endl;
		std::cout << "Replace:" << szBuff << std::endl;
		std::cout << std::endl;
		_snprintf_s(szTxt, _TRUNCATE, "GISxChann'el_'xssssssssx");
		UTILS::API::StringReplace(szTxt, "x", "v", szBuff, 128);
		std::cout << "源:" << szTxt << std::endl;
		std::cout << "Replace:" << szBuff << std::endl;
		std::cout << std::endl;
	}
	std::cin.ignore();
	{
		char szTxt[64];
		_snprintf_s(szTxt, _TRUNCATE, "GIS;Channel_id:2;");
		char szFlag[16], szContent[256];
		szFlag[0] = '\0';
		UTILS::API::StrTok(szTxt, ";", 0, szFlag, 16, FALSE);
		std::cout << szFlag << std::endl;
		szContent[0] = '\0';
		UTILS::API::StrTok(szTxt, ";", 1, szContent, 256, FALSE);
		std::cout << szContent << std::endl;

		_snprintf_s(szTxt, _TRUNCATE, "GIS;,sdfwe;,sdf;wef;:2;sdfwdf,sdfwef");
		std::vector<std::string> v;
		szFlag[0] = '\0';
		UTILS::API::StrTok(szTxt, ";,",v, TRUE);
		std::cout << "SRC:"<<szTxt << std::endl;
		for (auto& it : v)
		{
			std::cout << it.data() << std::endl;
		}
		szContent[0] = '\0';
		UTILS::API::StrTok(szTxt, ";,", 2, szContent, 256, TRUE);
		std::cout <<"2:    "<< szContent<<std::endl;
		std::cout << std::endl;

		v.clear();
		UTILS::API::StrTok(szTxt, ";,", v, FALSE);
		for (auto& it : v)
		{
			std::cout << it.data() << std::endl;
		}

		szContent[0] = '\0';
		UTILS::API::StrTok(szTxt, ";,", 2, szContent, 256, FALSE);
		std::cout << "2:    " << szContent << std::endl;

		std::cout << "END" << std::endl;
	}
	std::cin.ignore();
	{
		BYTE byValue[] = { 0xAA, 0xAA, 0x02, 0x00, 0x01, 0x00, 0x0E, 0x00, 0x00, 0x01, 0x42, 0xCF, 0xB0, 0x0A, 0x41, 0xF3, 0x60, 0x3F, 0x42, 0x07, 0x20, 0xA8,
			0x03, 0xBE, 0xFF, 0x4C, 0xF1, 0x1D, 0x5D, 0xEB, 0x04, 0x00, 0x00, 0x02, 0xA6, 0x42, 0xCF, 0xAF, 0x6F, 0x41, 0xF3, 0x53, 0xEC, 0x06,
			0xA4, 0x00, 0x00, 0x75, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48};
		//BYTE byValue[] = { 0xAA, 0xAA, 0x02, 0x00, 0x01, 0x00, 0x0E, 0x00, 0x00, 0x01, 0x42, 
		//	0xCF, 0xB0, 0x88, 0x41, 0xF3, 0x65, 0xD4, 0x41, 0x65, 0xDC, 0x1B, 0x04
		//	, 0xA9, 0xFF, 0x4C, 0xD6, 0x1D, 0x5D, 0xEB, 0x03, 0x00, 0x00, 0x03, 0xE1, 0x42, 0xCF, 0xAF, 0x6F, 0x41
		//	, 0xF3, 0x53, 0xEC, 0x06, 0xA4, 0x00, 0x00, 0x75, 0x30, 0x00, 0x00 
		//	, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C };

		BYTE byTmp = 0xAA^0xAA;
		for (int i = 2; i < 63; i++)
		{
			byTmp ^= byValue[i];
		}
		std::cout << (int)byTmp << std::endl;
	}
	std::cin.ignore();
	{
		std::cin.ignore();
		BYTE buff[] = { 0x19, 0x57, 0xD0, 0x42 };
		float tmp = UTILS::API::ByteToFloat(buff);
		char szTmp[64];
		_snprintf_s(szTmp, _TRUNCATE, "%f", tmp);
		std::cout << tmp << "\t" << szTmp << std::endl;

		std::cout << sizeof(float) << std::endl;
		BYTE data[4];
		memcpy(data, &tmp, min(sizeof(float), 4));
		_snprintf_s(szTmp, _TRUNCATE, "%02x %02x %02x %02x", data[0], data[1], data[2], data[3]);
		std::cout << szTmp << std::endl;
	}
	std::cin.ignore();
	{
		UTILS::CFile file("e:\\中文\\new.txt");
		std::cout << file.Open(UTILS::PATH_FILE_OPENMODE_OUT | UTILS::PATH_FILE_OPENMODE_BINARY) << std::endl;
		file.Close();
	}

	std::cin.ignore();
	{
		std::map<int, CTest> mapTests;

		mapTests.emplace(1, *(new CTest()));
		//mapTests.insert({ 2, *(new CTest()) });
		//mapTests[3] = *(new CTest());
		std::cout << mapTests.size() << std::endl;
		for (auto& it : mapTests)
		{
			std::cout << it.first << "\t"<< std::endl;
		}
	}

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
#if 0
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
#endif
	}


	std::cin.ignore();
	{
#if 0
		char key[] = "2E3234F6591B4E69";
		char* pkey = key;
		char iv[] = "B91D10A7BE5B1FAB";
		char* piv = iv;

		char file2[] = "E:\\SvnWorking\\SampleCode\\OpfsUpdate\\bin\\tmp_folder\\091622C9-0D08-4C6A-B96C-B34DF9CA66D5.patch";
		char* pfile2 = file2;

		int err = UTILS::API::DecryptionFile(pfile2, "x.zip", pkey, piv);
		std::cout << "解密:" << err << std::endl;
#endif
	}

	std::cin.ignore();
	{
		std::cout<<CopyFile("d:\\zeno.log", "e:\\zeno.log", FALSE)<<std::endl;
		std::cout << CopyFile("d:\\zeno.log", "e:\\SDK", FALSE) << std::endl;
	}
	std::cin.ignore();
	return 0;
}


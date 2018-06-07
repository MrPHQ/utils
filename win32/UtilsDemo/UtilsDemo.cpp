// UtilsDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <functional>
#include <thread>
#include <mutex> // std::mutex, std::unique_lock

#include <utils\utils.h>
#include <utils\Thread.h>
#include <iostream>
#pragma comment(lib, "utils/utils.lib")
UTILS::CProcessLock lock;

/// ģ���˳�֪ͨ�¼�
#define TEST_EVENT "Global\\{ECEF3391-EC8B-487F-8BBB-AE444B4EF4E2}"
#define TEST_MUTEX "Global\\{AAC6E35D-537B-4CC4-A935-E18B8780C04A}"

int _tmain(int argc, _TCHAR* argv[])
{
	bool ret = lock.Init(TEST_MUTEX, TEST_EVENT);
	std::cout << "..Init.. " << (ret ? "..Init" : "..Init file") << std::endl;
	ret = lock.WaitAck(30000);
	std::cout << "..�ȴ���.. " << (ret ? "..ack" : "..ack file") << std::endl;
	lock.Ack();
	lock.Lock();
	std::cout << "..�����ɹ�" << std::endl;
	lock.UnLock();
	std::cout << "..�ͷ���" << std::endl;
	Sleep(1000);
	std::cout << "..�ȴ���,�ȴ�5s" << std::endl;
	ret = lock.WaitLock(5000);
	std::cout << (ret ? "..���ɹ�" : "..��ʧ��") << std::endl;
	if (ret){
		lock.UnLock();
		std::cout << "..�ͷ���" << std::endl;
	}
	std::cin.ignore();
	return 0;
}


#ifndef __UTILS_SHAREMEMORY__
#define __UTILS_SHAREMEMORY__

#include <utils/config.h>
#include <utils/Thread.h>

// Start of namespace section

namespace UTILS
{
#define PROC_FLAG 0x0f0a0b0c
	/// 进程通信数据包.
	struct CommunicatorPacket_t
	{
		DWORD dwFlag; //PROC_FLAG
		DWORD dwType; //数据包类型
		int iLen; //数据长度
		BYTE bData[4];	//数据段
	};
	/// 进程通讯器类型.
	enum CommunicatorType {
		CT_READER = 0,	//只读
		CT_WRITER	//写入
	};
#ifdef WIN32
	typedef struct _SHARE_MEMORY_PROPERTY
	{
		/// 共享数据段名称.
		char szShareSegName[128];
		/// 数据变更事件名称.
		char szChangeEventName[128];
		/// 数据段互斥锁名称.
		char szMutexName[128];
	}SHARE_MEMORY_PROPERTY,*PSHARE_MEMORY_PROPERTY;
#endif
/**
\brief
	MyUtilException class to Exception.
*/
class UTILS_API CShareMemory
{
public:
	CShareMemory();
	~CShareMemory();

	/**
	\brief
		初始化进程通讯器
	\param nType
		类型.
	\param buff
		.
	\param iBuffLen
		.
	\param dwLength
		共享内存最大长度
	\return
		.
	*/
	int	Init(CommunicatorType nType, PSHARE_MEMORY_PROPERTY pProperty,DWORD dwLength);
	/**
	\brief
		释放资源
	*/
	void UnInit();
	/**
	\brief
		将数据写入共享缓存，初始化为CT_WRITER时有效
	*/
	int WriteData(CommunicatorPacket_t* pData);
	/**
	\brief
		注册获取共享缓存中写入的数据的回调函数，初始化为CT_READER时有效
	*/
	/*void RegisterDataCallback(std::function<void(CommunicatorPacket_t* lpPacket, void* pUsrPar)> pCallback, 
		void* pUsrPar);*/
	/**
	\brief
		清空数据，初始化为CT_READER时有效
	*/
	void ResetData();

protected:
	/**
	\brief
		数据读取线程处理函数
	*/
	static void  ReadDataThread(BOOL& bRun, HANDLE hWait, void* pUsrPar);
	void ReadData(BOOL& bRun, HANDLE hWait);

protected:
	/// 是否已经初始化.
	BOOL m_bInit; 
#ifdef WIN32
	/// 通讯器类型.
	CommunicatorType m_nType;
	/// 命名
	HANDLE m_hFileMapping;
	/// 内容变更通知.
	HANDLE m_hChangeEvent;
	/// 数据互斥锁.
	HANDLE m_hDataMutex;
	/// 数据读取线程，仅当创建为读取模式时创建
	CThreadBox m_read_thr;
	/// 数据输出回调函数
	//std::function<void(CommunicatorPacket_t* lpPacket, void* pUsrPar)> m_pfnOutPutData;
	void* m_pUsrpar;
	/// 共享内存基地址.
	BYTE* m_pBasePointer; 
#endif
};

} // End of  namespace section

#endif
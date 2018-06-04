#ifndef __UTILS_THREAD__
#define __UTILS_THREAD__

#include <utils/config.h>
#include <utils/define.h>
namespace UTILS
{
	/**
	\brief
		THREAD class.
	*/
	class UTILS_API CCritSec
	{
	public:
		CCritSec();
		~CCritSec();
		void Lock();
		void Unlock();
		bool TryLock(unsigned int timeout /*= 5000*/);

		CCritSec(CCritSec const &) = delete;
		CCritSec(CCritSec &&) = delete;
		CCritSec & operator = (CCritSec const &) = delete;
		CCritSec & operator = (CCritSec &&) = delete;
	public:
#ifdef WIN32
		CRITICAL_SECTION mutex;
#else
		pthread_mutex_t mutex;
		pthread_mutexattr_t mutex_attr;
#endif
	};

	class UTILS_API CAutoLock
	{
		CAutoLock(CAutoLock const &) = delete;
		CAutoLock(CAutoLock &&) = delete;
		CAutoLock & operator = (CAutoLock const &) = delete;
		CAutoLock & operator = (CAutoLock &&) = delete;

	protected:
		UTILS::CCritSec* m_pLock;

	public:
		CAutoLock(UTILS::CCritSec * plock){
			assert(plock != nullptr);
			m_pLock = plock;
			m_pLock->Lock();
		};

		~CAutoLock(){
			m_pLock->Unlock();
		};
	};

	/**
	\brief
		CLock class.
		多线程锁, [mutex] + [condition_variable]
	*/
	class UTILS_API CLock
	{
		CLock(CLock const &) = delete;
		CLock(CLock &&) = delete;
		CLock & operator = (CLock const &) = delete;
		CLock & operator = (CLock &&) = delete;
	public:
		CLock();
		~CLock();
	private:
		std::mutex _mtx;
		std::condition_variable _cv;
		/// 正在使用,其他线程不允许使用.
		bool _using;
		/// 等待答复
		bool _ack;
	public:
		/**
		\brief
			手动加锁
		*/
		bool Lock();
		/**
		\brief
			手动释放锁
		*/
		void UnLock();
		/**
		\brief
			其他其他线程处理结果
		\param uiTimeOut
			等待时间(毫秒).
		\return bool
			被激活返回 true ，超时或其他情况返回 false
		*/
		bool WaitAck(std::unique_lock<std::mutex>&, unsigned int uiTimeOut);
		/**
		\brief
			其他线程处理完成
		*/
		void Ack();
		/**
		\brief
			获取互斥对象
		*/
		std::mutex& GetMutex() { return _mtx; }
	};

	typedef void(*BOX_THREAD_PROCESS)(BOOL& bRun, HANDLE hWait, void* context);

	class UTILS_API CThreadBox
	{
	public:
		CThreadBox();
		virtual ~CThreadBox();

		static bool Run(DWORD(WINAPI*)(void*),void* pParam);
		static bool RunEx(std::function<void(void*)>, void* pParam);
	public:
		int Init(int iPacketSize = 1024, int iPacketNum = 10, BOOL bPacket = TRUE, BOOL bCreateEvent = FALSE);
		void UnInit();
		int Start(BOX_THREAD_PROCESS lpProcess, void* context, const char* lpszName = "");
		int Start(std::function<void(BOOL& bRun, HANDLE hWait, void* context)>, const char* lpszName = "");
		int Stop(uint32_t uiTimeOut = 5000);
		int WriteData(void* pData, int iDataLen, BOOL bEvent = TRUE, BOOL bForceWrite = FALSE);
		int WriteDataEx(void* pData[], int iDataLen[], int iNum, BOOL bEvent = TRUE, BOOL bForceWrite = FALSE);
		int ReadData(void* pData, int iDataLen, int* lpRequestSize = NULL, BOOL bForceGet = FALSE);
		int ReadDataEx(void* pHead, int iHeadLen, void* pData, int iDataLen, int* lpRequestSize = NULL, BOOL bForceGet = FALSE);
		BOOL IsDone();
		BOOL IsData();
		void ResetData();
		int Cancel();
		int Size();

		int OpenEvent();
		void CloseEvent();
		void SetEvent();
		HANDLE GetEventHandle();
		void ResetEvent();

		BOOL CheckSize(int size);
		DWORD GetCurrentThreadId();

	protected:
		static THREAD_RET WINAPI Process(void* lpThis);

	protected:
		BOOL m_bInit;
		HTHREAD m_hObject;
		HANDLE m_hWait;
		BOOL m_bDone;
		BOOL m_bRun;
		uint32_t m_dwID;
		BOOL m_bCreateEvent;
		BOOL m_bPacket;

		BOX_THREAD_PROCESS	m_pProcess;
		void* m_ProcessContext;
		std::function<void(BOOL& bRun, HANDLE hWait, void* context)> m_cb;
		char m_szThreadName[MAX_PATH];

		BYTE* m_pBuffer;
		int m_iBufferLen;
		int m_iWritePos;
		int m_iReadPos;
		int m_iDataLen;

		UTILS::CCritSec m_mutexPacket;
	};
}


#endif	//__AUTOLOCK_H__

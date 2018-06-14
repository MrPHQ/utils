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
		CCritSec& operator= (CCritSec const &) = delete;
		CCritSec& operator= (CCritSec &&) = delete;
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
		CAutoLock& operator= (CAutoLock const &) = delete;
		CAutoLock& operator= (CAutoLock &&) = delete;

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
	\brief 多线程锁.
		[mutex] + [condition_variable]
	*/
	class UTILS_API CLock
	{
		CLock(CLock const &) = delete;
		CLock(CLock &&) = delete;
		CLock& operator= (CLock const &) = delete;
		CLock& operator= (CLock &&) = delete;
	public:
		CLock();
		~CLock();
	private:
		/** 互斥对象*/
		std::mutex _mtx;
		/** 条件变量*/
		std::condition_variable _cv;
		/** 某个线程正在使用*/
		bool _using;
		/** 等待答复*/
		bool _ack;
		/** 自定义数据*/
		struct{
			BYTE* buff;
			int len;
		}_context;
	public:
		/**
		\brief 手动加锁.
			内部会判断[_using]标识,如果true,则返回失败.
		*/
		bool Lock();
		/**
		\brief 手动释放锁.
		*/
		void UnLock();
		/**
		\brief 等待其他线程处理结果.
			内部是调用条件变量 _cv.wait_for
		\param mt
			必须是同一实例的 [_mtx]
		\param uiTimeOut
			等待时间(毫秒).
		\return bool
			被激活返回 true ，超时或其他情况返回 false
		*/
		bool WaitAck(std::unique_lock<std::mutex>& mt, unsigned int uiTimeOut);
		/**
		\brief 其他线程处理完成.
			触发条件变量条件满足,调用此接口,外部不需要手动加锁.
		*/
		void Ack();
		/**
		\brief 获取互斥对象.
		*/
		std::mutex& GetMutex() { return _mtx; }
		/**
		\brief 判断其他线程是否已经锁定互斥对象.
		*/
		bool IsBusy() const { return _using; }
		/**
		\brief 复位自定义数据缓存区.
		*/
		void ResetContextData() { 
			_context.buff = nullptr; 
			_context.len = 0;
		}
		/**
		\brief 设置自定义数据.
		\param buff
			数据缓存区
		\param len
			数据缓存区大小
		*/
		void SetContextData(BYTE* buff, int len) {
			_context.buff = buff;
			_context.len = len;
		}
		/**
		\brief 拷贝数据.
		\param buff
			数据指针
		\param len
			数据大小
		*/
		void PutContextData(BYTE* buff, int len) {
			if (_context.buff != nullptr && (buff != nullptr)){
				memcpy(_context.buff, buff, min(len, _context.len));
			}
		}

		/**
		\brief 获取自定义数据.
		*/
		BYTE* GutContextData() {
			return _context.buff;
		}
	};

	/**
	\brief 多进程锁.
		[互斥量] + [事件]
	*/
	class UTILS_API CProcessLock
	{
		CProcessLock(CProcessLock const &) = delete;
		CProcessLock(CProcessLock &&) = delete;
		CProcessLock& operator= (CProcessLock const &) = delete;
		CProcessLock& operator= (CProcessLock &&) = delete;
	public:
		/**
		\brief 构造函数.
		\param 
		*/
		CProcessLock();
		~CProcessLock();
		/**
		\brief 初始化.
		*/
		bool Init(const char* lpMutex = nullptr, const char* lpEvent = nullptr);
		/**
		\brief 手动加锁.
		*/
		bool Lock();
		/**
		\brief 手动加锁,等待超时时间.
		\param uiTimeOut
			等待时间(毫秒).
		\return bool
			被激活返回 true ，超时或其他情况返回 false
		*/
		bool WaitLock(unsigned int uiTimeOut);
		/**
		\brief 手动释放锁.
		*/
		bool UnLock();
		/**
		\brief 等待其他进程处理结果.
		\param uiTimeOut
			等待时间(毫秒).
		\return bool
			被激活返回 true ，超时或其他情况返回 false
		*/
		bool WaitAck(unsigned int uiTimeOut);
		/**
		\brief 其他进程处理完成.
		*/
		void Ack();
	private:

		/** 互斥量句柄*/
#ifdef WIN32
		void* m_pMutex;
#endif
		/** 事件句柄*/
#ifdef WIN32
		void* m_pEvent;
#endif
	#ifdef linux
		sem_t* m_pSem;
	#endif
		/** 互斥量名称*/
		char m_szMutexName[64];
		/** 事件名称*/
		char m_szEventName[64];
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

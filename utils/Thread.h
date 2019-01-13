#ifndef __UTILS_THREAD__
#define __UTILS_THREAD__

#include <utils/config.h>
#include <utils/define.h>
namespace UTILS
{
	namespace LOCK_FLAG
	{
		// LOCK PROPERTIES
		struct __adopt_lock_t
		{	// indicates adopt lock
		};

		struct __defer_lock_t
		{	// indicates defer lock
		};

		const __adopt_lock_t lock_adopt;
		const __defer_lock_t lock_defer;
	}

	/**
	* \brief 线程、线程池、线程锁
	*/

	/**
	* \brief 临界区封装类
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

	/**
	* \brief 读写锁.
	*/
	class UTILS_API CRWLock
	{
		CRWLock(CRWLock const &) = delete;
		CRWLock(CRWLock &&) = delete;
		CRWLock& operator= (CRWLock const &) = delete;
		CRWLock& operator= (CRWLock &&) = delete;
	public:
		CRWLock();
		~CRWLock();
		/**
		* \brief 读锁
		* \return bool 被激活返回 true 其他情况返回 false
		*/
		bool RLock();
		/**
		* \brief 读锁
		* \return bool 被激活返回 true 其他情况返回 false
		*/
		bool TryRLock();
		/**
		* \brief 释放读锁
		*/
		void RUnLock();
		/**
		* \brief 写锁
		* \return bool 被激活返回 true 其他情况返回 false
		*/
		bool WLock();
		/**
		* \brief 写锁
		* \return bool 被激活返回 true 其他情况返回 false
		*/
		bool TryWLock();
		/**
		* \brief 释放写锁
		*/
		void WUnLock();
	private:
		int Init();
		void Destroy();
		bool IsInit() const { return init_; }
	private:
		/**< 释放初始化.*/
		bool init_;
		/**< 读者数量.*/
		unsigned int num_readers_;
		/**< 读.临界资源.*/
		CRITICAL_SECTION num_readers_lock_;
		/**< 写.信号量.*/
		HANDLE write_semaphore_;
	};

	class CCRWLock;
	class UTILS_API CAutoLock
	{
		CAutoLock(CAutoLock const &) = delete;
		CAutoLock(CAutoLock &&) = delete;
		CAutoLock& operator= (CAutoLock const &) = delete;
		CAutoLock& operator= (CAutoLock &&) = delete;

	protected:
		void* m_pLock;
		enum LOCK_TYPE
		{
			LOCK_TYPE_NONE = 0,
			/**< CCritSec.*/
			LOCK_TYPE_CRITSEC = 1,
			/**< CRWLock RLOCK.*/
			LOCK_TYPE_RWLOCK_RD = 2,
			/**< CRWLock WLOCK.*/
			LOCK_TYPE_RWLOCK_WR = 3
		};
		LOCK_TYPE m_nLockType;
		BOOL m_owns;
	public:
		CAutoLock(UTILS::CCritSec* plock)
		{
			assert(plock != nullptr);
			m_owns = FALSE;
			m_pLock = plock;
			UTILS::CCritSec* p = static_cast<UTILS::CCritSec*>(m_pLock);
			p->Lock();
			m_owns = TRUE;
			m_nLockType = LOCK_TYPE_CRITSEC;
		};
		CAutoLock(CCritSec * plock, LOCK_FLAG::__adopt_lock_t)
		{
			assert(plock != nullptr);
			m_owns = FALSE;
			m_pLock = plock;
			UTILS::CCritSec* p = static_cast<UTILS::CCritSec*>(m_pLock);
			p->Lock();
			m_owns = TRUE;
			m_nLockType = LOCK_TYPE_CRITSEC;
		};
		CAutoLock(CCritSec * plock, LOCK_FLAG::__defer_lock_t)
		{
			assert(plock != nullptr);
			m_owns = FALSE;
			m_pLock = plock;
			m_nLockType = LOCK_TYPE_CRITSEC;
		};

		CAutoLock(UTILS::CRWLock* plock, bool read = true)
		{
			assert(plock != nullptr);
			m_pLock = plock;
			m_nLockType = read ? LOCK_TYPE_RWLOCK_RD : LOCK_TYPE_RWLOCK_WR;
			if (read){
				plock->RLock();
			}
			else{
				plock->WLock();
			}
		};
		/**
		* \brief 是否加锁成功. LOCK_TYPE_CRITSEC
		*/
		BOOL Owns()
		{
			if (m_nLockType != LOCK_TYPE_CRITSEC)
			{
				return FALSE;
			}
			return m_owns;
		}
		/**
		* \brief 加锁.直到加锁成功.才返回。 LOCK_TYPE_CRITSEC
		*/
		BOOL Lock()
		{
			if (NULL == m_pLock)
			{
				return FALSE;
			}
			UTILS::CCritSec* p = static_cast<UTILS::CCritSec*>(m_pLock);
			p->Lock();
			m_owns = TRUE;
			return TRUE;
		}
		/**
		* \brief 加锁.超时返回。 LOCK_TYPE_CRITSEC
		* \param uiTimeOut	超时时间.
		* \return 返回 true加锁成功,否则失败
		*/
		BOOL Lock(unsigned int uiTimeOut)
		{
			if (NULL == m_pLock)
			{
				return FALSE;
			}
			UTILS::CCritSec* p = static_cast<UTILS::CCritSec*>(m_pLock);
			if (p->TryLock(uiTimeOut))
			{
				m_owns = TRUE;
			}
			return m_owns;
		}


		~CAutoLock()
		{
			if (m_nLockType == LOCK_TYPE_CRITSEC)
			{
				UTILS::CCritSec* p = static_cast<UTILS::CCritSec*>(m_pLock);
				if (p){
					if (Owns())
					{
						p->Unlock();
					}
				}
			}
			else if ((m_nLockType == LOCK_TYPE_RWLOCK_RD) || (m_nLockType == LOCK_TYPE_RWLOCK_WR))
			{
				UTILS::CRWLock* p = static_cast<UTILS::CRWLock*>(m_pLock);
				if (p){
					if (m_nLockType == LOCK_TYPE_RWLOCK_RD){
						p->RUnLock();
					}
					else{
						p->WUnLock();
					}
				}
			}
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

	/**
	* \brief 线程封装类.
	*/
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

	typedef void(*THREAD_POOL_PROCESS)(void* lpTask, int iTaskDataLen, void* lpUsr);

	/**
	* \brief 线程封装类.
	*/
	class CThreadPool;
	class UTILS_API CThreadWrapper
	{
		friend class CThreadPool;
	private:
		CThreadWrapper();
		~CThreadWrapper();

	public:
		/**
		* \brief 启动线程.
		* \param lpProcCallBack 任务处理回调函数
		* \param lpUsr 自定义上下文
		*/
		int Start(int iPacketSize, int iPacketNum, THREAD_POOL_PROCESS lpProcCallBack, void* lpUsr);
		/**
		* \brief 停止线程
		*/
		void Stop();

		/**
		* \brief 添加任务
		* \param lpTask 任务数据缓冲区, 不能为 NULL, 否则返回参数错误
		* \param iTaskDataLen 任务数据大小
		*/
		int Task(void* lpTask, int iTaskDataLen);
		/**
		* \brief 读取空闲时间(持续多少毫秒)
		*/
		int GetIdleTime();
		/**
		* \brief 是否发生错误
		*/
		bool IsError();
		/**
		* \brief 是否空闲状态
		*/
		bool IsIdle();
		operator bool(){return !IsError();}
	private:
		/**
		* \brief 
		*/
		void __UpdateIdleTime(unsigned int);
		/**
		* \brief
		*/
		void __UpdateError(int);
	private:
		/**工作线程*/
		UTILS::CThreadBox m_Thread;
		/**空闲时间戳*/
		unsigned int m_uiIdleTimestamp;
		/**锁*/
		UTILS::CCritSec m_lock;
		/**读取任务缓冲区*/
		struct  {
			int iBufferLen;
			int iDataLen;
			BYTE buff[1024 * 8];
			BYTE* lpBuff;
		}m_Task;
		/**内部发生错误*/
		int m_iError;
		/***/
		struct  {
			THREAD_POOL_PROCESS cb;
			void* lpUsr;
		}m_TaskCallBack;
	};

	/**
	* \brief 线程池封装类.
	*/
	class UTILS_API CThreadPool
	{
	public:
		CThreadPool();
		virtual ~CThreadPool();

		/**
		* \brief 资源初始化.
		* \param iThreadCnt 线程池线程个数上限
		* \param iTaskBufferLen 任务缓冲区总大小(字节),小于等于0x0FFFFFFF
		* \param iSingleTaskMaxLen 单个任务缓冲区最大长度(字节), 小于等于iTaskBufferLen
		* \param iMaxIdleTime 线程最大空闲时间(毫秒),达到最大空闲时间,线程会退出.
		*/
		int Init(int iThreadCnt, int iTaskBufferLen, int iSingleTaskMaxLen, int iMaxIdleTime);
		/**
		* \brief 停止线程池
		*/
		void UnInit();

		/**
		* \brief 启动线程池.
		* \param lpProcCallBack 任务处理回调函数
		* \param lpUsr 自定义上下文
		*/
		int Start(THREAD_POOL_PROCESS lpProcCallBack, void* lpUsr);
		/**
		* \brief 停止线程池
		*/
		void Stop();

		/**
		* \brief 添加任务
		* \param lpTask 任务数据缓冲区, 不能为 NULL, 否则返回参数错误
		* \param iTaskDataLen 任务数据大小
		* \return 0成功 ,否则失败
		*/
		int Task(void* lpTask, int iTaskDataLen);

	private:

		/**控制线程处理函数*/
		static void __CtrlThreadProc(BOOL& bRun, HANDLE hWait, void* context);
		void __CtrlThreadProcLoop(BOOL& bRun, HANDLE hWait);
		/****/
		static void __ThreadCallBack(void* lpTask, int iTaskDataLen, void* lpUsr);
		void __ThreadCallBackProc(void* lpTask, int iTaskDataLen);

		/**
		* \brief 得到一个线程实例
		*/
		CThreadWrapper* __GetThread();

		/**
		* \brief 检测处理线程
		*	超过最大的空闲时间,释放线程
		*/
		void __CheckProcThread();

		/***/
		BOOL __IsInit() const { return m_bInit; }
		/***/
		BOOL __IsStartUp() const { return m_bStartUp; }

	private:
		/**线程池控制线程*/
		UTILS::CThreadBox m_CtrlThread;
		/**工作线程集合*/
		std::list<CThreadWrapper*> m_lstThreads;
		/**是否初始化*/
		BOOL m_bInit;
		/**是否启动服务*/
		BOOL m_bStartUp;
		/**线程最大空闲时间(毫秒)*/
		int m_iMaxIdleTime;
		/**线程池最大线程数,上线256*/
		int m_iMaxThreadCnt;
		int m_iSingleTaskMaxLen;
		UTILS::CCritSec m_lock;
		/***/
		struct {
			THREAD_POOL_PROCESS lpPorc;
			void* lpUsr;
		}m_TaskCallBack;
		/**读取任务缓冲区*/
		struct  {
			int iBufferLen;
			int iDataLen;
			BYTE buff[1024 * 8];
			BYTE* lpBuff;
		}m_Task;
	};
}


#endif	//__AUTOLOCK_H__

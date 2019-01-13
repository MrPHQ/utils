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
	* \brief �̡߳��̳߳ء��߳���
	*/

	/**
	* \brief �ٽ�����װ��
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
	* \brief ��д��.
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
		* \brief ����
		* \return bool ������� true ����������� false
		*/
		bool RLock();
		/**
		* \brief ����
		* \return bool ������� true ����������� false
		*/
		bool TryRLock();
		/**
		* \brief �ͷŶ���
		*/
		void RUnLock();
		/**
		* \brief д��
		* \return bool ������� true ����������� false
		*/
		bool WLock();
		/**
		* \brief д��
		* \return bool ������� true ����������� false
		*/
		bool TryWLock();
		/**
		* \brief �ͷ�д��
		*/
		void WUnLock();
	private:
		int Init();
		void Destroy();
		bool IsInit() const { return init_; }
	private:
		/**< �ͷų�ʼ��.*/
		bool init_;
		/**< ��������.*/
		unsigned int num_readers_;
		/**< ��.�ٽ���Դ.*/
		CRITICAL_SECTION num_readers_lock_;
		/**< д.�ź���.*/
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
		* \brief �Ƿ�����ɹ�. LOCK_TYPE_CRITSEC
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
		* \brief ����.ֱ�������ɹ�.�ŷ��ء� LOCK_TYPE_CRITSEC
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
		* \brief ����.��ʱ���ء� LOCK_TYPE_CRITSEC
		* \param uiTimeOut	��ʱʱ��.
		* \return ���� true�����ɹ�,����ʧ��
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
	\brief ���߳���.
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
		/** �������*/
		std::mutex _mtx;
		/** ��������*/
		std::condition_variable _cv;
		/** ĳ���߳�����ʹ��*/
		bool _using;
		/** �ȴ���*/
		bool _ack;
		/** �Զ�������*/
		struct{
			BYTE* buff;
			int len;
		}_context;
	public:
		/**
		\brief �ֶ�����.
			�ڲ����ж�[_using]��ʶ,���true,�򷵻�ʧ��.
		*/
		bool Lock();
		/**
		\brief �ֶ��ͷ���.
		*/
		void UnLock();
		/**
		\brief �ȴ������̴߳�����.
			�ڲ��ǵ����������� _cv.wait_for
		\param mt
			������ͬһʵ���� [_mtx]
		\param uiTimeOut
			�ȴ�ʱ��(����).
		\return bool
			������� true ����ʱ������������� false
		*/
		bool WaitAck(std::unique_lock<std::mutex>& mt, unsigned int uiTimeOut);
		/**
		\brief �����̴߳������.
			��������������������,���ô˽ӿ�,�ⲿ����Ҫ�ֶ�����.
		*/
		void Ack();
		/**
		\brief ��ȡ�������.
		*/
		std::mutex& GetMutex() { return _mtx; }
		/**
		\brief �ж������߳��Ƿ��Ѿ������������.
		*/
		bool IsBusy() const { return _using; }
		/**
		\brief ��λ�Զ������ݻ�����.
		*/
		void ResetContextData() { 
			_context.buff = nullptr; 
			_context.len = 0;
		}
		/**
		\brief �����Զ�������.
		\param buff
			���ݻ�����
		\param len
			���ݻ�������С
		*/
		void SetContextData(BYTE* buff, int len) {
			_context.buff = buff;
			_context.len = len;
		}
		/**
		\brief ��������.
		\param buff
			����ָ��
		\param len
			���ݴ�С
		*/
		void PutContextData(BYTE* buff, int len) {
			if (_context.buff != nullptr && (buff != nullptr)){
				memcpy(_context.buff, buff, min(len, _context.len));
			}
		}

		/**
		\brief ��ȡ�Զ�������.
		*/
		BYTE* GutContextData() {
			return _context.buff;
		}
	};

	/**
	\brief �������.
		[������] + [�¼�]
	*/
	class UTILS_API CProcessLock
	{
		CProcessLock(CProcessLock const &) = delete;
		CProcessLock(CProcessLock &&) = delete;
		CProcessLock& operator= (CProcessLock const &) = delete;
		CProcessLock& operator= (CProcessLock &&) = delete;
	public:
		/**
		\brief ���캯��.
		\param 
		*/
		CProcessLock();
		~CProcessLock();
		/**
		\brief ��ʼ��.
		*/
		bool Init(const char* lpMutex = nullptr, const char* lpEvent = nullptr);
		/**
		\brief �ֶ�����.
		*/
		bool Lock();
		/**
		\brief �ֶ�����,�ȴ���ʱʱ��.
		\param uiTimeOut
			�ȴ�ʱ��(����).
		\return bool
			������� true ����ʱ������������� false
		*/
		bool WaitLock(unsigned int uiTimeOut);
		/**
		\brief �ֶ��ͷ���.
		*/
		bool UnLock();
		/**
		\brief �ȴ��������̴�����.
		\param uiTimeOut
			�ȴ�ʱ��(����).
		\return bool
			������� true ����ʱ������������� false
		*/
		bool WaitAck(unsigned int uiTimeOut);
		/**
		\brief �������̴������.
		*/
		void Ack();
	private:

		/** ���������*/
#ifdef WIN32
		void* m_pMutex;
#endif
		/** �¼����*/
#ifdef WIN32
		void* m_pEvent;
#endif
	#ifdef linux
		sem_t* m_pSem;
	#endif
		/** ����������*/
		char m_szMutexName[64];
		/** �¼�����*/
		char m_szEventName[64];
	};

	typedef void(*BOX_THREAD_PROCESS)(BOOL& bRun, HANDLE hWait, void* context);

	/**
	* \brief �̷߳�װ��.
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
	* \brief �̷߳�װ��.
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
		* \brief �����߳�.
		* \param lpProcCallBack ������ص�����
		* \param lpUsr �Զ���������
		*/
		int Start(int iPacketSize, int iPacketNum, THREAD_POOL_PROCESS lpProcCallBack, void* lpUsr);
		/**
		* \brief ֹͣ�߳�
		*/
		void Stop();

		/**
		* \brief �������
		* \param lpTask �������ݻ�����, ����Ϊ NULL, ���򷵻ز�������
		* \param iTaskDataLen �������ݴ�С
		*/
		int Task(void* lpTask, int iTaskDataLen);
		/**
		* \brief ��ȡ����ʱ��(�������ٺ���)
		*/
		int GetIdleTime();
		/**
		* \brief �Ƿ�������
		*/
		bool IsError();
		/**
		* \brief �Ƿ����״̬
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
		/**�����߳�*/
		UTILS::CThreadBox m_Thread;
		/**����ʱ���*/
		unsigned int m_uiIdleTimestamp;
		/**��*/
		UTILS::CCritSec m_lock;
		/**��ȡ���񻺳���*/
		struct  {
			int iBufferLen;
			int iDataLen;
			BYTE buff[1024 * 8];
			BYTE* lpBuff;
		}m_Task;
		/**�ڲ���������*/
		int m_iError;
		/***/
		struct  {
			THREAD_POOL_PROCESS cb;
			void* lpUsr;
		}m_TaskCallBack;
	};

	/**
	* \brief �̳߳ط�װ��.
	*/
	class UTILS_API CThreadPool
	{
	public:
		CThreadPool();
		virtual ~CThreadPool();

		/**
		* \brief ��Դ��ʼ��.
		* \param iThreadCnt �̳߳��̸߳�������
		* \param iTaskBufferLen ���񻺳����ܴ�С(�ֽ�),С�ڵ���0x0FFFFFFF
		* \param iSingleTaskMaxLen �������񻺳�����󳤶�(�ֽ�), С�ڵ���iTaskBufferLen
		* \param iMaxIdleTime �߳�������ʱ��(����),�ﵽ������ʱ��,�̻߳��˳�.
		*/
		int Init(int iThreadCnt, int iTaskBufferLen, int iSingleTaskMaxLen, int iMaxIdleTime);
		/**
		* \brief ֹͣ�̳߳�
		*/
		void UnInit();

		/**
		* \brief �����̳߳�.
		* \param lpProcCallBack ������ص�����
		* \param lpUsr �Զ���������
		*/
		int Start(THREAD_POOL_PROCESS lpProcCallBack, void* lpUsr);
		/**
		* \brief ֹͣ�̳߳�
		*/
		void Stop();

		/**
		* \brief �������
		* \param lpTask �������ݻ�����, ����Ϊ NULL, ���򷵻ز�������
		* \param iTaskDataLen �������ݴ�С
		* \return 0�ɹ� ,����ʧ��
		*/
		int Task(void* lpTask, int iTaskDataLen);

	private:

		/**�����̴߳�����*/
		static void __CtrlThreadProc(BOOL& bRun, HANDLE hWait, void* context);
		void __CtrlThreadProcLoop(BOOL& bRun, HANDLE hWait);
		/****/
		static void __ThreadCallBack(void* lpTask, int iTaskDataLen, void* lpUsr);
		void __ThreadCallBackProc(void* lpTask, int iTaskDataLen);

		/**
		* \brief �õ�һ���߳�ʵ��
		*/
		CThreadWrapper* __GetThread();

		/**
		* \brief ��⴦���߳�
		*	�������Ŀ���ʱ��,�ͷ��߳�
		*/
		void __CheckProcThread();

		/***/
		BOOL __IsInit() const { return m_bInit; }
		/***/
		BOOL __IsStartUp() const { return m_bStartUp; }

	private:
		/**�̳߳ؿ����߳�*/
		UTILS::CThreadBox m_CtrlThread;
		/**�����̼߳���*/
		std::list<CThreadWrapper*> m_lstThreads;
		/**�Ƿ��ʼ��*/
		BOOL m_bInit;
		/**�Ƿ���������*/
		BOOL m_bStartUp;
		/**�߳�������ʱ��(����)*/
		int m_iMaxIdleTime;
		/**�̳߳�����߳���,����256*/
		int m_iMaxThreadCnt;
		int m_iSingleTaskMaxLen;
		UTILS::CCritSec m_lock;
		/***/
		struct {
			THREAD_POOL_PROCESS lpPorc;
			void* lpUsr;
		}m_TaskCallBack;
		/**��ȡ���񻺳���*/
		struct  {
			int iBufferLen;
			int iDataLen;
			BYTE buff[1024 * 8];
			BYTE* lpBuff;
		}m_Task;
	};
}


#endif	//__AUTOLOCK_H__

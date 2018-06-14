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

#include "internal.h"
#include "../utils/Thread.h"
#include "../utils/api.h"

#define PACKET_HEAD_FLAGS	0x64683030
typedef struct PacketHead_t
{
	uint32_t	dwFlags;
	uint32_t	dwLen;
}PacketHead_t;

namespace UTILS
{
	CCritSec::CCritSec()
	{
#ifdef _WIN32
		InitializeCriticalSection(&mutex);
#else
		pthread_mutexattr_init(&mutex_attr);
		pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init(&mutex, &mutex_attr);
#endif
	}

	CCritSec::~CCritSec()
	{
#ifdef _WIN32
		DeleteCriticalSection(&mutex);
#else
		pthread_mutexattr_destroy(&mutex_attr);
#endif
		
	}

	void CCritSec::Lock()
	{
#ifdef _WIN32
		EnterCriticalSection(&mutex);
#else
		pthread_mutex_lock(&mutex);
#endif
		
	}

	void CCritSec::Unlock()
	{
#ifdef _WIN32
		LeaveCriticalSection(&mutex);
#else
		pthread_mutex_unlock(mutex);
#endif
	
	}

	bool CCritSec::TryLock(unsigned int timeout /*= 5000*/) {
		BOOL ret = false;
		DWORD dwCut = GetTickCount();
		do
		{
			ret = TryEnterCriticalSection(&mutex);
			if (ret) {
				break;
			}
			if ((unsigned int)abs((int)(GetTickCount() - dwCut)) >= timeout) {
				break;
			}
			Sleep(10);
		} while (true);
		return ret ? true : false;
	}

	CLock::CLock()
	{
		_using = false;
	}

	CLock::~CLock()
	{
	}

	bool CLock::Lock()
	{
		_mtx.lock();
		if (_using){//其他线程正在使用
			_mtx.unlock();
			return false;
		}
		return true;
	}

	bool CLock::WaitAck(std::unique_lock<std::mutex>& lck,unsigned int uiTimeOut)
	{
		if (_using){
			return false;
		}

		_using = true;
		_ack = false;
		if (!_cv.wait_for(lck, std::chrono::milliseconds(uiTimeOut), [this]() ->bool {
			return _ack;
		})) {//std::cv_status::timeout
			_using = false;
			return false;
		}
		
		_using = false;
		return true;
	}

	void CLock::UnLock()
	{
		_mtx.unlock();
	}

	void CLock::Ack()
	{
		std::unique_lock<std::mutex> lck(_mtx);
		_ack = true;
		_cv.notify_one();
	}

#ifdef _WIN32
	CProcessLock::CProcessLock()
		:m_pMutex(nullptr), m_pEvent(nullptr)
	{

	}

	CProcessLock::~CProcessLock()
	{
		if (m_pEvent != nullptr){
			CloseHandle(m_pEvent);
			m_pEvent = nullptr;
		}
		if (m_pMutex != nullptr){
			CloseHandle(m_pMutex);
			m_pMutex = nullptr;
		}
	}

	bool CProcessLock::Init(const char* lpMutex /*= nullptr*/, const char* lpEvent /*= nullptr*/)
	{
		SECURITY_DESCRIPTOR stSecDes;
		::InitializeSecurityDescriptor(&stSecDes, SECURITY_DESCRIPTOR_REVISION);
		//参数三设置为NULL，相当于将object的安全级别降到了最低，所有的访问请求都将成功
		::SetSecurityDescriptorDacl(&stSecDes, TRUE, NULL, FALSE);
		SECURITY_ATTRIBUTES stSecAttr;
		// set SECURITY_ATTRIBUTES
		stSecAttr.nLength = sizeof SECURITY_ATTRIBUTES;
		stSecAttr.bInheritHandle = FALSE;
		stSecAttr.lpSecurityDescriptor = &stSecDes;

		memset(m_szMutexName, 0, sizeof(m_szMutexName));
		memset(m_szEventName, 0, sizeof(m_szEventName));
		if ((nullptr != lpMutex) && (strlen(lpMutex) > 0)){
			int min = (strlen(lpMutex)>(sizeof(m_szMutexName)-1)) ? (sizeof(m_szMutexName)-1) : strlen(lpMutex);
			strncpy_s(m_szMutexName, _TRUNCATE, lpMutex, min);

			m_pMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, m_szMutexName);
			if (m_pMutex == nullptr) {
				m_pMutex = CreateMutex(&stSecAttr, FALSE, m_szMutexName);
			}
		}
		if ((nullptr != lpEvent) && (strlen(lpEvent) > 0)){
			int min = (strlen(lpEvent)>(sizeof(m_szEventName)-1)) ? (sizeof(m_szEventName)-1) : strlen(lpEvent);
			strncpy_s(m_szEventName, _TRUNCATE, lpEvent, min);
			m_pEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, m_szEventName);
			if (m_pEvent == nullptr) {
				m_pEvent = ::CreateEvent(&stSecAttr, FALSE, FALSE, m_szEventName);
			}
		}
		if ((m_pEvent == nullptr) || (m_pMutex == nullptr)){
			OutputDebugString("CProcessLock Error....");
			return false;
		}
		return true;
	}

	bool CProcessLock::Lock()
	{
		if (NULL == m_pMutex){
			return false;
		}
    
		DWORD nRet = WaitForSingleObject(m_pMutex, INFINITE);
		if (nRet != WAIT_OBJECT_0){
			return false;
		}
		return true;
	}

	bool CProcessLock::WaitLock(unsigned int uiTimeOut)
	{
		if (NULL == m_pMutex){
			return false;
		}

		DWORD nRet = WaitForSingleObject(m_pMutex, uiTimeOut);
		if (nRet != WAIT_OBJECT_0){
			return false;
		}
		return true;
	}
	bool CProcessLock::UnLock()
	{
		if (m_pMutex == nullptr){
			return false;
		}
		return ReleaseMutex(m_pMutex) ? true : false;
	}

	bool CProcessLock::WaitAck(unsigned int uiTimeOut)
	{
		if (m_pEvent == nullptr){
			OutputDebugString("WaitAck Error....");
			return false;
		}
		DWORD dwWaitResult = WaitForSingleObject(m_pEvent, uiTimeOut);
		if (WAIT_OBJECT_0 == dwWaitResult) {
			return true;
		}
		return false;
	}

	void CProcessLock::Ack()
	{
		if (m_pEvent == nullptr){
			return;
		}
		SetEvent(m_pEvent);
	}

#endif

#ifdef linux

	CProcessLock::CProcessLock(const char* name)
	{
		memset(m_cMutexName, 0 ,sizeof(m_cMutexName));
		int min = strlen(name)>(sizeof(m_cMutexName)-1)?(sizeof(m_cMutexName)-1):strlen(name);
		strncpy(m_cMutexName, name, min);
		m_pSem = sem_open(name, O_RDWR | O_CREAT, 0644, 1);
	}

	CProcessLock::~CProcessLock()
	{
		int ret = sem_close(m_pSem);
		if (0 != ret)
		{
			printf("sem_close error %d\n", ret);
		}
		sem_unlink(m_cMutexName);
	}

	bool CProcessLock::Lock()
	{
		int ret = sem_wait(m_pSem);
		if (ret != 0)
		{
			return false;
		}
		return true;
	}

	bool CProcessLock::UnLock()
	{
		int ret = sem_post(m_pSem);
		if (ret != 0)
		{
			return false;
		}
		return true;
	}
#endif

	CRWLock::CRWLock():
		init_(false)
	{
		Init();
	}

	CRWLock::~CRWLock()
	{
		Destroy();
	}

	int CRWLock::Init()
	{
		if (init_){
			return 0;
		}
		/* Initialize the semaphore that acts as the write lock. */
		HANDLE handle = CreateSemaphore(NULL, 1, 1, NULL);
		if (handle == NULL)
			return -1;
		write_semaphore_ = handle;

		/* Initialize the critical section protecting the reader count. */
		InitializeCriticalSection(&num_readers_lock_);

		/* Initialize the reader count. */
		num_readers_ = 0;
		init_ = true;
		return 0;
	}

	void CRWLock::Destroy()
	{
		if (!init_){
			return;
		}
		DeleteCriticalSection(&num_readers_lock_);
		CloseHandle(write_semaphore_);
		init_ = false;
	}

	bool CRWLock::RLock()
	{
		if (!IsInit()){
			return false;
		}
		/* Acquire the lock that protects the reader count. */
		EnterCriticalSection(&num_readers_lock_);

		/* Increase the reader count, and lock for write if this is the first
		* reader.
		* 增加读取器数量，如果这是第一个读取器，则锁定写入
		*/
		if (++num_readers_ == 1) {
			DWORD r = WaitForSingleObject(write_semaphore_, INFINITE);
			if (r != WAIT_OBJECT_0){
				//log(GetLastError(), "WaitForSingleObject");
				return false;
			}
		}

		/* Release the lock that protects the reader count. */
		LeaveCriticalSection(&num_readers_lock_);
		return true;
	}

	bool CRWLock::TryRLock()
	{
		if (!IsInit()){
			return false;
		}
		int err;

		if (!TryEnterCriticalSection(&num_readers_lock_))
			return false;

		err = 0;

		if (num_readers_ == 0) {
			/* Currently there are no other readers, which means that the write lock
			* needs to be acquired.
			* 目前没有其他读取器，这意味着需要获取写锁
			*/
			DWORD r = WaitForSingleObject(write_semaphore_, 0);
			if (r == WAIT_OBJECT_0)
				num_readers_++;
			else if (r == WAIT_TIMEOUT)
				err = -1;
			else if (r == WAIT_FAILED){
				err = -2;
				//log(GetLastError(), "WaitForSingleObject");
			}

		}
		else {
			/* The write lock has already been acquired because there are other
			* active readers.
			* 已经获取了写锁定，因为还有其他活动读取器。
			*/
			num_readers_++;
		}

		LeaveCriticalSection(&num_readers_lock_);
		return err == 0;
	}

	void CRWLock::RUnLock()
	{
		if (!IsInit()){
			return;
		}
		EnterCriticalSection(&num_readers_lock_);

		if (--num_readers_ == 0) {
			if (!ReleaseSemaphore(write_semaphore_, 1, NULL)){
				//uv_fatal_error(GetLastError(), "ReleaseSemaphore");
			}
		}

		LeaveCriticalSection(&num_readers_lock_);
	}

	bool CRWLock::WLock()
	{
		if (!IsInit()){
			return false;
		}
		DWORD r = WaitForSingleObject(write_semaphore_, INFINITE);
		if (r != WAIT_OBJECT_0){
			//log(GetLastError(), "WaitForSingleObject");
			return false;
		}
		return true;
	}

	bool CRWLock::TryWLock()
	{
		if (!IsInit()){
			return false;
		}
		DWORD r = WaitForSingleObject(write_semaphore_, 0);
		if (r == WAIT_OBJECT_0){
			return true;
		}
		else if (r == WAIT_TIMEOUT){
			return false;
		}
		else{
			//log(GetLastError(), "WaitForSingleObject");
			return false;
		}
		return false;
	}

	void CRWLock::WUnLock()
	{
		if (!IsInit()){
			return;
		}
		if (!ReleaseSemaphore(write_semaphore_, 1, NULL)){
			//log(GetLastError(), "ReleaseSemaphore");
		}
	}

	CThreadBox::CThreadBox()
	{
		m_bInit = FALSE;

		m_hObject = 0;
		m_hWait = 0;
		m_bDone = TRUE;
		m_bRun = FALSE;
		m_dwID = 0;
		m_pProcess = 0;
		m_ProcessContext = 0;

		m_pBuffer = NULL;
		m_iBufferLen = 0;
		m_iWritePos = 0;
		m_iReadPos = 0;
		m_iDataLen = 0;

		m_szThreadName[0] = '\0';
	}

	CThreadBox::~CThreadBox()
	{
	}

	int	CThreadBox::Init(int iPacketSize, int iPacketNum, BOOL bPacket, BOOL bCreateEvent)
	{
		if (!m_bInit){
			if (m_pBuffer == NULL && (iPacketSize*iPacketNum) > 0){
				m_iBufferLen = ((iPacketSize * iPacketNum + 4095) / 4096) * 4096;
				m_pBuffer = (BYTE*)API::Malloc(m_iBufferLen);
				if (m_pBuffer == NULL){
					return -1;
				}
			}
			m_iWritePos = 0;
			m_iReadPos = 0;
			m_iDataLen = 0;

			m_bPacket = bPacket;
			m_bCreateEvent = bCreateEvent;
			m_bInit = TRUE;
		}

		return 0;
	}

	void CThreadBox::UnInit()
	{
		if (m_pBuffer != NULL){
			API::Free(m_pBuffer);
			m_pBuffer = NULL;
			m_iBufferLen = 0;
		}
		m_iWritePos = 0;
		m_iReadPos = 0;
		m_iDataLen = 0;

		if (m_hWait != 0){
			API::fnCloseHandle(m_hWait);
			m_hWait = 0;
		}
		m_bInit = FALSE;
	}

	bool CThreadBox::Run(DWORD(WINAPI* cb)(void*), void* pParam)
	{
		DWORD dwThreadId = 0;
		HANDLE hThread = CreateThread(0, 0, cb, pParam, 0, &dwThreadId);
		if (hThread) {
			CloseHandle(hThread);
			return true;
		}
		return false;
	}

	struct _CONTEXT_THREAD_PAR
	{
		std::function<void(void*)> cb;
		void* par;
	};
	DWORD WINAPI CallThreadFn(void* par){
		_CONTEXT_THREAD_PAR* p = (_CONTEXT_THREAD_PAR*)par;
		if (nullptr == p){
			return -1;
		}
		if (p->cb){
			p->cb(p->par);
		}
		delete p;
		return 0;
	}
	bool CThreadBox::RunEx(std::function<void(void*)> cb, void* pParam)
	{
		_CONTEXT_THREAD_PAR* p = new _CONTEXT_THREAD_PAR();
		if (nullptr == p){
			return false;
		}
		p->cb = cb;
		p->par = pParam;
		DWORD dwThreadId = 0;
		HANDLE hThread = CreateThread(0, 0, CallThreadFn, p, 0, &dwThreadId);
		if (hThread) {
			CloseHandle(hThread);
			return true;
		}
		return false;
	}

	BOOL CThreadBox::IsDone(){
		return m_bDone;
	}

	BOOL CThreadBox::IsData(){
		return (m_iDataLen>0);
	}

	void CThreadBox::ResetData(){
		CAutoLock lock(&m_mutexPacket);
		m_iWritePos = 0;
		m_iReadPos = 0;
		m_iDataLen = 0;
	}

	int CThreadBox::Start(std::function<void(BOOL& bRun, HANDLE hWait, void* context)> cb, const char* lpszName) {
		if (!m_bDone) {
			return -1;
		}
		m_cb = cb;
		if (m_hObject != 0) {
			API::fnCloseThread(m_hObject);
			m_hObject = 0;
		}

		m_bRun = TRUE;
		m_bDone = FALSE;
		if (m_hWait == 0 && m_bCreateEvent) {
			m_hWait = API::fnCreateEvent(0, FALSE, FALSE, 0);
		}
		m_hObject = API::fnCreateThread(Process, (void*)this, &m_dwID);
		if (m_hObject == 0) {
			m_bRun = FALSE;
			m_bDone = TRUE;
			return -2;
		}
		return 0;
	}

	int	CThreadBox::Start(BOX_THREAD_PROCESS lpProcess, void* context, const char* lpszName){
		if (!m_bDone){
			return -1;
		}

		//fnSprintfn(m_szThreadName, MAX_PATH, "%s", lpszName);

		m_pProcess = lpProcess;
		m_ProcessContext = context;

		if (m_hObject != 0){
			API::fnCloseThread(m_hObject);
			m_hObject = 0;
		}

		m_bRun = TRUE;
		m_bDone = FALSE;
		if (m_hWait == 0 && m_bCreateEvent){
			m_hWait = API::fnCreateEvent(0, FALSE, FALSE, 0);
		}
		m_hObject = API::fnCreateThread(Process, (void*)this, &m_dwID);
		if (m_hObject == 0){
			m_bRun = FALSE;
			m_bDone = TRUE;
			return -2;
		}
		return 0;
	}

	int	CThreadBox::Stop(uint32_t uiTimeOut)
	{
		if (m_hObject != 0){
			uint32_t dwRet;
			BOOL bExit;

			m_bRun = FALSE;
			if (m_hWait != 0){
				API::fnSetEvent(m_hWait);
			}

			dwRet = API::fnWaitForThreadExit(m_hObject, uiTimeOut);
			if (dwRet != WAIT_OBJECT_0){
				bExit = API::fnGetExitCodeThread(m_hObject, &dwRet);
				if (!bExit){
					API::fnTerminateThread(m_hObject, dwRet);
				}
			}
			m_bDone = TRUE;
		}
		if (m_hObject != 0){
			API::fnCloseThread(m_hObject);
			m_hObject = 0;
		}
		if (m_hWait != 0 && m_bCreateEvent){
			API::fnCloseHandle(m_hWait);
			m_hWait = 0;
		}
		return 0;
	}

	THREAD_RET WINAPI CThreadBox::Process(void* lpThis){
		static int	thread_num = 0;
		CThreadBox* pBox = (CThreadBox*)lpThis;

		thread_num += 1;

		if (pBox->m_cb) {
			pBox->m_cb(pBox->m_bRun, pBox->m_hWait, pBox->m_ProcessContext);
		}
		else {
			pBox->m_pProcess(pBox->m_bRun, pBox->m_hWait, pBox->m_ProcessContext);
		}
		pBox->m_bDone = TRUE;

		thread_num -= 1;

		API::fnExitThread(0);

		return 0;
	}

	int	CThreadBox::Cancel(){
		if (m_bDone){
			return -1;
		}

		m_bRun = FALSE;
		if (m_hWait != 0){
			API::fnSetEvent(m_hWait);
		}
		return 0;
	}

	int	CThreadBox::Size(){
		return m_iDataLen;
	}

	BOOL CThreadBox::CheckSize(int size){
		CAutoLock lock(&m_mutexPacket);

		size += sizeof(PacketHead_t);
		if (size > (m_iBufferLen - m_iDataLen)){
			return FALSE;
		}else{
			return TRUE;
		}
	}

	DWORD CThreadBox::GetCurrentThreadId(){
		if (NULL == m_hObject)
			return 0;
		return API::fnGetThreadId(m_hObject);
	}

	int CThreadBox::OpenEvent(){
		if (m_hWait == 0){
			m_hWait = API::fnCreateEvent(0, FALSE, FALSE, 0);
		}
		return 0;
	}

	void CThreadBox::CloseEvent(){
		if (m_hWait != 0){
			API::fnCloseHandle(m_hWait);
			m_hWait = 0;
		}
	}

	void CThreadBox::SetEvent(){
		if (m_hWait != 0){
			API::fnSetEvent(m_hWait);
		}
	}

	void CThreadBox::ResetEvent(){
		if (m_hWait != 0){
			API::fnResetEvent(m_hWait);
		}
	}

	HANDLE CThreadBox::GetEventHandle(){
		return m_hWait;
	}

	int CThreadBox::WriteData(void* pData, int iDataLen, BOOL bEvent, BOOL bForceWrite){
		void* buf[1];
		int	  len[1];

		buf[0] = pData;
		len[0] = iDataLen;

		return WriteDataEx(buf, len, 1, bEvent, bForceWrite);
	}

	int CThreadBox::WriteDataEx(void* pData[], int iDataLen[], int iNum, BOOL bEvent, BOOL bForceWrite){
		int		i, iSize, iDataTotalLen, iLen;
		int		iResult;
		BYTE*	head = NULL;

		CAutoLock lock(&m_mutexPacket);

		if (!m_bInit){
			iResult = -1;
			goto _exit;
		}

		if (m_pBuffer == NULL){
			iResult = -2;
			goto _exit;
		}

		iDataTotalLen = 0;
		for (i = 0; i<iNum; i++){
			iDataTotalLen += iDataLen[i];
		}

		if (m_bPacket){
			PacketHead_t pkt;

			iLen = iDataTotalLen + sizeof(PacketHead_t);
			iSize = m_iBufferLen - m_iDataLen;
			if (iSize < iLen){
				BYTE* buf;
				int	  data_len;

				if (!bForceWrite){
					iResult = -3;
					goto _exit;
				}

				do{
					iSize = m_iBufferLen - m_iDataLen;
					if (iSize >= (int)(iDataTotalLen + sizeof(PacketHead_t))){
						break;
					}
					buf = (BYTE*)&pkt;
					data_len = sizeof(PacketHead_t);
					iLen = m_iBufferLen - m_iReadPos;
					if (data_len <= iLen){
						API::Memcpy(buf, m_pBuffer + m_iReadPos, data_len);
					}else{
						API::Memcpy(buf, m_pBuffer + m_iReadPos, iLen);
						API::Memcpy(buf + iLen, m_pBuffer, data_len - iLen);
					}

					if (pkt.dwFlags != PACKET_HEAD_FLAGS){
						m_iWritePos = 0;
						m_iReadPos = 0;
						m_iDataLen = 0;
						break;
					}

					data_len += pkt.dwLen;

					m_iDataLen -= data_len;
					m_iReadPos += data_len;
					if (m_iReadPos >= m_iBufferLen){
						m_iReadPos -= m_iBufferLen;
					}

				} while (m_iDataLen > 0);
			}

			pkt.dwFlags = PACKET_HEAD_FLAGS;
			pkt.dwLen = iDataTotalLen;

			head = (BYTE*)&pkt;
			iLen = sizeof(PacketHead_t);
			iSize = m_iBufferLen - m_iWritePos;
			if (iSize < iLen){
				API::Memcpy(m_pBuffer + m_iWritePos, head, iSize);
				API::Memcpy(m_pBuffer, head + iSize, iLen - iSize);
			}else{
				API::Memcpy(m_pBuffer + m_iWritePos, head, iLen);
			}
			m_iDataLen += iLen;
			m_iWritePos += iLen;
			if (m_iWritePos >= m_iBufferLen){
				m_iWritePos -= m_iBufferLen;
			}
		}else{
			iSize = m_iBufferLen - m_iDataLen;
			if (iSize < iDataTotalLen){
				if (!bForceWrite){
					iResult = -4;
					goto _exit;
				}

				iLen = iDataTotalLen - iSize;
				m_iDataLen -= iLen;
				m_iReadPos += iLen;
				if (m_iReadPos >= m_iBufferLen){
					m_iReadPos -= m_iBufferLen;
				}
			}
		}

		for (i = 0; i<iNum; i++)
		{
			head = (BYTE*)pData[i];
			iLen = iDataLen[i];
			if (head != NULL && iLen > 0)
			{
				iSize = m_iBufferLen - m_iWritePos;
				if (iSize < iLen){
					API::Memcpy(m_pBuffer + m_iWritePos, head, iSize);
					API::Memcpy(m_pBuffer, head + iSize, iLen - iSize);
				}else{
					API::Memcpy(m_pBuffer + m_iWritePos, head, iLen);
				}
				m_iDataLen += iLen;
				m_iWritePos += iLen;
				if (m_iWritePos >= m_iBufferLen){
					m_iWritePos -= m_iBufferLen;
				}
			}
			else
				if (iLen > 0){
					m_iDataLen += iLen;
					m_iWritePos += iLen;
					if (m_iWritePos >= m_iBufferLen){
						m_iWritePos -= m_iBufferLen;
					}
				}
		}

		iResult = 0;

	_exit:
		if (bEvent && m_hWait != NULL){
			API::fnSetEvent(m_hWait);
		}
		return iResult;
	}

	int CThreadBox::ReadData(void* pData, int iDataLen, int* lpRequestSize, BOOL bForceGet){
		return ReadDataEx(NULL, 0, pData, iDataLen, lpRequestSize, bForceGet);
	}

	int	CThreadBox::ReadDataEx(void* pHead, int iHeadLen, void* pData, int iDataLen, int* lpRequestSize, BOOL bForceGet)
	{
		int iResult;
		int iLen, data_len, head_len;
		BYTE* buf;
		PacketHead_t pkt;

		CAutoLock lock(&m_mutexPacket);

		if (!m_bInit){
			iResult = -1;
			goto _exit;
		}

		if (m_pBuffer == NULL){
			iResult = -2;
			goto _exit;
		}

		if (m_iDataLen <= 0){
			iResult = -3;
			goto _exit;
		}

		if (m_bPacket){
			buf = (BYTE*)&pkt;
			data_len = sizeof(PacketHead_t);
			iLen = m_iBufferLen - m_iReadPos;
			if (data_len <= iLen){
				API::Memcpy(buf, m_pBuffer + m_iReadPos, data_len);
			}
			else{
				API::Memcpy(buf, m_pBuffer + m_iReadPos, iLen);
				API::Memcpy(buf + iLen, m_pBuffer, data_len - iLen);
			}

			if (pkt.dwFlags != PACKET_HEAD_FLAGS){
				m_iWritePos = 0;
				m_iReadPos = 0;
				m_iDataLen = 0;
				iResult = -4;
				goto _exit;
			}

			if ((iHeadLen + iDataLen) < (int)pkt.dwLen){
				if (lpRequestSize != NULL){
					*lpRequestSize = (int)pkt.dwLen;
				}
				iResult = 0;
				goto _exit;
			}

			m_iDataLen -= data_len;
			m_iReadPos += data_len;
			if (m_iReadPos >= m_iBufferLen){
				m_iReadPos -= m_iBufferLen;
			}

			if ((int)pkt.dwLen < iHeadLen){
				head_len = (int)pkt.dwLen;
			}
			else{
				head_len = iHeadLen;
			}
			data_len = (int)pkt.dwLen - head_len;
		}
		else{
			if (m_iDataLen < (iHeadLen + iDataLen)){
				if (!bForceGet){
					iResult = -5;
					goto _exit;
				}
			}
			head_len = iHeadLen;
			data_len = iDataLen;
		}

		if (head_len > 0){
			buf = (BYTE*)pHead;
			iLen = m_iBufferLen - m_iReadPos;
			if (head_len <= iLen){
				API::Memcpy(buf, m_pBuffer + m_iReadPos, head_len);
			}
			else{
				API::Memcpy(buf, m_pBuffer + m_iReadPos, iLen);
				API::Memcpy(buf + iLen, m_pBuffer, head_len - iLen);
			}
			m_iDataLen -= head_len;
			m_iReadPos += head_len;
			if (m_iReadPos >= m_iBufferLen){
				m_iReadPos -= m_iBufferLen;
			}
		}

		if (data_len > 0){
			buf = (BYTE*)pData;
			iLen = m_iBufferLen - m_iReadPos;
			if (data_len <= iLen){
				API::Memcpy(buf, m_pBuffer + m_iReadPos, data_len);
			}
			else{
				API::Memcpy(buf, m_pBuffer + m_iReadPos, iLen);
				API::Memcpy(buf + iLen, m_pBuffer, data_len - iLen);
			}
			m_iDataLen -= data_len;
			m_iReadPos += data_len;
			if (m_iReadPos >= m_iBufferLen){
				m_iReadPos -= m_iBufferLen;
			}
		}

		iResult = data_len;

	_exit:
		return iResult;
	}

	CThreadWrapper::CThreadWrapper()
	{
		m_uiIdleTimestamp = 0;
		m_iError = 0;
		ZeroMemory(&m_Task, sizeof(m_Task));
		ZeroMemory(&m_TaskCallBack, sizeof(m_TaskCallBack));
	}

	CThreadWrapper::~CThreadWrapper()
	{

	}

	int CThreadWrapper::Start(int iPacketSize, int iPacketNum, THREAD_POOL_PROCESS lpProcCallBack, void* lpUsr)
	{
		if (NULL == lpProcCallBack)
		{
			__UpdateError(-1);
			return -1;
		}
		m_Task.iBufferLen = 1024 * 8;
		m_Task.lpBuff = m_Task.buff;
		m_Task.iDataLen = 0;

		m_TaskCallBack.cb = lpProcCallBack;
		m_TaskCallBack.lpUsr = lpUsr;

		m_Thread.Init(iPacketSize, iPacketNum, TRUE, TRUE);
		m_Thread.Start([this](BOOL& bRun, HANDLE hWait, void* context){
			int iTaskLen = 0;
			while (bRun)
			{
				//任务处理完成,进入空闲状态
				__UpdateIdleTime(GetTickCount());

				WaitForSingleObject(hWait, INFINITE);
				if (!bRun)
				{
					break;
				}

				if (NULL == m_Task.lpBuff)
				{
					__UpdateError(-1);
				}

				//正在处理任务.
				__UpdateIdleTime(0);

				do 
				{
					if (NULL == m_Task.lpBuff)
					{
						__UpdateError(-1);
						break;
					}

					iTaskLen = 0;
					m_Thread.ReadData(m_Task.lpBuff, 1, &iTaskLen);
					if ((iTaskLen <= 0) || (iTaskLen > 0x0FFFFFFF))
					{
						break;
					}

					if (iTaskLen > m_Task.iBufferLen)
					{
						//重新分配缓冲区大小
						if (m_Task.iBufferLen > 1024 * 8)
						{
							if (NULL != m_Task.lpBuff)
							{
								delete[] m_Task.lpBuff;
								m_Task.lpBuff = NULL;
							}
						}
						m_Task.iBufferLen = PAD_SIZE(iTaskLen);
						m_Task.lpBuff = new BYTE[m_Task.iBufferLen];
						if (NULL == m_Task.lpBuff)
						{
							break;
						}
					}

					iTaskLen = m_Thread.ReadData(m_Task.lpBuff, m_Task.iBufferLen);
					if ((iTaskLen <= 0) || (iTaskLen > 0x0FFFFFFF))
					{
						__UpdateError(-2);
						break;
					}
					m_TaskCallBack.cb(m_Task.lpBuff, iTaskLen, m_TaskCallBack.lpUsr);

				} while (true);

			}

		});
		if (m_Thread.IsDone())
		{
			return -1;
		}
		return 0;
	}

	void CThreadWrapper::Stop()
	{
		m_Thread.Cancel();
		m_Thread.Stop(100);
		m_Thread.UnInit();
		if (m_Task.iBufferLen > 1024*8)
		{
			if (NULL != m_Task.lpBuff)
			{
				delete[] m_Task.lpBuff;
				m_Task.lpBuff = NULL;
			}
		}
	}

	int CThreadWrapper::Task(void* lpTask, int iTaskDataLen)
	{
		if (NULL == lpTask)
		{
			return -1;
		}
		int err = m_Thread.WriteData(lpTask, iTaskDataLen);
		return err != 0 ? -1 : 0;
	}

	int CThreadWrapper::GetIdleTime()
	{
		CAutoLock lck(&m_lock);
		if (0 == m_uiIdleTimestamp)
		{
			return 0;
		}
		return abs((int)(GetTickCount() - m_uiIdleTimestamp));
	}

	bool CThreadWrapper::IsError()
	{
		CAutoLock lck(&m_lock);
		return 0 != m_iError;
	}

	bool CThreadWrapper::IsIdle()
	{
		CAutoLock lck(&m_lock);
		return m_uiIdleTimestamp > 0;
	}

	void CThreadWrapper::__UpdateIdleTime(unsigned int uiTimestamp)
	{
		CAutoLock lck(&m_lock);
		if (0 == uiTimestamp)
		{
			m_uiIdleTimestamp = 0;
			return;
		}
		if (m_uiIdleTimestamp == 0)
		{
			m_uiIdleTimestamp = uiTimestamp;
		}
	}

	void CThreadWrapper::__UpdateError(int err)
	{
		CAutoLock lck(&m_lock);
		m_iError = err;
	}

	CThreadPool::CThreadPool()
	{
		m_bInit = FALSE;
		m_bStartUp = FALSE;
		m_iMaxIdleTime = 0;
		m_iMaxThreadCnt = 0;
	}

	CThreadPool::~CThreadPool()
	{

	}

	int CThreadPool::Init(int iThreadCnt, int iTaskBufferLen, int iSingleTaskMaxLen, int iMaxIdleTime)
	{
		CAutoLock lck(&m_lock);
		if (0 >= iThreadCnt)
		{
			return -1;
		}
		if ((iTaskBufferLen < 0) || (iTaskBufferLen > 0x0FFFFFFF))
		{
			return -1;
		}
		if ((iSingleTaskMaxLen < 0) || (iSingleTaskMaxLen > iTaskBufferLen))
		{
			return -1;
		}
		m_iSingleTaskMaxLen = iSingleTaskMaxLen;
		m_iMaxThreadCnt = iThreadCnt;
		m_iMaxIdleTime = iMaxIdleTime;
		m_CtrlThread.Init(iTaskBufferLen, 1, TRUE, TRUE);
		return 0;
	}

	void CThreadPool::UnInit()
	{
		CAutoLock lck(&m_lock);
		Stop();
		m_CtrlThread.UnInit();
	}

	int CThreadPool::Start(THREAD_POOL_PROCESS lpProcCallBack, void* lpUsr)
	{
		if (NULL == lpProcCallBack)
		{
			return -1;
		}
		CAutoLock lck(&m_lock);
		m_TaskCallBack.lpPorc = lpProcCallBack;
		m_TaskCallBack.lpUsr = lpUsr;

		m_Task.iBufferLen = 1024 * 8;
		m_Task.lpBuff = m_Task.buff;
		m_Task.iDataLen = 0;

		m_CtrlThread.Start(__CtrlThreadProc, this);
		if (m_CtrlThread.IsDone())
		{
			return -2;
		}
		return 0;
	}

	void CThreadPool::Stop()
	{
		CAutoLock lck(&m_lock);
		for (auto& it : m_lstThreads)
		{
			it->Stop();
			delete it;
		}
		m_lstThreads.clear();

		m_CtrlThread.Cancel();
		m_CtrlThread.Stop(100);
	}

	int CThreadPool::Task(void* lpTask, int iTaskDataLen)
	{
		if (NULL == lpTask)
		{
			return -1;
		}
		int err = m_CtrlThread.WriteData(lpTask, iTaskDataLen);
		return 0 == err ? 0 : -2;
	}

	void CThreadPool::__CtrlThreadProc(BOOL& bRun, HANDLE hWait, void* context)
	{
		CThreadPool* lpObj = (CThreadPool*)context;
		if (NULL == lpObj)
		{
			return;
		}
		lpObj->__CtrlThreadProcLoop(bRun, hWait);
	}

	void CThreadPool::__CtrlThreadProcLoop(BOOL& bRun, HANDLE hWait)
	{
		int iTaskLen = 0;
		CThreadWrapper* lpProcThread;
		while (bRun)
		{
			WaitForSingleObject(hWait, 2000);
			if (!bRun)
			{
				break;
			}

			if (m_CtrlThread.IsData())
			{
				do
				{
					if (NULL == m_Task.lpBuff)
					{
						break;
					}

					if (!m_CtrlThread.IsData())
					{
						break;
					}

					lpProcThread = __GetThread();
					if (NULL == lpProcThread)
					{
						break;
					}

					iTaskLen = 0;
					m_CtrlThread.ReadData(m_Task.lpBuff, 1, &iTaskLen);
					if ((iTaskLen <= 0) || (iTaskLen > 0x0FFFFFFF))
					{
						break;
					}

					if (iTaskLen > m_Task.iBufferLen)
					{
						//重新分配缓冲区大小
						if (m_Task.iBufferLen > 1024 * 8)
						{
							if (NULL != m_Task.lpBuff)
							{
								delete[] m_Task.lpBuff;
								m_Task.lpBuff = NULL;
							}
						}
						m_Task.iBufferLen = PAD_SIZE(iTaskLen);
						m_Task.lpBuff = new BYTE[m_Task.iBufferLen];
						if (NULL == m_Task.lpBuff)
						{
							break;
						}
					}

					iTaskLen = m_CtrlThread.ReadData(m_Task.lpBuff, m_Task.iBufferLen);
					if ((iTaskLen <= 0) || (iTaskLen > 0x0FFFFFFF))
					{
						break;
					}
					lpProcThread->Task(m_Task.lpBuff, iTaskLen);

				} while (true);
			}

			__CheckProcThread();
		}
	}

	void CThreadPool::__ThreadCallBack(void* lpTask, int iTaskDataLen, void* lpUsr)
	{
		CThreadPool* lpObj = (CThreadPool*)lpUsr;
		if (NULL == lpObj)
		{
			return;
		}
		lpObj->__ThreadCallBackProc(lpTask, iTaskDataLen);
	}

	void CThreadPool::__ThreadCallBackProc(void* lpTask, int iTaskDataLen)
	{
		if (NULL != m_TaskCallBack.lpPorc)
		{
			m_TaskCallBack.lpPorc(lpTask, iTaskDataLen, m_TaskCallBack.lpUsr);
		}
	}

	UTILS::CThreadWrapper* CThreadPool::__GetThread()
	{
		CAutoLock lck(&m_lock);
		for (auto& it : m_lstThreads)
		{
			if (it->IsIdle())
			{
				return it;
			}
		}
		if (m_lstThreads.size() >= m_iMaxThreadCnt)
		{
			//MSG_INFO("线程池,工作线程满负荷");
			return NULL;
		}
		CThreadWrapper* lpObj = new CThreadWrapper();
		if (NULL == lpObj)
		{
			return NULL;
		}
		int err = lpObj->Start(m_iSingleTaskMaxLen, 1, __ThreadCallBack, this);
		if (0 != err)
		{
			return NULL;
		}
		m_lstThreads.emplace_back(lpObj);
		return lpObj;
	}

	void CThreadPool::__CheckProcThread()
	{
		CAutoLock lck(&m_lock);
		auto itBeg = m_lstThreads.begin();
		for (; itBeg != m_lstThreads.end();)
		{
			if ((*itBeg)->IsError() || ((*itBeg)->GetIdleTime() >= m_iMaxIdleTime))
			{
				/*MSG_INFO("空闲超时:%d 线程是否发生错误:%d", 
					(*itBeg)->GetIdleTime() >= m_iMaxIdleTime,
					(*itBeg)->IsError());*/
				(*itBeg)->Stop();
				delete (*itBeg);
				itBeg = m_lstThreads.erase(itBeg);
				continue;
			}
			itBeg++;
		}
		//MSG_INFO("线程池.工作线程:%d", m_lstThreads.size());
	}

}
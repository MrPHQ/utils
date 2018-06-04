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
			if (abs((int)(GetTickCount() - dwCut)) >= timeout) {
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
}
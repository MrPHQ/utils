#include "../../utils/ShareMemory.h"
#include "../../utils/api.h"
namespace UTILS
{
	//共享数据段头部
	struct  DataHeader_t {
		DWORD dwMaxSize;//数据段总长度	
		DWORD dwUsedSize;//已经使用的空间大小
		DWORD dwWriteOffset;//数据写入位置相对首地址的偏移地址	
		DWORD dwReadOffset;//当前读取位置相对首地址的偏移地址
	};

	CShareMemory::CShareMemory()
		: m_bInit(FALSE)
		, m_hFileMapping(NULL)
		, m_hChangeEvent(NULL)
		, m_hDataMutex(NULL)
		, m_nType(CT_WRITER)
		, m_pfnOutPutData(NULL)
		, m_pUsrpar(NULL)
		, m_pBasePointer(NULL)
	{

	}

	CShareMemory::~CShareMemory(){
		UnInit();
	}

	int	CShareMemory::Init(CommunicatorType nType, PSHARE_MEMORY_PROPERTY pProperty,DWORD dwLength){
		//指向共享内存段的首地址
		DataHeader_t*	lpDataHeader = NULL;
		BOOL bNeedInitHeader = TRUE;
		SECURITY_ATTRIBUTES		attr;
		PSECURITY_DESCRIPTOR	pSec = { NULL };
		int	nErrCode = UTILS_ERROR_SUCCESS;

		if (m_bInit){
			return nErrCode;
		}

		if (dwLength <= sizeof(DataHeader_t) || pProperty == NULL){
			return UTILS_ERROR_PAR;
		}
		if (NULL == pProperty->szShareSegName || 
			NULL == pProperty->szChangeEventName || 
			NULL == pProperty->szMutexName || 
			strlen(pProperty->szShareSegName) <= 0 || 
			strlen(pProperty->szChangeEventName) <= 0 || 
			strlen(pProperty->szMutexName) <= 0)
		{
			return UTILS_ERROR_PAR;
		}

		//设置共享属性，使共享数据段能被其他进程访问
		pSec = (PSECURITY_DESCRIPTOR)LocalAlloc(LMEM_FIXED, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (!pSec){
			return UTILS_ERROR_FAIL;
		}
		if (!InitializeSecurityDescriptor(pSec, SECURITY_DESCRIPTOR_REVISION)) {
			nErrCode = UTILS_ERROR_FAIL;
			goto EXIT;
		}
		if (!SetSecurityDescriptorDacl(pSec, TRUE, NULL, TRUE)) {
			nErrCode = UTILS_ERROR_FAIL;
			goto EXIT;
		}
		attr.bInheritHandle = FALSE;
		attr.lpSecurityDescriptor = pSec;
		attr.nLength = sizeof(SECURITY_ATTRIBUTES);

		m_nType = nType;

		//创建(或打开)共享数据段
		DWORD dwSize = dwLength + sizeof(DataHeader_t);
		dwSize = dwSize + (4 - dwSize % 4);//4字节对齐
		m_hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, &attr, PAGE_READWRITE, 0, dwSize, pProperty->szShareSegName);
		if (NULL == m_hFileMapping) {
			nErrCode = UTILS_ERROR_FAIL;
			goto EXIT;
		}
		if (ERROR_ALREADY_EXISTS == GetLastError())
		{//缓存区已经存在，则无需再初始化缓存区
			bNeedInitHeader = FALSE;
		}
		m_pBasePointer = (BYTE*)MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE, 0, 0, 0);
		if (NULL == m_pBasePointer) {
			nErrCode = UTILS_ERROR_FAIL;
			goto EXIT;
		}
		//将共享区域信息写入头部
		if (bNeedInitHeader) {
			lpDataHeader = (DataHeader_t*)m_pBasePointer;
			ZeroMemory(lpDataHeader, sizeof(DataHeader_t));
			lpDataHeader->dwMaxSize = dwLength;
			lpDataHeader->dwUsedSize = 0;
			lpDataHeader->dwWriteOffset = 0;
			lpDataHeader->dwReadOffset = 0;
		}
		//创建(或打开)数据写入通知
		m_hChangeEvent = CreateEvent(&attr, FALSE, FALSE, pProperty->szChangeEventName);
		if (NULL == m_hChangeEvent) {
			nErrCode = UTILS_ERROR_FAIL;
			goto EXIT;
		}
		//创建(或打开)数据段访问互斥锁
		m_hDataMutex = CreateMutex(&attr, FALSE, pProperty->szMutexName);
		if (NULL == m_hDataMutex) {
			nErrCode = UTILS_ERROR_FAIL;
			goto EXIT;
		}
		//如果为读取模式，创建数据读取线程
		if (CT_READER == m_nType) {
			m_read_thr.Init(0, 0, TRUE, TRUE);
			m_read_thr.Start(ReadDataThread, this);
		}

		m_bInit = TRUE;
	EXIT:
		LocalFree(pSec);
		pSec = NULL;
		if (!m_bInit){
			UnInit();
		}
		return nErrCode;
	}

	void CShareMemory::UnInit()
	{
		m_bInit = FALSE;
		if (CT_READER == m_nType) {
			m_read_thr.Cancel();
			m_read_thr.Stop();
			m_read_thr.UnInit();
		}

		if (NULL != m_pBasePointer) {
			UnmapViewOfFile(m_pBasePointer);
			m_pBasePointer = NULL;
		}

		if (NULL != m_hFileMapping) {
			CloseHandle(m_hFileMapping);
			m_hFileMapping = NULL;
		}

		if (NULL != m_hChangeEvent) {
			CloseHandle(m_hChangeEvent);
			m_hChangeEvent = NULL;
		}

		if (NULL != m_hDataMutex) {
			CloseHandle(m_hDataMutex);
			m_hDataMutex = NULL;
		}
	}

	int CShareMemory::WriteData(CommunicatorPacket_t* pData)
	{
		DWORD dwWaitResult = 0;
		DataHeader_t*	lpDataHeader = NULL;
		DWORD dwDataLen = 0;
		int Err = UTILS_ERROR_SUCCESS;

		if (!m_bInit){
			return UTILS_ERROR_FAIL;
		}
		if (CT_WRITER != m_nType){
			return UTILS_ERROR_FAIL;
		}
		if (NULL == pData){
			return UTILS_ERROR_FAIL;
		}
		if (PROC_FLAG != pData->dwFlag){
			return UTILS_ERROR_FAIL;
		}
		//锁定共享缓存
		dwWaitResult = WaitForSingleObject(m_hDataMutex, 2000);
		if (WAIT_TIMEOUT == dwWaitResult){
			return UTILS_ERROR_FAIL;
		}

		//读取共享数据段头部，获取写入位置等信息
		lpDataHeader = (DataHeader_t*)m_pBasePointer;

		dwDataLen = sizeof(CommunicatorPacket_t) + pData->iLen;
		if (dwDataLen > lpDataHeader->dwMaxSize) {
			Err = UTILS_ERROR_FAIL;//数据比总空间还大
			goto EXIT;
		}

		//判断从<当前位置>至<缓冲区尾>写入数据，空间是否满足	
		DWORD dwEmptyBuffSize = lpDataHeader->dwMaxSize - lpDataHeader->dwWriteOffset;
		if (dwEmptyBuffSize < dwDataLen) {
			//空间不足，则判断能否从缓冲头再次写入数据
			if (lpDataHeader->dwReadOffset < dwDataLen) {
				Err = UTILS_ERROR_FAIL;//剩余空间不满足数据长度要求
				goto EXIT;
			}
			else {//再次从头开始写
				lpDataHeader->dwWriteOffset = 0;
			}
		}

		BYTE* ptr = m_pBasePointer + (sizeof(DataHeader_t) + lpDataHeader->dwWriteOffset);
		memcpy(ptr, pData, dwDataLen);
		lpDataHeader->dwWriteOffset += dwDataLen;
		lpDataHeader->dwUsedSize += dwDataLen;

		Err = UTILS_ERROR_SUCCESS;
	EXIT:
		ReleaseMutex(m_hDataMutex);
		if (UTILS_ERROR_SUCCESS == Err){
			SetEvent(m_hChangeEvent);
		}
		return Err;
	}

	void CShareMemory::RegisterDataCallback(std::function<void(CommunicatorPacket_t* lpPacket, void* pUsrPar)> pCallback,
		void* pUsrPar){
		m_pfnOutPutData = pCallback;
		m_pUsrpar = pUsrPar;
	}

	void CShareMemory::ResetData(){
		if (!m_bInit || CT_READER != m_nType){
			return;
		}

		DWORD dwWaitResult = WaitForSingleObject(m_hDataMutex, 5000);
		if (WAIT_OBJECT_0 == dwWaitResult) {
			DataHeader_t*	lpDataHeader = (DataHeader_t*)m_pBasePointer;
			lpDataHeader->dwReadOffset = 0;
			lpDataHeader->dwUsedSize = 0;
			lpDataHeader->dwWriteOffset = 0;
			ReleaseMutex(m_hDataMutex);
		}
	}

	void CShareMemory::ReadData(BOOL& bRun, HANDLE hWait)
	{
		DWORD dwWait = 0;
		DataHeader_t*	lpDataHeader = NULL;
		lpDataHeader = (DataHeader_t*)m_pBasePointer;
		CommunicatorPacket_t* lpPacket;
		HANDLE ahObject[2] = { hWait, m_hDataMutex };
		while (bRun) {
			dwWait = WaitForMultipleObjects(2, ahObject, FALSE, 1000);
			if ((WAIT_OBJECT_0 + 1) != dwWait){
				continue;
			}
			if (lpDataHeader->dwUsedSize <= 0) {
				ReleaseMutex(m_hDataMutex);
				break;//数据已全部读完
			}

			if ((lpDataHeader->dwMaxSize - lpDataHeader->dwReadOffset) < sizeof(CommunicatorPacket_t))
			{
				lpDataHeader->dwReadOffset = 0;
			}
			lpPacket = (CommunicatorPacket_t*)(m_pBasePointer + (sizeof(DataHeader_t) + lpDataHeader->dwReadOffset));
			if (PROC_FLAG != lpPacket->dwFlag)
			{
				lpDataHeader->dwReadOffset = 0;
			}
			lpPacket = (CommunicatorPacket_t*)(m_pBasePointer + (sizeof(DataHeader_t) + lpDataHeader->dwReadOffset));
			if (PROC_FLAG != lpPacket->dwFlag) {
				ReleaseMutex(m_hDataMutex);
				break;
			}
			__try
			{
				//避免数据无法写入，在数据输出过程中可以继续写入数据
				ReleaseMutex(m_hDataMutex);
				//将数据输出给用户
				if (NULL != m_pfnOutPutData) {
					m_pfnOutPutData(lpPacket, m_pUsrpar);
				}
				//标志复位，防止重复输出，若超时而未被修改，则数据将被重复输出
				dwWait = WaitForMultipleObjects(2, ahObject, FALSE, 5000);
				if ((WAIT_OBJECT_0 + 1) == dwWait) {
					lpDataHeader->dwUsedSize -= sizeof(CommunicatorPacket_t) + lpPacket->iLen;
					lpDataHeader->dwReadOffset += sizeof(CommunicatorPacket_t) + lpPacket->iLen;
					lpPacket->dwFlag = 0;
					ReleaseMutex(m_hDataMutex);
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				ReleaseMutex(m_hDataMutex);
				break;
			}
		}
	}

	void CShareMemory::ReadDataThread(BOOL& bRun, HANDLE hWait, void* pUsrPar)
	{
		CShareMemory* self = (CShareMemory*)pUsrPar;
		HANDLE ahEvent[2] = { hWait, self->m_hChangeEvent };
		DWORD dwWait = 0;
		while (bRun) {
			//等待线程退出通知及数据写入通知
			dwWait = WaitForMultipleObjects(2, ahEvent, FALSE, INFINITE);
			if (!bRun)
			{
				break;
			}
			if ((WAIT_OBJECT_0 + 1) == dwWait)
			{
				self->ReadData(bRun, hWait);
			}
		}
	}
}


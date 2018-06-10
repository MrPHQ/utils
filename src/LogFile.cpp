#include "../utils/LogFile.h"
#include "../utils/api.h"
#include "../utils/file.h"
#include "../utils/Thread.h"
#include "internal.h"
#include <IO.H>

class CLogFileProperty
{
	struct LogHistoryFile{
		int iIndex;
		std::string strFile;
	};
	struct LogFileDate{
		int iYear;
		int iMonth;
		int iDay;
	};
public:
	CLogFileProperty() :m_nMode(UTILS::LOG_FILE_MODE_NONE), m_dwHead(0), m_uiDays(1), m_uiPerFileSize(1024 * 1024){
		m_szFile[0] = '\0';
		m_dwCheckTimestamp = 0;
	}
	~CLogFileProperty(){}

	int Init(UTILS::LOG_FILE_MODE nMode, DWORD nHead, const char* file, unsigned int uiDays, unsigned int uiPerFileSize)
	{
		std::unique_lock<std::mutex> lock(m_lock.GetMutex());
		m_mapFiles.clear();

		m_dwCheckTimestamp = GetTickCount();
		m_nMode = nMode;
		m_dwHead = nHead;
		UTILS::API::Strcpy(m_szFile, sizeof(m_szFile), file);
		m_uiDays = uiDays;
		m_uiPerFileSize = uiPerFileSize;

		GetHistoryLogFile();

		if (nMode == UTILS::LOG_FILE_MODE_ASYNC_IN){
			m_WriteThread.Init(1024, 1024, TRUE, TRUE);
			m_WriteThread.Start([this](BOOL& bRun, HANDLE hWait, void* context){
				DWORD dwCheck = GetTickCount();
				BYTE buff[1024 * 4];
				BYTE* pBuff = buff;
				int iBuffLen = 1024*4;
				int iNeedLen = 0, iDataLen = 0;

				while (bRun){
					WaitForSingleObject(hWait, 100);
					if (!bRun){
						break;
					}

					if (IsCheck())
					{
						m_lock.Lock();
						CheckDateChanged();
						CheckStorageDays();
						CheckStorageSize();
						m_lock.UnLock();
					}
					do
					{
						iNeedLen = 0;
						iDataLen = m_WriteThread.ReadData(pBuff, 1, &iNeedLen);
						if (iNeedLen <= 0){
							break;
						}
						if (iNeedLen > iBuffLen)
						{
							if ((iBuffLen > 1024 * 4) && (pBuff != nullptr)){
								delete[] pBuff;
								pBuff = nullptr;
							}
							if (iNeedLen > 1024 * 1024)
							{
								m_WriteThread.ResetData();
								break;
							}
							iBuffLen = PAD_SIZE(iNeedLen);
							pBuff = new BYTE[iBuffLen];
						}
						if (pBuff == nullptr){
							break;
						}

						iDataLen = m_WriteThread.ReadData(pBuff, iBuffLen);
						if (iDataLen <= 0){
							break;
						}
						m_logfile.Write((const char*)pBuff, iDataLen);
					} while (pBuff != nullptr);
				}
				if ((iBuffLen > 1024 * 4) && (pBuff != nullptr)){
					delete[] pBuff;
					pBuff = nullptr;
				}
			});
		}
		/// ����־�ļ�
		return Open();
	}

	void UnInit()
	{
		m_WriteThread.Cancel();
		m_WriteThread.Stop(100);
		m_WriteThread.UnInit();
		m_logfile.Close();
	}

	/**
	\brief
		д����־.
	*/
	void Write(const char* pLog, int len)
	{
		Write(nullptr, 0, pLog, len);
	}

	/**
	\brief
		д����־.
		��־�ļ�ģʽ(LOG_FILE_MODE):
			LOG_FILE_MODE_SYNC ����д����־�ļ�.
			LOG_FILE_MODE_ASYNC_IN д���ڲ�������,�ڲ���־�߳��ӳ�д����־�ļ�
			LOG_FILE_MODE_ASYNC_OUT д���ڲ�������,Ȼ���ⲿ����[Active],����д����־�ļ�
	\param file
		��־λ�������ļ�.
	\param line
		��־����λ���ļ��к�.
	\param data
		��־����
	\param len
		��־��С
	*/
	void Write(const char* file, int line, const char* data, int len)
	{
		std::unique_lock<std::mutex> lock(m_lock.GetMutex());
		int iLogLen = 0;
		const char* p = ConstructLog(file, line, data, len, &iLogLen);
		UTILS::MSG_INFO((char*)p);
		switch (m_nMode)
		{
		case UTILS::LOG_FILE_MODE_NONE:
			break;
		case UTILS::LOG_FILE_MODE_SYNC:
			if (p != nullptr){
				m_logfile.Write(p, iLogLen);
			}
			if (IsCheck())
			{
				m_lock.Lock();
				CheckDateChanged();
				CheckStorageDays();
				CheckStorageSize();
				m_lock.UnLock();
			}
			break;
		case UTILS::LOG_FILE_MODE_ASYNC_IN:
		case UTILS::LOG_FILE_MODE_ASYNC_OUT:
			if (p != nullptr){
				m_WriteThread.WriteData((BYTE*)p, iLogLen);
			}
			break;
		default:
			break;
		}
		
	}

	/**
	\brief
		д����־. �ѻ��������־д����־�ļ�
		ֻ����	��־�ļ�ģʽ(LOG_FILE_MODE):LOG_FILE_MODE_ASYNC_OUT
	*/
	void Active()
	{
		if (m_nMode != UTILS::LOG_FILE_MODE_ASYNC_OUT){
			return;
		}
		if (IsCheck())
		{
			m_lock.Lock();
			CheckDateChanged();
			CheckStorageDays();
			CheckStorageSize();
			m_lock.UnLock();
		}
		BYTE buff[1024 * 4];
		BYTE* pBuff = buff;
		int iBuffLen = 1024 * 4;
		int iNeedLen = 0, iDataLen = 0;
		do
		{
			iNeedLen = 0;
			iDataLen = m_WriteThread.ReadData(pBuff, 1, &iNeedLen);
			if (iNeedLen <= 0){
				break;
			}
			if (iNeedLen > iBuffLen)
			{
				if ((iBuffLen > 1024 * 4) && (pBuff != nullptr)){
					delete[] pBuff;
					pBuff = nullptr;
				}
				if (iNeedLen > 1024 * 1024)
				{
					m_WriteThread.ResetData();
					break;
				}
				iBuffLen = PAD_SIZE(iNeedLen);
				pBuff = new BYTE[iBuffLen];
			}
			if (pBuff == nullptr){
				break;
			}

			iDataLen = m_WriteThread.ReadData(pBuff, iBuffLen);
			if (iDataLen <= 0){
				break;
			}
			m_logfile.Write((const char*)pBuff, iDataLen);
		} while (pBuff != nullptr);
		if ((iBuffLen > 1024 * 4) && (pBuff != nullptr)){
			delete[] pBuff;
			pBuff = nullptr;
		}
	}
	/**
	\brief
		����ļ��Ƿ�ﵽ�洢��������

		ע��: �����������,�ڲ���ɾ�������һ�����־�ļ�
	*/
	void CheckStorageDays()
	{
		if (m_uiDays <= 0){
			return;
		}
		if (m_mapFiles.empty()){
			return;
		}

		if (m_mapFiles.size() > m_uiDays){
			/*auto itBeg = m_mapFiles.begin();
			itBeg->second.sort([](LogHistoryFile& t1, LogHistoryFile& t2) ->bool{
				return ((t1.iIndex - t2.iIndex< 0) ? true : false);
			});*/

			for (auto& it : m_mapFiles.begin()->second)
			{
				UTILS::MSG_INFO("log file delete :%s", it.strFile.data());
				DeleteFile(it.strFile.data());
			}
			m_mapFiles.erase(m_mapFiles.begin());
		}
	}
	/**
	\brief
		����ļ��Ƿ�ﵽ���õ���������
		ע��: ����ﵽ����,���Զ��л��ļ�
	*/
	void CheckStorageSize()
	{
		if (m_uiPerFileSize <= 0){
			return;
		}
		if (!UTILS::API::IsPathExists(m_szFile)){
			return;
		}
		uint64_t uiSize = UTILS::API::FileSize(m_szFile);
		if (uiSize >= m_uiPerFileSize){
			NewFile();
		}
	}
	/**
	\brief
		��������Ƿ��л�
		ע��: ��������л�,���Զ��л��ļ�
	*/
	void CheckDateChanged()
	{
		if (!m_logfile.IsOpen())
		{
			return;
		}
		SYSTEMTIME st;
		GetLocalTime(&st);
		if (st.wDay != m_stDate.iDay){
			NewFile();
		}
	}
	/**
	\brief
		�ж��Ƿ�ﵽ��ⳬʱʱ��
		���[�����ļ���С/�����л�/�洢����]. Ĭ�ϳ�ʱʱ��10s
	\return 
		���� true �ﵽ��ʱʱ��
	*/
	bool IsCheck()
	{
		if (abs((int)(GetTickCount() - m_dwCheckTimestamp)) >= 10000){
			return true;
		}
		return false;
	}

private:
	int Open()
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		m_stDate.iYear = st.wYear;
		m_stDate.iMonth = st.wMonth;
		m_stDate.iDay = st.wDay;
		return m_logfile.Open(UTILS::PATH_FILE_OPENMODE_APP | UTILS::PATH_FILE_OPENMODE_BINARY, m_szFile);
	}
	/**
	\brief
		�رյ�ǰ�ļ�
	\param pFile
		������رյ��ļ�������(ȫ·��)
	\param len
		��������С
	\param pIndex
		���ر��ļ������
	*/
	void Close(char* pFile, int len, int* pIndex)
	{
		if (!m_logfile.IsOpen())
		{
			return;
		}
		uint64_t uiSize = UTILS::API::FileSize(m_szFile);
		if (uiSize <= 0){
			return;
		}
		char szDate[64], szTmp[MAX_PATH], szNewFile[MAX_PATH], szDir[MAX_PATH];
		int iIndex = 0;
		SYSTEMTIME st;
		LogHistoryFile stFile;
		m_logfile.Close();

		GetLocalTime(&st);
		_snprintf_s(szDate, _TRUNCATE, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
		auto itFind = m_mapFiles.find(szDate);
		if (itFind != m_mapFiles.end()){
			itFind->second.sort([](LogHistoryFile& t1, LogHistoryFile& t2) ->bool{
				return ((t1.iIndex - t2.iIndex< 0) ? true : false);
			});
			iIndex = itFind->second.back().iIndex;
		}
		if (itFind == m_mapFiles.end()){
			auto itRet = m_mapFiles.emplace(szDate, std::list<LogHistoryFile>());
			if (itRet.second){
				itFind = itRet.first;
			}
		}
		iIndex++;

		UTILS::API::Strcpy(szTmp, sizeof(szTmp), m_szFile);
		UTILS::API::RemoveFileSpec(szTmp);
		UTILS::API::Strcpy(szDir, sizeof(szDir), szTmp);

		UTILS::API::Strcpy(szTmp, sizeof(szTmp), m_szFile);
		UTILS::API::RemoveExtension(szTmp);
		UTILS::API::StripPath(szTmp);
		_snprintf_s(szNewFile, _TRUNCATE, "%s\\%s_%s_%d.log", szDir, szTmp, szDate, iIndex);

		UTILS::API::PathReName(m_szFile, szNewFile);
		if (pFile != nullptr){
			UTILS::API::Strcpy(pFile, len, szNewFile);
		}
		if (pIndex != nullptr){
			*pIndex = iIndex;
		}
	}

	/**
	\brief
		�л��ļ�

		����������С, �����л�.��Ҫ����������־�ļ�.
	*/
	void NewFile()
	{
		char szFile[MAX_PATH],szKey[64];
		int iIndex = 0;
		SYSTEMTIME st;
		LogHistoryFile stFile;
		GetLocalTime(&st);

		szFile[0] = '\0';
		Close(szFile, MAX_PATH, &iIndex);
		_snprintf_s(szKey, _TRUNCATE, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
		auto itFind = m_mapFiles.find(szKey);
		if (itFind != m_mapFiles.end()){
			stFile.iIndex = iIndex;
			stFile.strFile = szFile;
			itFind->second.emplace_back(stFile);
		}
		DeleteFile(m_szFile);
		Open();
	}

	/**
	\brief
		��ȡ��ʷ��־�ļ�
	*/
	void GetHistoryLogFile()
	{
		char szTmp[MAX_PATH],szName[64],szDir[MAX_PATH];
		char szFileName[64];
		int iFileIndex;
		szTmp[0] = '\0';
		UTILS::API::Strcpy(szTmp, sizeof(szTmp), m_szFile);
		UTILS::API::RemoveFileSpec(szTmp);
		UTILS::API::Strcpy(szDir, sizeof(szDir), szTmp);
		UTILS::API::Strcpy(szTmp, sizeof(szTmp), m_szFile);
		UTILS::API::RemoveExtension(szTmp);
		UTILS::API::StripPath(szTmp);
		UTILS::API::Strcpy(szName, sizeof(szName), szTmp);

		char szExt[1][16] = { ".log" };
		std::list<std::string> lstFile;
		LogHistoryFile stHisFile;
		LogFileDate stDate;
		UTILS::API::EnumDirectoryFiles(szDir, szExt, 1, nullptr, &lstFile, false);
		char szKey[64];
		for (auto& it : lstFile)
		{
			UTILS::API::Strcpy(szTmp, sizeof(szTmp), it.data());
			UTILS::API::RemoveExtension(szTmp);
			UTILS::API::StripPath(szTmp);
			szFileName[0] = '\0';
			iFileIndex = 0;
			ZeroMemory(&stDate, sizeof(LogFileDate));
			sscanf_s(szTmp, "%s_%04d%02d%02d_%d", szFileName, &stDate.iYear, &stDate.iMonth, &stDate.iDay, &iFileIndex);
			if (strcmp(szFileName, szName)){
				continue;
			}
			stHisFile.iIndex = iFileIndex;
			stHisFile.strFile = it;
			_snprintf_s(szKey, _TRUNCATE, "%04d%02d%02d", stDate.iYear, stDate.iMonth, stDate.iDay);
			auto itFind = m_mapFiles.find(szKey);
			if (itFind == m_mapFiles.end()){
				auto itRet = m_mapFiles.emplace(szKey, std::list<LogHistoryFile>());
				if (itRet.second){
					itFind = itRet.first;
				}
			}
			if (itFind != m_mapFiles.end()){
				itFind->second.emplace_back(stHisFile);
			}
		}
	}
	/**
	\brief
		������־��¼
	\param file
		��־λ�������ļ�.
	\param line
		��־����λ���ļ��к�.
	*/
	const char* ConstructLog(const char* file, int line, const char* log, int len, int* pNewLen)
	{
		if (log == nullptr || len <= 0){
			return nullptr;
		}
		static char szLog[1024 * 16];
		char szTmp[512];
		szLog[0] = '\0';
		if ((m_dwHead&UTILS::LOG_FILE_HEAD_TIME) == UTILS::LOG_FILE_HEAD_TIME){
			SYSTEMTIME st;
			GetLocalTime(&st);
			_snprintf_s(szTmp, _TRUNCATE, "[%04d-%02d-%02d %02d:%02d:%02d]", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			strncat_s(szLog, 1024 * 16, szTmp, min(strlen(szTmp), 1024 * 16 - 1 - strlen(szLog)));
		}
		if ((m_dwHead&UTILS::LOG_FILE_HEAD_PROC_NAME) == UTILS::LOG_FILE_HEAD_PROC_NAME){
			_snprintf_s(szTmp, _TRUNCATE, "[]");
			strncat_s(szLog, 1024 * 16, szTmp, min(strlen(szTmp), 1024 * 16 - 1 - strlen(szLog)));
		}
		if ((m_dwHead&UTILS::LOG_FILE_HEAD_PROC_ID) == UTILS::LOG_FILE_HEAD_PROC_ID){
			_snprintf_s(szTmp, _TRUNCATE, "[PID:%d]",GetCurrentProcessId());
			strncat_s(szLog, 1024 * 16, szTmp, min(strlen(szTmp), 1024 * 16 - 1 - strlen(szLog)));
		}
		if ((m_dwHead&UTILS::LOG_FILE_HEAD_THREAD_ID) == UTILS::LOG_FILE_HEAD_THREAD_ID){
			_snprintf_s(szTmp, _TRUNCATE, "[TID:%d]", GetCurrentThreadId());
			strncat_s(szLog, 1024 * 16, szTmp, min(strlen(szTmp), 1024 * 16 - 1 - strlen(szLog)));
		}
		if ((m_dwHead&UTILS::LOG_FILE_HEAD_FILE_NAME) == UTILS::LOG_FILE_HEAD_FILE_NAME){
			if (nullptr != file){
				_snprintf_s(szTmp, _TRUNCATE, "[FILE:%s]", file);
				strncat_s(szLog, 1024 * 16, szTmp, min(strlen(szTmp), 1024 * 16 - 1 - strlen(szLog)));
			}
		}
		if ((m_dwHead&UTILS::LOG_FILE_HEAD_FILE_LINE) == UTILS::LOG_FILE_HEAD_FILE_LINE){
			_snprintf_s(szTmp, _TRUNCATE, "[LINE:%d]", line);
			strncat_s(szLog, 1024 * 16, szTmp, min(strlen(szTmp), 1024 * 16 - 1 - strlen(szLog)));
		}
		int iTotalLen = strlen(szLog) + len;
		iTotalLen = min(len, 1024 * 16 - 1);
		memcpy(szLog + strlen(szLog), log, iTotalLen);
		szLog[iTotalLen] = '\0';
		if (pNewLen != nullptr){
			*pNewLen = iTotalLen;
		}
		return szLog;
	}
private:
	/**< ��־�ļ�ģʽ*/
	UTILS::LOG_FILE_MODE m_nMode;
	/**< ��־��¼ͷ��Ϣ*/
	DWORD m_dwHead;
	/**< �洢���� .>0���ж��Ƿ񳬹�ָ���洢����,��������Զ�ɾ���������־*/
	unsigned int m_uiDays;
	/**< �����ļ���С .>0���ж��Ƿ񳬹�ָ����С,��������Զ��л��ļ�*/
	unsigned int m_uiPerFileSize;
	/**< ��ǰ��־�ļ�*/
	char m_szFile[MAX_PATH];
	/**< ��ʷ��־�ļ�������*/
	std::map<std::string,std::list<LogHistoryFile>> m_mapFiles;
	/**< ��ǰ����*/
	LogFileDate m_stDate;
	/**< ��ⳬʱʱ��*/
	DWORD m_dwCheckTimestamp;
	/**< �ļ�����ʵ��*/
	UTILS::CFile m_logfile;
	UTILS::CLock m_lock;
	/**< �ڲ�д��־�ļ��߳�*/
	UTILS::CThreadBox m_WriteThread;
};

namespace UTILS
{
	CLogFile::CLogFile()
	{
		m_pFile = new CLogFileProperty();
	}

	CLogFile::~CLogFile()
	{

	}

	int CLogFile::Open(LOG_FILE_MODE nMode, DWORD nHead, const char* file, unsigned int uiDays, unsigned int uiPerFileSize)
	{
		if (nullptr == m_pFile){
			return -1;
		}
		return ((CLogFileProperty*)m_pFile)->Init(nMode, nHead, file, uiDays, uiPerFileSize);
	}

	void CLogFile::Close()
	{
		if (nullptr == m_pFile){
			return;
		}
		((CLogFileProperty*)m_pFile)->UnInit();
	}

	void CLogFile::Write(const char* pFormat, ...)
	{
		if (nullptr == m_pFile){
			return;
		}
		char buff[1024 * 8];
		va_list args;
		va_start(args, pFormat);
		_vsnprintf_s(buff, sizeof(buff)-1, pFormat, args);
		va_end(args);
		((CLogFileProperty*)m_pFile)->Write(buff, strlen(buff));
	}

	void CLogFile::WriteEx(const char* file, int line, const char* pFormat, ...)
	{
		if (nullptr == m_pFile){
			return;
		}
		char buff[1024 * 8];
		va_list args;
		va_start(args, pFormat);
		_vsnprintf_s(buff, sizeof(buff)-1, pFormat, args);
		va_end(args);
		((CLogFileProperty*)m_pFile)->Write(file, line,buff, strlen(buff));
	}

	void CLogFile::Active()
	{
		if (nullptr == m_pFile){
			return;
		}
		((CLogFileProperty*)m_pFile)->Active();
	}
}
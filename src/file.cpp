#include "../utils/file.h"
#include "../utils/api.h"
#include "internal.h"
#include <IO.H>
namespace UTILS
{
	CFile::CFile()
	{
		Clear();
	}

	CFile::CFile(const char* path)
	{
		Clear();
		if (nullptr != path) {
			API::Strcpy(m_stPath.szPath, sizeof(m_stPath.szPath), path);
		}
	}
	CFile::~CFile()
	{

	}

	bool CFile::operator==(const CFile& file)
	{
		return false;
	}

	void CFile::Clear()
	{
		API::Memset(&m_stPath, 0, sizeof(m_stPath));
	}

	int CFile::GetStat(const char* path, PFILE_PROPERTY pInfo)
	{
		struct _stat64	st;
		if (path == NULL || strlen(path) <= 0 || pInfo == NULL){
			return UTILS::UTILS_ERROR_PAR;
		}
		if (_stat64(path, &st) == 0) {
			pInfo->st_size = st.st_size;
			pInfo->st_atime = st.st_atime;
			pInfo->st_mtime = st.st_mtime;
			pInfo->st_ctime = st.st_ctime;
		}else{
			return UTILS_ERROR_FAIL;
		}

		return UTILS_ERROR_SUCCESS;
	}

	bool CFile::Exists(const char* path)
	{
		return API::IsPathExists(path);
	}

	uint64_t CFile::Size(const char* path) {

		CFile file;
		file.Open(PATH_FILE_OPENMODE_IN| PATH_FILE_OPENMODE_BINARY, path);
		uint64_t l= file.Size();
		file.Close();
		return l;
	}

	uint64_t CFile::Size() {
		if (!m_file.is_open()){
			return UTILS_ERROR_FAIL;
		}
		if ((m_stPath.uiMode&PATH_FILE_OPENMODE_IN) == PATH_FILE_OPENMODE_IN) {
			m_file.seekg(0, std::ios::beg);
			uint64_t b = m_file.tellg();
			m_file.seekg(0, std::ios::end);
			uint64_t e = m_file.tellg();
			return e - b;
		}
		else {
			m_file.seekp(0, std::ios::beg);
			uint64_t b = m_file.tellp();
			m_file.seekp(0, std::ios::end);
			uint64_t e = m_file.tellp();
			return e - b;
		}
	}

	void CFile::GetDir(const char* path, char* buff, int len)
	{
		if (NULL == buff || len <= 1){
			return;
		}
		API::Strcpy(buff, len, path);
		API::RemoveFileSpec(buff);
	}

	void CFile::GetFileName(const char* path, char* buff, int len)
	{
		if (NULL == buff || len <= 1){
			return;
		}
		char szPath[MAX_PATH];
		API::Strcpy(szPath, sizeof(szPath), path);
		API::StripPath(szPath);
		API::Strcpy(buff, len, szPath);
	}

	void CFile::GetSuffix(const char* path, char* buff, int len)
	{
		if (NULL == buff || len <= 1){
			return;
		}
		char szPath[MAX_PATH];;
		szPath[0] = '\0';
		API::Strcpy(szPath, sizeof(szPath), path);
		char* p = API::FindExtension(szPath);
		if (NULL == p){
			return;
		}
		API::Strcpy(buff, len, p);
	}

	int CFile::Open(unsigned int mode, const char* path)
	{
		if (mode == 0 || NULL == path || strlen(path) <= 0){
			return UTILS_ERROR_PAR;
		}
		Close();
		unsigned int uiMode =0;
		if ((mode&PATH_FILE_OPENMODE_IN) == PATH_FILE_OPENMODE_IN){
			uiMode |= std::ios_base::in;
		}
		if ((mode&PATH_FILE_OPENMODE_OUT) == PATH_FILE_OPENMODE_OUT){
			uiMode |= std::ios_base::out;
		}
		if ((mode&PATH_FILE_OPENMODE_ATE) == PATH_FILE_OPENMODE_ATE){
			uiMode |= std::ios_base::ate;
		}
		if ((mode&PATH_FILE_OPENMODE_APP) == PATH_FILE_OPENMODE_APP){
			uiMode |= std::ios_base::app;
		}
		if ((mode&PATH_FILE_OPENMODE_TRUNC) == PATH_FILE_OPENMODE_TRUNC){
			uiMode |= std::ios_base::trunc;
		}
		if ((mode&PATH_FILE_OPENMODE_BINARY) == PATH_FILE_OPENMODE_BINARY){
			uiMode |= std::ios_base::binary;
		}
		m_stPath.uiMode = uiMode;
		m_file.open(path, uiMode);
		if (!m_file.is_open() || m_file.bad()){
			return UTILS_ERROR_FAIL;
		}
		API::Sprintf(m_stPath.szPath, sizeof(m_stPath.szPath), path);
		m_stPath.bOpen = true;
		return UTILS_ERROR_SUCCESS;
	}

	int CFile::ReOpen()
	{
		Close();
		m_file.open(m_stPath.szPath, m_stPath.uiMode);
		if (!m_file.is_open() || m_file.bad()){
			return UTILS_ERROR_FAIL;
		}
		return UTILS_ERROR_SUCCESS;
	}

	int CFile::Write(const char* data, int len)
	{
		if (NULL == data || len <= 0){
			return UTILS_ERROR_PAR;
		}
		if (!m_file.is_open()){
			return UTILS_ERROR_FAIL;
		}
		if (m_file.bad()){
			return UTILS_ERROR_FAIL;
		}

		m_file.write(data, len);
		if (m_file.bad()){
			return UTILS_ERROR_FAIL;
		}

		return UTILS_ERROR_SUCCESS;
	}

	int CFile::Read(char* buff, int len, int* pDataLen)
	{
		if (NULL == buff || len <= 0){
			return UTILS_ERROR_PAR;
		}
		if (!m_file.is_open()){
			return UTILS_ERROR_FAIL;
		}
		if (m_file.bad()){
			return UTILS_ERROR_FAIL;
		}
		m_file.read(buff, len);
		if (m_file.bad()){
			return UTILS_ERROR_FAIL;
		}
		if (pDataLen != nullptr) {
			*pDataLen = m_file.gcount();
		}
		return UTILS_ERROR_SUCCESS;
	}

	int CFile::Seek(unsigned int offset, PATH_SEEK_TYPE origin)
	{
		if (!m_file.is_open()){
			return UTILS_ERROR_FAIL;
		}
		int flag = -1;
		switch (origin)
		{
		case  UTILS::PATH_SEEK_SET:
			flag = std::ios::beg;
			break;
		case  UTILS::PATH_SEEK_CUR:
			flag = std::ios::cur;
			break;
		case  UTILS::PATH_SEEK_END:
			flag = std::ios::end;
			break;
		default:
			break;
		}
		if ((m_stPath.uiMode&PATH_FILE_OPENMODE_IN) == PATH_FILE_OPENMODE_IN){
			if (flag != -1){
				m_file.seekg(offset, (unsigned int)flag);
			}else {
				m_file.seekg(offset);
			}
		}
		else {
			if (flag != -1){
				m_file.seekp(offset, (unsigned int)flag);
			}
			else {
				m_file.seekp(offset);
			}
		}
		if (m_file.bad() && m_file.fail()){
			return UTILS_ERROR_FAIL;
		}
		return UTILS_ERROR_SUCCESS;
	}

	int CFile::Close()
	{
		if (!m_file.is_open()){
			return UTILS_ERROR_FAIL;
		}
		m_file.close();
		m_file.clear();
		m_stPath.bOpen = false;
		//seek
		return UTILS_ERROR_SUCCESS;
	}

	int CFile::Read(int mode, const char* path, char* buff, int len, int* pDataLen /*= nullptr*/)
	{
		UTILS::CFile file;
		file.Open(mode,path);
		if (!file.IsOpen()) {
			return UTILS_ERROR_FAIL;
		}
		int err = file.Read(buff, len, pDataLen);
		if (err != UTILS_ERROR_SUCCESS) {
			return err;
		}
		file.Close();
		return err;
	}

	int CFile::Write(int mode, const char* path, const char* data, int len)
	{
		UTILS::CFile file;
		file.Open(mode, path);
		if (!file.IsOpen()) {
			return UTILS_ERROR_FAIL;
		}
		int err = file.Write(data, len);
		if (err != UTILS_ERROR_SUCCESS) {
			return err;
		}
		file.Close();
		return err;
	}

	int CFile::Delete(const char* path){
		bool ret = API::DelFile(path) ? true : false;
		return ret ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
	}

	int CFile::Delete(){
		bool ret = API::DelFile(m_stPath.szPath) ? true : false;
		Clear();
		return ret ? UTILS_ERROR_SUCCESS : UTILS_ERROR_FAIL;
	}

	void CFile::SetContextData(void* p) {
		m_stPath.pContext = p;
	}

	void* CFile::GetContextData() {
		return m_stPath.pContext;
	}
}
#include "../../utils/ConfigFile.h"
#include "../../utils/api.h"
namespace UTILS
{
	ConfigFile::ConfigFile()
	{
		API::Memset(szConfigFile, 0, sizeof(szConfigFile));
	}

	ConfigFile::ConfigFile(const char* path)
	{
		API::Memset(szConfigFile, 0, sizeof(szConfigFile));
		API::Strcpy(szConfigFile, path, strlen(path));
	}
	ConfigFile::~ConfigFile()
	{

	}

	void ConfigFile::SetConfigFile(const char* path)
	{
		API::Strcpy(szConfigFile, path, strlen(path));
	}

	int ConfigFile::ReadData(const char* lpszFileName,
		std::list<std::map<std::string, std::list<std::pair<std::string, std::string>>>>& lstContent)
	{
#ifdef _DEBUG
		assert(lpszFileName != NULL && strlen(lpszFileName) > 0);
#else
		if (lpszFileName == NULL || strlen(lpszFileName) <= 0)
		{
			return UTILS_ERROR_PAR;
		}
#endif
		char* pAppData = NULL;
		int iAppNameLen = 0, iAppDataLen = 0, iKeyNameLen = 0;
		char szReturn[1024], szSection[1024 * 16];
		int iTmp = 0;

		if (lpszFileName == NULL || strlen(lpszFileName) <= 0)
			return UTILS_ERROR_PAR;
		if (!API::IsPathExists(lpszFileName))
		{
			return UTILS_ERROR_EXISTS;
		}

		lstContent.clear();

		struct _stat64	st;
		if (_stat64(lpszFileName, &st) == 0) {
			iTmp = (int)st.st_size;
		}

		if (iTmp <= 0)
		{
			return UTILS_ERROR_FAIL;
		}
		iTmp = PAD_SIZE(iTmp);
		pAppData = (char*)new BYTE[iTmp];

		//如果此参数为NULL,将配置文件中的所有节点名称复制到lpReturnedString参数指定的缓冲区
		iAppDataLen = GetPrivateProfileString(NULL, NULL, NULL, pAppData, iTmp, lpszFileName);
		if (iAppDataLen <= 0) {
			return UTILS_ERROR_FAIL;
		}
		char* pAppName = new char[iAppDataLen];

		if (pAppName == NULL) {
			return UTILS_ERROR_FAIL;
		}
		iAppNameLen = 0;

		for (int i = 0; i < iAppDataLen; i++)
		{
			pAppName[iAppNameLen++] = pAppData[i];

			if (pAppData[i] != '\0')
			{
				continue;
			}
			//如果此参数为NULL,将配置文件中的指定的节点下的关键字复制到lpReturnedString参数指定的缓冲区
			iKeyNameLen = GetPrivateProfileString(pAppName, NULL, NULL, szSection, 1024 * 16, lpszFileName);
			if (iKeyNameLen <= 0)
			{
				memset(pAppName, 0, iAppDataLen);
				iAppNameLen = 0;
				continue;
			}

			std::map<std::string, std::list<std::pair<std::string, std::string>>> mapSection;
			std::list<std::pair<std::string, std::string>> lstKeys;
			auto retIter = mapSection.emplace(pAppName, lstKeys);
			if (!retIter.second)
			{
				memset(pAppName, 0, iAppDataLen);
				iAppNameLen = 0;
				continue;
			}

			char* pKeyName = new char[iKeyNameLen];
			int nKeyNameLen = 0; //每个KeyName的长度
								 //ZeroMemory(pKeyName, sizeof(TCHAR)*dwKeyNameSize);

			for (int j = 0; j < iKeyNameLen; j++)
			{
				pKeyName[nKeyNameLen++] = szSection[j];

				if (szSection[j] != '\0')
				{
					continue;
				}
				if (GetPrivateProfileString(pAppName, pKeyName, NULL, szReturn, 1024, lpszFileName))
				{
					retIter.first->second.emplace(retIter.first->second.end(),
						std::pair<std::string, std::string>(pKeyName, szReturn));
				}
				memset(pKeyName, 0, iKeyNameLen);
				nKeyNameLen = 0;
			}

			delete[] pKeyName;
			memset(pAppName, 0, iAppDataLen);
			iAppNameLen = 0;

			lstContent.emplace(lstContent.end(), mapSection);
		}
		delete[] pAppName;
		return UTILS_ERROR_SUCCESS;
	}

	int ConfigFile::ReadStr(const char* lpszFileName,
		const char* lpszSection,
		const char* lpszValueName,
		char* lpszValue,
		int iValueBuffLen,
		char* lpszDefaultValue /*= NULL*/)
	{
		if (NULL == lpszValue || iValueBuffLen <= 0 || NULL == lpszFileName || NULL == lpszSection || NULL == lpszValueName)
		{
			return UTILS_ERROR_PAR;
		}
		if (!API::IsPathExists(lpszFileName))
		{
			return UTILS_ERROR_EXISTS;
		}
		::GetPrivateProfileString(lpszSection, lpszValueName, lpszDefaultValue == NULL ? "" : lpszDefaultValue, lpszValue, iValueBuffLen, lpszFileName);
		return 0;
	}

	int ConfigFile::ReadInt(const char* lpszFileName,
		const char* lpszSection,
		const char* lpszValueName,
		int& iValue,
		int iDefaultValue /*= 0*/)
	{
		if (NULL == lpszFileName || NULL == lpszSection || NULL == lpszValueName)
		{
			return UTILS_ERROR_PAR;
		}
		if (!API::IsPathExists(lpszFileName))
		{
			return UTILS_ERROR_EXISTS;
		}
		iValue = ::GetPrivateProfileInt(lpszSection, lpszValueName, iDefaultValue, lpszFileName);
		return 0;
	}

	int ConfigFile::WriteStr(const char* lpszFileName,
		const char* lpszSection,
		const char* lpszValueName,
		const char* lpszValue)
	{
		if (NULL == lpszValue || NULL == lpszFileName || NULL == lpszSection || NULL == lpszValueName)
		{
			return UTILS_ERROR_PAR;
		}
		if (!API::IsPathExists(lpszFileName))
		{
			return UTILS_ERROR_EXISTS;
		}
		::WritePrivateProfileString(lpszSection, lpszValueName, lpszValue, lpszFileName);
		return 0;
	}

	int ConfigFile::WriteInt(const char* lpszFileName,
		const char* lpszSection,
		const char* lpszValueName,
		int iValue)
	{
		if (NULL == lpszFileName || NULL == lpszSection || NULL == lpszValueName)
		{
			return UTILS_ERROR_PAR;
		}
		if (!API::IsPathExists(lpszFileName))
		{
			return UTILS_ERROR_EXISTS;
		}
		char szValue[32] = { 0 };
		API::Sprintf(szValue, sizeof(szValue), "%d", iValue);
		::WritePrivateProfileString(lpszSection, lpszValueName, szValue, lpszFileName);
		return 0;
	}

	int ConfigFile::DeleteKey(const char* lpszFileName,
		const char* lpszSection,
		const char* lpszKeyName)
	{
		if (NULL == lpszFileName || NULL == lpszSection || NULL == lpszKeyName)
		{
			return UTILS_ERROR_PAR;
		}
		if (!API::IsPathExists(lpszFileName))
		{
			return UTILS_ERROR_EXISTS;
		}
		::WritePrivateProfileString(lpszSection, lpszKeyName, NULL, lpszFileName);
		return 0;
	}

	int ConfigFile::DeleteNode(const char* lpszFileName,
		const char* lpszSection)
	{
		if (NULL == lpszFileName || NULL == lpszSection)
		{
			return UTILS_ERROR_PAR;
		}
		if (!API::IsPathExists(lpszFileName))
		{
			return UTILS_ERROR_EXISTS;
		}
		::WritePrivateProfileString(lpszSection, NULL, NULL, lpszFileName);
		return 0;
	}

	int ConfigFile::Clear(const char* lpszFileName)
	{
		std::list<std::map<std::string, std::list<std::pair<std::string, std::string>>>> lstContent;
		int ret = ConfigFile::ReadData(lpszFileName, lstContent);
		if (ret != UTILS_ERROR_SUCCESS)
		{
			return ret;
		}
		for (auto& it : lstContent)
		{
			for (auto& itNode : it)
			{
				ConfigFile::DeleteNode(lpszFileName, itNode.first.data());
			}
		}
		return UTILS_ERROR_SUCCESS;
	}
}
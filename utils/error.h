#ifndef __UTILS_ERROR__
#define __UTILS_ERROR__


namespace UTILS
{
	enum
	{
		/// û�д���
		UTILITY_ERROR_SUCCESS = 0,
		/// ��������
		UTILITY_ERROR_PAR = 1,
		/// �����ڴ�ʧ��
		UTILITY_ERROR_MEMORY = 2,
		/// �쳣
		UTILITY_ERROR_ABNORMAL = 3,
		/// δ�ҵ�,Ŀ¼/�ļ�/...
		UTILITY_ERROR_FIND = 4,
		/// ����ʧ��..
		UTILITY_ERROR_FAIL = 5,
		/// ��ʱ..
		UTILITY_ERROR_TIMEOUT = 6,
		/// ��֧��..
		UTILITY_ERROR_NOSUPPOERT = 7,
		/// ������..
		UTILITY_ERROR_EXISTS = 8,
	};
}
#endif //_TYPEDEF_H__

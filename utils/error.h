#ifndef __UTILS_ERROR__
#define __UTILS_ERROR__


namespace UTILS
{
	enum
	{
		/// û�д���
		UTILS_ERROR_SUCCESS = 0,
		/// ��������
		UTILS_ERROR_PAR = -1,
		/// �����ڴ�ʧ��
		UTILS_ERROR_MEMORY = -2,
		/// �쳣
		UTILS_ERROR_ABNORMAL = -3,
		/// δ�ҵ�,Ŀ¼/�ļ�/...
		UTILS_ERROR_FIND = -4,
		/// ����ʧ��..
		UTILS_ERROR_FAIL = -5,
		/// ��ʱ..
		UTILS_ERROR_TIMEOUT = -6,
		/// ��֧��..
		UTILS_ERROR_NOSUPPOERT = -7,
		/// ������..
		UTILS_ERROR_EXISTS = -8,
		/// ������̫С..
		UTILS_ERROR_BUFFER_SMALL = -9,
		/// �����������ʧ��..
		UTILS_ERROR_DLL = -10,
	};
}
#endif //_TYPEDEF_H__

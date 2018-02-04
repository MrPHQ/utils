#ifndef __UTILS_ERROR__
#define __UTILS_ERROR__


namespace UTILS
{
	enum
	{
		/// 没有错误
		UTILITY_ERROR_SUCCESS = 0,
		/// 参数错误
		UTILITY_ERROR_PAR = 1,
		/// 分配内存失败
		UTILITY_ERROR_MEMORY = 2,
		/// 异常
		UTILITY_ERROR_ABNORMAL = 3,
		/// 未找到,目录/文件/...
		UTILITY_ERROR_FIND = 4,
		/// 操作失败..
		UTILITY_ERROR_FAIL = 5,
		/// 超时..
		UTILITY_ERROR_TIMEOUT = 6,
		/// 不支持..
		UTILITY_ERROR_NOSUPPOERT = 7,
		/// 不存在..
		UTILITY_ERROR_EXISTS = 8,
	};
}
#endif //_TYPEDEF_H__

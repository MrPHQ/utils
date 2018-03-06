#ifndef __UTILS_ERROR__
#define __UTILS_ERROR__


namespace UTILS
{
	enum
	{
		/// 没有错误
		UTILS_ERROR_SUCCESS = 0,
		/// 参数错误
		UTILS_ERROR_PAR = -1,
		/// 分配内存失败
		UTILS_ERROR_MEMORY = -2,
		/// 异常
		UTILS_ERROR_ABNORMAL = -3,
		/// 未找到,目录/文件/...
		UTILS_ERROR_FIND = -4,
		/// 操作失败..
		UTILS_ERROR_FAIL = -5,
		/// 超时..
		UTILS_ERROR_TIMEOUT = -6,
		/// 不支持..
		UTILS_ERROR_NOSUPPOERT = -7,
		/// 不存在..
		UTILS_ERROR_EXISTS = -8,
		/// 缓存区太小..
		UTILS_ERROR_BUFFER_SMALL = -9,
		/// 第三方库加载失败..
		UTILS_ERROR_DLL = -10,
	};
}
#endif //_TYPEDEF_H__

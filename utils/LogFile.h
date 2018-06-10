#ifndef __UTILS_LOG_FILE__
#define __UTILS_LOG_FILE__
#include <utils/config.h>
#include <utils/define.h>

namespace UTILS
{
	/**
	\brief
		日志文件模式.
	*/
	enum LOG_FILE_MODE {
		LOG_FILE_MODE_NONE = 0,
		/**< 同步写文件 */
		LOG_FILE_MODE_SYNC = 1,
		/**< 异步写文件,内部线程自动写文件 */
		LOG_FILE_MODE_ASYNC_IN = 2,
		/**< 异步写文件,外部触发写文件,否则不记录文件 */
		LOG_FILE_MODE_ASYNC_OUT = 3,
	};

	/**
	\brief
		日志文件记录头信息.
	*/
	enum LOG_FILE_HEAD {
		/**< 写时间 */
		LOG_FILE_HEAD_TIME = 0x01,
		/**< 写进程名 */
		LOG_FILE_HEAD_PROC_NAME = 0x02,
		/**< 写进程ID */
		LOG_FILE_HEAD_PROC_ID = 0x04,
		/**< 写线程ID */
		LOG_FILE_HEAD_THREAD_ID = 0x08,
		/**< 写文件名 */
		LOG_FILE_HEAD_FILE_NAME = 0x10,
		/**< 写行号 */
		LOG_FILE_HEAD_FILE_LINE = 0x20,
	};

	/**
	\brief
		日志文件类.
	*/
	class UTILS_API CLogFile
	{
	public:
		/**
		\brief
			构造函数,默认使用当前程序的根目录
		*/
		CLogFile();
		~CLogFile();

		/**
		\brief
			打开文件

			注意: 日志指定的日志文件不存在,则会创建,如果已经存在,则打开后直接定位到文件末尾.
		\param nMode
			日志文件操作模式,
		\param nHead
			日志文件记录头信息. 参见LOG_FILE_HEAD
		\param file
			日志文件路径.
		\param uiDays
			存储天数
		\param uiPerFileSize
			每个日志文件最大容量. 字节.
		\return
			Error.h
		*/
		int Open(LOG_FILE_MODE nMode, DWORD nHead, const char* file, unsigned int uiDays, unsigned int uiPerFileSize);
		/**
		\brief
			关闭文件
		*/
		void Close();
		/**
		\brief
			写入日志.
			日志文件模式(LOG_FILE_MODE):
				LOG_FILE_MODE_SYNC 立即写入日志文件.
				LOG_FILE_MODE_ASYNC_IN 写入内部缓存区,内部日志线程延迟写入日志文件
				LOG_FILE_MODE_ASYNC_OUT 写入内部缓存区,然后外部调用[Active],触发写入日志文件
		\param data
			日志内容
		\param len
			日志大小
		*/
		void Write(const char* pFormat, ...);
		/**
		\brief
			写入日志.
			日志文件模式(LOG_FILE_MODE):
				LOG_FILE_MODE_SYNC 立即写入日志文件.
				LOG_FILE_MODE_ASYNC_IN 写入内部缓存区,内部日志线程延迟写入日志文件
				LOG_FILE_MODE_ASYNC_OUT 写入内部缓存区,然后外部调用[Active],触发写入日志文件
		\param file
			日志位置所在文件.
		\param line
			日志所在位置文件行号.
		\param data
			日志内容
		\param len
			日志大小
		*/
		void WriteEx(const char* file, int line, const char* pFormat, ...);
		/**
		\brief
			写入日志. 把缓存里的日志写入日志文件
			只适用	日志文件模式(LOG_FILE_MODE):LOG_FILE_MODE_ASYNC_OUT
		*/
		void Active();

	public:
		static inline CLogFile* getSingletonPtr()
		{
			return &getSingleton();
		}
		static inline CLogFile& getSingleton()
		{
			static CLogFile _instance;
			return _instance;
		}

	protected:

		CLogFile(CLogFile const &) = delete;
		CLogFile(CLogFile &&) = delete;
		CLogFile& operator= (CLogFile const &) = delete;
		CLogFile& operator= (CLogFile &&) = delete;

	private:
		void* m_pFile;
	};

#define LF_LOGGER CLogFile::getSingleton()
#define PLF_LOGGER CLogFile::getSingletonPtr()

#define LOG_INIT(P1,P2,P3,P4,P5) LF_LOGGER.Open(P1,P2,P3,P4,P5);
#define LOG_UNINIT() LF_LOGGER.Close();
#define LOG_WRITE(...) LF_LOGGER.Write(__VA_ARGS__);
#define LOG_WRITE_EX(...) LF_LOGGER.WriteEx(__FILE__, __LINE__, __VA_ARGS__);
#define LOG_ACTIVE() LF_LOGGER.Active();
}

#endif	//__AUTOLOCK_H__

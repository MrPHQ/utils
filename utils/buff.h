#ifndef __UTILS_BUFF__
#define __UTILS_BUFF__

#include <utils/config.h>
#include <utils/define.h>

/**< 最大缓存区大小.*/
#define BUFFER_MAX_LEN 1024*1024*1024

namespace UTILS
{
	namespace BUFFER
	{
		/**
		@brief
		环形(循环)缓存区(CRingBuffer) class.
		*/

		class UTILS_API CRingBuffer
		{
		public:
			CRingBuffer();
			/*
			@brief 构造函数..
				客户端创建套接字使用.内部会创建\连接套接字
			\param uiCapacity
				缓存大小
			*/
			CRingBuffer(size_t uiCapacity);
			virtual ~CRingBuffer();
			void Init(size_t uiCapacity);
			void UnInit();

			/*
			@brief 判断缓冲区为空..
			\return [bool]
			*/
			bool IsEmpty() const { return _in == _out; }

			/*
			@brief 判断缓冲区满..
			\return [bool]
			*/
			bool IsFull() const { return _size == (_in - _out); }

			/*
			@brief 获取缓存区起始位置..
			\return [ptr]
			*/
			const char* GetBuffer() const { return _buffer; }

			/*
			@brief 获取缓存区数据起始位置..
			\return [ptr]
			*/
			const char* GetDataStart() const { return _buffer + _out; }

			/*
			@brief 获取缓存区数据结束位置..
			\return [ptr]
			*/
			const char* GetDataEnd() const {
				char log[64];
				OutputDebugString("xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
				_snprintf_s(log, _TRUNCATE, ".............:%d %d", _buffer, _in);
				OutputDebugString(log);
				return _buffer + _in; 
			}

			/*
			@brief 获取缓存区大小..
			\return [size_t]
			*/
			size_t Capacity() const { return _size; }

			/*
			@brief 获取缓存区数据大小..
			\return [size_t]
			*/
			size_t Size();

			/*
			@brief 获取缓存区可用空间大小..
			\return [size_t]
			*/
			size_t Space();

			/*
			@brief 复位数据..
			\return [size_t]
			*/
			void Clear();

			/*
			@brief 修改缓存区数据大小.
			\param size
				缓存区数据新增指定大小字节数据
			\return [void]
			*/
			void Write(size_t size);

			/*
			@brief 向缓存区写入数据.
			\param data
				数据缓存区
			\param bytesWriteOfNumber
				待写入数据大小.
			\return [size_t]
				返回写入数据大小
			*/
			size_t Write(const char *data, size_t bytesWriteOfNumber);

			/*
			@brief 从缓存区读取指定大小数据.
			\param buff
				数据接收缓存区
			\param bytesReadOfNumber
				待读取数据大小.
			\return [size_t]
				返回读取的数据大小
			*/
			size_t Read(char* buff, size_t bytesReadOfNumber);
			/*
			@brief 从缓存区读取指定大小数据.
			\param buff
				指向数据缓存区的数据指针.
			\param bytesReadOfNumber
				待读取数据大小.
			\return [size_t]
				返回读取的数据大小
			*/
			size_t ReadEx(char*& buff, size_t bytesReadOfNumber);

		private:
			/**< 可用数据大小. */
			size_t _len() const { return (_in - _out); }
			/**< 可用空间大小. */
			size_t _space() const { return (_size - _in + _out); }
			/**< 写入数据. */
			size_t _write(const char *data, size_t bytes);
			/**< 读取数据. */
			size_t _read(char *data, size_t bytes);
			size_t _read2(char*& buff, size_t bytesReadOfNumber);
			/**< 初始化锁. */
			void _InitLock();
			void _UnInitLock();

			CRingBuffer(CRingBuffer const &) = delete;
			CRingBuffer(CRingBuffer &&) = delete;
			CRingBuffer& operator= (CRingBuffer const &) = delete;
			CRingBuffer& operator= (CRingBuffer &&) = delete;
		protected:
			/**< 缓存区*/
			char* _buffer;
			/**< 缓存区大小*/
			uint32_t _size;
			/**< 数据最后写入位置.*/
			uint32_t _in;
			/**< 数据读取的最后位置.*/
			uint32_t _out;
			/**< 临时缓存区.*/
			struct {
				char* _buffer;
				uint32_t _size;
			}_temp_buffer;
#ifdef _WIN32
			CRITICAL_SECTION _lock;
#else
			pthread_mutex_t _lock;    //互斥锁
#endif
			bool _init;
			char _buff[1024 * 16];
		};
	}
}


#endif	//__AUTOLOCK_H__

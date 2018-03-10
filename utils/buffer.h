#ifndef __UTILS_CIRCULAR_BUFFER__
#define __UTILS_CIRCULAR_BUFFER__

#include <utils/config.h>

#define BUFFER_MAX_LEN 1024*1024*1024

namespace UTILS
{
	/**
	\brief
		CircularBuffer class.
		环形(循环)缓存区
	*/

	class UTILS_API buffer
	{
	public:
		buffer();
		buffer(size_t capacity);
		virtual ~buffer();

#ifdef _WIN32
		void init(size_t capacity, CRITICAL_SECTION* lock = nullptr);
#else
		void init(size_t capacity, pthread_mutex_t* lock = nullptr);
#endif
		void uninit();

		const char* get_buffer() const { return _buffer; }
		const char* data_start() const { return _buffer + _out; }
		const char* data_end() const { return _buffer + _in; }
		size_t capacity() const { return _size; }

		/// 可用数据.
		size_t size() const;
		/// 可用空间.
		size_t space() const;

		virtual bool is_wiret() const = 0;

		void capacity(size_t size, bool reserve = true);
		// Return number of bytes written.
		size_t write(const char *data, size_t bytes);
		// Return number of bytes read.
		size_t read(char *data, size_t bytes);
		size_t read2(size_t bytes, char*& data1, size_t& bytes1, char*& data2, size_t& bytes2);

	private:
		/// 可用数据.
		size_t _len() const { return (_in - _out); }
		/// 可用空间.
		size_t _space() const { return (_size - _in + _out); }
		size_t _write(const char *data, size_t bytes);
		// Return number of bytes read.
		size_t _read(char *data, size_t bytes);
		size_t _read2(size_t bytes, char*& data1, size_t& bytes1, char*& data2, size_t& bytes2);
	protected:
		char* _buffer;//缓冲区
		uint32_t _size;//大小
		uint32_t _in;//入口位置
		uint32_t _out;//出口位置
#ifdef _WIN32
		CRITICAL_SECTION* _lock;
#else
		pthread_mutex_t* _lock;    //互斥锁
#endif
		bool _init;
		char _buff[1024 * 16];
	};

	/**
	\brief
		CircularBuffer class.
		环形(循环)缓存区
	*/
	class UTILS_API CircularBuffer :
		public buffer
	{
	public:
		CircularBuffer();
		CircularBuffer(size_t capacity);
		~CircularBuffer();

		bool is_wiret() const;

		CircularBuffer(CircularBuffer const &) = delete;
		CircularBuffer(CircularBuffer &&) = delete;
		CircularBuffer & operator = (CircularBuffer const &) = delete;
		CircularBuffer & operator = (CircularBuffer &&) = delete;
	};

	/**
	\brief
		CircularBuffer class.
		边界缓存区
	*/

	class UTILS_API BoundaryBuffer :
		public buffer
	{
	public:
		BoundaryBuffer();
		BoundaryBuffer(size_t capacity);
		~BoundaryBuffer();
	
		bool is_wiret() const;

		BoundaryBuffer(BoundaryBuffer const &) = delete;
		BoundaryBuffer(BoundaryBuffer &&) = delete;
		BoundaryBuffer & operator = (BoundaryBuffer const &) = delete;
		BoundaryBuffer & operator = (BoundaryBuffer &&) = delete;

	private:
		bool _is_wiret() const;
	};
}


#endif	//__AUTOLOCK_H__

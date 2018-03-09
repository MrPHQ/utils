#ifndef __UTILS_CIRCULAR_BUFFER__
#define __UTILS_CIRCULAR_BUFFER__

#include <utils/config.h>

#define CIRCULAR_BUFFER_MAX_LEN 1024*1024*1024

namespace UTILS
{
	/**
	\brief
		CircularBuffer class.
		环形(循环)缓存区
	*/

	class UTILS_API CBaseBuffer
	{
	public:
		CBaseBuffer();
		CBaseBuffer(size_t capacity);
		virtual ~CBaseBuffer();

		void init(size_t capacity);
		void uninit();

		const char* buffer() const { return _data; }
		const char* data_start() const { return _data + _beg_index; }
		const char* data_end() const { return _data + _end_index; }
		size_t capacity() const { return _capacity; }
		void reset();

		/// 可用数据.
		virtual size_t size() const { return _size; }
		/// 可用空间.
		virtual size_t space() const { return _capacity - _size; }

		virtual bool empty() const = 0;
		virtual bool full() const = 0;

		virtual void capacity(size_t size, bool reserve = true) = 0;
		// Return number of bytes written.
		virtual size_t write(const char *data, size_t bytes) = 0;
		// Return number of bytes read.
		virtual size_t read(char *data, size_t bytes) = 0;
		virtual size_t read2(char*& data, size_t bytes) = 0;

	protected:
		size_t _beg_index, _end_index, _size, _capacity;
		bool _init;
		char* _data;
		char _buff[1024 * 16];
	};

	/**
	\brief
		CircularBuffer class.
		环形(循环)缓存区
	*/

	class UTILS_API CircularBuffer:
		public CBaseBuffer
	{
	public:
		CircularBuffer();
		CircularBuffer(size_t capacity);
		~CircularBuffer();

		bool empty() const { return false; }
		bool full() const {return false;}

		void capacity(size_t size, bool reserve = true);

		// Return number of bytes written.
		size_t write(const char *data, size_t bytes);
		// Return number of bytes read.
		size_t read(char *data, size_t bytes);
		size_t read2(char*& data, size_t bytes);

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
		public CBaseBuffer
	{
	public:
		BoundaryBuffer();
		BoundaryBuffer(size_t capacity);
		~BoundaryBuffer();

		void capacity(size_t size, bool reserve = true);

		bool empty() const;
		bool full() const;
		size_t size() const;
		size_t space() const;

		// Return number of bytes written.
		size_t write(const char *data, size_t bytes);
		// Return number of bytes read.
		size_t read(char *data, size_t bytes);
		size_t read2(char*& data, size_t bytes);

		BoundaryBuffer(BoundaryBuffer const &) = delete;
		BoundaryBuffer(BoundaryBuffer &&) = delete;
		BoundaryBuffer & operator = (BoundaryBuffer const &) = delete;
		BoundaryBuffer & operator = (BoundaryBuffer &&) = delete;
	private:
		size_t available_data() const;
		size_t available_space() const;
		void commit_write(int length);
		void commit_read(int amount);
	};
}


#endif	//__AUTOLOCK_H__

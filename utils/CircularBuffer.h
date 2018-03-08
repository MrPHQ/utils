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

	class UTILS_API CircularBuffer
	{
	public:
		CircularBuffer();
		CircularBuffer(size_t capacity);
		~CircularBuffer();

		void init(size_t capacity);
		void uninit();

		const char* buffer() const {return data_;}
		const char* data() const { return data_+ beg_index_; }
		void capacity(size_t size, bool reserve = true);
		size_t size() const { return size_; }
		size_t capacity() const { return capacity_; }

		size_t seek(size_t size);
		// Return number of bytes written.
		size_t write(const char *data, size_t bytes);
		// Return number of bytes read.
		size_t read(char *data, size_t bytes);

		CircularBuffer(CircularBuffer const &) = delete;
		CircularBuffer(CircularBuffer &&) = delete;
		CircularBuffer & operator = (CircularBuffer const &) = delete;
		CircularBuffer & operator = (CircularBuffer &&) = delete;
	private:
		size_t beg_index_, end_index_, size_, capacity_;
		bool init_;
		char *data_;
		char buff_[1024 * 16];
	};

	/**
	\brief
		CircularBuffer class.
		边界缓存区
	*/

}


#endif	//__AUTOLOCK_H__

#include "../utils/CircularBuffer.h"
#include "../utils/api.h"
#include <algorithm> // for std::min
#ifdef min
#undef min
#endif
namespace UTILS
{
	CircularBuffer::CircularBuffer()
		: beg_index_(0)
		, end_index_(0)
		, size_(0)
		, capacity_(0)
		, init_(false)
		, data_(nullptr)
	{
		capacity_ = 1024 * 16;
	}
	CircularBuffer::CircularBuffer(size_t capacity)
		: beg_index_(0)
		, end_index_(0)
		, size_(0)
		, init_(false)
		, capacity_(capacity)
		, data_(nullptr)
	{
		init(capacity);
	}

	CircularBuffer::~CircularBuffer()
	{
		uninit();
	}

	void CircularBuffer::init(size_t capacity)
	{
		if (init_) {
			return;
		}
		capacity_ = capacity;
		if (capacity_ <= 0) {
			return;
		}
		if (capacity_ > 1024 * 16) {
			data_ = new char[capacity_];
		}
		else {
			data_ = buff_;
		}
		init_ = true;
	}

	void CircularBuffer::uninit()
	{
		if (!init_) {
			return;
		}
		if ((capacity_ > 1024 * 16) && data_ != nullptr) {
			delete[] data_;
		}
		beg_index_ = 0;
		end_index_ = 0;
		size_ = 0;
		init_ = false;
		capacity_ = 0;
	}

	void CircularBuffer::capacity(size_t size, bool reserve /*= true*/)
	{
		if (size <= 0 || size >CIRCULAR_BUFFER_MAX_LEN) {
			return;
		}
		if (size == capacity_) {
			if (!reserve) {
				uninit();
				init(size);
			}
			return;
		}
		char* tmp = nullptr;
		int data_size = size_;
		if (reserve && data_size > 0) {
			tmp = new char[data_size];
			if (tmp == nullptr) {
				uninit();
				return;
			}
			memcpy(tmp, data_, data_size);
		}
		uninit();
		init(size);

		if ((data_ != nullptr) && (data_size > 0) && (tmp != nullptr)) {
			write(tmp, data_size);
		}
		if (tmp != nullptr) {
			delete[] tmp;
		}
	}

	size_t CircularBuffer::seek(size_t size)
	{
		return 0;
	}

	size_t CircularBuffer::write(const char *data, size_t bytes)
	{
		if (!init_) {
			return 0;
		}
		if (bytes == 0) return 0;

		size_t capacity = capacity_;
		size_t bytes_to_write = std::min(bytes, capacity - size_);

		// Write in a single step
		if (bytes_to_write <= capacity - end_index_)
		{
			memcpy(data_ + end_index_, data, bytes_to_write);
			end_index_ += bytes_to_write;
			if (end_index_ == capacity) end_index_ = 0;
		}
		// Write in two steps
		else
		{
			size_t size_1 = capacity - end_index_;
			memcpy(data_ + end_index_, data, size_1);
			size_t size_2 = bytes_to_write - size_1;
			memcpy(data_, data + size_1, size_2);
			end_index_ = size_2;
		}

		size_ += bytes_to_write;
		return bytes_to_write;
	}

	size_t CircularBuffer::read(char *data, size_t bytes)
	{
		if (!init_) {
			return 0;
		}
		if (bytes == 0) return 0;

		size_t capacity = capacity_;
		size_t bytes_to_read = std::min(bytes, size_);

		// Read in a single step
		if (bytes_to_read <= capacity - beg_index_)
		{
			memcpy(data, data_ + beg_index_, bytes_to_read);
			beg_index_ += bytes_to_read;
			if (beg_index_ == capacity) beg_index_ = 0;
		}
		// Read in two steps
		else
		{
			size_t size_1 = capacity - beg_index_;
			memcpy(data, data_ + beg_index_, size_1);
			size_t size_2 = bytes_to_read - size_1;
			memcpy(data + size_1, data_, size_2);
			beg_index_ = size_2;
		}

		size_ -= bytes_to_read;
		return bytes_to_read;
	}
}
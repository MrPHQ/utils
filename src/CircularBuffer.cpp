#include "../utils/CircularBuffer.h"
#include "../utils/api.h"
#include <algorithm> // for std::min
#ifdef min
#undef min
#endif
namespace UTILS
{
	CBaseBuffer::CBaseBuffer()
		: _beg_index(0)
		, _end_index(0)
		, _size(0)
		, _capacity(0)
		, _init(false)
		, _data(nullptr)
	{
		_capacity = 1024 * 16;
	}
	CBaseBuffer::CBaseBuffer(size_t capacity)
		: _beg_index(0)
		, _end_index(0)
		, _size(0)
		, _capacity(capacity)
		, _init(false)
		, _data(nullptr)
	{
		init(capacity);
	}

	CBaseBuffer::~CBaseBuffer()
	{
		uninit();
	}

	void CBaseBuffer::init(size_t capacity){
		if (_init) {
			return;
		}
		_capacity = capacity;
		if (_capacity <= 0) {
			return;
		}
		if (_capacity > 1024 * 16) {
			_data = new char[_capacity];
		}
		else {
			_data = _buff;
		}
		_init = true;
	}

	void CBaseBuffer::uninit() {
		if (!_init) {
			return;
		}
		if ((_capacity > 1024 * 16) && _data != nullptr) {
			delete[] _data;
		}
		_beg_index = 0;
		_end_index = 0;
		_size = 0;
		_init = false;
		_capacity = 0;
	}

	void CBaseBuffer::reset() {
		_beg_index = 0;
		_end_index = 0;
		_size = 0;
	}

	CircularBuffer::CircularBuffer()
	{
	}
	CircularBuffer::CircularBuffer(size_t capacity)
		: CBaseBuffer(capacity)
	{
	}

	CircularBuffer::~CircularBuffer()
	{
	}

	void CircularBuffer::capacity(size_t size, bool reserve /*= true*/)
	{
		if (size <= 0 || size >CIRCULAR_BUFFER_MAX_LEN) {
			return;
		}
		if (size == _capacity) {
			if (!reserve) {
				uninit();
				init(size);
			}
			return;
		}
		char* tmp = nullptr;
		size_t data_size = _size;
		if (reserve && data_size > 0) {
			tmp = new char[data_size];
			if (tmp == nullptr) {
				uninit();
				return;
			}
			memcpy(tmp, _data, data_size);
		}
		uninit();
		init(size);

		if ((_data != nullptr) && (data_size > 0) && (tmp != nullptr)) {
			write(tmp, std::min(data_size,_capacity));
		}
		if (tmp != nullptr) {
			delete[] tmp;
		}
	}

	size_t CircularBuffer::write(const char *data, size_t bytes)
	{
		if (!_init) {
			return 0;
		}
		if (bytes == 0) return 0;

		size_t capacity = _capacity;
		size_t bytes_to_write = std::min(bytes, capacity - _size);

		// Write in a single step
		if (bytes_to_write <= capacity - _end_index)
		{
			memcpy(_data + _end_index, data, bytes_to_write);
			_end_index += bytes_to_write;
			if (_end_index == capacity) _end_index = 0;
		}
		// Write in two steps
		else
		{
			size_t size_1 = capacity - _end_index;
			memcpy(_data + _end_index, data, size_1);
			size_t size_2 = bytes_to_write - size_1;
			memcpy(_data, data + size_1, size_2);
			_end_index = size_2;
		}

		_size += bytes_to_write;
		return bytes_to_write;
	}

	size_t CircularBuffer::read(char *data, size_t bytes)
	{
		if (!_init) {
			return 0;
		}
		if (bytes == 0) return 0;

		size_t capacity = _capacity;
		size_t bytes_to_read = std::min(bytes, _size);

		// Read in a single step
		if (bytes_to_read <= capacity - _beg_index)
		{
			memcpy(data, _data + _beg_index, bytes_to_read);
			_beg_index += bytes_to_read;
			if (_beg_index == capacity) _beg_index = 0;
		}
		// Read in two steps
		else
		{
			size_t size_1 = capacity - _beg_index;
			memcpy(data, _data + _beg_index, size_1);
			size_t size_2 = bytes_to_read - size_1;
			memcpy(data + size_1, _data, size_2);
			_beg_index = size_2;
		}

		_size -= bytes_to_read;
		return bytes_to_read;
	}

	size_t CircularBuffer::read2(char*& data, size_t bytes) {
		return 0;
	}

	BoundaryBuffer::BoundaryBuffer()
	{

	}
	BoundaryBuffer::BoundaryBuffer(size_t capacity)
		: CBaseBuffer(capacity)
	{

	}
	BoundaryBuffer::~BoundaryBuffer()
	{

	}

	bool BoundaryBuffer::empty() const
	{
		return available_data() == 0;
	}

	bool BoundaryBuffer::full() const
	{
		return available_space() == 0;
	}

	size_t BoundaryBuffer::size() const
	{
		return available_data();
	}

	size_t BoundaryBuffer::space() const
	{
		return available_space();
	}

	void BoundaryBuffer::capacity(size_t size, bool reserve /*= true*/)
	{
		if (size <= 0 || size >CIRCULAR_BUFFER_MAX_LEN) {
			return;
		}
		if (size == _capacity) {
			if (!reserve) {
				uninit();
				init(size);
			}
			return;
		}
		char* tmp = nullptr;
		size_t data_size = _size;
		if (reserve && data_size > 0) {
			tmp = new char[data_size];
			if (tmp == nullptr) {
				uninit();
				return;
			}
			memcpy(tmp, _data, data_size);
		}
		uninit();
		init(size);

		if ((_data != nullptr) && (data_size > 0) && (tmp != nullptr)) {
			write(tmp, std::min(_capacity,data_size));
		}
		if (tmp != nullptr) {
			delete[] tmp;
		}
	}

	size_t BoundaryBuffer::write(const char *data, size_t bytes)
	{
		if (!_init) {
			return 0;
		}
		if (available_data() == 0) {
			_beg_index = _end_index = 0;
		}

		if (bytes > available_space()) {
			return 0;
		}

		void *result = memcpy((void*)data_end(), data, bytes);
		if (result != NULL) {
			return 0;
		}

		commit_write(bytes);

		return bytes;
	}

	size_t BoundaryBuffer::read(char *data, size_t bytes)
	{
		if (!_init) {
			return 0;
		}
		if (bytes > available_data()) {
			return 0;
		}

		void *result = memcpy(data, (void*)data_start(), bytes);
		if (result == nullptr) {
			return 0;
		}

		commit_read(bytes);

		if (_end_index == _beg_index) {
			_beg_index = _end_index = 0;
		}

		return bytes;
	}

	size_t BoundaryBuffer::read2(char*& data, size_t bytes) {
		if (!_init) {
			return 0;
		}
		if (bytes > available_data()) {
			return 0;
		}

		char* p = (char*)data_start();
		data = p;

		commit_read(bytes);

		if (_end_index == _beg_index) {
			_beg_index = _end_index = 0;
		}

		return bytes;
	}

	size_t BoundaryBuffer::available_data() const
	{
		if (!_init) {
			return 0;
		}
		return _end_index % _capacity - _beg_index;
	}

	size_t BoundaryBuffer::available_space() const
	{
		if (!_init) {
			return 0;
		}
		return _capacity - _end_index;
	}

	void BoundaryBuffer::commit_write(int length)
	{
		if (!_init) {
			return;
		}
		_end_index = (_end_index + length) % _capacity;
	}

	void BoundaryBuffer::commit_read(int amount)
	{
		if (!_init) {
			return;
		}
		_beg_index = (_beg_index + amount) % _capacity;
	}
}
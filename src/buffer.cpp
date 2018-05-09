#include "../utils/buffer.h"
#include "../utils/api.h"
#include <algorithm> // for std::min
#ifdef min
#undef min
#endif

//判断x是否是2的次方
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x)-1)) == 0))

namespace UTILS
{
	buffer::buffer()
		: _size(0)
		, _in(0)
		, _out(0)
		, _init(false)
		, _lock(nullptr)
	{
	}
	buffer::buffer(size_t capacity)
		: _size(0)
		, _in(0)
		, _out(0)
		, _init(false)
		, _lock(nullptr)
	{
		init(capacity);
	}

	buffer::~buffer()
	{
		uninit();
	}

#ifdef _WIN32
	void buffer::init(size_t capacity, CRITICAL_SECTION* lock /*= nullptr*/)
#else
	void buffer::init(size_t capacity, pthread_mutex_t* lock /*= nullptr*/)
#endif
	{
		if (_init) {
			return;
		}
		_size = capacity;
		if (_size <= 0) {
			return;
		}
		if (_size > 1024 * 16) {
			if (!is_power_of_2(_size)){
				return;
			}
			_buffer = new char[_size];
			if (nullptr == _buffer) {
				return;
			}
		}
		else {
			_buffer = _buff;
		}
		_lock = lock;
		_init = true;
	}

	void buffer::uninit() {
		if (!_init) {
			return;
		}
		if ((_size > 1024 * 16) && _buffer != nullptr) {
			delete[] _buffer;
			_buffer = nullptr;
		}
		_in = 0;
		_out = 0;
		_size = 0;
		_init = false;
	}

	size_t buffer::size() const { 
		uint32_t len = 0;
		if (nullptr != _lock) {
#ifdef _WIN32
			EnterCriticalSection(_lock);
#else
			pthread_mutex_lock(_lock);
#endif
		}
		len = _len();
		if (nullptr != _lock) {
#ifdef _WIN32
			LeaveCriticalSection(_lock);
#else
			pthread_mutex_unlock(_lock);
#endif
		}
		return len;
	}

	size_t buffer::space() const {
		uint32_t len = 0;
		if (nullptr != _lock) {
#ifdef _WIN32
			EnterCriticalSection(_lock);
#else
			pthread_mutex_lock(_lock);
#endif
		}
		len = _space();
		if (nullptr != _lock) {
#ifdef _WIN32
			LeaveCriticalSection(_lock);
#else
			pthread_mutex_unlock(_lock);
#endif
		}
		return len;
	}

	void buffer::clear()
	{
		if (nullptr != _lock) {
#ifdef _WIN32
			EnterCriticalSection(_lock);
#else
			pthread_mutex_lock(_lock);
#endif
		}
		_in = _out = 0;
		if (nullptr != _lock) {
#ifdef _WIN32
			LeaveCriticalSection(_lock);
#else
			pthread_mutex_unlock(_lock);
#endif
		}
	}

	void buffer::write(size_t size) {
		if (nullptr != _lock) {
#ifdef _WIN32
			EnterCriticalSection(_lock);
#else
			pthread_mutex_lock(_lock);
#endif
		}
		_in += size;
		if (nullptr != _lock) {
#ifdef _WIN32
			LeaveCriticalSection(_lock);
#else
			pthread_mutex_unlock(_lock);
#endif
		}
	}
	size_t buffer::write(const char *data, size_t bytes)
	{
		uint32_t ret;
		if (nullptr != _lock) {
#ifdef _WIN32
			EnterCriticalSection(_lock);
#else
			pthread_mutex_lock(_lock);
#endif
		}
		ret = _write(data, bytes);
		if (nullptr != _lock) {
#ifdef _WIN32
			LeaveCriticalSection(_lock);
#else
			pthread_mutex_unlock(_lock);
#endif
		}
		return ret;
	}

	size_t buffer::read(char *data, size_t bytes)
	{
		uint32_t ret;
		if (nullptr != _lock) {
#ifdef _WIN32
			EnterCriticalSection(_lock);
#else
			pthread_mutex_lock(_lock);
#endif
		}
		ret = _read(data, bytes);
		//buffer中没有数据
		if (_in == _out)
			_in = _out = 0;
		if (nullptr != _lock) {
#ifdef _WIN32
			LeaveCriticalSection(_lock);
#else
			pthread_mutex_unlock(_lock);
#endif
		}
		return ret;
	}

	size_t buffer::read2(size_t bytes, char*& data1, size_t& bytes1, char*& data2, size_t& bytes2)
	{
		uint32_t ret;
		if (nullptr != _lock) {
#ifdef _WIN32
			EnterCriticalSection(_lock);
#else
			pthread_mutex_lock(_lock);
#endif
		}
		ret = _read2(bytes, data1, bytes1, data2, bytes2);
		//buffer中没有数据
		if (_in == _out)
			_in = _out = 0;
		if (nullptr != _lock) {
#ifdef _WIN32
			LeaveCriticalSection(_lock);
#else
			pthread_mutex_unlock(_lock);
#endif
		}
		return ret;
	}

	uint32_t buffer::_read(char *data, uint32_t size)
	{
		assert(data);
		uint32_t len = 0;
		uint32_t size_ = std::min(size, _len());
		/* first get the data from fifo->out until the end of the buffer */
		len = std::min(size_, _size - (_out & (_size - 1)));
		memcpy(data, _buffer + (_out & (_size - 1)), len);
		/* then get the rest (if any) from the beginning of the buffer */
		memcpy(data + len, _buffer, size_ - len);
		_out += size_;
		return size_;
	}

	size_t buffer::_read2(size_t bytes, char*& data1, size_t& bytes1, char*& data2, size_t& bytes2)
	{
		uint32_t len = 0;
		uint32_t size_ = std::min(bytes, _len());
		/* first get the data from fifo->out until the end of the buffer */
		len = std::min(size_, _size - (_out & (_size - 1)));
		data1 = _buffer + (_out & (_size - 1));
		bytes1 = len;
		/* then get the rest (if any) from the beginning of the buffer */
		data2 = _buffer;
		bytes2 = size_ - len;
		if (bytes2 == 0) {
			data2 = nullptr;
		}
		_out += size_;
		return size_;
	}

	uint32_t buffer::_write(const char *data, uint32_t size)
	{
		assert(data);
		uint32_t len = 0;
		uint32_t size_ = std::min(size, _space());
		/* first put the data starting from fifo->in to buffer end */
		len = std::min(size_, _size - (_in & (_size - 1)));
		if (len < size) {
			API::DEBUG_INFO("utils","分段..len:%d total:%d", len, size);
		}
		memcpy(_buffer + (_in & (_size - 1)), data, len);
		/* then put the rest (if any) at the beginning of the buffer */
		memcpy(_buffer, data + len, size_ - len);
		_in += size_;
		return size_;
	}

	void buffer::capacity(size_t size, bool reserve /*= true*/)
	{
		if (size <= 0 || size >BUFFER_MAX_LEN) {
			return;
		}
		if (size == _size) {
			if (!reserve) {
				uninit();
				init(size);
			}
			return;
		}
		char* tmp = nullptr;
		size_t data_size = _size;

		if (_buffer != nullptr) {
			if (reserve && data_size > 0) {
				tmp = new char[data_size];
				if (tmp == nullptr) {
					uninit();
					return;
				}
				memcpy(tmp, _buffer, data_size);
			}
		}

		uninit();
		init(size);

		if ((_buffer != nullptr) && (data_size > 0) && (tmp != nullptr)) {
			_write(tmp, std::min(_size, data_size));
		}
		if (tmp != nullptr) {
			delete[] tmp;
		}
	}

	CircularBuffer::CircularBuffer()
	{

	}
	CircularBuffer::CircularBuffer(size_t capacity)
		: buffer(capacity)
	{

	}
	CircularBuffer::~CircularBuffer()
	{

	}

	bool CircularBuffer::is_wiret() const
	{
		return true;
	}

	BoundaryBuffer::BoundaryBuffer()
	{

	}
	BoundaryBuffer::BoundaryBuffer(size_t capacity)
		: buffer(capacity)
	{

	}
	BoundaryBuffer::~BoundaryBuffer()
	{

	}

	bool BoundaryBuffer::is_wiret() const
	{
		bool ret = false;
		if (nullptr != _lock) {
#ifdef _WIN32
			EnterCriticalSection(_lock);
#else
			pthread_mutex_lock(_lock);
#endif
		}
		ret = _is_wiret();
		if (nullptr != _lock) {
#ifdef _WIN32
			LeaveCriticalSection(_lock);
#else
			pthread_mutex_unlock(_lock);
#endif
		}
		return ret;
	}

	bool BoundaryBuffer::_is_wiret() const
	{
		return (_in - _out) < _size;
	}
}
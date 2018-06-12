#include "../utils/buff.h"
#include "../utils/api.h"
#include "../src/internal.h"
#include <algorithm> // for std::min
#ifdef min
#undef min
#endif

//判断x是否是2的次方
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x)-1)) == 0))

namespace UTILS
{
	namespace BUFFER
	{

		CRingBuffer::CRingBuffer()
			: _size(0)
			, _in(0)
			, _out(0)
			, _init(false)
		{
			_temp_buffer._buffer = nullptr;
			_temp_buffer._size = 0;
			_InitLock();
		}
		CRingBuffer::CRingBuffer(size_t uiCapacity)
			: _size(0)
			, _in(0)
			, _out(0)
			, _init(false)
		{
			_temp_buffer._buffer = nullptr;
			_temp_buffer._size = 0;
			_InitLock();
			Init(uiCapacity);
		}

		CRingBuffer::~CRingBuffer()
		{
			UnInit();
			_UnInitLock();
		}

		void CRingBuffer::_InitLock()
		{
#ifdef _WIN32
			InitializeCriticalSection(&_lock);
#else
			pthread_mutexattr_init(&mutex_attr);
			pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
			pthread_mutex_init(&mutex, &mutex_attr);
#endif
		}

		void CRingBuffer::_UnInitLock()
		{
#ifdef _WIN32
			DeleteCriticalSection(&_lock);
#else
			pthread_mutexattr_destroy(&mutex_attr);
#endif
		}

		void CRingBuffer::Init(size_t uiCapacity)
		{
			if (_init) {
				return;
			}
			_size = uiCapacity;
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
			_init = true;
		}

		void CRingBuffer::UnInit() {
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

		size_t CRingBuffer::Size() {
			if (!_init) {
				return 0;
			}
			uint32_t len = 0;
			
#ifdef _WIN32
			EnterCriticalSection(&_lock);
#else
			pthread_mutex_lock(&_lock);
#endif
			len = _len();
#ifdef _WIN32
			LeaveCriticalSection(&_lock);
#else
			pthread_mutex_unlock(&_lock);
#endif
			return len;
		}

		size_t CRingBuffer::Space() {
			if (!_init) {
				return 0;
			}
			uint32_t len = 0;
#ifdef _WIN32
			EnterCriticalSection(&_lock);
#else
			pthread_mutex_lock(&_lock);
#endif
			len = _space();
#ifdef _WIN32
			LeaveCriticalSection(&_lock);
#else
			pthread_mutex_unlock(&_lock);
#endif
			return len;
		}

		void CRingBuffer::Clear()
		{
#ifdef _WIN32
			EnterCriticalSection(&_lock);
#else
			pthread_mutex_lock(&_lock);
#endif
			_in = _out = 0;
#ifdef _WIN32
			LeaveCriticalSection(&_lock);
#else
			pthread_mutex_unlock(&_lock);
#endif
		}

		void CRingBuffer::Write(size_t size) {
			if (!_init) {
				return;
			}
#ifdef _WIN32
			EnterCriticalSection(&_lock);
#else
			pthread_mutex_lock(&_lock);
#endif
			_in += size;
#ifdef _WIN32
			LeaveCriticalSection(&_lock);
#else
			pthread_mutex_unlock(&_lock);
#endif
		}
		size_t CRingBuffer::Write(const char *data, size_t bytesWriteOfNumber)
		{
			if (!_init) {
				return 0;
			}
			uint32_t ret;
#ifdef _WIN32
			EnterCriticalSection(&_lock);
#else
			pthread_mutex_lock(&_lock);
#endif
			ret = _write(data, bytesWriteOfNumber);
#ifdef _WIN32
			LeaveCriticalSection(&_lock);
#else
			pthread_mutex_unlock(&_lock);
#endif
			return ret;
		}

		size_t CRingBuffer::Read(char *data, size_t bytesReadOfNumber)
		{
			if (!_init) {
				return 0;
			}
			uint32_t ret;
#ifdef _WIN32
			EnterCriticalSection(&_lock);
#else
			pthread_mutex_lock(&_lock);
#endif
			ret = _read(data, bytesReadOfNumber);
			//buffer中没有数据
			if (_in == _out)
				_in = _out = 0;
#ifdef _WIN32
			LeaveCriticalSection(&_lock);
#else
			pthread_mutex_unlock(&_lock);
#endif
			return ret;
		}

		size_t CRingBuffer::ReadEx(char*& buff, size_t bytesReadOfNumber)
		{
			if (!_init) {
				return 0;
			}
			uint32_t ret;
#ifdef _WIN32
			EnterCriticalSection(&_lock);
#else
			pthread_mutex_lock(&_lock);
#endif
			ret = _read2(buff, bytesReadOfNumber);
			//buffer中没有数据
			if (_in == _out)
				_in = _out = 0;
#ifdef _WIN32
			LeaveCriticalSection(&_lock);
#else
			pthread_mutex_unlock(&_lock);
#endif
			return ret;
		}

		uint32_t CRingBuffer::_read(char *data, uint32_t size)
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

		size_t CRingBuffer::_read2(char*& buff, size_t bytesReadOfNumber)
		{
			uint32_t len = 0, ret = 0;
			uint32_t size_ = std::min(bytesReadOfNumber, _len());
			ret = size_;
			/* first get the data from fifo->out until the end of the buffer */
			len = std::min(size_, _size - (_out & (_size - 1)));
			buff = _buffer + (_out & (_size - 1));
			/* then get the rest (if any) from the beginning of the buffer */
			if (size_ - len > 0){
				ret = 0;
				if (size_ > _temp_buffer._size){
					if (_temp_buffer._size > 0 && (_temp_buffer._buffer != nullptr)){
						delete[] _temp_buffer._buffer;
						_temp_buffer._buffer = nullptr;
					}
					_temp_buffer._size = PAD_SIZE(size_);
					_temp_buffer._buffer = new char[_temp_buffer._size];

				}
				if (size_ <= _temp_buffer._size){
					if (_temp_buffer._buffer != nullptr){
						UTILS::MSG_INFO(".new char.LINE:%d",__LINE__);
						memcpy(_temp_buffer._buffer, buff, len);
						memcpy(_temp_buffer._buffer + len, _buffer, size_ - len);
						buff = _temp_buffer._buffer;
						ret = size_;
					}
				}
			}
			_out += size_;
			return ret;
		}

		uint32_t CRingBuffer::_write(const char *data, uint32_t size)
		{
			assert(data);
			uint32_t len = 0;
			uint32_t size_ = std::min(size, _space());
			/* first put the data starting from fifo->in to buffer end */
			len = std::min(size_, _size - (_in & (_size - 1)));
			if (len < size) {
				API::DEBUG_INFO("utils", "分段..len:%d total:%d", len, size);
			}
			memcpy(_buffer + (_in & (_size - 1)), data, len);
			/* then put the rest (if any) at the beginning of the buffer */
			memcpy(_buffer, data + len, size_ - len);
			_in += size_;
			return size_;
		}
	}
}
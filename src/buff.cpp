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
			, _UseInternalLock(true)
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
			, _UseInternalLock(true)
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
			if (_UseInternalLock){
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
			else{
				return _len();
			}
		}

		size_t CRingBuffer::Space() {
			if (!_init) {
				return 0;
			}
			if (_UseInternalLock){
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
			else{
				return _space();
			}
		}

		size_t CRingBuffer::SpaceEx()
		{
			if (!_init) {
				return 0;
			}
			uint32_t len = 0;
			if (_UseInternalLock){
#ifdef _WIN32
				EnterCriticalSection(&_lock);
#else
				pthread_mutex_lock(&_lock);
#endif
				len = _space();
				len = std::min(len, _size - (_in & (_size - 1)));
#ifdef _WIN32
				LeaveCriticalSection(&_lock);
#else
				pthread_mutex_unlock(&_lock);
#endif
			}else{
				len = _space();
				len = std::min(len, _size - (_in & (_size - 1)));
			}
			return len;
		}

		void CRingBuffer::Clear()
		{
			if (_UseInternalLock){
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
			else{
				_in = _out = 0;
			}
		}

		void CRingBuffer::Write(size_t size) {
			if (!_init) {
				return;
			}
			if (_UseInternalLock){
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
			else{
				_in += size;
			}
		}
		size_t CRingBuffer::Write(const char *data, size_t bytesWriteOfNumber)
		{
			if (!_init) {
				return 0;
			}
			uint32_t ret;
			if (_UseInternalLock){
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
			}
			else{
				ret = _write(data, bytesWriteOfNumber);
			}
			return ret;
		}

		size_t CRingBuffer::Read(char *data, size_t bytesReadOfNumber)
		{
			if (!_init) {
				return 0;
			}
			uint32_t ret;
			if (_UseInternalLock){
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
			}else{
				ret = _read(data, bytesReadOfNumber);
				//buffer中没有数据
				if (_in == _out)
					_in = _out = 0;
			}
			return ret;
		}

		size_t CRingBuffer::ReadEx(char*& buff, size_t bytesReadOfNumber)
		{
			if (!_init) {
				return 0;
			}
			uint32_t ret;
			if (_UseInternalLock){
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
			}
			else{
				ret = _read2(buff, bytesReadOfNumber);
				//buffer中没有数据
				if (_in == _out)
					_in = _out = 0;
			}
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
						MSG_INFO(".new char.");
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

		CBoundBuffer::CBoundBuffer()
		{
			m_bInit = FALSE;
			m_iBlockSize = 0;
			m_pBasePointer = NULL;
			m_iCurrentPosition = 0;
			m_UseInternalLock = TRUE;
			_InitLock();
		}


		CBoundBuffer::~CBoundBuffer()
		{
			_UnInitLock();
		}

		int CBoundBuffer::Init(int iBlockSize)
		{
			int err = 0;
			if (m_UseInternalLock){
				EnterCriticalSection(&m_lock);
				err = Init(iBlockSize);
				LeaveCriticalSection(&m_lock);
			}
			else{
				err = Init(iBlockSize);
			}
			return err;
		}

		int CBoundBuffer::UnInit()
		{
			int err = 0;
			if (m_UseInternalLock){
				EnterCriticalSection(&m_lock);
				err = _UnInit();
				LeaveCriticalSection(&m_lock);
			}
			else{
				err = _UnInit();
			}
			return err;
		}

		int CBoundBuffer::Clear()
		{
			int err = 0;
			if (m_UseInternalLock){
				EnterCriticalSection(&m_lock);
				err = _Clear();
				LeaveCriticalSection(&m_lock);
			}
			else{
				err = _Clear();
			}
			return err;
		}

		int CBoundBuffer::Write(BYTE* pDataBuff, int iDataLen, int* pBlockPostion)
		{
			int err = 0;
			if (m_UseInternalLock){
				EnterCriticalSection(&m_lock);
				err = _Write(pDataBuff, iDataLen, pBlockPostion);
				LeaveCriticalSection(&m_lock);
			}
			else{
				err = _Write(pDataBuff, iDataLen, pBlockPostion);
			}
			return err;
		}

		int CBoundBuffer::Read(int iBlockPostion, BYTE* pDataBuff, int iDataLen)
		{
			int err = 0;
			if (m_UseInternalLock){
				EnterCriticalSection(&m_lock);
				err = _Read(iBlockPostion, pDataBuff, iDataLen);
				LeaveCriticalSection(&m_lock);
			}
			else{
				err = _Read(iBlockPostion, pDataBuff, iDataLen);
			}
			return err;
		}

		int CBoundBuffer::GetAddr(int iBlockPostion, BYTE*& pBlockAddr)
		{
			int err = 0;
			if (m_UseInternalLock){
				EnterCriticalSection(&m_lock);
				err = _GetAddr(iBlockPostion, pBlockAddr);
				LeaveCriticalSection(&m_lock);
			}
			else{
				err = _GetAddr(iBlockPostion, pBlockAddr);
			}
			return err;
		}

		int CBoundBuffer::_Init(int iBlockSize)
		{
			if (m_bInit){
				return 0;
			}

			if (iBlockSize < 1024)
			{
				iBlockSize = 1024;
			}

			if (iBlockSize >BUFFER_MAX_LEN){
				return -1;
			}

			//创建(或打开)共享数据段
			m_iBlockSize = PAD_SIZE(iBlockSize);
			m_pBasePointer = new BYTE[m_iBlockSize];
			if (nullptr == m_pBasePointer){
				return -2;
			}
			m_bInit = TRUE;
			return 0;
		}

		int CBoundBuffer::_UnInit()
		{
			if (!m_bInit){
				return 0;
			}
			m_bInit = FALSE;
			if (NULL != m_pBasePointer){
				delete[] m_pBasePointer;
			}
			return 0;
		}

		//清空内存块
		int CBoundBuffer::_Clear()
		{
			m_iCurrentPosition = 0;
			return 0;
		}

		int CBoundBuffer::_Write(BYTE* pDataBuff, int iDataLen, int* pBlockPostion)
		{
			if (NULL == pDataBuff || iDataLen > m_iBlockSize)
			{
				return -1;
			}
			if (!m_bInit)
			{
				return -2;
			}

			BYTE* ptr = NULL;
			int iBlockPosition = 0;
			if ((m_iBlockSize - m_iCurrentPosition) > iDataLen)
			{
				ptr = (BYTE*)(m_pBasePointer + m_iCurrentPosition);
				iBlockPosition = m_iCurrentPosition;
				m_iCurrentPosition += iDataLen;
			}
			else
			{
				ptr = m_pBasePointer;
				iBlockPosition = 0;
				m_iCurrentPosition = iDataLen;
			}
			memcpy(ptr, pDataBuff, iDataLen);
			if (NULL != pBlockPostion)
			{
				*pBlockPostion = iBlockPosition;
			}
			return 0;
		}

		int CBoundBuffer::_Read(int iBlockPostion, BYTE* pDataBuff, int iDataLen)
		{
			if (iBlockPostion < 0 || iBlockPostion > m_iBlockSize)
			{
				return -1;
			}
			if (NULL == pDataBuff || (iBlockPostion + iDataLen) > m_iBlockSize)
			{
				return -2;
			}
			if (!m_bInit)
			{
				return -3;
			}

			BYTE* ptr = m_pBasePointer + iBlockPostion;
			memcpy(pDataBuff, ptr, iDataLen);
			return 0;
		}

		int CBoundBuffer::_GetAddr(int iBlockPostion, BYTE*& pBlockAddr)
		{
			if (iBlockPostion < 0 || iBlockPostion > m_iBlockSize)
			{
				return -1;
			}
			if (!m_bInit)
			{
				return -2;
			}
			BYTE* ptr = m_pBasePointer + iBlockPostion;
			if (NULL != pBlockAddr)
			{
				pBlockAddr = ptr;
			}

			return 0;
		}

		void CBoundBuffer::_InitLock()
		{
			InitializeCriticalSection(&m_lock);
		}

		void CBoundBuffer::_UnInitLock()
		{
			DeleteCriticalSection(&m_lock);
		}
	}
}
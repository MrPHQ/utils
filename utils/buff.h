#ifndef __UTILS_BUFF__
#define __UTILS_BUFF__

#include <utils/config.h>
#include <utils/define.h>

/**< ��󻺴�����С.*/
#define BUFFER_MAX_LEN 1024*1024*1024

namespace UTILS
{
	namespace BUFFER
	{
		/**
		@brief
		����(ѭ��)������(CRingBuffer) class.
		*/

		class UTILS_API CRingBuffer
		{
		public:
			CRingBuffer();
			/*
			@brief ���캯��..
				�ͻ��˴����׽���ʹ��.�ڲ��ᴴ��\�����׽���
			\param uiCapacity
				�����С
			*/
			CRingBuffer(size_t uiCapacity);
			virtual ~CRingBuffer();
			void Init(size_t uiCapacity);
			void UnInit();

			/*
			@brief �жϻ�����Ϊ��..
			\return [bool]
			*/
			bool IsEmpty() const { return _in == _out; }

			/*
			@brief �жϻ�������..
			\return [bool]
			*/
			bool IsFull() const { return _size == (_in - _out); }

			/*
			@brief ��ȡ��������ʼλ��..
			\return [ptr]
			*/
			const char* GetBuffer() const { return _buffer; }

			/*
			@brief ��ȡ������������ʼλ��..
			\return [ptr]
			*/
			const char* GetDataStart() const { return _buffer + _out; }

			/*
			@brief ��ȡ���������ݽ���λ��..
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
			@brief ��ȡ��������С..
			\return [size_t]
			*/
			size_t Capacity() const { return _size; }

			/*
			@brief ��ȡ���������ݴ�С..
			\return [size_t]
			*/
			size_t Size();

			/*
			@brief ��ȡ���������ÿռ��С..
			\return [size_t]
			*/
			size_t Space();

			/*
			@brief ��λ����..
			\return [size_t]
			*/
			void Clear();

			/*
			@brief �޸Ļ��������ݴ�С.
			\param size
				��������������ָ����С�ֽ�����
			\return [void]
			*/
			void Write(size_t size);

			/*
			@brief �򻺴���д������.
			\param data
				���ݻ�����
			\param bytesWriteOfNumber
				��д�����ݴ�С.
			\return [size_t]
				����д�����ݴ�С
			*/
			size_t Write(const char *data, size_t bytesWriteOfNumber);

			/*
			@brief �ӻ�������ȡָ����С����.
			\param buff
				���ݽ��ջ�����
			\param bytesReadOfNumber
				����ȡ���ݴ�С.
			\return [size_t]
				���ض�ȡ�����ݴ�С
			*/
			size_t Read(char* buff, size_t bytesReadOfNumber);
			/*
			@brief �ӻ�������ȡָ����С����.
			\param buff
				ָ�����ݻ�����������ָ��.
			\param bytesReadOfNumber
				����ȡ���ݴ�С.
			\return [size_t]
				���ض�ȡ�����ݴ�С
			*/
			size_t ReadEx(char*& buff, size_t bytesReadOfNumber);

		private:
			/**< �������ݴ�С. */
			size_t _len() const { return (_in - _out); }
			/**< ���ÿռ��С. */
			size_t _space() const { return (_size - _in + _out); }
			/**< д������. */
			size_t _write(const char *data, size_t bytes);
			/**< ��ȡ����. */
			size_t _read(char *data, size_t bytes);
			size_t _read2(char*& buff, size_t bytesReadOfNumber);
			/**< ��ʼ����. */
			void _InitLock();
			void _UnInitLock();

			CRingBuffer(CRingBuffer const &) = delete;
			CRingBuffer(CRingBuffer &&) = delete;
			CRingBuffer& operator= (CRingBuffer const &) = delete;
			CRingBuffer& operator= (CRingBuffer &&) = delete;
		protected:
			/**< ������*/
			char* _buffer;
			/**< ��������С*/
			uint32_t _size;
			/**< �������д��λ��.*/
			uint32_t _in;
			/**< ���ݶ�ȡ�����λ��.*/
			uint32_t _out;
			/**< ��ʱ������.*/
			struct {
				char* _buffer;
				uint32_t _size;
			}_temp_buffer;
#ifdef _WIN32
			CRITICAL_SECTION _lock;
#else
			pthread_mutex_t _lock;    //������
#endif
			bool _init;
			char _buff[1024 * 16];
		};
	}
}


#endif	//__AUTOLOCK_H__

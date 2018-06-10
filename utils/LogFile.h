#ifndef __UTILS_LOG_FILE__
#define __UTILS_LOG_FILE__
#include <utils/config.h>
#include <utils/define.h>

namespace UTILS
{
	/**
	\brief
		��־�ļ�ģʽ.
	*/
	enum LOG_FILE_MODE {
		LOG_FILE_MODE_NONE = 0,
		/**< ͬ��д�ļ� */
		LOG_FILE_MODE_SYNC = 1,
		/**< �첽д�ļ�,�ڲ��߳��Զ�д�ļ� */
		LOG_FILE_MODE_ASYNC_IN = 2,
		/**< �첽д�ļ�,�ⲿ����д�ļ�,���򲻼�¼�ļ� */
		LOG_FILE_MODE_ASYNC_OUT = 3,
	};

	/**
	\brief
		��־�ļ���¼ͷ��Ϣ.
	*/
	enum LOG_FILE_HEAD {
		/**< дʱ�� */
		LOG_FILE_HEAD_TIME = 0x01,
		/**< д������ */
		LOG_FILE_HEAD_PROC_NAME = 0x02,
		/**< д����ID */
		LOG_FILE_HEAD_PROC_ID = 0x04,
		/**< д�߳�ID */
		LOG_FILE_HEAD_THREAD_ID = 0x08,
		/**< д�ļ��� */
		LOG_FILE_HEAD_FILE_NAME = 0x10,
		/**< д�к� */
		LOG_FILE_HEAD_FILE_LINE = 0x20,
	};

	/**
	\brief
		��־�ļ���.
	*/
	class UTILS_API CLogFile
	{
	public:
		/**
		\brief
			���캯��,Ĭ��ʹ�õ�ǰ����ĸ�Ŀ¼
		*/
		CLogFile();
		~CLogFile();

		/**
		\brief
			���ļ�

			ע��: ��־ָ������־�ļ�������,��ᴴ��,����Ѿ�����,��򿪺�ֱ�Ӷ�λ���ļ�ĩβ.
		\param nMode
			��־�ļ�����ģʽ,
		\param nHead
			��־�ļ���¼ͷ��Ϣ. �μ�LOG_FILE_HEAD
		\param file
			��־�ļ�·��.
		\param uiDays
			�洢����
		\param uiPerFileSize
			ÿ����־�ļ��������. �ֽ�.
		\return
			Error.h
		*/
		int Open(LOG_FILE_MODE nMode, DWORD nHead, const char* file, unsigned int uiDays, unsigned int uiPerFileSize);
		/**
		\brief
			�ر��ļ�
		*/
		void Close();
		/**
		\brief
			д����־.
			��־�ļ�ģʽ(LOG_FILE_MODE):
				LOG_FILE_MODE_SYNC ����д����־�ļ�.
				LOG_FILE_MODE_ASYNC_IN д���ڲ�������,�ڲ���־�߳��ӳ�д����־�ļ�
				LOG_FILE_MODE_ASYNC_OUT д���ڲ�������,Ȼ���ⲿ����[Active],����д����־�ļ�
		\param data
			��־����
		\param len
			��־��С
		*/
		void Write(const char* pFormat, ...);
		/**
		\brief
			д����־.
			��־�ļ�ģʽ(LOG_FILE_MODE):
				LOG_FILE_MODE_SYNC ����д����־�ļ�.
				LOG_FILE_MODE_ASYNC_IN д���ڲ�������,�ڲ���־�߳��ӳ�д����־�ļ�
				LOG_FILE_MODE_ASYNC_OUT д���ڲ�������,Ȼ���ⲿ����[Active],����д����־�ļ�
		\param file
			��־λ�������ļ�.
		\param line
			��־����λ���ļ��к�.
		\param data
			��־����
		\param len
			��־��С
		*/
		void WriteEx(const char* file, int line, const char* pFormat, ...);
		/**
		\brief
			д����־. �ѻ��������־д����־�ļ�
			ֻ����	��־�ļ�ģʽ(LOG_FILE_MODE):LOG_FILE_MODE_ASYNC_OUT
		*/
		void Active();

	public:
		static inline CLogFile* getSingletonPtr()
		{
			return &getSingleton();
		}
		static inline CLogFile& getSingleton()
		{
			static CLogFile _instance;
			return _instance;
		}

	protected:

		CLogFile(CLogFile const &) = delete;
		CLogFile(CLogFile &&) = delete;
		CLogFile& operator= (CLogFile const &) = delete;
		CLogFile& operator= (CLogFile &&) = delete;

	private:
		void* m_pFile;
	};

#define LF_LOGGER CLogFile::getSingleton()
#define PLF_LOGGER CLogFile::getSingletonPtr()

#define LOG_INIT(P1,P2,P3,P4,P5) LF_LOGGER.Open(P1,P2,P3,P4,P5);
#define LOG_UNINIT() LF_LOGGER.Close();
#define LOG_WRITE(...) LF_LOGGER.Write(__VA_ARGS__);
#define LOG_WRITE_EX(...) LF_LOGGER.WriteEx(__FILE__, __LINE__, __VA_ARGS__);
#define LOG_ACTIVE() LF_LOGGER.Active();
}

#endif	//__AUTOLOCK_H__

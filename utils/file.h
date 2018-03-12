#ifndef __UTILS_FILE__
#define __UTILS_FILE__
#include <utils/config.h>
#include <fstream>

namespace UTILS
{
	typedef struct _FILE_PROPERTY
	{
		int iSize;
		_off_t st_size; //�ļ��ֽ���  
		time_t st_atime; //�ϴη���ʱ��  
		time_t st_mtime; //�ϴ��޸�ʱ��  
		time_t st_ctime; //����ʱ��  
	}FILE_PROPERTY, *PFILE_PROPERTY;

	enum PATH_FILE_OPENMODE {
		/// ������������ȡ�ļ��е�����. ���ָ��·����û�а������ļ������ᴴ������������std::ios_base::badbit.
		PATH_FILE_OPENMODE_IN = 0x01,
		/// д��������д�����ݵ��ļ�.���ָ��·���в�û�а������ļ����ᴴ��һ���µ�.���ָ�����и��ļ�����ô�򿪺󣬻�����ļ�������
		PATH_FILE_OPENMODE_OUT = 0x02,
		/// ���ļ�֮��������λ���ļ�ĩβ.���ָ����·�������ڸ��ļ����ᴴ��.
		PATH_FILE_OPENMODE_ATE = 0x04,
		/// ÿ�ν���д�������ʱ�򶼻����¶�λ���ļ���ĩβ..���ָ��·�������ڸ��ļ���ô����һ��
		PATH_FILE_OPENMODE_APP = 0x08,
		/// ���ļ������ļ��Ѵ�����ô������ļ�����..���ָ����·�������ڸ��ļ����ᴴ��.���ָ����·�����и��ļ���ô����ļ��е�����.
		PATH_FILE_OPENMODE_TRUNC = 0x10,
		/// �Զ����Ƶķ�ʽ�Դ򿪵��ļ����ж�д.���ָ����·�������ڸ��ļ����ᴴ��.
		PATH_FILE_OPENMODE_BINARY = 0x20,
	};
	enum PATH_SEEK_TYPE {
		///�ļ��Ŀ�ͷ.
		PATH_SEEK_NONE = -1,
		///�ļ��Ŀ�ͷ.
		PATH_SEEK_SET = 0,
		///�ļ�ָ��ĵ�ǰλ��.
		PATH_SEEK_CUR = 1,
		///����β.
		PATH_SEEK_END = 2,
	};
	/**
	\brief
		CPath class.
	*/
	class UTILS_API CFile
	{
	public:
		/**
		\brief
			���캯��,Ĭ��ʹ�õ�ǰ����ĸ�Ŀ¼
		*/
		CFile();
		/**
		\brief
			���캯��
		\param path
			�ļ���,
		*/
		CFile(const char* path);
		~CFile();

		/**
		\brief
			��ȡ��ϸ��Ϣ
		\param pInfo
			�������,
		\return
			Error.h
		*/
		static int GetStat(const char* path, PFILE_PROPERTY pInfo);
		int GetStat(PFILE_PROPERTY pInfo) const { return UTILS::CFile::GetStat(this->m_stPath.szPath, pInfo); }
		/**
		\brief
			·����ָ��һ�����е��ļ���Ŀ¼.
		\param pInfo
			path.
		\return
			���ڷ���TRUE������FALSE
		*/
		static bool Exists(const char* path);
		bool Exists() const{return UTILS::CFile::Exists(this->m_stPath.szPath);}
		/**
		\brief
			�ļ���С..
		\param pInfo
			path.
		\return
			���ڷ���TRUE������FALSE
		*/
		static uint64_t Size(const char* path);
		uint64_t Size();
		/**
		\brief
			�ļ��Ƿ��.
		\param pInfo
			path.
		\return
		.
		*/
		bool IsOpen() const { return this->m_stPath.bOpen; }
		/**
		\brief
			��ȡ�ļ�Ŀ¼.
		\param pInfo
			path.
		\return
		.
		*/
		static void GetDir(const char* path, char* buff, int len);
		void GetDir(char* buff, int len) const { UTILS::CFile::GetDir(this->m_stPath.szPath, buff,len); }
		/**
		\brief
			��ȡ�ļ�����.
		\param pInfo
			path.
		\return
		.
		*/
		static void GetFileName(const char* path, char* buff, int len);
		void GetFileName(char* buff, int len) const { UTILS::CFile::GetFileName(this->m_stPath.szPath, buff, len); }
		/**
		\brief
			��ȡ��׺.
		\param pInfo
			path.
		\return
			.
		*/
		static void GetSuffix(const char* path, char* buff, int len);
		void GetSuffix(char* buff, int len) const { UTILS::CFile::GetSuffix(this->m_stPath.szPath, buff, len); }

		/**
		\brief
			ɾ��Ŀ¼/�ļ�.
		\param pInfo
			path.
		\return
		.
		*/
		static int Delete(const char* path);
		int Delete();

		/**
		\brief
			���ļ�.
		\param mode
			PATH_FILE_OPENMODE
		\param pInfo
			path.
		\return
		.
		*/
		int Open(unsigned int mode, const char* path = NULL);

		/**
		\brief
			д�ļ�.
		\param data
			path.
		\return
			int �ɹ�д���ֽڳ���
		*/
		static int Write(int mode, const char* path, const char* data, int len);
		int Write(const char* data, int len);

		/**
		\brief
			���ļ�.
		\param buff
			���ݴ�Ż�����.
		\param len
			���ݴ�Ż���������,len=0 ��ȡ��������,����������,len������Ҫ�ĳ���
		\return
			int Error.h
		*/
		static int Read(int mode, const char* path, char* buff, int len, int* pDataLen = nullptr);
		int Read(char* buff, int len, int* pDataLen = nullptr);

		/**
		\brief
			ƫ��.
		\param offset
			ƫ�Ƴ���.
		\param origin
			.
		\return
			int Error.h
		*/
		int Seek(unsigned int offset, PATH_SEEK_TYPE origin = PATH_SEEK_NONE);

		/**
		\brief
			�ر��ļ�.
		\return
			int Error.h
		*/
		int Close();

		/**
		\brief
			��ȡ·��.
		\return
			int Error.h
		*/
		const char* GetPath() const { return m_stPath.szPath; }

		/**
		\brief
			�Զ�������.
		\return
			int Error.h
		*/
		void SetContextData(void*);
		void* GetContextData();
	protected:
		/**
		\brief
			�ж�·����ָ��һ�����е��ļ���Ŀ¼.
		*/
		operator bool(){
			return UTILS::CFile::Exists(this->m_stPath.szPath);
		}
		bool operator==(const bool&rhs){
			bool ret = UTILS::CFile::Exists(this->m_stPath.szPath);
			return ret == rhs;
		}
		bool operator==(const CFile& file);

	private:
		/**
		\brief
			������.
		*/
		void Clear();
	private:
		//const int iPathLen;
		/// �ļ�/Ŀ¼
		struct {
			char szPath[512];
			bool bOpen;
			unsigned int uiMode;
			void* pContext;
		}m_stPath;
		std::fstream m_file;
	};
}


#endif	//__AUTOLOCK_H__

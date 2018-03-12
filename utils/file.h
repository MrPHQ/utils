#ifndef __UTILS_FILE__
#define __UTILS_FILE__
#include <utils/config.h>
#include <fstream>

namespace UTILS
{
	typedef struct _FILE_PROPERTY
	{
		int iSize;
		_off_t st_size; //文件字节数  
		time_t st_atime; //上次访问时间  
		time_t st_mtime; //上次修改时间  
		time_t st_ctime; //创建时间  
	}FILE_PROPERTY, *PFILE_PROPERTY;

	enum PATH_FILE_OPENMODE {
		/// 读操作，即读取文件中的数据. 如果指定路径中没有包含该文件，不会创建，而且设置std::ios_base::badbit.
		PATH_FILE_OPENMODE_IN = 0x01,
		/// 写操作，即写入数据到文件.如果指定路径中并没有包含该文件，会创建一个新的.如果指包含有该文件，那么打开后，会清空文件中内容
		PATH_FILE_OPENMODE_OUT = 0x02,
		/// 打开文件之后立即定位到文件末尾.如果指定的路径不存在该文件不会创建.
		PATH_FILE_OPENMODE_ATE = 0x04,
		/// 每次进行写入操作的时候都会重新定位到文件的末尾..如果指定路径不存在该文件那么创建一个
		PATH_FILE_OPENMODE_APP = 0x08,
		/// 打开文件，若文件已存在那么，清空文件内容..如果指定的路径不存在该文件不会创建.如果指定的路径含有该文件那么清空文件中的内容.
		PATH_FILE_OPENMODE_TRUNC = 0x10,
		/// 以二进制的方式对打开的文件进行读写.如果指定的路径不存在该文件不会创建.
		PATH_FILE_OPENMODE_BINARY = 0x20,
	};
	enum PATH_SEEK_TYPE {
		///文件的开头.
		PATH_SEEK_NONE = -1,
		///文件的开头.
		PATH_SEEK_SET = 0,
		///文件指针的当前位置.
		PATH_SEEK_CUR = 1,
		///件结尾.
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
			构造函数,默认使用当前程序的根目录
		*/
		CFile();
		/**
		\brief
			构造函数
		\param path
			文件名,
		*/
		CFile(const char* path);
		~CFile();

		/**
		\brief
			获取详细信息
		\param pInfo
			输出参数,
		\return
			Error.h
		*/
		static int GetStat(const char* path, PFILE_PROPERTY pInfo);
		int GetStat(PFILE_PROPERTY pInfo) const { return UTILS::CFile::GetStat(this->m_stPath.szPath, pInfo); }
		/**
		\brief
			路径是指向一个现有的文件或目录.
		\param pInfo
			path.
		\return
			存在返回TRUE，否则FALSE
		*/
		static bool Exists(const char* path);
		bool Exists() const{return UTILS::CFile::Exists(this->m_stPath.szPath);}
		/**
		\brief
			文件大小..
		\param pInfo
			path.
		\return
			存在返回TRUE，否则FALSE
		*/
		static uint64_t Size(const char* path);
		uint64_t Size();
		/**
		\brief
			文件是否打开.
		\param pInfo
			path.
		\return
		.
		*/
		bool IsOpen() const { return this->m_stPath.bOpen; }
		/**
		\brief
			获取文件目录.
		\param pInfo
			path.
		\return
		.
		*/
		static void GetDir(const char* path, char* buff, int len);
		void GetDir(char* buff, int len) const { UTILS::CFile::GetDir(this->m_stPath.szPath, buff,len); }
		/**
		\brief
			获取文件名称.
		\param pInfo
			path.
		\return
		.
		*/
		static void GetFileName(const char* path, char* buff, int len);
		void GetFileName(char* buff, int len) const { UTILS::CFile::GetFileName(this->m_stPath.szPath, buff, len); }
		/**
		\brief
			获取后缀.
		\param pInfo
			path.
		\return
			.
		*/
		static void GetSuffix(const char* path, char* buff, int len);
		void GetSuffix(char* buff, int len) const { UTILS::CFile::GetSuffix(this->m_stPath.szPath, buff, len); }

		/**
		\brief
			删除目录/文件.
		\param pInfo
			path.
		\return
		.
		*/
		static int Delete(const char* path);
		int Delete();

		/**
		\brief
			打开文件.
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
			写文件.
		\param data
			path.
		\return
			int 成功写入字节长度
		*/
		static int Write(int mode, const char* path, const char* data, int len);
		int Write(const char* data, int len);

		/**
		\brief
			读文件.
		\param buff
			数据存放缓存区.
		\param len
			数据存放缓存区长度,len=0 读取所有数据,缓存区不足,len返回需要的长度
		\return
			int Error.h
		*/
		static int Read(int mode, const char* path, char* buff, int len, int* pDataLen = nullptr);
		int Read(char* buff, int len, int* pDataLen = nullptr);

		/**
		\brief
			偏移.
		\param offset
			偏移长度.
		\param origin
			.
		\return
			int Error.h
		*/
		int Seek(unsigned int offset, PATH_SEEK_TYPE origin = PATH_SEEK_NONE);

		/**
		\brief
			关闭文件.
		\return
			int Error.h
		*/
		int Close();

		/**
		\brief
			获取路径.
		\return
			int Error.h
		*/
		const char* GetPath() const { return m_stPath.szPath; }

		/**
		\brief
			自定义数据.
		\return
			int Error.h
		*/
		void SetContextData(void*);
		void* GetContextData();
	protected:
		/**
		\brief
			判断路径是指向一个现有的文件或目录.
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
			清理缓存.
		*/
		void Clear();
	private:
		//const int iPathLen;
		/// 文件/目录
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

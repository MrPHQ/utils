#ifndef __UTILS_TIME__
#define __UTILS_TIME__

#include <utils/config.h>

namespace UTILS
{
	/**
	\brief 时间操作管理..
	*/
	class UTILS_API Time
	{
	public:
		typedef struct _TIME_INFO
		{
			short wYear;
			short wMonth;
			short wDay;
			short wHour;
			short wMinute;
			short wSecond;
		}TIME_INFO, *PTIME_INFO;
	public:
		Time();
		~Time();

		/**
		\brief 获取本地当前时间
		\return time_t
		*/
		static void GetNowTime(TIME_INFO&);
		/**
		\brief 将时间转换为自1970年1月1日以来持续时间的秒数，
		\return time_t
		*/
		static time_t Convert(short wYear, short wMonth, short wDay, 
			short wHour, short wMinute,short wSecond);
		static time_t Convert(TIME_INFO&);
		static int Convert(time_t, TIME_INFO&);


	private:
		Time(const Time &);
		Time &operator=(const Time &);
	};

	/**
	\brief 时间戳..
	*/
	class UTILS_API CTimeStampWrapper {
	private:
		unsigned long long _uiNow;
		int _iTimeOut;
	public:
		CTimeStampWrapper();
		~CTimeStampWrapper();

		/**
		\brief
			初始化时间戳
		\param uiNow
			当前运行时间
		\param iTimeOut
			超时时间
		\return
			void
		*/
		void Init(unsigned long long uiNow, int iTimeOut);

		/**
		\brief
			更新时间戳
		\param uiNow
			当前运行时间
		\return
			void
		*/
		void Update(unsigned long long uiNow);

		/**
		\brief
			检测本地时间戳是否已经到达超时时间.
		\param uiNow
			当前运行时间
		\param update
			是否更新时间戳
		\return
			bool 如果达到超时时间,返回true,否则返回false
		*/
		bool Check(unsigned long long uiNow, bool update = true);

		/**
		\brief
			检测本地时间戳是否已经到达超时时间.
		\param uiNow
			当前运行时间
		\param iTimeOut
			超时时间
		\return
			bool 如果达到超时时间,返回true,否则返回false
		*/
		bool Check(unsigned long long uiNow, int iTimeOut);

		/**
		\brief
			获取超时时间.
		\return
		*/
		int GetTimeOut();
	};
}


#endif	//__AUTOLOCK_H__

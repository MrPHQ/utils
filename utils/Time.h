#ifndef __UTILS_TIME__
#define __UTILS_TIME__

#include <utils/config.h>

namespace UTILS
{
	/**
	\brief
		时间操作管理..

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
		\brief
			获取本地当前时间

		\return
			time_t
		*/
		static void GetNowTime(TIME_INFO&);
		/**
		\brief
			将时间转换为自1970年1月1日以来持续时间的秒数，

		\return
			time_t
		*/
		static time_t Convert(short wYear, short wMonth, short wDay, 
			short wHour, short wMinute,short wSecond);
		static time_t Convert(TIME_INFO&);
		static int Convert(time_t, TIME_INFO&);


	private:
		Time(const Time &);
		Time &operator=(const Time &);
	};
}


#endif	//__AUTOLOCK_H__

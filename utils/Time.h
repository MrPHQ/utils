#ifndef __UTILS_TIME__
#define __UTILS_TIME__

#include <utils/config.h>

namespace UTILS
{
	/**
	\brief
		ʱ���������..

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
			��ȡ���ص�ǰʱ��

		\return
			time_t
		*/
		static void GetNowTime(TIME_INFO&);
		/**
		\brief
			��ʱ��ת��Ϊ��1970��1��1����������ʱ���������

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

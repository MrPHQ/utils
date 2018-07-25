#ifndef __UTILS_TIME__
#define __UTILS_TIME__

#include <utils/config.h>

namespace UTILS
{
	/**
	\brief ʱ���������..
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
		\brief ��ȡ���ص�ǰʱ��
		\return time_t
		*/
		static void GetNowTime(TIME_INFO&);
		/**
		\brief ��ʱ��ת��Ϊ��1970��1��1����������ʱ���������
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
	\brief ʱ���..
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
			��ʼ��ʱ���
		\param uiNow
			��ǰ����ʱ��
		\param iTimeOut
			��ʱʱ��
		\return
			void
		*/
		void Init(unsigned long long uiNow, int iTimeOut);

		/**
		\brief
			����ʱ���
		\param uiNow
			��ǰ����ʱ��
		\return
			void
		*/
		void Update(unsigned long long uiNow);

		/**
		\brief
			��Ȿ��ʱ����Ƿ��Ѿ����ﳬʱʱ��.
		\param uiNow
			��ǰ����ʱ��
		\param update
			�Ƿ����ʱ���
		\return
			bool ����ﵽ��ʱʱ��,����true,���򷵻�false
		*/
		bool Check(unsigned long long uiNow, bool update = true);

		/**
		\brief
			��Ȿ��ʱ����Ƿ��Ѿ����ﳬʱʱ��.
		\param uiNow
			��ǰ����ʱ��
		\param iTimeOut
			��ʱʱ��
		\return
			bool ����ﵽ��ʱʱ��,����true,���򷵻�false
		*/
		bool Check(unsigned long long uiNow, int iTimeOut);

		/**
		\brief
			��ȡ��ʱʱ��.
		\return
		*/
		int GetTimeOut();
	};
}


#endif	//__AUTOLOCK_H__

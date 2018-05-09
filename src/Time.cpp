#include "../utils/Time.h"
#include "../utils/api.h"

#ifdef WIN32
#include <time.h>
#endif
namespace UTILS
{
	Time::Time()
	{
	}
	Time::~Time()
	{

	}

	time_t Time::Convert(short wYear, short wMonth, short wDay, short wHour, short wMinute, short wSecond)
	{
		short wM = 0, wY = 0;
		wY = (wYear == 0 ? 0 : (wYear - 1900));
		wM = (wMonth == 0 ? 0 : (wMonth - 1));
		struct tm gm = { wSecond, wMinute, wHour, wDay, wM, wY, 0, 0, 0 };
		return mktime(&gm);
	}

	time_t Time::Convert(TIME_INFO& v)
	{
		return Convert(v.wYear,v.wMonth,v.wDay,v.wHour,v.wMinute,v.wSecond);
	}

	int Time::Convert(time_t v, TIME_INFO& info)
	{
		tm ov;
		int err = localtime_s(&ov, &v);
		if (err == 0)
		{
			info.wYear = ov.tm_year+1900;
			info.wMonth = ov.tm_mon+1;
			info.wDay = ov.tm_mday;
			info.wHour = ov.tm_hour;
			info.wMinute = ov.tm_min;
			info.wSecond = ov.tm_sec;
		}
		return err;
	}

	void Time::GetNowTime(TIME_INFO& time) {
#ifdef _WIN32
		SYSTEMTIME st;
		GetLocalTime(&st);
		time.wYear = st.wYear;
		time.wMonth = st.wMonth;
		time.wDay = st.wDay;
		time.wHour = st.wHour;
		time.wMinute = st.wMinute;
		time.wSecond = st.wSecond;
#else
		static int gZone = 28800;
		static int gDayLightTime = 0;

		struct tm*	p;
		time_t timep;

		time(&timep);

		timep += gZone;
		timep += gDayLightTime;

		p = localtime(&timep);
		if (p != NULL){
			time.wYear = p->tm_year + 1900;
			time.wMonth = p->tm_mon + 1;
			time.wDay = p->tm_mday;
			//time.wDayOfWeek = p->tm_wday;
			time.wHour = p->tm_hour;
			time.wMinute = p->tm_min;
			time.wSecond = p->tm_sec;
			//time.wMilliseconds = 0;
		}
#endif
	}

	CTimeStampWrapper::CTimeStampWrapper() {
		_uiNow = 0;
		_iTimeOut = 0;
	}
	CTimeStampWrapper::~CTimeStampWrapper() {}

	void CTimeStampWrapper::Init(unsigned long long uiNow, int iTimeOut) {
		_uiNow = uiNow;
		_iTimeOut = iTimeOut;
	}

	void CTimeStampWrapper::Update(unsigned long long uiNow) {
		_uiNow = uiNow;
	}

	bool CTimeStampWrapper::Check(unsigned long long uiNow, bool update /*= true*/) {
		if (abs((int)(uiNow - _uiNow)) >= _iTimeOut) {
			if (update) {
				_uiNow = uiNow;
			}
			return true;
		}
		return false;
	}

	bool CTimeStampWrapper::Check(unsigned long long uiNow, int iTimeOut) {
		if (abs((int)(uiNow - _uiNow)) >= iTimeOut) {
			return true;
		}
		return false;
	}

	int CTimeStampWrapper::GetTimeOut() {
		return _iTimeOut;
	}
}
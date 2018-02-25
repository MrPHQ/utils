#include "../utils/logger.h"
#include "../utils/api.h"

#undef LOG_INIT
#undef LOG_UNINIT
#undef LOG_DEBUG
#undef LOG_ERROR
#undef LOG_FATAL
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_TRACE
#undef LOG_DEBUG_LOGER
#undef LOG_ERROR_LOGER
#undef LOG_FATAL_LOGER
#undef LOG_INFO_LOGER
#undef LOG_WARN_LOGER
#undef LOG_TRACE_LOGER

#include <utility_log\utility_logger.h>

#ifdef WIN32
/*#ifdef _DEBUG
#pragma comment(lib,"utility_log/utility_loggerd.lib")
#else
#pragma comment(lib,"utility_log/utility_logger.lib")
#endif*/
#else
#include <cstring>
#endif

#ifdef _WIN32
#define DO_LOGGER(bufSize,pFormat)\
	va_list args;            \
	va_start(args, pFormat);        \
	char buf[bufSize];        \
	_vsnprintf_s(buf, sizeof(buf)-1, pFormat, args);    \
	va_end(args); 
#else
#define DO_LOGGER(bufSize,pFormat)\
	va_list args;            \
	va_start(args, pFormat);        \
	char buf[bufSize];        \
	vsnprintf(buf, sizeof(buf)-1, pFormat, args);    \
	va_end(args);
#endif

namespace UTILS{namespace LOG{
Logger::Logger()
{
	memset(logger_name, 0, sizeof(logger_name));
}

Logger::~Logger()
{
}

void Logger::Debug(const char* logger, const char* filename, const int fileline, const char* pFormat, ...)
{
	DO_LOGGER(1024, pFormat);
	LOG_DEBUG_LOGER_EX(logger, buf);
}

void Logger::Error(const char* logger, const char* filename, const int fileline, const char* pFormat, ...)
{
	DO_LOGGER(256, pFormat);
	LOG_ERROR_LOGER_EX(logger, buf);
}

void Logger::Fatal(const char* logger, const char* filename, const int fileline, const char* pFormat, ...)
{
	DO_LOGGER(256, pFormat);
	LOG_FATAL_LOGER_EX(logger, buf);
}

void Logger::Info(const char* logger, const char* filename, const int fileline, const char* pFormat, ...)
{
	DO_LOGGER(512, pFormat);
	LOG_INFO_LOGER_EX(logger, buf);
}

void Logger::Warn(const char* logger, const char* filename, const int fileline, const char* pFormat, ...)
{
	DO_LOGGER(256, pFormat);
	LOG_WARN_LOGER_EX(logger, buf);
}

void Logger::Trace(const char* logger, const char* filename, const int fileline, const char* pFormat, ...)
{
	DO_LOGGER(1024, pFormat);
	LOG_TRACE_LOGER_EX(logger, buf);
}

void Logger::StartSystem(const char* properties_filename, bool create_console /*= false*/)
{
	char szDll[256];
	API::Sprintf(szDll, 256,"%s", API::GetCurrentPath(ghUtilsInstance));
	LOG_INIT(properties_filename, create_console, szDll);
}

void Logger::StopSystem()
{
	LOG_UNINIT();
}
}}
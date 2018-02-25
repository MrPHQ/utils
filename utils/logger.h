#ifndef __UTILS_LOGGER__
#define __UTILS_LOGGER__

#include <utils/config.h>
namespace UTILS {namespace LOG{
/**
\brief
	日志包装类..

	使用log4cplus
*/ 
class UTILS_API Logger final
{
private:
	Logger();
	virtual ~Logger();

public:
	/// 启动日志系统
	/// @param[in] properties_filename 日志系统配置文件文件名
	void StartSystem(const char* properties_filename, bool create_console = false);

	/// 关闭日志系统
	void StopSystem();

public:
	void Debug(const char* logger, const char* filename, const int fileline, const char* pFormat, ...);

	void Error(const char* logger, const char* filename, const int fileline, const char* pFormat, ...);

	void Fatal(const char* logger, const char* filename, const int fileline, const char* pFormat, ...);

	void Info(const char* logger, const char* filename, const int fileline, const char* pFormat, ...);

	void Warn(const char* logger, const char* filename, const int fileline, const char* pFormat, ...);

	void Trace(const char* logger, const char* filename, const int fileline, const char* pFormat, ...);

public:
	static inline Logger* getSingletonPtr()
	{
		return &getSingleton();
	}
	static inline Logger& getSingleton()
	{
		static Logger _instance;
		return _instance;
	}
private:
	Logger(const Logger &);
	Logger& operator=(const Logger &);
private:
	char logger_name[128];
};
#define g_Logger Logger::getSingleton()
#define g_pLogger Logger::getSingletonPtr()

#define LOG_INIT(P1,P2) g_Logger.StartSystem(P1,P2);
#define LOG_UNINIT() g_Logger.StopSystem();
#define LOG_DEBUG(...) g_Logger.Debug("",__FILE__, __LINE__, __VA_ARGS__);
#define LOG_ERROR(...) g_Logger.Error("",__FILE__, __LINE__, __VA_ARGS__);
#define LOG_FATAL(...) g_Logger.Fatal("",__FILE__, __LINE__, __VA_ARGS__);
#define LOG_INFO(...) g_Logger.Info("",__FILE__, __LINE__, __VA_ARGS__);
#define LOG_WARN(...) g_Logger.Warn("",__FILE__, __LINE__, __VA_ARGS__);
#define LOG_TRACE(...) g_Logger.Trace("",__FILE__, __LINE__, __VA_ARGS__);

#define LOG_DEBUG_LOGER(P,...) g_Logger.Debug(P,__FILE__, __LINE__, __VA_ARGS__);
#define LOG_ERROR_LOGER(P,...) g_Logger.Error(P,__FILE__, __LINE__, __VA_ARGS__);
#define LOG_FATAL_LOGER(P,...) g_Logger.Fatal(P,__FILE__, __LINE__, __VA_ARGS__);
#define LOG_INFO_LOGER(P,...) g_Logger.Info(P,__FILE__, __LINE__, __VA_ARGS__);
#define LOG_WARN_LOGER(P,...) g_Logger.Warn(P,__FILE__, __LINE__, __VA_ARGS__);
#define LOG_TRACE_LOGER(P,...) g_Logger.Trace(P,__FILE__, __LINE__, __VA_ARGS__);
}}
#endif
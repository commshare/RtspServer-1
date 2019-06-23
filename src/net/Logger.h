// PHZ
// 2018-5-15

#ifndef XOP_LOGGER_H
#define XOP_LOGGER_H
#include <stdarg.h>
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fstream>
#include <cstring>
#include <iostream>
#include <sstream>
#include <windows.h>
#include "net/Timestamp.h"

namespace xop
{
#define MAX_BUF_SIZE 4096
  enum class LogLevel {
	Debug = 0,
	Info,
	Notice,
	Warn,
	Error,
	Fatal,
  };

}
using LogFunc = std::function<void(xop::LogLevel level, const char* msg)>;
void mlog(xop::LogLevel level, const char* msg);
namespace xop
{
  class XLog {
  public:
	static LogFunc logfunc;
  public:
	XLog(const char* file, const char* func, int line, xop::LogLevel  level)
	  : m_file(basename(file))
	  , m_func(func)
	  , m_line(line)
	  , m_level(level) {
	  m_stream << "[" << xop::Timestamp::localtime() << "]";
	}
	~XLog() {
	  log_noLevelCmp(m_level, "[%s::%s():%d]%s", m_stream.str().c_str(), m_file, m_func, m_line); }
	std::ostream& stream() { return m_stream; }
  private:
	const char* basename(const char* filepath)
	{
	  if (!filepath) {
		return NULL;
	  }
	  const char* base = strrchr(filepath, '/');
#ifdef WIN32  // Look for either path separator in Windows
	  if (!base) {
		base = strrchr(filepath, '\\');
	  }
#endif
	  return base ? (base + 1) : filepath;
	}
	void log_noLevelCmp(xop::LogLevel  l, const char* fmt, ...)
	{
	  char buf[MAX_BUF_SIZE] = { 0 };
	  va_list args;
	  uint32_t uLen = 0;
	  int tid = get_thread_id();
	  snprintf(buf, 20, "[%d]", tid);

	  uLen = (uint32_t)strlen(buf);
	  va_start(args, fmt);
	  vsnprintf((char*)(buf + uLen), (MAX_BUF_SIZE - 1 - uLen), fmt, args);
	  va_end(args);
	  buf[MAX_BUF_SIZE - 1] = 0;
	  if (logfunc != nullptr)
	  {
		logfunc(l, (const char*)buf);
	  }
	  return;
	}
	inline unsigned int get_thread_id()
	{
#ifdef WIN32
	  return::GetCurrentThreadId();
#else
	  return thread_self();
#endif
	}
  private:
	xop::LogLevel  m_level;
	const char* m_file;
	const char* m_func;
	const int m_line;
	std::ostringstream m_stream;
  };








enum Priority 
{
    LOG_DEBUG, LOG_STATE, LOG_INFO, LOG_WARNING, LOG_ERROR,
};	

class Logger
{
public:
    Logger &operator=(const Logger &) = delete;
    Logger(const Logger &) = delete;	
    static Logger& instance();
    ~Logger();

    void setLogFile(char *pathname);
    void log(Priority priority, const char* __file, const char* __func, int __line, const char *fmt, ...);
	void log(Priority priority, const char *fmt, ...);

private:
    Logger();
    void run();

    std::atomic<bool> _shutdown;
    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _cond;
    std::queue<std::string> _queue;
    std::ofstream _ofs;
};
}

#ifdef _DEBUG
//#define DLOG(fmt,...)  xop::Logger::instance().log(xop::LOG_DEBUG, __FILE__, __FUNCTION__,__LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) xop::Logger::instance().log(xop::LOG_DEBUG, __FILE__, __FUNCTION__,__LINE__, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif
#define LOG_INFO(fmt, ...) xop::Logger::instance().log(xop::LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) xop::Logger::instance().log(xop::LOG_ERROR, __FILE__, __FUNCTION__,__LINE__, fmt, ##__VA_ARGS__)


#define FLOG() (xop::XLog(__FILE__, __FUNCTION__, __LINE__, xop::LogLevel::Info).stream())	

#endif


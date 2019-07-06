
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/daily_file_sink.h"

class Logger
{
public:
	~Logger() { spdlog::drop_all(); }
	Logger(){
		m_pSpdLogger = spdlog::daily_logger_mt<spdlog::async_factory>("daily_logger", "log.txt", 23, 59);
		spdlog::set_level(spdlog::level::level_enum::info);
		m_pSpdLogger->flush_on(spdlog::level::err);
	}
	

	static Logger* Instance() {
		if (NULL == m_pInstance)
		{
			if (NULL == m_pInstance)
			{
				m_pInstance = new Logger;
			}
		}
		return m_pInstance;
	}

	std::shared_ptr<spdlog::logger> GetLogger()
	{
		return m_pSpdLogger;
	}

private:
	static std::shared_ptr<spdlog::logger> m_pSpdLogger;
	static Logger* m_pInstance;

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
};

std::shared_ptr<spdlog::logger> Logger::m_pSpdLogger;
Logger* Logger::m_pInstance;


#define LOG_DEBUG(...) Logger::Instance()->GetLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)  Logger::Instance()->GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)  Logger::Instance()->GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::Instance()->GetLogger()->error(__VA_ARGS__)


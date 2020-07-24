#ifndef LOG_H
#define LOG_H

#include <iostream>

/*
#include <memory>
#include <string>
#include "spdlog/spdlog.h"

	
	class Log{
		
	public:
		static void Init(const std::string& logger_name = "Default_Logger");
		static std::shared_ptr<spdlog::logger> GetLogger();
	private:
		Log();
		static std::shared_ptr<spdlog::logger> s_coreLogger;
	};
}

#ifdef ENABLE_LOGGING

#define LOG_TRACE(...) CloudConnectionAPI::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) CloudConnectionAPI::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) CloudConnectionAPI::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) CloudConnectionAPI::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) CloudConnectionAPI::Log::GetLogger()->critical(__VA_ARGS__)

#else

*/

#define LOG_TRACE(...) std::cout<<__VA_ARGS__<<std::endl 
#define LOG_INFO(...) std::cout<<__VA_ARGS__<<std::endl
#define LOG_WARN(...) std::cout<<__VA_ARGS__<<std::endl
#define LOG_ERROR(...) std::cout<<__VA_ARGS__<<std::endl
#define LOG_CRITICAL(...) std::cout<<__VA_ARGS__<<std::endl

#endif /* LOG_H */








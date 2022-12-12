#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

class Log
{
public:

	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;

};



// Core log macros
#define PS_CORE_WARN(...)        ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PS_CORE_INFO(...)        ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PS_CORE_TRACE(...)       ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PS_CORE_ERROR(...)       ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PS_CORE_FATAL(...)       ::Log::GetCoreLogger()->critical(__VA_ARGS__)

#ifndef PS_DEBUG
#define PS_CORE_WARN(...)
#define PS_CORE_INFO(...)		 ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PS_CORE_TRACE(...)
#define PS_CORE_ERROR(...)
#define PS_CORE_FATAL(...)
#endif

#define  PS_ENABLE_ASSERTS

#ifdef PS_ENABLE_ASSERTS
#define PS_CORE_ASSERT(x) {if(!(x)) {PS_CORE_ERROR("{0}() Line: {1} | Assertion Failed: {2}",__FUNCTION__, __LINE__,#x); __debugbreak();}}
#else 
#define PS_CORE_ASSERT(x)
#endif
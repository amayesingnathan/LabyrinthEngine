#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace Laby {

	class Log
	{
	public:
		static void Init(const std::string& appName);

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return sClientLogger; }


	private:
		inline static std::shared_ptr<spdlog::logger> sCoreLogger;
		inline static std::shared_ptr<spdlog::logger> sClientLogger;

	};
}

//Core log macros
#define LAB_CORE_TRACE(...)		::Laby::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LAB_CORE_INFO(...)		::Laby::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LAB_CORE_WARN(...)		::Laby::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LAB_CORE_ERROR(...)		::Laby::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LAB_CORE_CRITICAL(...)	::Laby::Log::GetCoreLogger()->critical(__VA_ARGS__)

//Client log macros
#define LAB_TRACE(...)			::Laby::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LAB_INFO(...)			::Laby::Log::GetClientLogger()->info(__VA_ARGS__)
#define LAB_WARN(...)			::Laby::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LAB_ERROR(...)			::Laby::Log::GetClientLogger()->error(__VA_ARGS__)
#define LAB_CRITICAL(...)       ::Laby::Log::GetCoreLogger()->critical(__VA_ARGS__)
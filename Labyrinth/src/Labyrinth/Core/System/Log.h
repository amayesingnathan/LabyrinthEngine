#pragma once

#include "Labyrinth/Core/System/Base.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace Labyrinth {

	class LAB_API Log
	{
	public:
		static void Init(const std::string& appName);

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return sClientLogger; }


	private:
		static std::shared_ptr<spdlog::logger> sCoreLogger;
		static std::shared_ptr<spdlog::logger> sClientLogger;

	};

}

//Core log macros
#define LAB_CORE_TRACE(...)		::Labyrinth::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LAB_CORE_INFO(...)		::Labyrinth::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LAB_CORE_WARN(...)		::Labyrinth::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LAB_CORE_ERROR(...)		::Labyrinth::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LAB_CORE_CRITICAL(...)	::Labyrinth::Log::GetCoreLogger()->critical(__VA_ARGS__)
	
//Client log macros
#define LAB_TRACE(...)			::Labyrinth::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LAB_INFO(...)			::Labyrinth::Log::GetClientLogger()->info(__VA_ARGS__)
#define LAB_WARN(...)			::Labyrinth::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LAB_ERROR(...)			::Labyrinth::Log::GetClientLogger()->error(__VA_ARGS__)
#define LAB_CRITICAL(...)       ::Labyrinth::Log::GetCoreLogger()->critical(__VA_ARGS__)

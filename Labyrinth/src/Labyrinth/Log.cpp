#include "Lpch.h"
#include "Log.h"

namespace Labyrinth {

	std::shared_ptr<spdlog::logger> Log::sCoreLogger;
	std::shared_ptr<spdlog::logger> Log::sClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		sCoreLogger = spdlog::stdout_color_mt("LABYRINTH");
		sCoreLogger->set_level(spdlog::level::trace);
		LAB_CORE_WARN("Initialised Log!");

		sClientLogger = spdlog::stdout_color_mt("APP");
		sClientLogger->set_level(spdlog::level::trace);
	}

}


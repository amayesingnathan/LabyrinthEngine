#pragma once

#include <string_view>

namespace Laby {

	class System
	{
	public:
		static void SetEnv(std::string_view envVar);
		static void Call(std::string_view cmd);
	};
}
#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace Laby::Utils {

	namespace String
	{
		std::string& ToLower(std::string& string);
		std::string ToLowerCopy(std::string_view string);
	}
}
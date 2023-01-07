#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace Laby {

	namespace StringUtils
	{
		std::string& ToLower(std::string& string);
		std::string ToLowerCopy(std::string_view string);

		void ReplaceToken(std::string& str, const char* token, const std::string& value);
	}
}
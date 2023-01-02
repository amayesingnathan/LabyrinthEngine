#include "Lpch.h"
#include "StringUtils.h"

namespace Laby::Utils {

	namespace String {

		std::string& ToLower(std::string& string)
		{
			std::transform(string.begin(), string.end(), string.begin(),
				[](const unsigned char c) { return std::tolower(c); });
			return string;
		}

		std::string ToLowerCopy(const std::string_view string)
		{
			std::string result(string);
			ToLower(result);
			return result;
		}
	}
}
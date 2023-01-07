#include "Lpch.h"
#include "StringUtils.h"

namespace Laby::StringUtils {

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


	void ReplaceToken(std::string& str, const char* token, const std::string& value)
	{
		usize pos = 0;
		while ((pos = str.find(token, pos)) != std::string::npos)
		{
			str.replace(pos, strlen(token), value);
			pos += strlen(token);
		}
	}
}
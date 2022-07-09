#pragma once

#include "json.hpp"

namespace Labyrinth {

	using JsonObj = nlohmann::json;

	class JSON
	{
	public:
		static JsonObj Open(const std::string& path)
		{
			std::ifstream i(path);
			JsonObj j;
			i >> j;
			return j;
		}

		static void Write(const std::string& path, const JsonObj& obj)
		{
			std::ofstream o(path);
			o << std::setw(4) << obj << std::endl;
		}

	};
}
#pragma once

#include "json.hpp"

namespace Labyrinth {

	using JsonObj = nlohmann::json;

	class JSON
	{
	public:
		static JsonObj Open(const std::filesystem::path& path)
		{
			std::ifstream i(path);
			JsonObj j;
			if (i) i >> j;
			return j;
		}

		static void Write(const std::filesystem::path& path, const JsonObj& obj)
		{
			std::ofstream o(path);
			o << std::setw(4) << obj << std::endl;
		}

	};
}
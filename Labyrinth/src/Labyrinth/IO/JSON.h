#pragma once

#include <fstream>

#include "json.hpp"

#include "Filesystem.h"

namespace Laby {

	using JsonObj = nlohmann::json;

	class JSON
	{
	public:
		static JsonObj Open(const fs::path& path)
		{
			std::ifstream i(path);
			if (!i)
				return JsonObj();

			JsonObj out;
			i >> out;
			return out;
		}

		static void Write(const fs::path& path, const JsonObj& obj)
		{
			std::ofstream o(path);
			o << std::setw(4) << obj << std::endl;
		}

	};
}
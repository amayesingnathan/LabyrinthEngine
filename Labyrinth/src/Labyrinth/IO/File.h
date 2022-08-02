#pragma once

#include <Labyrinth/Core/Buffer.h>

namespace Labyrinth {

	class FileUtils
	{
	public:
		static Buffer Read(const std::filesystem::path& filepath);
		static void Read(const std::filesystem::path& filepath, std::string& string);
		static void Write(const std::filesystem::path& filepath, Buffer buffer);
	};

}
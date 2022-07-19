#pragma once

namespace Labyrinth {

	class FileUtils
	{
	public:
		static char* Read(const std::filesystem::path& filepath, size_t* outSize);
		static void Read(const std::filesystem::path& filepath, std::string& string);
		static void Write(const std::filesystem::path& filepath, char* data, size_t size);
	};

}
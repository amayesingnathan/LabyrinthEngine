#pragma once

#include <Labyrinth/Core/Buffer.h>

namespace Labyrinth {

	namespace fs = std::filesystem;

	class FileUtils
	{
	public:
		static Buffer Read(const fs::path& filepath);
		static void Read(const fs::path& filepath, std::string& string);

		static void Write(const fs::path& filepath, Buffer buffer);
		static void Write(const fs::path& filepath, const std::string& string);

		static void Create(const fs::path& filepath);
		static void CreateDir(const fs::path& filepath);

		static void CopyDir(const fs::path& src, const fs::path& dest);

		// These return empty strings if cancelled
		static std::string OpenFile(const std::vector<std::string>& filter);
		static std::string OpenDir();
		static std::string SaveFile(const std::vector<std::string>& filter);
	};

}
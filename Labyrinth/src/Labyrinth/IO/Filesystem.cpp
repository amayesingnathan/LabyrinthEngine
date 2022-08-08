#include "Lpch.h"
#include "Filesystem.h"

#include <portable-file-dialogs.h>

namespace Labyrinth {

	Buffer FileUtils::Read(const fs::path& filepath)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			LAB_CORE_ERROR("Failed to open {}", filepath);
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		usize size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			LAB_CORE_ERROR("File {} was empty!", filepath);
			return nullptr;
		}

		Buffer buffer(size);
		stream.read(buffer.as<char>(), size);
		stream.close();

		return buffer;
	}

	void FileUtils::Read(const fs::path& filepath, std::string& string)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			LAB_CORE_ERROR("Failed to open {}", filepath);
			return;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		size_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			LAB_CORE_ERROR("File {} was empty!", filepath);
			return;
		}

		string.resize(size);
		stream.read(&string[0], size);
		stream.close();
	}

	void FileUtils::Write(const fs::path& filepath, Buffer buffer)
	{
		std::ofstream stream(filepath, std::ios::binary);

		if (!stream)
		{
			// Failed to open the file
			LAB_CORE_ERROR("Failed to open file {}", filepath);
			return;
		}

		if (buffer.size == 0)
			return;

		stream.write(buffer.as<char>(), buffer.size);
	}

	void FileUtils::Write(const fs::path& filepath, const std::string& string)
	{
		std::ofstream stream(filepath, std::ios::binary);

		if (!stream)
		{
			// Failed to open the file
			LAB_CORE_ERROR("Failed to open file {}", filepath);
			return;
		}

		if (string.size() == 0)
			return;

		stream.write(string.c_str(), string.size());
	}

	void FileUtils::Create(const fs::path& filepath)
	{
		std::ofstream stream(filepath, std::ios::binary);
		if (!stream)
		{
			// Failed to open the file
			LAB_CORE_ERROR("Failed to create file {}", filepath);
			return;
		}
	}

	void FileUtils::CreateDir(const fs::path& filepath)
	{
		std::filesystem::create_directories(filepath);
	}

	void FileUtils::CopyDir(const fs::path& src, const fs::path& dest)
	{
		std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive);
	}

	fs::path FileUtils::OpenFile(const std::vector<std::string>& filter)
	{
		auto selection = pfd::open_file("Select a file", ".", filter).result();
		if (!selection.empty())
			return selection[0];

		return fs::path();
	}

	fs::path FileUtils::OpenDir()
	{
		return pfd::select_folder("Select a folder", ".").result();
	}

	fs::path FileUtils::SaveFile(const std::vector<std::string>& filter)
	{
		return pfd::save_file("Save file as", ".", filter).result();
	}

}
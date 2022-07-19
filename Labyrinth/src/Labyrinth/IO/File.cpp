#include "Lpch.h"
#include "File.h"

namespace Labyrinth {

	char* FileUtils::Read(const std::filesystem::path& filepath, size_t* outSize)
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
		size_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			LAB_CORE_ERROR("File {} was empty!", filepath);
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read(buffer, size);
		stream.close();

		if (outSize)
			*outSize = size;

		return buffer;
	}

	void FileUtils::Read(const std::filesystem::path& filepath, std::string& string)
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

	void FileUtils::Write(const std::filesystem::path& filepath, char* data, size_t size)
	{
		std::ofstream stream(filepath, std::ios::binary);

		if (!stream)
		{
			// Failed to open the file
			LAB_CORE_ERROR("Failed to open file {}", filepath);
			return;
		}

		stream.write(data, size);
	}

}
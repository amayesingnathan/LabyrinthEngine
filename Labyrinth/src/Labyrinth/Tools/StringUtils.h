#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace Labyrinth::Utils {

	namespace String
	{
		bool EqualsIgnoreCase(const std::string_view a, const std::string_view b);
		std::string& ToLower(std::string& string);
		std::string ToLowerCopy(std::string_view string);
		void Erase(std::string& str, const char* chars);
		void Erase(std::string& str, const std::string& chars);
		std::string SubStr(const std::string& string, size_t offset, size_t count = std::string::npos);
	}

	std::string_view GetFilename(const std::string_view filepath);
	std::string GetExtension(const std::string& filename);
	std::string RemoveExtension(const std::string& filename);
	bool StartsWith(const std::string& string, const std::string& start);

	// Keeps delimiters except for spaces, used for shaders
	std::vector<std::string> SplitStringAndKeepDelims(std::string str);

	std::vector<std::string> SplitString(const std::string_view string, const std::string_view& delimiters);
	std::vector<std::string> SplitString(const std::string_view string, const char delimiter);
	std::string ToLower(const std::string_view& string);
	std::string ToUpper(const std::string_view& string);
	std::string BytesToString(uint64_t bytes);

	int SkipBOM(std::istream& in);
	std::string ReadFileAndSkipBOM(const std::filesystem::path& filepath);

	template<class...Durations, class DurationIn>
	std::tuple<Durations...> BreakDownDuration(DurationIn d) {
		std::tuple<Durations...> retval;
		using discard = int[];
		(void)discard {
			0, (void((
				(std::get<Durations>(retval) = std::chrono::duration_cast<Durations>(d)),
				(d -= std::chrono::duration_cast<DurationIn>(std::get<Durations>(retval)))
				)), 0)...
		};
		return retval;
	}

	std::string DurationToString(std::chrono::duration<double> duration);

	template <typename IsAlreadyUsedFn>
	static std::string AddSuffixToMakeUnique(const std::string& name, IsAlreadyUsedFn&& isUsed)
	{
		auto nameToUse = name;
		int suffix = 1;

		while (isUsed(nameToUse))
			nameToUse = name + "_" + std::to_string(++suffix);

		return nameToUse;
	}
}

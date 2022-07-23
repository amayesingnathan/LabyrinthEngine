#include "Lpch.h"
#include "YAML.h"

namespace Labyrinth {

#ifdef LAB_PLATFORM_WINDOWS
	template<typename T>
	void YAMLParser::EncodeObject(const T& data, bool flag)
	{
		static_assert(false);
	}

	template<typename T, typename Target>
	bool YAMLParser::DecodeObject(Target data)
	{
		static_assert(false);
	}

	template<typename T, typename Target>
	bool YAMLParser::DecodeObject(Target data, YAML::Node node)
	{
		static_assert(false);
	}
#endif
}

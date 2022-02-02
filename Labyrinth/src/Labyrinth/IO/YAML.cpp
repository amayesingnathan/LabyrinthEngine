#include "Lpch.h"
#include "YAML.h"

#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"

namespace Labyrinth {

	template<typename T>
	void YAMLParser::EncodeObject(const T& data, bool flag)
	{
		static_assert(false);
	}

	template<typename T>
	T YAMLParser::DecodeObject()
	{
		static_assert(false);
	}

	template<typename T>
	void YAMLParser::DecodeObject(T& output)
	{
		static_assert(false);
	}

	template<typename T, typename Target>
	Ref<T> YAMLParser::DecodeObject(Target data)
	{
		static_assert(false);
	}

	template<typename T, typename Target>
	Ref<T> YAMLParser::DecodeObject(Target data, YAML::Node node)
	{
		static_assert(false);
	}

}

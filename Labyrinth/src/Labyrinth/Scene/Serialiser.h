#pragma once

#include "Scene.h"

#include "Labyrinth/IO/YAML.h"

namespace Labyrinth {

	class Serialiser
	{
	public:
		template<typename T>
		static void Serialise(T obj, const std::string& filepath)
		{
			YAMLParser parser;
			parser.encodeObject(obj);

			std::ofstream fout(filepath);
			fout << parser.getData();
		}
		static void SerialiseRuntime(const std::string& filepath)
		{
			// Not implemented
			LAB_CORE_ASSERT(false);
		}

		template<typename T, typename Target = void*>
		static bool Deserialise(const std::string& filepath, Target target = nullptr)
		{
			YAMLParser parser(filepath);
			if (parser.decodeObject<T>(target)) return true;
			return false;
		}
		static bool SeserialiseRuntime(const std::string& filepath)
		{
			// Not implemented
			LAB_CORE_ASSERT(false);
			return false;
		}
	};
}
#pragma once

#include "Scene.h"

#include "Labyrinth/IO/YAML.h"
#include "Labyrinth/Renderer/SubTexture.h"

namespace Labyrinth {

	class LAB_API Serialiser
	{
	public:
		template<typename T>
		static void Serialise(const T& obj, const std::string& filepath)
		{
			YAMLParser parser;

			parser.EncodeObject(obj);

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
			if (parser.DecodeObject<T>(target)) return true;
			return false;
		}
		static bool DeserialiseRuntime(const std::string& filepath)
		{
			// Not implemented
			LAB_CORE_ASSERT(false);
			return false;
		}
	};
}
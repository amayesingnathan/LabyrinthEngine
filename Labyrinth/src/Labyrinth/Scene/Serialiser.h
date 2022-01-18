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
			return parser.decodeObject<T>(target);
		}
		static bool SeserialiseRuntime(const std::string& filepath)
		{
			// Not implemented
			LAB_CORE_ASSERT(false);
			return false;
		}
	};


	class SceneSerialiser
	{
	public:
		SceneSerialiser(const Ref<Scene>& scene);

		void serialise(const std::string& filepath);
		void serialiseRuntime(const std::string& filepath);

		bool deserialise(const std::string& filepath);
		bool deserialiseRuntime(const std::string& filepath);

	private:
		Ref<Scene> mScene;
	};

}
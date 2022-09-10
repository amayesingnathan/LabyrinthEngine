#pragma once

#include "Labyrinth/Renderer/SubTexture.h"

namespace YAML {
	class Emitter;
	class Node;
}

namespace Labyrinth {

	class Scene;
	class Entity;

	class SceneSerialiser
	{
	public:
		SceneSerialiser(const Ref<Scene>& scene);

		void serialise(const std::filesystem::path& filepath);
		void serialiseRuntime(const std::filesystem::path& filepath)
		{
			// Not implemented
			LAB_CORE_ASSERT(false);
		}

		bool deserialise(const std::filesystem::path& filepath);
		bool deserialiseRuntime(const std::filesystem::path& filepath)
		{
			// Not implemented
			LAB_CORE_ASSERT(false);
			return false;
		}

	public:
		static void SerializeEntity(YAML::Emitter& out, Entity entity);
		static void DeserializeEntities(YAML::Node& entitiesNode, Ref<Scene> scene);

	public:
		inline static std::string_view FileFilter = "Hazel Scene (*.hscene)\0*.hscene\0";
		inline static std::string_view DefaultExtension = ".hscene";

	private:
		Ref<Scene> mScene;
	};
}